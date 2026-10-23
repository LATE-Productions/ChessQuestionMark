#include "MarkerManager.hpp"

#include <godot_cpp/classes/box_mesh.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/core/print_string.hpp>
#include <godot_cpp/variant/callable_method_pointer.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
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
                        .attach<late::ImageRenderer>()
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

auto MarkerManager::_bind_methods() -> void {
  godot::ClassDB::bind_method(
      godot::D_METHOD("update_marker", "id", "position", "rotation"),
      &MarkerManager::update_marker
  );

  godot::ClassDB::bind_method(
      godot::D_METHOD("update_all_markers", "markers"),
      &MarkerManager::update_all_markers
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
  marker_node = m_markers[id];

  marker_node->set_position(position);
  marker_node->set_rotation(rotation);
  godot::UtilityFunctions::print("Marker pos: ", position);
};
}  // namespace chess
