#include "MarkerRecognition.hpp"

#include <algorithm>
#include <array>
#include <opencv2/opencv.hpp>
#include <string>

#include "Marker.hpp"
#include "MarkerPoseEstimator.hpp"
#include "MarkerTemplates.hpp"

namespace late {
MarkerRecognition::MarkerRecognition(
    double markerMatchPercentage, const std::string& filePath
)
    : m_markerMatchPercentage(markerMatchPercentage),
      m_poseEstimator{filePath} {
  // Make m_storedMarkers here

  m_storedMarkers.insert({0, std::array<bool, 36>{0, 0, 0, 1, 1, 1, 
                                                  1, 0, 0, 0, 1, 1, 
                                                  1, 1, 0, 1, 1, 1,
                                                  0, 1, 1, 0, 0, 0, 
                                                  0, 0, 1, 0, 1, 0, 
                                                  1, 0, 0, 1, 1, 0}});

  m_storedMarkers.insert({1, std::array<bool, 36>{0, 0, 0, 0, 1, 1, 
                                                  1, 0, 1, 1, 1, 1, 
                                                  1, 0, 1, 1, 1, 0,
                                                  1, 0, 0, 0, 1, 1, 
                                                  1, 0, 0, 0, 1, 0, 
                                                  0, 1, 0, 0, 0, 1}});

  m_storedMarkers.insert({2, std::array<bool, 36>{0, 0, 0, 1, 0, 1, 
                                                  0, 1, 1, 0, 0, 1, 
                                                  0, 0, 0, 0, 0, 1,
                                                  1, 1, 1, 1, 1, 0, 
                                                  1, 0, 1, 0, 1, 1, 
                                                  0, 0, 1, 1, 0, 1}});
  
  m_storedMarkers.insert({3, std::array<bool, 36>{1, 1, 1, 0, 0, 1, 
                                                  0, 0, 0, 1, 1, 1, 
                                                  1, 0, 0, 1, 1, 1,
                                                  1, 0, 1, 0, 0, 1, 
                                                  1, 1, 0, 0, 1, 0, 
                                                  0, 1, 1, 0, 0, 0}});
};

auto MarkerRecognition::GetValidMarkers(
    const std::vector<CandidateMarker>& markers
) -> std::vector<Marker> {
  std::vector<Marker> validMarkers{};
  for (const auto& marker : markers) {
    if (marker.m_markerImage.cols - marker.m_markerImage.rows !=
        0) {  // Ignore non-square markers
      continue;
    }

    if (marker.m_markerImage.cols < 8) continue;

    Marker validMarker;
    if (DecodeMarker(marker, validMarker)) {
      validMarkers.push_back(validMarker);
    }
  }

  return validMarkers;
};

auto MarkerRecognition::DecodeMarker(
    const CandidateMarker& candidateMarker, Marker& destination
) -> bool {
  int bestDistance   = std::numeric_limits<int>::max();
  int secondDistance = std::numeric_limits<int>::max();

  int bestId       = -1;
  int bestRotation = 0;

  for (const auto& entry : m_storedMarkers) {
    const int id         = entry.first;
    auto referenceMarker = entry.second;

    std::array<cv::Mat, 4> markerRotations = {
        referenceMarker(
            candidateMarker.m_markerImage.cols,
            MarkerTemplate<6>::Rotation::NONE
        ),
        referenceMarker(
            candidateMarker.m_markerImage.cols,
            MarkerTemplate<6>::Rotation::ROTATE_90
        ),
        referenceMarker(
            candidateMarker.m_markerImage.cols,
            MarkerTemplate<6>::Rotation::ROTATE_180
        ),
        referenceMarker(
            candidateMarker.m_markerImage.cols,
            MarkerTemplate<6>::Rotation::ROTATE_270
        ),
    };

    int rotation = 0;
    for (const cv::Mat& candidate : markerRotations) {
      int distance = HammingDistance(candidate, candidateMarker.m_markerImage);
      // std::cout << "Hamming distance: " << distance << " of " <<
      // candidate.cols * candidate.rows  << " pixels" << std::endl;

      if (distance >= 0 && distance < bestDistance) {
        secondDistance = bestDistance;
        bestDistance   = distance;
        bestId         = id;
        bestRotation   = rotation;
      } else if (distance >= 0 && distance < secondDistance) {
        secondDistance = distance;
      }
      rotation += 90;
    }
  }

  int maxErrors = (candidateMarker.m_markerImage.cols *
                   candidateMarker.m_markerImage.rows) *
                  (1 - m_markerMatchPercentage);
  double dist   = maxErrors / static_cast<double>(bestDistance);
  // std::cout << "Max errors: " << maxErrors << " with " << bestDistance << "
  // errors dist: " << dist << std::endl;
  if (bestDistance < 0 || bestDistance > maxErrors) {
    return false;
  }

  if (secondDistance ==
      bestDistance) {  // Not rotation safe, not a valid marker
    return false;
  }

  destination.m_markerTypeId       = bestId;
  destination.m_rotationCorrection = bestRotation;
  destination.m_apperentSize       = candidateMarker.m_apperentSize;
  destination.m_markerImage        = candidateMarker.m_markerImage;
  destination.m_corners =
      CorrectCornerOrder(candidateMarker.m_corners, bestRotation);

  return m_poseEstimator.estimate_pose_calibrated(destination, 0.04f);
};

auto MarkerRecognition::HammingDistance(const cv::Mat& a, const cv::Mat b)
    -> int {
  if (a.size() != b.size()) return -1;

  cv::Mat difference;
  cv::compare(a, b, difference, cv::CMP_NE);

  return cv::countNonZero(difference);
};

auto MarkerRecognition::RotateMat(const cv::Mat& mat, int k) -> cv::Mat {
  cv::Mat result;

  switch (k) {
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
      cv::rotate(mat, result, cv::ROTATE_90_COUNTERCLOCKWISE);
      break;
  }

  return result;
};

auto MarkerRecognition::GetCenter(const CandidateMarker& candidateMarker)
    -> cv::Point3f {
  cv::Point3f center;
  center.x = candidateMarker.m_corners[0].x +
             ((double)candidateMarker.m_boundingBox.width / 2);
  center.y = candidateMarker.m_corners[0].y +
             ((double)candidateMarker.m_boundingBox.height / 2);
  center.z = 0;
  return center;
};

auto MarkerRecognition::process(CandidatePayload&& payload) -> FinalPayload {
  return {payload.m_frame, GetValidMarkers(payload.m_markers)};
};

auto MarkerRecognition::CorrectCornerOrder(
    const std::array<cv::Point2f, 4>& corners, int bestRotation
) -> std::array<cv::Point2f, 4> {
  std::array<cv::Point2f, 4> corrected{};

  const int shift = (bestRotation / 90) % 4;

  for (int i = 0; i < corners.size(); i++) {
    corrected[i] = corners[(i + shift) % 4];
  }

  std::cout << "bestRotation: " << bestRotation << '\n';
  std::cout << "shift: " << shift << '\n';

  return corrected;
};

auto MarkerRecognition::draw_markers(
    const cv::Mat& frame, const std::vector<Marker> markers
) -> cv::Mat {
  cv::Mat display_frame;
  cv::cvtColor(frame, display_frame, cv::COLOR_GRAY2BGR);

  for (const auto& marker : markers) {
    cv::Point2f center{0.0f, 0.0f};

    for (size_t i = 0; i < marker.m_corners.size(); i++) {
      cv::Point2f corner = marker.m_corners[i];
      cv::Point2f next   = marker.m_corners[(i + 1) % marker.m_corners.size()];
      cv::circle(display_frame, corner, 5, cv::Scalar(0, 0, 255), -1);

      cv::putText(
          display_frame, std::to_string(i), corner + cv::Point2f(5, -5),
          cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 0, 0), 2
      );

      cv::line(display_frame, corner, next, cv::Scalar(255, 255, 0), 2);

      center += corner;
    }

    center = center * 0.25f;

    cv::circle(display_frame, center, 5, cv::Scalar(0, 255, 0), -1);
  }

  return display_frame;
}
}  // namespace late
