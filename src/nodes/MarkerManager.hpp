#pragma once

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <memory>
#include <thread>
#include <unordered_map>

#include "RingBuffer.hpp"
#include "objects/MarkerContainer.hpp"
#include "singletons/VideoSource.hpp"

namespace chess {
using late::OverflowPolicy;
using PiplineSourceBuffer =
    late::RingBuffer<cv::Mat, 4, OverflowPolicy::DROP>;

class MarkerManager : public godot::Node3D {
  GDCLASS(MarkerManager, godot::Node3D)

 public:
  auto _ready() -> void override;
  auto _exit_tree() -> void override;
  auto update_all_markers(godot::Ref<MarkerContainer> markers) -> void;
  auto update_marker(int id, godot::Vector3 position, godot::Vector3 rotation)
      -> void;

 protected:
  static auto _bind_methods() -> void;

 private:
  VideoSource* m_source = nullptr;
  godot::Ref<MarkerContainer> m_markerData;
  std::unordered_map<int, godot::Node3D*> m_markers;

  std::jthread m_worker;
  godot::Callable m_updateAllMarkers;

  std::shared_ptr<PiplineSourceBuffer> m_buffer =
      std::make_shared<PiplineSourceBuffer>();
};
}  // Namespace chess
