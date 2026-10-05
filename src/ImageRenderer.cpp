#include "ImageRenderer.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

namespace late {

    auto ImageRenderer::getBlackWhiteImage(cv::Mat& frame) -> cv::Mat {
        frame = deNoise(frame); 
        
        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
        
        cv::threshold(frame, frame, 200, 255, cv::THRESH_BINARY);
        
        return frame;
    };
        
    auto ImageRenderer::deNoise(cv::Mat& frame) -> cv::Mat {
        //cv::fastNlMeansDenoisingColored(frame, frame);

        cv::Mat kernel = cv::getStructuringElement(
            cv::MORPH_ELLIPSE,
            cv::Size(3, 3)
        );

        cv::morphologyEx(frame, frame, cv::MORPH_OPEN, kernel); // Opening
        cv::morphologyEx(frame, frame, cv::MORPH_CLOSE, kernel); // Closing

        return frame;
    };
}