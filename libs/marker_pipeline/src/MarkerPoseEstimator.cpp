#include "MarkerPoseEstimator.hpp"

#include <cmath>
#include <opencv2/core.hpp>

namespace late {
auto MarkerPoseEstimator::estimate_pose(
    CandidateMarker& candidate, const cv::Size& frameSize, float focalLengthPx,
    float markerSize
) -> void {
  const auto& corners = candidate.m_corners;

  cv::Point2f center(0.0f, 0.0f);

  for (const auto& corner : corners) {
    center += corner;
  }

  center *= 0.25f;

  float totalLength = 0.0f;

  for (int i = 0; i < 4; ++i) {
    totalLength += cv::norm(corners[(i + 1) % 4] - corners[i]);
  }

  candidate.m_apperentSize = totalLength / 4.0f;

  if (focalLengthPx <= 0.0f || markerSize <= 0.0f ||
      candidate.m_apperentSize <= 0.0f || frameSize.width <= 0 ||
      frameSize.height <= 0) {
    return;
  }

  const float z = (focalLengthPx * markerSize) / candidate.m_apperentSize;

  const float cx = frameSize.width * 0.5f;
  const float cy = frameSize.height * 0.5f;

  const float x = (center.x - cx) * z / focalLengthPx;
  const float y = (center.y - cy) * z / focalLengthPx;

  candidate.m_markerPos         = cv::Point3f(x, y, z);
  candidate.m_estimatedDistance = z;

  const cv::Point2f edge = corners[1] - corners[0];

  candidate.m_markerRotation =
      cv::Vec3f(0.0f, 0.0f, std::atan2(edge.y, edge.x));
};
}  // namespace late
