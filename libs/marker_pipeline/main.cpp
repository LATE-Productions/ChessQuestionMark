#include <atomic>
#include <format>
#include <iostream>
#include <memory>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <stop_token>
#include <thread>

#include "CameraCapture.hpp"
#include "ImageRenderer.hpp"
#include "Marker.hpp"
#include "MarkerDetection.hpp"
#include "MarkerRectification.hpp"
#include "MarkerRecognition.hpp"
#include "Marker.hpp"
#include "RingBuffer.hpp"

int main() {
  using late::CameraCapture;
  using late::CandidateMarker;
  using late::ImageRenderer;
  using late::MarkerDetection;
  using late::MarkerRectification;
  using late::MarkerRecognition;
  using late::Marker;
  using late::OverflowPolicy;
  using late::RingBuffer;
  CameraCapture cameraCapture{};

  cv::Mat shared_frame;

  auto buffer =
      std::make_shared<RingBuffer<cv::Mat, 3, OverflowPolicy::DROP>>();
  cameraCapture.attach_output(buffer);
  ImageRenderer renderer{};
  MarkerDetection marker_detection{0.0005};
  MarkerRectification marker_rectification{};
  MarkerRecognition marker_recognition{};
  std::string windowTitle = "Pipeline";

  int keyPressed = '1';
  while (true) {
    std::optional<cv::Mat> frame = buffer->read();
    if (frame) {
      shared_frame = frame.value();
      if (keyPressed == '1') cv::imshow(windowTitle, shared_frame);

      renderer.getBlackWhiteImage(shared_frame);

      if (keyPressed == '2')
        cv::imshow(windowTitle, shared_frame);  // Use for debug

      std::vector<CandidateMarker> candidate_markers =
          marker_detection.detect_candidate_markers(shared_frame);

      cv::Mat debug_frame =
          marker_detection.draw_markers(shared_frame, candidate_markers);

      std::vector<CandidateMarker> rectified_markers =
          marker_rectification.rectify_markers(shared_frame, candidate_markers);

      cv::Mat rectified_debug_frame =
          marker_detection.draw_markers(shared_frame, rectified_markers);

      std::vector<Marker> valid_markers = 
        marker_recognition.GetValidMarkers(rectified_markers);

      if (keyPressed == '3')
        cv::imshow(windowTitle, debug_frame);  // Use for debug

      if (keyPressed == '4') {
        cv::imshow(windowTitle, debug_frame);
        for (auto& rect : candidate_markers) {
          cv::Point2f center;
          center.x =
          ((float)rect.m_bounding_box.width / 2);
          center.y =
          ((float)rect.m_bounding_box.height / 2);
    
          cv::circle(rect.markerImage, center, 5, cv::Scalar(0, 0, 255), -1);

          cv::imshow(
              std::format("rect {}", rect.m_markerId), rect.markerImage
          );  // Use for debug

        }
      }

      if (keyPressed == '5') {
        cv::imshow(windowTitle, rectified_debug_frame);
        for (auto& rect : rectified_markers) {
          std::string rectWinName = std::format("rect {}", rect.m_markerId);
          cv::imshow(rectWinName, rect.markerImage);  // Use for debug
        }
      }

      if (keyPressed == '6'){
        cv::imshow(windowTitle, shared_frame);
        for (int i = 0; i < valid_markers.size(); i++){
          std::string rectWinName = std::format("id: {}, index: {}", valid_markers[i].m_marker_id, i);
          cv::imshow(rectWinName, valid_markers[i].markerImage);  // Use for debug
        }
      }
    }

    int key = cv::waitKey(20);
    switch (key) {
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
        keyPressed = key;
        break;
      default:
        break;
    }
    if (key == 27) break;
  }

  return 0;
}
