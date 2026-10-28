#include "MarkerManager.hpp"

#include <algorithm>
#include <godot_cpp/classes/box_mesh.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/core/print_string.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/callable_method_pointer.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <opencv2/core/mat.hpp>
#include <thread>
#include <vector>

#include "DataConversion.hpp"
#include "DataStorage.hpp"
#include "ImageRenderer.hpp"
#include "MarkerDetection.hpp"
#include "MarkerRecognition.hpp"
#include "MarkerRectification.hpp"
#include "pipeline/Components.hpp"

namespace chess {
auto MarkerManager::update_all_markers(godot::Ref<MarkerContainer> markers)
    -> void {
  if (markers.is_null()) return;

  godot::UtilityFunctions::print("Markers ", markers->data().size());
  for (const auto& marker : markers->data()) {
    int id = marker.m_instanceId;
    godot::Vector3 position =
        godot::Vector3(marker.m_posX, marker.m_posY, marker.m_posZ);
    godot::Vector3 rotation =
        godot::Vector3(marker.m_rotX, marker.m_rotY, marker.m_rotZ);

    update_marker(id, position, rotation);
  }
}

auto MarkerManager::_ready() -> void {
  if (godot::Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  m_source = godot::Object::cast_to<VideoSource>(
      godot::Engine::get_singleton()->get_singleton("VideoSource")
  );

  m_source->attach_output(m_buffer);

  m_updateAllMarkers = callable_mp(this, &MarkerManager::update_all_markers);

  m_worker = std::jthread([buffer    = m_buffer,
                           &callable = m_updateAllMarkers](std::stop_token st) {
    auto pipeline = late::pipeline::make_pipeline<cv::Mat>()
                        .attach<late::ImageRenderer>(113)
                        .attach<late::MarkerDetection>(0.025, 0.25)
                        .attach<late::MarkerRectification>()
                        .attach<late::MarkerRecognition>(0.85)
                        .attach<late::DataStorage>(100.0f)
                        .attach<late::DataConversion>();

    while (!st.stop_requested()) {
      auto data = buffer->read();
      if (!data) continue;
      pipeline << *data;
      std::optional<std::vector<late::GodotMarker>> output;
      pipeline >> output;
      if (!output) continue;
      godot::Ref<MarkerContainer> wrapper = memnew(MarkerContainer(*output));
      callable.call_deferred(wrapper);
    }
  });
};

auto MarkerManager::_exit_tree() -> void {}
auto MarkerManager::_process(double delta) -> void {}

auto MarkerManager::_bind_methods() -> void {
  godot::ClassDB::bind_method(
      godot::D_METHOD("update_marker", "id", "position", "rotation"),
      &MarkerManager::update_marker
  );

  godot::ClassDB::bind_method(
      godot::D_METHOD("update_all_markers", "markers"),
      &MarkerManager::update_all_markers
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

auto MarkerManager::update_marker(
    int id, godot::Vector3 position, godot::Vector3 rotation
) -> void {
  godot::Node3D* marker_node = nullptr;

  if (!m_markers.contains(id)) {
    marker_node = memnew(godot::Node3D);
    marker_node->set_name(
        godot::String("Marker_") + godot::String::num_int64(id)
    );

    add_child(marker_node);

    godot::MeshInstance3D* mesh_instance = memnew(godot::MeshInstance3D);
    godot::BoxMesh* mesh                 = memnew(godot::BoxMesh);

    mesh_instance->set_mesh(mesh);
    marker_node->add_child(mesh_instance);

    m_markers[id] = marker_node;
  }
  marker_node      = m_markers[id];
  float depth      = static_cast<float>(m_depth);
  float scale      = static_cast<float>(m_scale2d);
  auto current_pos = marker_node->get_position();
  auto new_pos     = current_pos.lerp(
      {depth * scale * position.x, depth * scale * position.y,
       (scale + depth) * position.z},
      m_spring
  );
  marker_node->set_position(new_pos);
  marker_node->set_rotation(rotation);
  godot::UtilityFunctions::print("Marker pos: ", position);
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
