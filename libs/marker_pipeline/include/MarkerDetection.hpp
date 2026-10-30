#pragma once

#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

#include "Marker.hpp"
#include "pipeline/Components.hpp"

namespace late {

class MarkerDetection
    : public pipeline::IStage<ImagePayload, CandidatePayload> {
 public:
  MarkerDetection(double minContourThreshold, double maxContourThreshold);

  auto detect_candidate_markers(ImagePayload& imagePayload) -> std::vector<CandidateMarker>;
  static auto draw_markers(
      const cv::Mat& frame, const std::vector<CandidateMarker> markers
  ) -> cv::Mat;

  auto process(ImagePayload&& frame) -> CandidatePayload override;

 protected:
 private:
  auto detect_corners(const std::vector<cv::Point>& contour)
      -> std::array<cv::Point2f, 4>;
  auto order_corners_sums_diffs(const std::array<cv::Point2f, 4>& corners)
      -> std::array<cv::Point2f, 4>;
  auto order_corners_angles(const std::array<cv::Point2f, 4> & corners
  ) -> std::array<cv::Point2f, 4>;
  auto detect_rect(const std::vector<cv::Point>& contour) -> cv::Rect;
  auto equilateral_control(
      const std::vector<cv::Point>& contour, const double& threshold
  ) -> bool;

 private:
  double m_minContourThreshold;
  double m_maxContourThreshold;
};
}  // namespace late
