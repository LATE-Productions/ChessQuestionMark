#include "main.hpp"

#include <cstddef>
#include <cstring>
#include <godot_cpp/classes/canvas_layer.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/texture.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <string>

#include "MarkerDetection.hpp"

namespace chess {
using godot::Image;
using godot::ImageTexture;
using godot::PackedByteArray;
using godot::Ref;

using late::CandidateMarker;

auto Main::_ready() -> void {
  std::string video_canvas_name = "VideoCanvasLayer";
  if (!has_node(video_canvas_name.c_str())) {
    m_canvas_layer = memnew(godot::CanvasLayer);
    m_canvas_layer->set_name(video_canvas_name.c_str());
    m_canvas_layer->set_layer(-1);
    this->add_child(m_canvas_layer);
  } else {
    m_canvas_layer = get_node<godot::CanvasLayer>(video_canvas_name.c_str());
  }

  std::string video_display_name = "VideoDisplay";

  if (!has_node(video_display_name.c_str())) {
    m_image_display = memnew(godot::TextureRect);
    m_image_display->set_name(video_display_name.c_str());
    m_image_display->set_anchors_preset(godot::Control::PRESET_FULL_RECT);
    m_image_display->set_stretch_mode(
        godot::TextureRect::STRETCH_KEEP_ASPECT_COVERED
    );
    m_canvas_layer->add_child(m_image_display);
  } else {
    m_image_display = get_node<godot::TextureRect>(video_display_name.c_str());
  }

  if (godot::Engine::get_singleton()->is_editor_hint()) {
    Node* edited_root = get_tree()->get_edited_scene_root();

    if (edited_root) {
      m_canvas_layer->set_owner(edited_root);
      m_image_display->set_owner(edited_root);
    }
  } else {
    m_video_capture = std::make_unique<cv::VideoCapture>(0);
  }
}

auto Main::_process(double delta_time) -> void {
  if (m_video_capture == nullptr) return;
  if (!m_video_capture->isOpened()) return;

  cv::Mat frame_input;
#if 0
  if (m_video_capture->read(frame_input)) 
 
    cv::Mat rgb_frame;
    cv::Mat frame = m_image_renderer->getBinaryImage(frame_input);

    std::vector<CandidateMarker> candidate_markers =
        m_marker_detection->detect_candidate_markers(frame);
    cv::Mat debug_frame =
        m_marker_detection->draw_markers(frame, candidate_markers);

    cv::cvtColor(debug_frame, rgb_frame, cv::COLOR_BGR2RGB);

    // cv::cvtColor(frame_input, rgb_frame, cv::COLOR_BGR2RGB);

    size_t image_size = rgb_frame.total() * rgb_frame.elemSize();
    PackedByteArray bytes;
    bytes.resize(image_size);
    std::memcpy(bytes.ptrw(), rgb_frame.data, image_size);
    Ref<Image> image = Image::create_from_data(
        rgb_frame.cols, rgb_frame.rows, false, Image::FORMAT_RGB8, bytes);

    Ref<ImageTexture> texture = ImageTexture::create_from_image(image);

    m_image_display->set_texture(texture);
#endif
}

}  // namespace chess
