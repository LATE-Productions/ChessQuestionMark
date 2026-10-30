#include "FrameSource.hpp"

#include <cassert>
#include <cstddef>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/variant/callable_method_pointer.hpp>
#include <godot_cpp/variant/node_path.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <thread>

namespace chess {
using godot::Image;
using godot::ImageTexture;
using godot::PackedByteArray;
using godot::Ref;
using godot::TextureRect;

auto FrameSource::_ready() -> void {
  if (godot::Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  //set_expand_mode(TextureRect::EXPAND_KEEP_SIZE);
  //set_stretch_mode(TextureRect::STRETCH_KEEP_ASPECT_COVERED);
  //set_anchors_preset(godot::Control::PRESET_FULL_RECT, true);

  m_source = godot::Object::cast_to<VideoSource>(
      godot::Engine::get_singleton()->get_singleton("VideoSource"));

  m_source->attach_output(m_buffer);
  m_createTexture = callable_mp(this, &FrameSource::defered_create_texture);
  m_worker = std::thread([&running = m_running, &buffer = m_buffer,
                          &callable = m_createTexture]() {
    cv::Mat frame;
    while (running) {
      auto data = buffer->read();
      if (data) {
        cv::Mat rgb_frame;
        cv::cvtColor(data.value(), rgb_frame, cv::COLOR_BGR2RGB);

        size_t image_size = rgb_frame.total() * rgb_frame.elemSize();
        PackedByteArray bytes;
        bytes.resize(image_size);
        std::memcpy(bytes.ptrw(), rgb_frame.data, image_size);
        callable.call_deferred(bytes, rgb_frame.cols, rgb_frame.rows);
      }
    }
  });
}

auto FrameSource::_bind_methods() -> void {}

auto FrameSource::_exit_tree() -> void {
  m_running = false;
  if (m_worker.joinable()) m_worker.join();
}

auto FrameSource::defered_create_texture(godot::PackedByteArray bytes,
                                         int width, int height) -> void {
  Ref<Image> image =
      Image::create_from_data(width, height, false, Image::FORMAT_RGB8, bytes);

  Ref<ImageTexture> texture = ImageTexture::create_from_image(image);
  set_texture(texture);
}

}  // namespace chess
