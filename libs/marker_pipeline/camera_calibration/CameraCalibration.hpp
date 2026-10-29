#pragma once

#include <opencv2/core.hpp>
#include <string>
#include <vector>

class CameraCalibration {
 public:
  CameraCalibration(cv::Size boardSize, float squareSize);

  auto calibrate(const std::vector<std::string>& imagePaths) -> bool;

  auto save_calibration(const std::string& fileName) -> bool;

  auto get_camera_matrix() const -> cv::Mat;

  auto get_dist_coeffs() const -> cv::Mat;

 private:
  cv::Size m_boardSize;
  float m_squareSize;

  cv::Mat m_cameraMatrix;
  cv::Mat m_distCoeffs;

  double m_reprojectionError = -1.0;
};
