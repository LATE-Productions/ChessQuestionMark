#include "MarkerPoseEstimator.hpp"

#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>
#include <cmath>

namespace late {

  MarkerPoseEstimator::MarkerPoseEstimator() {
    #ifdef _WIN32
    std::string fileName =
        "libs\\marker_pipeline\\camera_calibration\\camera_calibration.yaml";
    #elif __unix__
      std::string fileName =
          "libs/marker_pipeline/camera_calibration/camera_calibration.yaml";
    #endif

    bool isLoaded = load_calibration(fileName);
    assert(isLoaded);
  };

auto MarkerPoseEstimator::load_calibration(const std::string& fileName) -> bool{
  cv::FileStorage fs(fileName, cv::FileStorage::READ);

  if (!fs.isOpened()){
    return false;
  }

  fs["camera_matrix"] >> m_cameraMatrix;
  fs["distortion_coefficients"] >> m_distCoeffs;

  
  fs.release();
  
  if (m_cameraMatrix.empty() || m_distCoeffs.empty()){
    return false;
  }


  const double scaleX = 1920.0 / 1600.0;
  const double scaleY = 1080.0 / 900.0;
  
  m_cameraMatrix.at<double>(0, 0) *= scaleX; // fx
  m_cameraMatrix.at<double>(0, 2) *= scaleX; // cx
  
  m_cameraMatrix.at<double>(1, 1) *= scaleY; // fy
  m_cameraMatrix.at<double>(1, 2) *= scaleY; // cy

  return true;
};

// auto MarkerPoseEstimator::estimate_pose(
//     CandidateMarker& candidate, const cv::Size& frameSize, float focalLengthPx,
//     float markerSize
// ) -> void {
//   const auto& corners = candidate.m_corners;

//   cv::Point2f center(0.0f, 0.0f);

//   for (const auto& corner : corners) {
//     center += corner;
//   }

//   center *= 0.25f;

//   float totalLength = 0.0f;

//   for (int i = 0; i < 4; ++i) {
//     totalLength += cv::norm(corners[(i + 1) % 4] - corners[i]);
//   }

//   candidate.m_apperentSize = totalLength / 4.0f;

//   if (focalLengthPx <= 0.0f || markerSize <= 0.0f ||
//       candidate.m_apperentSize <= 0.0f || frameSize.width <= 0 ||
//       frameSize.height <= 0) {
//     return;
//   }

//   const float z = (focalLengthPx * markerSize) / candidate.m_apperentSize;

//   const float cx = frameSize.width * 0.5f;
//   const float cy = frameSize.height * 0.5f;

//   const float x = (center.x - cx) * z / focalLengthPx;
//   const float y = (center.y - cy) * z / focalLengthPx;

//   candidate.m_markerPos         = cv::Point3f(x, y, z);
//   candidate.m_estimatedDistance = z;

//   const cv::Point2f edge = corners[1] - corners[0];

//   candidate.m_markerRotation =
//       cv::Vec3f(0.0f, 0.0f, std::atan2(edge.y, edge.x));
// };

auto MarkerPoseEstimator::estimate_pose_calibrated(Marker& marker, float markerSize) -> bool {
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

const std::vector<cv::Point2f> imagePoints(marker.m_corners.begin(), marker.m_corners.end());

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

const cv::Matx33d rotGodot = S * R * S;

marker.m_markerPos = cv::Point3f(static_cast<float>(tvec.at<double>(0)), static_cast<float>(tvec.at<double>(1)), static_cast<float>(tvec.at<double>(2)));
marker.m_markerRotation = rotation_matrix_conversion_quat(rotGodot);
marker.m_estimatedDistance = static_cast<float>(cv::norm(marker.m_markerPos));

return true;

};

auto MarkerPoseEstimator::rotation_matrix_conversion_quat(const cv::Matx33d& R) -> cv::Vec4f{
  double x, y, z, w;

  const double trace = R(0, 0) + R(1, 1) + R(2, 2);

  if (trace > 0.0) {
    const double s = 2.0 * std::sqrt(trace + 1.0);

    w = 0.25 * s;
    x = (R(2, 1) - R(1, 2)) / s;
    y = (R(0, 2) - R(2, 0)) / s;
    z = (R(1, 0) - R(0, 1)) / s;
  } else if (R(0, 0) > R(1, 1) && R(0, 0) > R(2, 2)) {
    const double s = 2.0 * std::sqrt(
      1.0 + R(0, 0) - R(1, 1) - R(2, 2)
    );

      w = (R(2, 1) - R(1, 2)) / s;
        x = 0.25 * s;
        y = (R(0, 1) + R(1, 0)) / s;
        z = (R(0, 2) + R(2, 0)) / s;
    } else if (R(1, 1) > R(2, 2)) {
        const double s = 2.0 * std::sqrt(
            1.0 + R(1, 1) - R(0, 0) - R(2, 2)
        );

        w = (R(0, 2) - R(2, 0)) / s;
        x = (R(0, 1) + R(1, 0)) / s;
        y = 0.25 * s;
        z = (R(1, 2) + R(2, 1)) / s;
    } else {
        const double s = 2.0 * std::sqrt(
            1.0 + R(2, 2) - R(0, 0) - R(1, 1)
        );

        w = (R(1, 0) - R(0, 1)) / s;
        x = (R(0, 2) + R(2, 0)) / s;
        y = (R(1, 2) + R(2, 1)) / s;
        z = 0.25 * s;
    }

  return cv::Vec4f(
    static_cast<float>(x),
    static_cast<float>(y),
    static_cast<float>(z),
    static_cast<float>(w)
  );
};

}  // namespace late
