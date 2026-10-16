#include <atomic>
#include <iostream>
#include <memory>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <stop_token>
#include <thread>

#include "CameraCapture.hpp"
#include "ImageRenderer.hpp"
#include "Marker.hpp"
#include "MarkerDetection.hpp"
#include "RingBuffer.hpp"

int main() {
  using late::CameraCapture;
  using late::ImageRenderer;
  using late::Marker;
  using late::MarkerDetection;
  using late::OverflowPolicy;
  using late::RingBuffer;
  CameraCapture cameraCapture{};

  cv::Mat shared_frame;

  auto buffer =
      std::make_shared<RingBuffer<cv::Mat, 3, OverflowPolicy::DROP>>();
  cameraCapture.attach_output(buffer);
  ImageRenderer renderer{};
  MarkerDetection marker_detection{};

  // Read and display one frame at a time.
  int keyPressed = '1';
  while (true) {
    int key = cv::waitKey(5);
    switch (key) {
      case '1':
      case '2':
      case '3':
      case '4':
        keyPressed = key;
        break;
      default:
        break;
    }
    if (key == 27) break;
    std::optional<cv::Mat> frame = buffer->read();
    if (frame) {
      shared_frame = frame.value();
      if (keyPressed == '1') cv::imshow("Pipeline", shared_frame);

      renderer.getBlackWhiteImage(shared_frame);

      if (keyPressed == '2')
        cv::imshow("Pipeline", shared_frame);  // Use for debug

      std::vector<Marker> candidate_markers =
          marker_detection.detect_candidate_markers(shared_frame);

      cv::Mat debug_frame =
          marker_detection.draw_markers(shared_frame, candidate_markers);

      if (keyPressed == '3')
        cv::imshow("Pipeline", debug_frame);  // Use for debug

      if (keyPressed == '4') {
        if (candidate_markers.size() > 0)
          cv::imshow("Pipeline", candidate_markers[0].markerImage);
        else
          cv::imshow("Pipeline", debug_frame);  // Use for debug
      }
    }
  }

  return 0;
}
