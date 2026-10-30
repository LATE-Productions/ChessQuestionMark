#include "DataConversion.hpp"

#include <vector>

namespace late {
auto DataConversion::convert_to_markers(const std::vector<Marker>& markers)
    -> std::vector<GodotMarker> {
  std::vector<GodotMarker> output;
  for (const auto& marker : markers) {
    output.push_back(convert_marker(marker));
  }
  return output;
};

auto DataConversion::convert_marker(const Marker& marker) -> GodotMarker {
  GodotMarker output;

  output.m_typeId     = marker.m_markerTypeId;
  output.m_instanceId = marker.m_instanceId;
  output.m_posX       = marker.m_markerPos.x;
  output.m_posY       = -marker.m_markerPos.y;
  output.m_posZ       = -marker.m_markerPos.z;
  output.m_quatX      = marker.m_markerRotation[0];
  output.m_quatY      = marker.m_markerRotation[1];
  output.m_quatZ      = marker.m_markerRotation[2];
  output.m_quatW      = marker.m_markerRotation[3];

  return output;
};
auto DataConversion::process(std::vector<Marker>&& payload)
    -> std::vector<GodotMarker> {
  return convert_to_markers(payload);
}
}  // Namespace late
