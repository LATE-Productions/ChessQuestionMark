#pragma once

#include <opencv2/opencv.hpp>

#include "Marker.hpp"

namespace late {
namespace MarkerPoseEstimator {
auto estimate_pose(CandidateMarker& candidate, float focalLengthPx, float markerSize = 40.0f) -> void;
}
}  // namespace late