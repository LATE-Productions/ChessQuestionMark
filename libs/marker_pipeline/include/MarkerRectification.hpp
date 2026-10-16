#pragma once

#include <opencv2/opencv.hpp>

#include "MarkerDetection.hpp"

namespace late {
class MarkerRectification {
    public:
    // Use cv::findhomography to rectify the marker image based on its corners
    // warpPerspective
    // getPerspectiveTransform

    auto rectify_markers(const cv::Mat& frame, std::vector<Marker>& markers) -> std::vector<cv::Mat> {
        
    }



    private:
    // Might need to add some private members or methods for internal use
}

} // namespace late