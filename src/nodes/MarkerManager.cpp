#include "MarkerManager.hpp"

#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/box_mesh.hpp>

namespace chess{    
auto MarkerManager::update_all_markers(const std::vector<late::GodotMarker>& markers) -> void{
    for (const auto& marker : markers){
        int id = marker.m_instanceId;
        godot::Vector3 position = godot::Vector3(
            marker.m_posX,
            marker.m_posY,
            marker.m_posZ
        );
        godot::Vector3 rotation = godot::Vector3(
            marker.m_rotX,
            marker.m_rotY,
            marker.m_rotZ
        );

        update_marker(id, position, rotation);
    }
}

auto MarkerManager::bind_methods() -> void {
    godot::ClassDB::bind_method(
        godot::D_METHOD("update_marker", "id", "position", "rotation"),
        &MarkerManager::update_marker
    );
};

auto MarkerManager::update_marker(int id, godot::Vector3 position, godot::Vector3 rotation) -> void {    
    godot::Node3D* marker_node = nullptr;

    auto it = m_markers.find(id);

    if (it == m_markers.end()){
        marker_node = memnew(godot::Node3D);
        marker_node -> set_name(godot::String("Marker_") + godot::String::num_int64(id));

        add_child(marker_node);

        godot::MeshInstance3D* mesh_instance = memnew(godot::MeshInstance3D);
        godot::BoxMesh* mesh = memnew(godot::BoxMesh);

        mesh_instance->set_mesh(mesh);
        marker_node->add_child(mesh_instance);
        
        m_markers[id] = marker_node;
    }
    else {
        marker_node = it->second;
    }

    marker_node->set_position(position);
    marker_node->set_rotation(rotation);
};
}