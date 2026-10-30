#include "MarkerRectification.hpp"

#include <algorithm>
#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>

namespace late {

auto MarkerRectification::rectify_markers(
    const cv::Mat& frame, std::vector<CandidateMarker>& markers
) -> std::vector<CandidateMarker> {
  for (auto& marker : markers) {
    marker.m_markerImage = rectify_markerImage(frame, marker);
  }
  return markers;
}

auto MarkerRectification::rectify_markerImage(
    const cv::Mat& frame, const CandidateMarker& marker
) -> cv::Mat {
  int rows     = marker.m_markerImage.rows;
  int cols     = marker.m_markerImage.cols;
  int destSize = std::max(rows, cols);
  if (destSize < 2) return {};
  const float last = static_cast<float>(destSize - 1);

  std::array<cv::Point2f, 4> dst_corners = {
      cv::Point2f(0, 0), cv::Point2f(last, 0), cv::Point2f(last, last),
      cv::Point2f(0, last)
  };

  cv::Mat homography =
      cv::getPerspectiveTransform(marker.m_corners, dst_corners);

  cv::Mat rectified_image;

  cv::warpPerspective(
      frame, rectified_image, homography, cv::Size{destSize, destSize}
  );

  return rectified_image;
}

auto MarkerRectification::process(CandidatePayload&& payload)
    -> CandidatePayload {
  payload.m_markers = rectify_markers(payload.m_frame, payload.m_markers);
  return payload;
}
}  // namespace late
