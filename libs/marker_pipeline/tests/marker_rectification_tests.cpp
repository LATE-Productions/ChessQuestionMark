#include <doctest/doctest.h>

#include "Marker.hpp"
#include "MarkerRectification.hpp"

TEST_SUITE_BEGIN("Marker Rectification");


// Purpose is to test the rectify_markerImage function, which is responsible for rectifying the marker image based on the provided corners. 
// The tests will cover various scenarios, including valid and invalid inputs, to ensure the function behaves as expected.

// PASSED TEST CASES TO CHECK IF FUNCTIONALITY IS CORRECT

// Test cases for the rectify_markerImage function, this test check if rectified image is square and has the correct size based on the marker's corners.
TEST_CASE("rectified image is square") {
  cv::Mat frame(200, 200, CV_8UC1, cv::Scalar(0));

  late::CandidateMarker marker;
  marker.m_markerImage = cv::Mat(40, 40, CV_8UC1);
  marker.m_corners     = {
      cv::Point2f(50, 50), cv::Point2f(90, 50), cv::Point2f(90, 90),
      cv::Point2f(50, 90)
  };

  late::MarkerRectification rectification;
  auto result = rectification.rectify_markerImage(frame, marker);

  CHECK_FALSE(result.empty());
  CHECK(result.rows == result.cols);
  CHECK(result.rows == 40);
};

// Test case to check if the rectified image has the correct size based on the marker's corners.
TEST_CASE("rectified bounding box") {
cv::Mat frame(200, 200, CV_8UC1, cv::Scalar(0));

cv::rectangle(frame, cv::Point(50, 50), cv::Point(110, 90), cv::Scalar(255), cv::FILLED);

late::CandidateMarker marker;
marker.m_markerImage = cv::Mat(40, 60, CV_8UC1);
marker.m_corners = {
  cv::Point2f(50, 50),
  cv::Point2f(110, 50),
  cv::Point2f(110, 90),
  cv::Point2f(50, 90)
};

late::MarkerRectification rectification;
auto result = rectification.rectify_markerImage(frame, marker);

CHECK_FALSE(result.empty());
CHECK(result.rows == 60);
CHECK(result.cols == 60);

// The rectified image should still contain the white rectangle.
CHECK(cv::countNonZero(result) > 0);
};

// Test case to check if the rectified image has the correct size based on the marker's corners.  
TEST_CASE("valid corners") {
cv::Mat frame(200, 200, CV_8UC1, cv::Scalar(0));

cv::rectangle(frame, cv::Point(40, 40), cv::Point(120, 120), cv::Scalar(255), cv::FILLED);

late::CandidateMarker marker;
marker.m_markerImage = cv::Mat(80, 80, CV_8UC1);
marker.m_corners = {
  cv::Point2f(40, 40),
  cv::Point2f(120, 40),
  cv::Point2f(120, 120),
  cv::Point2f(40, 120)
};

late::MarkerRectification rectification;
auto result = rectification.rectify_markerImage(frame, marker);

CHECK_FALSE(result.empty());
CHECK(result.rows == 80);
CHECK(result.cols == 80);
CHECK(cv::countNonZero(result) > 0);
};

// Test case to check if bounding box size is valid and the rectified image has the correct size based on the marker's corners.
TEST_CASE("valid bounding box size") {
  cv::Mat frame(200, 200, CV_8UC1, cv::Scalar(255));

  late::CandidateMarker marker;
  marker.m_markerImage = cv::Mat(40, 60, CV_8UC1);
  marker.m_corners     = {
      cv::Point2f(20, 20), cv::Point2f(80, 20), cv::Point2f(80, 80),
      cv::Point2f(20, 80)
  };
  late::MarkerRectification rectification;
  auto result = rectification.rectify_markerImage(frame, marker);

  // Im using destSize and std::max with rows and cols so I will always get 60

  CHECK_FALSE(result.empty());
  CHECK(result.rows == 60);
  CHECK(result.cols == 60);
};

// Test case to check if the rectified image is empty when the marker image is empty.
TEST_CASE("empty marker image") {
  cv::Mat frame(200, 200, CV_8UC1, cv::Scalar(255));

  late::CandidateMarker marker;
  marker.m_markerImage = cv::Mat();
  marker.m_corners     = {
      cv::Point2f(20, 20), 
      cv::Point2f(60, 20), 
      cv::Point2f(60, 60),
      cv::Point2f(20, 60)
  };

  late::MarkerRectification rectification;
  auto result = rectification.rectify_markerImage(frame, marker);

  // Should result empty because of current checks with destSize.
  CHECK(result.empty());
};

