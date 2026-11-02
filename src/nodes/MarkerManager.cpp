#include "MarkerManager.hpp"

#include <algorithm>
#include <filesystem>
#include <format>
#include <godot_cpp/classes/box_mesh.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/core/print_string.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/callable_method_pointer.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <mutex>
#include <opencv2/core/mat.hpp>
#include <optional>
#include <thread>
#include <vector>

#include "DataConversion.hpp"
#include "DataStorage.hpp"
#include "ImageRenderer.hpp"
#include "MarkerDetection.hpp"
#include "MarkerPoseEstimator.hpp"
#include "MarkerRecognition.hpp"
#include "MarkerRectification.hpp"
#include "pipeline/Components.hpp"

namespace chess {

auto MarkerManager::_ready() -> void {
  if (godot::Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  m_source = godot::Object::cast_to<VideoSource>(
      godot::Engine::get_singleton()->get_singleton("VideoSource")
  );

  m_source->attach_output(m_buffer);
  m_worker = std::jthread([buffer = m_buffer, &mtx = m_mtx,
                           &output = m_output](std::stop_token st) {
    std::string calinbrationPath =
        godot::ProjectSettings::get_singleton()
            ->globalize_path("res://camera_calibration.yaml")
            .utf8()
            .get_data();
    auto pipeline =
        late::pipeline::make_pipeline<cv::Mat>()
            .attach<late::ImageRenderer>(113)
            .attach<late::MarkerDetection>(0.025, 0.25)
            .attach<late::MarkerRectification>()
            .attach<late::MarkerRecognition>(0.7, calinbrationPath)
            .attach<late::DataStorage>(0.1f)
            .attach<late::DataConversion>();

    while (!st.stop_requested()) {
      auto data = buffer->read();
      if (!data) continue;
      pipeline << *data;
      {
        std::lock_guard lock{mtx};
        pipeline >> output;
      }
    }
  });
};

auto MarkerManager::_exit_tree() -> void { m_worker.request_stop(); }
auto MarkerManager::_process(double delta) -> void {
  m_elapsedTime += delta;
  std::optional<std::vector<late::GodotMarker>> data;
  {
    std::lock_guard lock{m_mtx};
    data = std::move(m_output);
    m_output.reset();
  }
  if (!data) return;

  for (const auto& marker : *data) {
    int id = marker.m_typeId;
    godot::Vector3 position =
        godot::Vector3(marker.m_posX, marker.m_posY, marker.m_posZ);
    godot::Quaternion rotation(
        marker.m_quatX, marker.m_quatY, marker.m_quatZ, marker.m_quatW
    );

    update_marker(id, position, rotation);
  }

  for (auto i = m_markers.begin(); i != m_markers.end();) {
    int id = i->first;

    if (m_elapsedTime - m_lastSeenMarkers[id] > m_markerTimeout) {
        godot::UtilityFunctions::print(
            "Removing marker: ", id
        );

        i->second->queue_free();

        m_lastSeenMarkers.erase(id);
        i = m_markers.erase(i);
    }
    else {
        ++i;
    }
  }
}

auto MarkerManager::_bind_methods() -> void {
  godot::ClassDB::bind_method(
      godot::D_METHOD("update_marker", "id", "position", "rotation"),
      &MarkerManager::update_marker
  );

  godot::ClassDB::bind_method(
      godot::D_METHOD("get_depth"), &MarkerManager::get_depth
  );
  godot::ClassDB::bind_method(
      godot::D_METHOD("set_depth", "depth"), &MarkerManager::set_depth
  );
  ADD_PROPERTY(
      godot::PropertyInfo(godot::Variant::FLOAT, "depth"), "set_depth",
      "get_depth"
  );

  godot::ClassDB::bind_method(
      godot::D_METHOD("get_scale2d"), &MarkerManager::get_scale2d
  );
  godot::ClassDB::bind_method(
      godot::D_METHOD("set_scale2d", "scale"), &MarkerManager::set_scale2d
  );
  ADD_PROPERTY(
      godot::PropertyInfo(godot::Variant::FLOAT, "scale2d"), "set_scale2d",
      "get_scale2d"
  );

  godot::ClassDB::bind_method(
      godot::D_METHOD("get_spring"), &MarkerManager::get_spring
  );
  godot::ClassDB::bind_method(
      godot::D_METHOD("set_spring", "spring"), &MarkerManager::set_spring
  );
  ADD_PROPERTY(
      godot::PropertyInfo(
          godot::Variant::FLOAT, "spring", godot::PROPERTY_HINT_RANGE,
          "0.1,1,0.001"
      ),
      "set_spring", "get_spring"
  );
};

auto MarkerManager::create_marker(int markerId) -> godot::Node3D*{
    godot::String path = godot::String("res://Markers/Marker")
                       + godot::String::num_int64(markerId)
                       + ".tscn";
    godot::Ref<godot::PackedScene> scene = 
        godot::ResourceLoader::get_singleton() -> load(path);

    if (scene.is_null()) {
        godot::UtilityFunctions::printerr(
            "Could not load scene: ", path
        );
        return nullptr;
    }

    godot::Node3D* marker =
        godot::Object::cast_to<godot::Node3D>(scene->instantiate());

    if (marker == nullptr) {
        //instance->queue_free();
        return nullptr;
    }

    marker->set_name(
        godot::String("Marker_") + godot::String::num_int64(markerId)
    );

    add_child(marker);

    return marker;
};

auto MarkerManager::update_marker(
    int id, godot::Vector3 position, godot::Quaternion rotation
) -> void {
    m_lastSeenMarkers[id] = m_elapsedTime;

  if (!m_markers.contains(id)) {
    godot::Node3D* markerNode = create_marker(id);
    
    if (markerNode == nullptr) {
        return;
    }

    m_markers[id] = markerNode;
  }

  godot::Node3D* markerNode = m_markers[id];

  godot::Vector3 localOffset{0.0f, markerNode->get_scale().y / 2, 0.0f};

  markerNode->set_position(position);
  markerNode->translate_object_local(localOffset);
  markerNode->set_quaternion(rotation);
};
auto MarkerManager::get_scale2d() const -> double { return m_scale2d; }
auto MarkerManager::set_scale2d(double scale) -> void { m_scale2d = scale; }
auto MarkerManager::get_depth() const -> double { return m_depth; }
auto MarkerManager::set_depth(double depth) -> void { m_depth = depth; }
auto MarkerManager::get_spring() const -> double { return m_spring; }
auto MarkerManager::set_spring(double spring) -> void {
  m_spring = std::max(0.1, std::min(1.0, spring));
}

}  // namespace chess
