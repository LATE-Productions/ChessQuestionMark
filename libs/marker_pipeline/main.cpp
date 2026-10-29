#include <algorithm>
#include <atomic>
#include <chrono>
#include <format>
#include <iostream>
#include <memory>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <stop_token>
#include <thread>
#include <vector>
#include <filesystem>

#include "CameraCapture.hpp"
#include "DataStorage.hpp"
#include "ImageRenderer.hpp"
#include "Marker.hpp"
#include "MarkerDetection.hpp"
#include "MarkerRecognition.hpp"
#include "MarkerRectification.hpp"
#include "MarkerPoseEstimator.hpp"
#include "RingBuffer.hpp"
#include "pipeline/Components.hpp"

int main() {
  using late::CameraCapture;
  using late::CandidateMarker;
  using late::DataStorage;
  using late::ImageRenderer;
  using late::Marker;
  using late::MarkerDetection;
  using late::MarkerRecognition;
  using late::MarkerRectification;
  using late::OverflowPolicy;
  using late::RingBuffer;
  using namespace std::chrono_literals;
  CameraCapture cameraCapture{};

  cv::Mat shared_frame;

  if (!late::MarkerPoseEstimator::load_calibration("libs\\marker_pipeline\\camera_calibration\\camera_calibration.yaml")) {
    return 1;
  }
  

  auto buffer =
      std::make_shared<RingBuffer<cv::Mat, 3, OverflowPolicy::DROP>>();
  cameraCapture.attach_output(buffer);
  std::string windowTitle = "Pipeline";

  int keyPressed = '1';
  std::vector<std::string> last_open_windows{};
  std::vector<std::string> open_windows{};
  auto pipeline =
      late::pipeline::make_pipeline<cv::Mat>()
          .attach<ImageRenderer>(115)
          .attach<MarkerDetection>(0.025, 0.25)
          .attach<MarkerRectification>()
          .attach<MarkerRecognition>(0.775)
          .attach<DataStorage>(100.0f)
          .on_input<ImageRenderer>([&keyPressed,
                                    &windowTitle](const cv::Mat& frame) {
            if (keyPressed == '1') cv::imshow(windowTitle, frame);
          })
          .on_output<ImageRenderer>([&keyPressed,
                                     &windowTitle](const cv::Mat& frame) {
            if (keyPressed == '2') cv::imshow(windowTitle, frame);
          })
          .on_output<MarkerDetection>([&keyPressed, &windowTitle, &open_windows,
                                       &last_open_windows](
                                          const late::CandidatePayload& data
                                      ) {
            if (keyPressed == '3') {
              auto frame =
                  MarkerDetection::draw_markers(data.m_frame, data.m_markers);
              cv::imshow(windowTitle, frame);
            }
            if (keyPressed == '4') {
              cv::imshow(windowTitle, data.m_frame);
              for (auto& rect : data.m_markers) {
                cv::Point2f center;
                center.x = ((float)rect.m_boundingBox.width / 2);
                center.y = ((float)rect.m_boundingBox.height / 2);

                cv::circle(
                    rect.m_markerImage, center, 5, cv::Scalar(0, 0, 255), -1
                );
                auto rectWinName = std::format("rect {}", rect.m_markerId);
                cv::imshow(rectWinName, rect.m_markerImage);  // Use for debug

                open_windows.push_back(rectWinName);
                std::erase(last_open_windows, rectWinName);
              }
            }
          })
          .on_output<MarkerRectification>([&keyPressed, &windowTitle,
                                           &open_windows, &last_open_windows](
                                              const late::CandidatePayload& data
                                          ) {
            if (keyPressed == '5') {
              cv::imshow(windowTitle, data.m_frame);
              for (auto& rect : data.m_markers) {
                std::string rectWinName =
                    std::format("rect {}", rect.m_markerId);
                cv::imshow(rectWinName, rect.m_markerImage);  // Use for debug

                open_windows.push_back(rectWinName);
                std::erase(last_open_windows, rectWinName);
              }
            }
          })
          .on_output<MarkerRecognition>(
              [&keyPressed, &windowTitle, &open_windows,
               &last_open_windows](const late::FinalPayload& data) {
                if (keyPressed == '6') {
                  cv::imshow(windowTitle, data.m_frame);
                  auto valid_markers = data.m_markers;

                  for (int i = 0; i < valid_markers.size(); i++) {
                    std::string rectWinName =
                        std::format("id: {}", valid_markers[i].m_markerTypeId);
                    cv::imshow(
                        rectWinName, valid_markers[i].m_markerImage
                    );  // Use for debug

                    open_windows.push_back(rectWinName);
                    std::erase(last_open_windows, rectWinName);
                  }
                }
              }
          );

  cv::imshow(windowTitle, cv::Mat{100, 100, CV_8UC1, cv::Scalar::all(0)});
  std::this_thread::sleep_for(3000ms);

  while (true) {
    std::optional<cv::Mat> frame = buffer->read();
    if (frame) {
      pipeline << std::move(*frame);
      std::optional<std::vector<Marker>> output;
      pipeline >> output;
      if (output) {
        if (output->size() > 0)
          std::println("Detects {} markers", output->size());
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

    for (const auto& winName : last_open_windows) {
      cv::destroyWindow(winName);
    }
    last_open_windows = open_windows;
    open_windows.clear();
  }

  return 0;
}
