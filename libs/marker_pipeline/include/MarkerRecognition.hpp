#pragma once

#include <map>
#include <limits>
#include "Marker.hpp"
#include "MarkerRectification.hpp"

namespace late {
class MarkerRecognition {
  public:
  MarkerRecognition();
  auto GetValidMarkers(const std::vector<Marker>& markers) -> std::vector<CandidateMarker>;

  private:
  auto DecodeMarker(const Marker& marker, CandidateMarker& destination) -> bool;
  auto HammingDistance(const cv::Mat& a, const cv::Mat b) -> int;
  auto RotateMat(const cv::Mat& mat, int k) -> cv::Mat;
  std::map<cv::Mat, int> m_stored_markers;
  int m_marker_square_threshold = 5;
  float m_marker_match_percentage = 0.85f;
};
}  // namespace late
