#pragma once

#include <opencv2/core/hal/interface.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

namespace late {
template <std::size_t Dimensions>
class MarkerTemplate {
 public:
  MarkerTemplate(
      const std::array<bool, Dimensions * Dimensions>& inputTemplate
  );
  MarkerTemplate(const MarkerTemplate<Dimensions>& other);
  MarkerTemplate<Dimensions>& operator=(
      const MarkerTemplate<Dimensions>& other
  );
  MarkerTemplate(MarkerTemplate<Dimensions>&& other);
  MarkerTemplate<Dimensions>& operator=(MarkerTemplate<Dimensions>&& other);
  ~MarkerTemplate();

  void operator()(int size, cv::OutputArray output);

 private:
  cv::Mat m_marker;
};

template <std::size_t Dimensions>
MarkerTemplate<Dimensions>::MarkerTemplate(
    const std::array<bool, Dimensions * Dimensions>& inputTemplate
)
    : m_marker{Dimensions + 2, Dimensions + 2, CV_8UC1, cv::Scalar::all(0)} {
  cv::Mat innerMarker{Dimensions, Dimensions, CV_8UC1};
  std::transform(
      inputTemplate.begin(), inputTemplate.end(), innerMarker.begin<uchar>(),
      [](bool b) { return b ? uchar(255) : uchar(0); }
  );
  innerMarker.copyTo(m_marker(cv::Rect{1, 1, Dimensions, Dimensions}));
}

template <std::size_t Dimensions>
void MarkerTemplate<Dimensions>::operator()(int size, cv::OutputArray output) {
  assert(size >= (Dimensions * 2) * (Dimensions * 2));
  output.create(size, size, CV_8UC1);
  cv::resize(m_marker, output, output.size(), 0, 0, cv::INTER_NEAREST);
}

}  // namespace late
