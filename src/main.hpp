#include <godot_cpp/classes/canvas_layer.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/classes/world_environment.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/core/memory.hpp>
#include <memory>
#include <opencv2/opencv.hpp>
#include "ImageRenderer.hpp"

namespace late {
class Main : public godot::Node {
  GDCLASS(Main, godot::Node);

 public:
  Main()
      : m_canvas_layer{nullptr},
        m_image_display{nullptr},
        m_video_capture{nullptr},
        m_image_renderer(std::make_unique<ImageRenderer>()) {}
  ~Main() = default;

  auto _ready() -> void override;
  auto _process(double delta_time) -> void override;

 protected:
  static auto _bind_methods() -> void {}

 private:
  godot::CanvasLayer* m_canvas_layer = nullptr;
  godot::TextureRect* m_image_display = nullptr;

  std::unique_ptr<cv::VideoCapture> m_video_capture;
  std::unique_ptr<ImageRenderer> m_image_renderer;
};
}  // namespace late
