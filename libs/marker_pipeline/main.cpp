#include <iostream>
#include <opencv2/opencv.hpp>
#include <thread>
#include <stop_token>
#include <memory>
#include <atomic>
#include "CameraCapture.hpp"
#include "RingBuffer.hpp"
#include "ImageRenderer.hpp"
#include "MarkerDetection.hpp"
#include "Marker.hpp"

int main() {
  using late::CameraCapture;
  using late::OverflowPolicy;
  using late::RingBuffer;
  using late::ImageRenderer;
  using late::MarkerDetection;
  using late::Marker;
  CameraCapture cameraCapture{};


  cv::Mat shared_frame;
  

    auto buffer = std::make_shared<RingBuffer<cv::Mat, 3, OverflowPolicy::DROP>>();
    cameraCapture.attach_output(buffer);
    ImageRenderer renderer{};
    MarkerDetection marker_detection{};

    
    
  
  // Read and display one frame at a time.
  while (true) {
    std::optional<cv::Mat> frame = buffer->read();
      if(frame){
        
        shared_frame = frame.value();
        cv::imshow("Video input", shared_frame);

        renderer.getBlackWhiteImage(shared_frame);
        //cv::imshow("Black and WHite", shared_frame); // Use for debug
        
        std::vector<Marker> candidate_markers =
        marker_detection.detect_candidate_markers(shared_frame);

        cv::Mat debug_frame =
        marker_detection.draw_markers(shared_frame, candidate_markers);

        cv::imshow("Debug Markers", debug_frame); // Use for debug
      }
    // Wait briefly; Escape closes the program.
    if (cv::waitKey(20) == 27) {
      break;
    }
  }


  return 0;
}
