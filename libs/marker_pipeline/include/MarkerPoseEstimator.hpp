#pragma once

#include <opencv2/opencv.hpp>

#include "Marker.hpp"

namespace late {
class IPoseEstimator {
  virtual auto estimate_pose(
      CandidateMarker& candidate, float markerSize = 0.04f
  ) -> bool                 = 0;
  virtual ~IPoseEstimator() = default;
};

class MarkerPoseEstimator {
 public:
  MarkerPoseEstimator();


  auto estimate_pose(
      CandidateMarker& candidate, const cv::Size& frameSize,
      float focalLengthPx, float markerSize = 0.04f
  ) -> void;

  auto estimate_pose_calibrated(
      Marker& marker, float markerSize = 0.04f
  ) -> bool;

  private:
    auto rotation_matrix_conversion_quat(const cv::Matx33d& R) -> cv::Vec4f;
    auto load_calibration(const std::string& fileName) -> bool;
  private:
   cv::Mat m_cameraMatrix;
   cv::Mat m_distCoeffs;
};
}  // namespace late
