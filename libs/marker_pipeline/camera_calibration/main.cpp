#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "CameraCalibration.hpp"

int main(int argc, char* argv[]) {
  // std::filesystem::path file =
  // "\\libs\\marker_pipeline\\camera_calibration\\images\\0.jpeg";
  // std::cout << std::filesystem::absolute(file) << std::endl;

  CameraCalibration calibration(cv::Size(9, 6), 25.0f);

  std::vector<std::string> imagePaths;

  for (int i = 0; i < 27; i++) {
#ifdef _WIN32
    imagePaths.emplace_back("images\\" + std::to_string(i) + ".jpeg");
#elif __unix__
    imagePaths.emplace_back("images/" + std::to_string(i) + ".jpeg");
#endif  // _WIN32
  }

  if (!calibration.calibrate(imagePaths)) {
    std::cerr << "Calibration failed.\n";
    return 1;
  }

  if (!calibration.save_calibration("camera_calibration.yaml")) {
    return 1;
  }

  std::cout << "Calibration saved.\n";

  return 0;
}
