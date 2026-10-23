#pragma once

#include "Marker.hpp"

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <unordered_map>

namespace chess{
class MarkerManager : public godot::Node3D{
    GDCLASS(MarkerManager, godot::Node3D)

    public:
    auto update_all_markers(const std::vector<late::GodotMarker>& markers) -> void;
    auto update_marker(int id, godot::Vector3 position, godot::Vector3 rotation) -> void;

    protected:
    static auto bind_methods() -> void;

    private:
    std::unordered_map<int, godot::Node3D*> m_markers;
};
} // Namespace chess