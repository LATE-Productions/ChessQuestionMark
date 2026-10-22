#include "MarkerRecognition.hpp"
#include "MarkerTemplates.hpp"
#include <array>
#include <opencv2/opencv.hpp>

namespace late {
MarkerRecognition::MarkerRecognition(double markerMatchPercentage)
    : m_markerMatchPercentage(markerMatchPercentage){
  // Make m_storedMarkers here

  m_storedMarkers.insert({0, std::array<bool, 36>{
    0, 0, 0, 1, 1, 1,
    1, 0, 0, 0, 1, 1,
    1, 1, 0, 1, 1, 1,
    0, 1, 1, 0, 0, 0,
    0, 0, 1, 0, 1, 0,
    1, 0, 0, 1, 1, 0,
 }});

  m_storedMarkers.insert({1, std::array<bool, 36>{
    0, 0, 0, 0, 1, 1,
    1, 0, 1, 1, 1, 1,
    1, 0, 1, 1, 1, 0,  
    1, 0, 0, 0, 1, 1,
    1, 0, 0, 0, 1, 0,  
    0, 1, 0, 0, 0, 1
  }});
  
  m_storedMarkers.insert({2, std::array<bool, 36>{
    0, 0, 0, 1, 0, 1,
    0, 1, 1, 0, 0, 1,
    0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 0,  
    1, 0, 1, 0, 1, 1,
    0, 0, 1, 1, 0, 1
  }});
};

auto MarkerRecognition::GetValidMarkers(const std::vector<CandidateMarker>& markers) -> std::vector<Marker>{
  std::vector<Marker> validMarkers{};
  for(const auto& marker : markers){
    if(marker.markerImage.cols - marker.markerImage.rows != 0){ // Ignore non-square markers
      continue;
    }

    if(marker.markerImage.cols < 8) continue;

    Marker validMarker;
    if (DecodeMarker(marker, validMarker)){
      validMarkers.push_back(validMarker);
    }
  }

  if(validMarkers.size() > 0){
    std::cout << "valid markers found: " << validMarkers.size() << std::endl;
    for (const auto& validMarker : validMarkers){
      std::cout << "Valid marker ID: " << validMarker.m_markerId << std::endl;
    }
  }
  else{
    std::cout << "No valid markers" << std::endl;
  }

  return validMarkers;
};

auto MarkerRecognition::DecodeMarker(const CandidateMarker& candidateMarker, Marker& destination) -> bool{
  int bestDistance = std::numeric_limits<int>::max();
  int secondDistance = std::numeric_limits<int>::max();

  int bestId = -1;
  int bestRotation = 0;

  for(const auto& entry : m_storedMarkers){
    
    const int id = entry.first;
    auto referenceMarker = entry.second;
    
    std::array<cv::Mat, 4> markerRotations = {
      referenceMarker(candidateMarker.markerImage.cols, MarkerTemplate<6>::Rotation::NONE),
      referenceMarker(candidateMarker.markerImage.cols, MarkerTemplate<6>::Rotation::ROTATE_90),
      referenceMarker(candidateMarker.markerImage.cols, MarkerTemplate<6>::Rotation::ROTATE_180),
      referenceMarker(candidateMarker.markerImage.cols, MarkerTemplate<6>::Rotation::ROTATE_270),
    };

    int rotation = 0;
    for (const cv::Mat& candidate: markerRotations){ 
      int distance = HammingDistance(candidate, candidateMarker.markerImage);
      //std::cout << "Hamming distance: " << distance << " of " << candidate.cols * candidate.rows  << " pixels" << std::endl;

      if (distance >= 0 && distance < bestDistance){
        secondDistance = bestDistance;
        bestDistance = distance;
        bestId = id;
        bestRotation = rotation;
      }
      else if (distance >= 0 && distance < secondDistance){
        secondDistance = distance;
      }
      rotation += 90;
    }
  }

  int maxErrors = (candidateMarker.markerImage.cols * candidateMarker.markerImage.rows) * (1 - m_markerMatchPercentage);
  double dist = maxErrors / static_cast<double>(bestDistance);
  //std::cout << "Max errors: " << maxErrors << " with " << bestDistance << " errors dist: " << dist << std::endl;
  if (bestDistance < 0 || bestDistance > maxErrors){
    return false;
  }

  if (secondDistance == bestDistance){ // Not rotation safe, not a valid marker
    return false;
  }

  destination.m_markerId = bestId;
  //destination.m_markerPos = GetCenter(candidateMarker);
  destination.m_markerPos = candidateMarker.m_markerPos;
  destination.m_markerRotation = candidateMarker.m_markerRotation;
  destination.m_apperentSize = candidateMarker.m_apperentSize;
  destination.markerImage = candidateMarker.markerImage;

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

auto MarkerRecognition::GetCenter(const CandidateMarker& candidateMarker) -> cv::Point3f{
  cv::Point3f center;
  center.x = candidateMarker.m_corners[0].x + ((double)candidateMarker.m_boundingBox.width / 2);
  center.y = candidateMarker.m_corners[0].y + ((double)candidateMarker.m_boundingBox.height / 2);
  center.z = 0;
  return center;
};
}  // namespace late
