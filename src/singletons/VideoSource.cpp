#include "VideoSource.hpp"

#include <godot_cpp/core/class_db.hpp>

namespace late {
auto VideoSource::_bind_methods() -> void {}
auto VideoSource::attach_output(std::shared_ptr<IBuffer<cv::Mat>> output)
    -> void {
  m_capture.attach_output(output);  // blocking
}
}  // namespace late
