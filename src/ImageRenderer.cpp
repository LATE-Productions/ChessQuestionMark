#include "ImageRenderer.hpp"

namespace late {

    auto ImageRenderer::getBlackWhiteImage(cv::Mat frame) -> cv::Mat {
        cv::Mat black_white_frame;

        cv::cvtColor(frame, black_white_frame, cv::COLOR_BGR2GRAY);
        cv::threshold(black_white_frame, black_white_frame, 127, 255, cv::THRESH_BINARY);

        black_white_frame = deNoise(black_white_frame); // May want to de noise before making it binary

        return black_white_frame;
    };
        
    auto ImageRenderer::deNoise(cv::Mat frame) -> cv::Mat {
        cv::Mat de_noised_frame;

        cv::fastNlMeansDenoising(frame, de_noised_frame);

        return de_noised_frame;
    };
}