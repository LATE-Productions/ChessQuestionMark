#include "MarkerContainer.hpp"

#include <vector>

#include "Marker.hpp"
namespace chess {
MarkerContainer::MarkerContainer(std::vector<late::GodotMarker> data)
    : m_data(data) {}
auto MarkerContainer::_bind_methods() -> void {}
auto MarkerContainer::set(std::vector<late::GodotMarker> data) -> void {
  m_data = data;
};
auto MarkerContainer::data() const -> std::vector<late::GodotMarker> {
  return m_data;
}
}  // namespace chess
