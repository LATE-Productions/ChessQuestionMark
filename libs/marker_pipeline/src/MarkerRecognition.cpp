#include "MarkerRecognition.hpp"

namespace late {
MarkerRecognition::MarkerRecognition(){
  // Make m_stored_markers here
};

auto MarkerRecognition::GetValidMarkers(const std::vector<Marker>& markers) -> std::vector<CandidateMarker>{
  std::vector<CandidateMarker> valid_markers{};
  for(const auto& marker : markers){
    CandidateMarker valid_marker = DecodeMarker(marker);
    valid_markers.push(valid_marker);
  }
};

auto MarkerRecognition::DecodeMarker(const Marker& marker) -> CandidateMarker{
  for(const auto& entry : m_stored_markers){
    cv::Mat reference = entery.first;
    const int id = entry.second;

    reference.resize();
  }
};
}  // namespace late
