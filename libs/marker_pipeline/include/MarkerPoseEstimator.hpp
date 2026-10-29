#pragma once

#include <opencv2/opencv.hpp>

#include "Marker.hpp"

namespace late {
class MarkerPoseEstimator {
 public:
  static auto load_calibration(const std::string& fileName) -> bool;

  static auto estimate_pose(
      CandidateMarker& candidate, const cv::Size& frameSize,
      float focalLengthPx, float markerSize = 0.04f
  ) -> void;

  static auto estimate_pose_calibrated(
      Marker& marker, float markerSize = 0.04f
  ) -> bool;

  private:
    static auto rotation_matrix_conversion_quat(const cv::Matx33d& R) -> cv::Vec4f;

  static cv::Mat m_cameraMatrix;
  static cv::Mat m_distCoeffs;
};
}  // namespace late
