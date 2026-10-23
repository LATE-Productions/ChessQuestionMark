#pragma once

#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/photo.hpp>

#include "pipeline/Components.hpp"

namespace late {
class ImageRenderer : public pipeline::IStage<cv::Mat, cv::Mat> {
 public:
  auto getBlackWhiteImage(cv::Mat& frame) -> void;
  auto process(cv::Mat&& input) -> cv::Mat override;

 private:
  auto deNoise(cv::Mat& frame) -> void;
};
}  // namespace late
