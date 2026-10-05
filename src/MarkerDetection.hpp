#pragma once

#include <opencv2/opencv.hpp>
#include "Marker.hpp"

namespace late {

    class MarkerDetection {
        public:
        
            auto detect_candidate_markers(cv::Mat& frame) -> std::vector<Marker>;

        protected:

        private:

            auto detect_corners(cv::Mat& frame) -> std::array<cv::Point2f, 4>;
            auto detect_rect(cv::Mat& frame) -> cv::Rect;

    };

}