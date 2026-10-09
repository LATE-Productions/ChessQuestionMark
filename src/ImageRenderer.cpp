#include "ImageRenderer.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

namespace late {

    auto ImageRenderer::getBlackWhiteImage(cv::Mat frame) -> cv::Mat {
        frame = deNoise(frame); 
        
        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
        
        cv::threshold(frame, frame, 175, 255, cv::THRESH_BINARY); // Threshoold may need to be changed for better resaults
        
        return frame;
    };
        
    auto ImageRenderer::deNoise(cv::Mat& frame) -> cv::Mat {
        cv::Mat kernel = cv::getStructuringElement(
            cv::MORPH_ELLIPSE,
            cv::Size(3, 3) // Numbers may need to be tweaked for better resaults
        );

        cv::morphologyEx(frame, frame, cv::MORPH_OPEN, kernel); // Opening removes white pixels in a group of black pixles
        cv::morphologyEx(frame, frame, cv::MORPH_CLOSE, kernel); // Closing removes black pixles in a group of white pixels

        return frame;
    };
}