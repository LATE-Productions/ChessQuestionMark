#pragma once

#include <opencv2/core/hal/interface.h>

#include <algorithm>
#include <array>
#include <print>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <unordered_map>

namespace late {
template <std::size_t Dimensions>
class MarkerTemplate {
 public:
  MarkerTemplate(
      const std::array<bool, Dimensions * Dimensions>& inputTemplate
  );
  MarkerTemplate(const MarkerTemplate<Dimensions>& other) = default;
  MarkerTemplate<Dimensions>& operator=(
      const MarkerTemplate<Dimensions>& other
  )                                                  = default;
  MarkerTemplate(MarkerTemplate<Dimensions>&& other) = default;
  MarkerTemplate<Dimensions>& operator=(MarkerTemplate<Dimensions>&& other) =
      default;
  ~MarkerTemplate() = default;

  enum class Rotation { NONE, ROTATE_90, ROTATE_180, ROTATE_270 };

  auto operator()(int size) -> cv::Mat&;
  auto operator()(int size, Rotation rotation) -> cv::Mat&;

 private:
  cv::Mat m_markerBase;
  cv::Mat m_markerRotation90;
  cv::Mat m_markerRotation180;
  cv::Mat m_markerRotation270;
  std::unordered_map<int, cv::Mat> m_cacheBase;
  std::unordered_map<int, cv::Mat> m_cacheRotation90;
  std::unordered_map<int, cv::Mat> m_cacheRotation180;
  std::unordered_map<int, cv::Mat> m_cacheRotation270;
};

template <std::size_t Dimensions>
MarkerTemplate<Dimensions>::MarkerTemplate(
    const std::array<bool, Dimensions * Dimensions>& inputTemplate
)
    : m_markerBase{Dimensions + 2, Dimensions + 2, CV_8UC1, cv::Scalar::all(0)},
      m_markerRotation90(Dimensions + 2, Dimensions + 2, CV_8UC1),
      m_markerRotation180(Dimensions + 2, Dimensions + 2, CV_8UC1),
      m_markerRotation270(Dimensions + 2, Dimensions + 2, CV_8UC1),
      m_cacheBase{},
      m_cacheRotation90{},
      m_cacheRotation180{},
      m_cacheRotation270{} {
  cv::Mat innerMarker{Dimensions, Dimensions, CV_8UC1};
  std::transform(
      inputTemplate.begin(), inputTemplate.end(), innerMarker.begin<uchar>(),
      [](bool b) { return b ? uchar(255) : uchar(0); }
  );
  innerMarker.copyTo(m_markerBase(cv::Rect{1, 1, Dimensions, Dimensions}));
  cv::rotate(m_markerBase, m_markerRotation90, cv::ROTATE_90_CLOCKWISE);
  cv::rotate(m_markerBase, m_markerRotation180, cv::ROTATE_180);
  cv::rotate(m_markerBase, m_markerRotation270, cv::ROTATE_90_COUNTERCLOCKWISE);
}

template <std::size_t Dimensions>
auto MarkerTemplate<Dimensions>::operator()(int size) -> cv::Mat& {
  assert(size >= (Dimensions + 2));
  return this(size, Rotation::NONE);
}

template <std::size_t Dimensions>
auto MarkerTemplate<Dimensions>::operator()(int size, Rotation rotation)
    -> cv::Mat& {
  assert(size >= (Dimensions + 2));
  cv::Mat* marker;
  std::unordered_map<int, cv::Mat>* cache;
  int rot = 0;
  switch (rotation) {
    case Rotation::NONE:
      marker = &m_markerBase;
      cache  = &m_cacheBase;
      break;
    case Rotation::ROTATE_90:
      marker = &m_markerRotation90;
      cache  = &m_cacheRotation90;
      rot = 90;
      break;
    case Rotation::ROTATE_180:
      marker = &m_markerRotation180;
      cache  = &m_cacheRotation180;
      rot = 180;
      break;
    case Rotation::ROTATE_270:
      marker = &m_markerRotation270;
      cache  = &m_cacheRotation270;
      rot = 270;
      break;
  }
  if (size == Dimensions + 2) return *marker;
  if (cache->contains(size)) {
    std::println("cache hit for rotation {} and size {}", rot, size);
    return cache->at(size);
}
  cv::Mat output;
  cv::resize(*marker, output, cv::Size(size, size), 0, 0, cv::INTER_NEAREST);
  cache->insert_or_assign(size, std::move(output));
  return cache->at(size);
}
}  // namespace late
