#pragma once

#include <opencv2/core/hal/interface.h>

#include <algorithm>
#include <array>
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
  if (size == Dimensions + 2) return m_markerBase;
  if (m_cacheBase.contains(size)) return m_cacheBase[size];
  m_cacheBase[size] = cv::Mat{size, size, CV_8UC1};
  auto& output      = m_cacheBase[size];
  cv::resize(m_markerBase, output, output.size(), 0, 0, cv::INTER_NEAREST);
  return output;
}

template <std::size_t Dimensions>
auto MarkerTemplate<Dimensions>::operator()(int size, Rotation rotation)
    -> cv::Mat& {
  assert(size >= (Dimensions + 2));
  cv::Mat* marker;
  std::unordered_map<int, cv::Mat>* cache;
  switch (rotation) {
    case Rotation::NONE:
      marker = &m_markerBase;
      cache  = &m_cacheBase;
      break;
    case Rotation::ROTATE_90:
      marker = &m_markerRotation90;
      cache  = &m_cacheRotation90;
      break;
    case Rotation::ROTATE_180:
      marker = &m_markerRotation180;
      cache  = &m_cacheRotation180;
    case Rotation::ROTATE_270:
      marker = &m_markerRotation270;
      cache  = &m_cacheRotation270;
  }
  if (size == Dimensions + 2) return *marker;
  if (m_cacheBase.contains(size)) return cache->at(size);
  cache->insert({size, cv::Mat{size, size, CV_8UC1}});
  auto& output = cache->at(size);
  cv::resize(*marker, output, output.size(), 0, 0, cv::INTER_NEAREST);
  return output;
}

}  // namespace late
