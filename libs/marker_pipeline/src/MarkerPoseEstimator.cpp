#include "MarkerPoseEstimator.hpp"

#include <cmath>
#include <opencv2/core.hpp>

namespace late {
auto MarkerPoseEstimator::estimate_pose(
    CandidateMarker& candidate, float focalLengthPx, float markerSize
) -> void {
  const auto& corners = candidate.m_corners;

  cv::Point2f center(0.0f, 0.0f);

  for (const auto& corner : corners) {
    center += corner;
  }

  center *= 0.25f;

  candidate.m_markerPos.x = center.x;
  candidate.m_markerPos.y = center.y;
  candidate.m_markerPos.z = 0.0f;

  const cv::Point2f edge = corners[1] - corners[0];

  candidate.m_markerRotation =
      cv::Vec3f(0.0f, 0.0f, std::atan2(edge.y, edge.x));

  float totalLength = 0.0f;

  for (int i = 0; i < 4; ++i) {
    totalLength += cv::norm(corners[(i + 1) % 4] - corners[i]);
  }

  candidate.m_apperentSize = totalLength / 4.0f;

  if (focalLengthPx > 0.0f && candidate.m_apperentSize > 0.0f) {
    candidate.m_estimateDistance =
        (focalLengthPx * markerSize) / candidate.m_apperentSize;
  }
};
}  // namespace late