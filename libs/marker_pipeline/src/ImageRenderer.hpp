#pragma once

#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/photo.hpp>

namespace late {
    class ImageRenderer {
        public: 
            auto getBlackWhiteImage(cv::Mat frame) -> cv::Mat;

        private:
            auto deNoise(cv::Mat& frame) -> cv::Mat;
    };
}