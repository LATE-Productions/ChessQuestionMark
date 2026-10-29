#include "CameraCalibration.hpp"

#include <iostream>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

CameraCalibration::CameraCalibration(cv::Size boardSize, float squareSize)
    : m_boardSize(boardSize), m_squareSize(squareSize) {};

auto CameraCalibration::calibrate(const std::vector<std::string>& imagePaths)
    -> bool {
  std::vector<std::vector<cv::Point3f>> objectPoints;
  std::vector<std::vector<cv::Point2f>> imagePoints;

  cv::Size imageSize;
  int validImages = 0;

  std::vector<cv::Point3f> boardPoints;

  for (int y = 0; y < m_boardSize.height; ++y) {
    for (int x = 0; x < m_boardSize.width; ++x) {
      boardPoints.emplace_back(x * m_squareSize, y * m_squareSize, 0.0f);
    }
  }

  for (const auto& path : imagePaths) {
    cv::Mat image = cv::imread(path);

    if (image.empty()) {
      std::cout << "Couldn't read: " << path << std::endl;
      continue;
    }

    if (validImages == 0) {
      imageSize = image.size();
    } else if (image.size() != imageSize) {
      std::cout << "Wrong image size: " << path << std::endl;
      continue;
    }

    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

    std::vector<cv::Point2f> corners;

    bool found = cv::findChessboardCorners(
        gray, m_boardSize, corners,
        cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE
    );

    if (!found) {
      std::cout << "Chessboard not found" << path << std::endl;
      continue;
    }

    cv::cornerSubPix(
        gray, corners, cv::Size(11, 11), cv::Size(-1, -1),
        cv::TermCriteria(
            cv::TermCriteria::EPS | cv::TermCriteria::COUNT, 30, 0.001
        )
    );

    imagePoints.push_back(corners);
    objectPoints.push_back(boardPoints);

    validImages++;

    std::cout << "Image passed: " << path << std::endl;
  }

  if (validImages < 5) {
    std::cout << "Too few passed images.\n";
    return false;
  }

  m_cameraMatrix = cv::Mat::eye(3, 3, CV_64F);

  m_distCoeffs = cv::Mat::zeros(8, 1, CV_64F);

  std::vector<cv::Mat> rvecs;
  std::vector<cv::Mat> tvecs;

  m_reprojectionError = calibrateCamera(
      objectPoints, imagePoints, imageSize, m_cameraMatrix, m_distCoeffs, rvecs,
      tvecs
  );

  std::cout << "\nCamera matrix:\n" << m_cameraMatrix << std::endl;

  std::cout << "\nDistortion coefficients:\n" << m_distCoeffs << std::endl;

  std::cout << "\nRMS reprojection error:\n"
            << m_reprojectionError << std::endl;

  return true;
};

auto CameraCalibration::save_calibration(const std::string& fileName) -> bool {
  if (m_cameraMatrix.empty() || m_distCoeffs.empty()) {
    std::cerr << "No calibration to save.\n";
    return false;
  }

  cv::FileStorage fs(fileName, cv::FileStorage::WRITE);

  if (!fs.isOpened()) {
    std::cerr << "Unable to create YAML-file. \n";
    return false;
  }

  fs << "camera_matrix" << m_cameraMatrix;
  fs << "distortion_coefficients" << m_distCoeffs;
  fs << "reprojection_error" << m_reprojectionError;

  fs.release();

  return true;
};

auto CameraCalibration::get_camera_matrix() const -> cv::Mat {
  return m_cameraMatrix;
};

auto CameraCalibration::get_dist_coeffs() const -> cv::Mat {
  return m_distCoeffs;
};