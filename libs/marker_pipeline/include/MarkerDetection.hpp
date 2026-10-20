#pragma once

#include <opencv2/opencv.hpp>

#include "Marker.hpp"

namespace late {

class MarkerDetection {
 public:
  MarkerDetection(double minContourThreshold, double maxContourThreshold);

  auto detect_candidate_markers(cv::Mat& frame) -> std::vector<Marker>;
  auto draw_markers(const cv::Mat& frame, const std::vector<Marker> markers)
      -> cv::Mat;

 protected:
 private:
  auto detect_corners(const std::vector<cv::Point>& contour)
      -> std::array<cv::Point2f, 4>;
  auto order_corners(const std::array<cv::Point2f, 4>& corners)
      -> std::array<cv::Point2f, 4>;
  auto detect_rect(const std::vector<cv::Point>& contour) -> cv::Rect;
  auto equilateral_control(const std::vector<cv::Point>& contour, const double& threshold) -> bool;

 private:
  double m_minCountourThreshold;
  double m_maxContourThreshold;
};

}  // namespace late
