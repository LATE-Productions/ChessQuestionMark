#pragma once

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/quaternion.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <map>
#include <memory>
#include <thread>
#include <unordered_map>

#include "RingBuffer.hpp"
#include "objects/MarkerContainer.hpp"
#include "singletons/VideoSource.hpp"

namespace chess {
using late::OverflowPolicy;
using PiplineSourceBuffer =
    late::RingBuffer<cv::Mat, 4, OverflowPolicy::OVERWRITE>;

class MarkerManager : public godot::Node3D {
  GDCLASS(MarkerManager, godot::Node3D)

 public:
  auto _ready() -> void override;
  auto _exit_tree() -> void override;
  auto _process(double delta) -> void override;
  auto create_marker(int markerId) -> godot::Node3D*;
  auto update_marker(
      int id, godot::Vector3 position, godot::Quaternion rotation
  ) -> void;

  auto get_scale2d() const -> double;
  auto set_scale2d(double scale) -> void;

  auto get_depth() const -> double;
  auto set_depth(double depth) -> void;

  auto get_spring() const -> double;
  auto set_spring(double spring) -> void;

 protected:
  static auto _bind_methods() -> void;

 private:
  VideoSource* m_source = nullptr;
  godot::Ref<MarkerContainer> m_markerData;
  std::map<int, godot::Node3D*> m_markers;
  std::map<int, godot::Vector3> m_targetPositions;
  std::unordered_map<int, double> m_lastSeenMarkers;

  std::jthread m_worker;
  std::mutex m_mtx{};
  std::optional<std::vector<late::GodotMarker>> m_output;

  std::shared_ptr<PiplineSourceBuffer> m_buffer =
      std::make_shared<PiplineSourceBuffer>();

  double m_depth;
  double m_scale2d;
  double m_spring        = 0.1f;
  double m_elapsedTime   = 0.0;
  double m_markerTimeout = 1.0;
};
}  // Namespace chess
