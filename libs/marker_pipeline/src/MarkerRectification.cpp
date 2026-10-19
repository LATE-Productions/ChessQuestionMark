#include "MarkerRectification.hpp"

#include <algorithm>
#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>

namespace late {

auto MarkerRectification::rectify_markers(
    const cv::Mat& frame, std::vector<Marker>& markers
) -> std::vector<Marker> {
  for (auto& marker : markers) {
    marker.markerImage = rectify_markerImage(frame, marker);
  }
  return markers;
}

auto MarkerRectification::rectify_markerImage(
    const cv::Mat& frame, const Marker& marker
) -> cv::Mat {
  int rows                               = marker.markerImage.rows - 1;
  int cols                               = marker.markerImage.cols - 1;
  int destSize                           = std::max(rows, cols);
  std::array<cv::Point2f, 4> dst_corners = {
      cv::Point2f(0, 0), cv::Point2f(destSize, 0),
      cv::Point2f(destSize, destSize), cv::Point2f(0, destSize)
  };

  cv::Mat homography =
      cv::getPerspectiveTransform(marker.m_corners, dst_corners);

  cv::Mat rectified_image;

  cv::warpPerspective(
      frame, rectified_image, homography, cv::Size{destSize, destSize}
  );

  return rectified_image;
}
}  // namespace late
