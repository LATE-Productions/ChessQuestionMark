#include "CameraCalibration.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

int main(int argc, char* argv[]) {     
    //std::filesystem::path file = "\\libs\\marker_pipeline\\camera_calibration\\images\\0.jpeg";
   // std::cout << std::filesystem::absolute(file) << std::endl;
    
    CameraCalibration calibration(
        cv::Size(9, 6),
        25.0f
    );

    std::vector<std::string> imagePaths;

    for (int i = 0; i < 21; i++){
        imagePaths.emplace_back("libs\\marker_pipeline\\camera_calibration\\images\\" + std::to_string(i) + ".jpeg");
    }

    if (!calibration.calibrate(imagePaths)){
        std::cerr << "Calibration failed.\n";
        return 1;
    }

    if (!calibration.save_calibration("libs\\marker_pipeline\\camera_calibration\\camera_calibration.yaml")){
        return 1;
    }

    std::cout << "Calibration saved.\n";
    
    return 0; 
}
