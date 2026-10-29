#include "MarkerPoseEstimator.hpp"

#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>
#include <cmath>

namespace late {
cv::Mat MarkerPoseEstimator::m_cameraMatrix;
cv::Mat MarkerPoseEstimator::m_distCoeffs;

auto MarkerPoseEstimator::load_calibration(const std::string& fileName) -> bool{
  cv::FileStorage fs(fileName, cv::FileStorage::READ);

  if (!fs.isOpened()){
    return false;
  }

  fs["camera_matrix"] >> m_cameraMatrix;
  fs["distortion_coefficients"] >> m_distCoeffs;

  fs.release();

  return !m_cameraMatrix.empty() && !m_distCoeffs.empty();
};

auto MarkerPoseEstimator::estimate_pose(
    CandidateMarker& candidate, const cv::Size& frameSize, float focalLengthPx,
    float markerSize
) -> void {
  const auto& corners = candidate.m_corners;

  cv::Point2f center(0.0f, 0.0f);

  for (const auto& corner : corners) {
    center += corner;
  }

  center *= 0.25f;

  float totalLength = 0.0f;

  for (int i = 0; i < 4; ++i) {
    totalLength += cv::norm(corners[(i + 1) % 4] - corners[i]);
  }

  candidate.m_apperentSize = totalLength / 4.0f;

  if (focalLengthPx <= 0.0f || markerSize <= 0.0f ||
      candidate.m_apperentSize <= 0.0f || frameSize.width <= 0 ||
      frameSize.height <= 0) {
    return;
  }

  const float z = (focalLengthPx * markerSize) / candidate.m_apperentSize;

  const float cx = frameSize.width * 0.5f;
  const float cy = frameSize.height * 0.5f;

  const float x = (center.x - cx) * z / focalLengthPx;
  const float y = (center.y - cy) * z / focalLengthPx;

  candidate.m_markerPos         = cv::Point3f(x, y, z);
  candidate.m_estimatedDistance = z;

  const cv::Point2f edge = corners[1] - corners[0];

  candidate.m_markerRotation =
      cv::Vec3f(0.0f, 0.0f, std::atan2(edge.y, edge.x));
};

auto MarkerPoseEstimator::estimate_pose_calibrated(CandidateMarker& candidate, float markerSize) -> bool {
if (m_cameraMatrix.empty() || m_distCoeffs.empty() || markerSize <= 0.0f) {
  return false;
}

const float half = markerSize * 0.5f;

const std::vector<cv::Point3f> objectPoints = {
  {-half, -half, 0.0f},
  {half, -half, 0.0f},
  {half, half, 0.0f},
  {-half, half, 0.0f}
};

const std::vector<cv::Point2f> imagePoints(candidate.m_corners.begin(), candidate.m_corners.end());

cv::Mat rvec, tvec;

const bool success = cv::solvePnP(objectPoints, imagePoints, m_cameraMatrix, m_distCoeffs, rvec, tvec, false, cv::SOLVEPNP_ITERATIVE);

if (!success) {
  return false;
}

cv::Mat rotationMatrix;
cv::Rodrigues(rvec, rotationMatrix);
const cv::Matx33d S(
  1, 0, 0,
  0, -1, 0,
  0, 0, -1
);

cv::Matx33d R;
rotationMatrix.convertTo(rotationMatrix, CV_64F);

for (int row = 0; row < 3; ++row){
  for (int col = 0; col < 3; ++col){
    R(row, col) = rotationMatrix.at<double>(row, col);
  }
}

const cv::Matx33d rotGodot = (S * R * S).t();

candidate.m_markerPos = cv::Point3f(tvec.at<double>(0), tvec.at<double>(1), tvec.at<double>(2));
candidate.m_markerRotation = rotation_matrix_convertion_yxz(rotGodot);
candidate.m_estimatedDistance = static_cast<float>(cv::norm(candidate.m_markerPos));

return true;

};

auto MarkerPoseEstimator::rotation_matrix_convertion_yxz(const cv::Matx33d& R) -> cv::Vec3f{
  const double pitch = std::asin(
    std::clamp(-R(1, 2), -1.0, 1.0)
  );

  double yaw;
  double roll;

  if (std::abs(std::cos(pitch)) > 1e-6) {
    yaw = std::atan2(R(0, 2), R(2, 2));
    roll = std::atan2(R(1, 0), R(1, 1));
  }
  else{
    yaw = std::atan2(-R(2, 0), R(0, 0));
    roll = 0.0;
  }

  return cv::Vec3f(
    static_cast<float>(pitch),
    static_cast<float>(yaw),
    static_cast<float>(roll)
  );
};

}  // namespace late
