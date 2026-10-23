#pragma once

#include "Marker.hpp"

#include <opencv2/opencv.hpp>

namespace late{
class DataConversion{
    public:
    auto convert_to_markers(const std::vector<Marker>& markers) -> std::vector<GodotMarker>;

    private:
    auto convert_marker(const Marker& marker) -> GodotMarker;
};
}  // namespace late