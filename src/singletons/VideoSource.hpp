#pragma once
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <memory>
#include <opencv2/core/mat.hpp>

#include "pipelines/CameraCapture.hpp"
#include "pipelines/RingBuffer.hpp"

namespace late {
class VideoSource : public godot::Object {
  GDCLASS(VideoSource, godot::Object)
 public:
  auto attach_output(std::shared_ptr<IBuffer<cv::Mat>> output) -> void;

 protected:
  static auto _bind_methods() -> void;

 private:
  CameraCapture m_capture{};
};
}  // namespace late
