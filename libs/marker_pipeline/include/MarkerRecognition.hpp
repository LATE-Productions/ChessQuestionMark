#pragma once

#include <map>
#include <limits>
#include "Marker.hpp"
#include "MarkerRectification.hpp"
#include "MarkerTemplates.hpp"

namespace late {
class MarkerRecognition {
  public:
  MarkerRecognition(double markerMatchPercentage);
  auto GetValidMarkers(const std::vector<CandidateMarker>& markers) -> std::vector<Marker>;

  private:
  auto DecodeMarker(const CandidateMarker& candidateMarker, Marker& destination) -> bool;
  auto HammingDistance(const cv::Mat& a, const cv::Mat b) -> int;
  auto RotateMat(const cv::Mat& mat, int k) -> cv::Mat;
  auto GetCenter(const CandidateMarker& candidateMarker) -> cv::Point3f;
  std::map<int, MarkerTemplate<6>> m_storedMarkers;
  double m_markerMatchPercentage;
};
}  // namespace late
