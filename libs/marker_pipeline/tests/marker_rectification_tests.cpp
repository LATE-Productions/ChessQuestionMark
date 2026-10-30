#include <doctest/doctest.h>

#include "MarkerRectification.hpp"
#include "Marker.hpp"

TEST_SUITE_BEGIN("Marker Rectification");


TEST_CASE("rectified image is square") {
cv::Mat frame(200, 200, CV_8UC1, cv::Scalar(0));

CandidateMarker marker;
marker.m_markerImage = cv::Mat(40, 40, CV_8UC1);
marker.m_corners = {
    cv::Point2f(50, 50),
    cv::Point2f(90, 50),
    cv::Point2f(90, 90),
    cv::Point2f(50, 90)
};

MarkerRectification rectification;
auto result = rectification.rectify_markerImage(frame, marker);

CHECK_FALSE(result.empty());
CHECK(result.rows == result.cols);
CHECK(result.rows == 40);
};


TEST_CASE("rectified bounding box") {

};

TEST_CASE("valid corners") {

};

TEST_CASE("valid bounding box size") {
cv::Mat frame(200, 200, CV_8UC1, cv::Scalar(255));

CandidateMarker marker;
marker.m_markerImage = cv::Mat(40, 60, CV_8UC1);
marker.m_corners = {
    cv::Point2f(20, 20),
    cv::Point2f(80, 20),
    cv::Point2f(80, 80),
    cv::Point2f(20, 80)
};
MarkerRectification rectification;
auto result = rectification.rectify_markerImage(frame, marker);

//Im using destSize and std::max with rows and cols so I will always get 60

CHECK_FALSE(result.empty());
CHECK(result.rows == 60);
CHECK(result.cols == 60);

};

TEST_CASE("empty marker image") {

};

TEST_SUITE_END();