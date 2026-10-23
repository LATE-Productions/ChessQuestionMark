#pragma once

#include <limits>
#include <map>
#include <vector>

#include "Marker.hpp"
#include "MarkerRectification.hpp"
#include "MarkerTemplates.hpp"
#include "pipeline/Components.hpp"

namespace late {
class MarkerRecognition
    : public pipeline::IStage<CandidatePayload, FinalPayload> {
 public:
  MarkerRecognition(double markerMatchPercentage);
  auto GetValidMarkers(const std::vector<CandidateMarker>& markers)
      -> std::vector<Marker>;

  auto process(CandidatePayload&& payload) -> FinalPayload override;

 private:
  auto DecodeMarker(const CandidateMarker& candidateMarker, Marker& destination)
      -> bool;
  auto HammingDistance(const cv::Mat& a, const cv::Mat b) -> int;
  auto RotateMat(const cv::Mat& mat, int k) -> cv::Mat;
  auto GetCenter(const CandidateMarker& candidateMarker) -> cv::Point3f;
  std::map<int, MarkerTemplate<6>> m_storedMarkers;
  double m_markerMatchPercentage;
};
}  // namespace late
