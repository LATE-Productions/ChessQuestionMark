#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

#include "Marker.hpp"
#include "pipeline/Components.hpp"

namespace late {
class DataConversion
    : public pipeline::IStage<std::vector<Marker>, std::vector<GodotMarker>> {
 public:
  auto convert_to_markers(const std::vector<Marker>& markers)
      -> std::vector<GodotMarker>;

  auto process(std::vector<Marker>&& payload) -> std::vector<GodotMarker>;

 private:
  auto convert_marker(const Marker& marker) -> GodotMarker;
};
}  // namespace late
