#include "MarkerRecognition.hpp"
#include <opencv2/opencv.hpp>

namespace late {
MarkerRecognition::MarkerRecognition(){
  // Make m_stored_markers here
  cv::Mat template0 = (cv::Mat_<uchar>(8, 8) << 
    0, 0,   0,   0,   0,   0,   0,   0,
    0, 0,   0,   0,   255, 255, 255, 0,
    0, 255, 0,   0,   0 ,  255, 255, 0,
    0, 255, 255, 0,   255, 255, 255, 0,
    0, 0,   255, 255, 0,   0,   0,   0,
    0, 0,   0,   255, 0,   255, 0,   0,
    0, 255, 0,   0,   255, 255, 0,   0,
    0, 0,   0,   0,   0,   0,   0,   0
  );

  m_stored_markers.insert({0, template0});

  cv::Mat template1 = (cv::Mat_<uchar>(8, 8) << 
    0, 0,   0,   0,   0,   0,   0,   0,
    0, 0,   0,   0,   0,   255, 255, 0,
    0, 255, 0,   255, 255, 255, 255, 0,
    0, 255, 0,   255, 255, 255, 0,   0,
    0, 255, 0,   0,   0,   255, 255, 0,
    0, 255, 0,   0,   0,   255, 0,   0,
    0, 0,   255, 0,   0,   0,   255, 0,
    0, 0,   0,   0,   0,   0,   0,   0
  );

  m_stored_markers.insert({1, template1});

  cv::Mat template2 = (cv::Mat_<uchar>(8, 8) << 
    0, 0,   0,   0,   0,   0,   0,   0,
    0, 0,   0,   0,   255, 0,   255, 0,
    0, 0,   255, 255, 0,   0,   255, 0,
    0, 0,   0,   0,   0,   0,   255, 0,
    0, 255, 255, 255, 255, 255, 0,   0,
    0, 255, 0,   255, 0,   255, 255, 0,
    0, 0,   0,   255, 255, 0,   255, 0,
    0, 0,   0,   0,   0,   0,   0,   0
  );  
  
  m_stored_markers.insert({2, template2});
};

auto MarkerRecognition::GetValidMarkers(const std::vector<CandidateMarker>& markers) -> std::vector<Marker>{
  std::vector<Marker> valid_markers{};
  for(const auto& marker : markers){
    Marker valid_marker;
    if (DecodeMarker(marker, valid_marker)){
      valid_markers.push_back(valid_marker);
    }
  }

  if(valid_markers.size() > 0){
    std::cout << "valid markers found: " << valid_markers.size() << std::endl;
    for (const auto& valid_marker : valid_markers){
      std::cout << "Valid marker ID: " << valid_marker.m_marker_id << std::endl;
    }
  }
  else{
    std::cout << "No valid markers" << std::endl;
  }

  return valid_markers;
};

auto MarkerRecognition::DecodeMarker(const CandidateMarker& candidate_marker, Marker& destination) -> bool{
  int best_distance = std::numeric_limits<int>::max();
  int second_distance = std::numeric_limits<int>::max();

  int best_id = -1;
  int best_rotation = 0;

  for(const auto& entry : m_stored_markers){
    
    const int id = entry.first;
    cv::Mat reference_matrix = entry.second;
    
    if(std::abs(reference_matrix.cols - reference_matrix.rows) > m_marker_square_threshold){ // Ignore non-square markers
      continue;
    }
    
    cv::Mat resized_reference_marker;
    cv::resize(reference_matrix, resized_reference_marker, cv::Size(candidate_marker.markerImage.cols, candidate_marker.markerImage.rows));

    for (int rotation = 0; rotation <= 4; rotation++){
      cv::Mat candidate = RotateMat(resized_reference_marker, rotation);

      int distance = HammingDistance(candidate, candidate_marker.markerImage);
      //std::cout << "Hamming distance: " << distance << " of " << candidate.cols * candidate.rows  << " pixels" << std::endl;

      if (distance >= 0 && distance < best_distance){
        second_distance = best_distance;
        best_distance = distance;
        best_id = id;
        best_rotation = rotation;
      }
      else if (distance >= 0 && distance < second_distance){
        second_distance = distance;
      }
    }
  }

  int max_errors = (candidate_marker.markerImage.cols * candidate_marker.markerImage.rows) * (1 - m_marker_match_percentage);
  double dist = max_errors / static_cast<double>(best_distance);
  std::cout << "Max errors: " << max_errors << " with " << best_distance << " errors dist: " << dist << std::endl;
  if (best_distance < 0 || best_distance > max_errors){
    return false;
  }

  if (second_distance == best_distance){ // Not rotation safe, not a valid marker
    return false;
  }

  destination.m_marker_id = best_id;
  destination.m_marker_pos = GetCenter(candidate_marker);
  destination.markerImage = candidate_marker.markerImage;

  return true;
};

auto MarkerRecognition::HammingDistance(const cv::Mat& a, const cv::Mat b) -> int{
  if(a.size() != b.size())
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

auto MarkerRecognition::GetCenter(const CandidateMarker& candidate_marker) -> cv::Point3f{
  cv::Point3f center;
  center.x = candidate_marker.m_corners[0].x + ((float)candidate_marker.m_bounding_box.width / 2);
  center.y = candidate_marker.m_corners[0].y + ((float)candidate_marker.m_bounding_box.height / 2);
  center.z = 0;
  return center;
};
}  // namespace late
