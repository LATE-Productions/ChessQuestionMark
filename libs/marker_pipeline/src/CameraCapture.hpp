#pragma once
#include <atomic>
#include <memory>
#include <opencv2/videoio.hpp>
#include <thread>

#include "RingBuffer.hpp"

namespace late {
class CameraCapture {
 public:
  CameraCapture();
  ~CameraCapture();

  CameraCapture(const CameraCapture&) = delete;
  CameraCapture& operator=(const CameraCapture&) = delete;
  CameraCapture(CameraCapture&&) = delete;
  CameraCapture& operator=(CameraCapture&&) = delete;

  auto attach_output(std::shared_ptr<IBuffer<cv::Mat>> output) -> void;

 private:
  std::unique_ptr<cv::VideoCapture> m_captureFeed;
  std::vector<std::shared_ptr<IBuffer<cv::Mat>>> m_outputs;
  std::atomic<bool> m_running;
  std::mutex m_mutex;
  std::thread m_captureThread;
};
}  // namespace late
