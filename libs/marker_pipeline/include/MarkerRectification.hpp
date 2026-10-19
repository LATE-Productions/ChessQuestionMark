#pragma once

#include <opencv2/opencv.hpp>

#include "MarkerDetection.hpp"

namespace late {
class MarkerRectification {
 public:
  auto rectify_markers(const cv::Mat& frame, std::vector<Marker>& markers)
      -> std::vector<Marker>;

 private:
  auto rectify_markerImage(const cv::Mat& frame, const Marker& marker)
      -> cv::Mat;
};

}  // namespace late
