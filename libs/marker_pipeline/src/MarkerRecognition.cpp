#include "MarkerRecognition.hpp"
#include <opencv2/opencv.hpp>

namespace late {
MarkerRecognition::MarkerRecognition(){
  // Make m_stored_markers here
};

auto MarkerRecognition::GetValidMarkers(const std::vector<Marker>& markers) -> std::vector<CandidateMarker>{
  std::vector<CandidateMarker> valid_markers{};
  for(const auto& marker : markers){
    CandidateMarker valid_marker;
    if (DecodeMarker(marker, valid_marker)){
      valid_markers.push_back(valid_marker);
    }
  }

  return valid_markers;
};

auto MarkerRecognition::DecodeMarker(const Marker& marker, CandidateMarker& destination) -> bool{
  int best_distance = std::numeric_limits<int>::max();
  int second_distance = std::numeric_limits<int>::max();

  int best_id = -1;
  int best_rotation = 0;

  for(const auto& entry : m_stored_markers){
    
    cv::Mat reference_matrix = entry.first;
    const int id = entry.second;
    
    if(std::abs(reference_matrix.cols - reference_matrix.rows) > m_marker_square_threshold){ // Ignore non-square markers
      continue;
    }
    
    cv::Mat resized_reference_marker;
    cv::resize(reference_matrix, resized_reference_marker, cv::Size(marker.markerImage.cols, marker.markerImage.rows));

    for (int rotation = 0; rotation <= 4; rotation++){
      cv::Mat candidate = RotateMat(reference_matrix, rotation);

      int distance = HammingDistance(candidate, marker.markerImage);

      if (distance < best_distance){
        second_distance = best_distance;
        best_distance = distance;
        best_id = id;
        best_rotation = rotation;
      }
      else if (distance < second_distance){
        second_distance = distance;
      }
    }
  }

  int max_errors = marker.markerImage.cols * marker.markerImage.rows * m_marker_match_percentage;
  if (best_id < 0 || best_distance > max_errors){
    return false;
  }

  if (second_distance == best_distance){ // Not rotation safe, not a valid marker
    return false;
  }

  // // TODO: Assign values to destination here
  // destination.type = 
  // destination.color = 
  // destination.position = 

  return true;
};

auto MarkerRecognition::HammingDistance(const cv::Mat& a, const cv::Mat b) -> int{
  if(a.size != b.size)
  return -1;

  cv::Mat difference;
  cv::compare(a, b, difference, cv::CMP_NE);

  return cv::countNonZero(difference);
};

auto MarkerRecognition::RotateMat(const cv::Mat& mat, int k) -> cv::Mat{
  cv::Mat result;

  switch(k){
    case 0:
      result = mat.clone();
      break;
    case 1:
      cv::rotate(mat, result, cv::ROTATE_90_CLOCKWISE);
      break;
    case 2:
      cv::rotate(mat, result, cv::ROTATE_180);
      break;
    case 3:
      cv::rotate(mat, result,
      cv::ROTATE_90_COUNTERCLOCKWISE);
    break;
  }

  return result;
};
}  // namespace late
