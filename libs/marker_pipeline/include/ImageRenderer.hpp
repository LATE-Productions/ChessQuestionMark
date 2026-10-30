#pragma once

#include <cstdint>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/photo.hpp>

#include "Marker.hpp"
#include "pipeline/Components.hpp"

namespace late {
class ImageRenderer : public pipeline::IStage<cv::Mat, ImagePayload> {
 public:
  ImageRenderer();
  ImageRenderer(uint8_t m_threshold);
  auto getBinaryImage(cv::Mat& frame) -> void;
  auto getGrayScaleImage(cv::Mat& frame) -> void;
  auto process(cv::Mat&& input) -> ImagePayload override;

 private:
  auto deNoise(cv::Mat& frame) -> void;

 private:
  uint8_t m_threshold;
};
}  // namespace late
