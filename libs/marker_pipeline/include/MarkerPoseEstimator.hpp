#pragma once

#include <opencv2/opencv.hpp>

#include "Marker.hpp"

namespace late {
namespace MarkerPoseEstimator {
auto estimate_pose(
    CandidateMarker& candidate, const cv::Size& frameSize, float focalLengthPx,
    float markerSize = 0.04f
) -> void;
}
}  // namespace late
