#include "CameraCapture.hpp"

#include <mutex>

#include <opencv2/imgproc.hpp>

namespace late {
CameraCapture::CameraCapture()
    : m_captureFeed(std::make_unique<cv::VideoCapture>(0)),
      m_outputs{},
      m_mutex{},
      m_running{true} {
        m_captureFeed->set(cv::CAP_PROP_AUTO_EXPOSURE, 0.25);
        m_captureFeed->set(cv::CAP_PROP_EXPOSURE, -6);
        m_captureFeed->set(cv::CAP_PROP_GAIN, 0.0);

  m_captureThread = std::thread([&]() {
    while (m_running && m_captureFeed->isOpened()) {
      cv::Mat frame;

      if (m_captureFeed->read(frame)) {  
        std::lock_guard lock{m_mutex};

        for (auto& output : m_outputs) output->write(frame.clone());
      }
    }
  });
}

CameraCapture::~CameraCapture() {
  m_running = false;
  if (m_captureThread.joinable()) m_captureThread.join();
}

auto CameraCapture::attach_output(std::shared_ptr<IBuffer<cv::Mat>> output)
    -> void {
  std::lock_guard lock{m_mutex};
  m_outputs.push_back(output);
}

}  // namespace late