// Test case to check if the rectified image is empty when the marker is outside the frame.
TEST_CASE("marker outside frame") {
cv::Mat frame(200, 200, CV_8UC1, cv::Scalar(255));

late::CandidateMarker marker;
marker.m_markerImage = cv::Mat(40, 40, CV_8UC1);
marker.m_corners = {
  cv::Point2f(210, 210), 
  cv::Point2f(250, 210), 
  cv::Point2f(250, 250),
  cv::Point2f(210, 250)

};

late::MarkerRectification rectification;
auto result = rectification.rectify_markerImage(frame, marker);

CHECK_FALSE(result.empty());
CHECK(result.rows == 40);
CHECK(result.cols == 40);

// Marker is outside the frame, so the rectified image should be empty (all black).
CHECK(cv::countNonZero(result) == 0);
};


// FAILED TEST CASES TO CHECK IF LIMITATIONS ARE HANDLED CORRECTLY

// Test case intended to fail, to check if the rectified image has the correct size based on the marker's corners.
TEST_CASE("rectified bounding box failed") {
  cv::Mat frame(200, 200, CV_8UC1, cv::Scalar(0));
  
  cv::rectangle(frame, cv::Point(50, 50), cv::Point(110, 90), cv::Scalar(255), cv::FILLED);
  
  late::CandidateMarker marker;
  marker.m_markerImage = cv::Mat(40, 60, CV_8UC1);
  marker.m_corners = {
    cv::Point2f(50, 50),
    cv::Point2f(110, 50),
    cv::Point2f(110, 90),
    cv::Point2f(50, 90)
  };
  
  late::MarkerRectification rectification;
  auto result = rectification.rectify_markerImage(frame, marker);
  
  CHECK_FALSE(result.empty());
  CHECK(result.rows == 80); // Testing wrong size, to ensure a failure occurs when the expected size is not met.
  CHECK(result.cols == 80); 
  
  // The rectified image should still contain the white rectangle.
  CHECK(cv::countNonZero(result) > 0);
};

// Test case intended to fail, to check if the rectified image has the correct size based on the marker's corners.
TEST_CASE("valid corners failed") {
cv::Mat frame(200, 200, CV_8UC1, cv::Scalar(0));

cv::rectangle(frame, cv::Point(40, 40), cv::Point(120, 120), cv::Scalar(255), cv::FILLED);

late::CandidateMarker marker;
marker.m_markerImage = cv::Mat(80, 80, CV_8UC1);
marker.m_corners = {
  cv::Point2f(40, 40),
  cv::Point2f(120, 40),
  cv::Point2f(120, 120),
  cv::Point2f(40, 120)
};

late::MarkerRectification rectification;
auto result = rectification.rectify_markerImage(frame, marker);

CHECK_FALSE(result.empty());
CHECK(result.rows == 100); // Testing wrong size, to ensure a failure occurs when the expected size is not met.
CHECK(result.cols == 100);
CHECK(cv::countNonZero(result) > 0);
};

// Test case intended to fail, to check if the rectified image is empty when the marker image is empty.
TEST_CASE("empty marker image failed") {
  cv::Mat frame(200, 200, CV_8UC1, cv::Scalar(255));

  late::CandidateMarker marker;
  marker.m_markerImage = cv::Mat();
  marker.m_corners     = {
      cv::Point2f(20, 20), 
      cv::Point2f(60, 20), 
      cv::Point2f(60, 60),
      cv::Point2f(20, 60)
  };

  late::MarkerRectification rectification;
  auto result = rectification.rectify_markerImage(frame, marker);

  // Intentionally checking for a non-empty result to see if the function incorrectly handles empty marker images.
  CHECK_FALSE(result.empty());
};

// Test case intentionally failing to see if marker outside frame is handled correctly.
TEST_CASE("marker outside frame failed") {
  cv::Mat frame(200, 200, CV_8UC1, cv::Scalar(255));
  
  late::CandidateMarker marker;
  marker.m_markerImage = cv::Mat(40, 40, CV_8UC1);
  marker.m_corners = {
    cv::Point2f(110, 110), 
    cv::Point2f(150, 110), 
  cv::Point2f(150, 150),
  cv::Point2f(110, 150)
  
};

late::MarkerRectification rectification;
auto result = rectification.rectify_markerImage(frame, marker);

CHECK_FALSE(result.empty());
CHECK(result.rows == 40);
CHECK(result.cols == 40);

// Marker is outside the frame, so the rectified image should be empty (all black).
CHECK(cv::countNonZero(result) == 0);
};

TEST_SUITE_END();
