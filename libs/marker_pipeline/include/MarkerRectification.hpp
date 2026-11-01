#pragma once

#include <opencv2/opencv.hpp>
#include <utility>
#include <vector>

#include "Marker.hpp"
#include "MarkerDetection.hpp"

namespace late {
class MarkerRectification
    : public pipeline::IStage<CandidatePayload, CandidatePayload> {
 public:
  auto rectify_markers(
      const cv::Mat& frame, std::vector<CandidateMarker>& markers
  ) -> std::vector<CandidateMarker>;

  auto process(CandidatePayload&& payload) -> CandidatePayload override;

  auto rectify_markerImage(const cv::Mat& frame, const CandidateMarker& marker)
      -> cv::Mat;
};

}  // namespace late
