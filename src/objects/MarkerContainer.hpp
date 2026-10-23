#pragma once
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <vector>

#include "Marker.hpp"
namespace chess {
class MarkerContainer : public godot::RefCounted {
  GDCLASS(MarkerContainer, godot::RefCounted)
 public:
  MarkerContainer() = default;
  MarkerContainer(std::vector<late::GodotMarker> data);
  auto data() const -> std::vector<late::GodotMarker>;
  auto set(std::vector<late::GodotMarker> data) -> void;

 protected:
  static auto _bind_methods() -> void;

 private:
  std::vector<late::GodotMarker> m_data;
};
}  // namespace chess
