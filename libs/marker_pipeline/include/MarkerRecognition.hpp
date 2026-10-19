#pragma once

#include <map>
#include <limits>
#include "Marker.hpp"
#include "MarkerRectification.hpp"

namespace late {
class MarkerRecognition {
  public:
  MarkerRecognition();
  auto GetValidMarkers(const std::vector<CandidateMarker>& markers) -> std::vector<Marker>;

  private:
  auto DecodeMarker(const CandidateMarker& candidate_marker, Marker& destination) -> bool;
  auto HammingDistance(const cv::Mat& a, const cv::Mat b) -> int;
  auto RotateMat(const cv::Mat& mat, int k) -> cv::Mat;
  auto GetCenter(const CandidateMarker& candidate_marker) -> cv::Point3f;
  std::map<int, cv::Mat> m_stored_markers;
  int m_marker_square_threshold = 5;
  float m_marker_match_percentage = 0.75f;
};
}  // namespace late
