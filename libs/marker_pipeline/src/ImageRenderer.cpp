#include "ImageRenderer.hpp"

#include <cstdint>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

namespace late {
ImageRenderer::ImageRenderer(uint8_t threshold) : m_threshold(threshold) {}
ImageRenderer::ImageRenderer() : m_threshold(127) {}

auto ImageRenderer::getBinaryImage(cv::Mat& frame) -> void {
  // this->deNoise(frame);

  //cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);

  cv::adaptiveThreshold(
      frame,                      // Input image
      frame,                      // Output image
      255,                        // Maximum output value
      cv::ADAPTIVE_THRESH_GAUSSIAN_C, // Adaptive method
      cv::THRESH_BINARY,          // Threshold type
      31,                         // Block size (must be odd and > 1)
      2                           // Constant subtracted from the local mean
  );

  // cv::threshold(
  //     frame, frame, m_threshold, 255, cv::THRESH_BINARY
  // );  // Threshoold may need to be changed for better resaults
};

auto ImageRenderer::getGrayScaleImage(cv::Mat& frame) -> void {
  cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
};

auto ImageRenderer::deNoise(cv::Mat& frame) -> void {
  cv::Mat kernel = cv::getStructuringElement(
      cv::MORPH_ELLIPSE,
      cv::Size(1, 1)  // Numbers may need to be tweaked for better resaults
  );

  cv::morphologyEx(
      frame, frame, cv::MORPH_OPEN, kernel
  );  // Opening removes white pixels in a group of black pixles
  cv::morphologyEx(
      frame, frame, cv::MORPH_CLOSE, kernel
  );  // Closing removes black pixles in a group of white pixels
};

auto ImageRenderer::process(cv::Mat&& data) -> ImagePayload {
  getGrayScaleImage(data);
  auto greyscale = data.clone();
  getBinaryImage(data);
  return {data, greyscale};
};
}  // namespace late
