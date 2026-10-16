#pragma once

#include "Marker.hpp"

namespace late {
class MarkerRecognition {
  public:
  MarkerRecognition();
  auto GetValidMarkers(const std::vector<Marker>& markers) -> std::vector<CandidateMarker>;

  private:
  auto DecodeMarker(const Marker& marker) -> CandidateMarker;
  std::map<cv::Mat, int> m_stored_markers;
};
}  // namespace late
