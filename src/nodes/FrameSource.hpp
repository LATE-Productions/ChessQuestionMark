#pragma once

#include <RingBuffer.hpp>
#include <atomic>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/node_path.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <memory>
#include <opencv2/core/mat.hpp>
#include <thread>

#include "singletons/VideoSource.hpp"
namespace late {

using VideoOutputBuffer = RingBuffer<cv::Mat, 4, OverflowPolicy::DROP>;

class FrameSource : public godot::TextureRect {
  GDCLASS(FrameSource, godot::TextureRect)
 public:
  auto _ready() -> void override;
  auto _exit_tree() -> void override;

 protected:
  static auto _bind_methods() -> void;

 private:
  auto defered_create_texture(godot::PackedByteArray, int width, int height)
      -> void;

 private:
  VideoSource* m_source = nullptr;
  godot::Callable m_createTexture;
  std::thread m_worker;
  std::atomic<bool> m_running{true};
  std::shared_ptr<VideoOutputBuffer> m_buffer =
      std::make_shared<VideoOutputBuffer>();

  godot::TypedArray<godot::NodePath> m_textureRects;
};
}  // namespace late
