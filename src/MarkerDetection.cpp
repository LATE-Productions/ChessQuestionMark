#include "MarkerDetection.hpp"

namespace late {

    auto MarkerDetection::detect_candidate_markers(cv::Mat& frame) -> std::vector<Marker>{
        std::vector<Marker> markers{};
        std::array<cv::Point2f, 4> corners{};

        if (frame.empty()) {
            return markers;
        }

        std::vector<std::vector<cv::Point>> contours;

        cv::findContours(frame, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

        for (const auto& contour : contours) {
            
            if (cv::contourArea(contour) < 100) {
                continue; // Skipping small contours
            }

            std::vector<cv::Point> approx;

            const double perimeter = cv::arcLength(contour, true);

            cv::approxPolyDP(contour, approx, 0.02 * perimeter, true);

            if (approx.size() != 4){
                continue; // Skipping non-rectangular contours
            }

            if (!cv::isContourConvex(approx)) {
                continue; // Skipping non-convex contours
            }

            Marker marker;

            marker.m_corners = detect_corners(approx);
            marker.m_bounding_box = detect_rect(approx);

            markers.push_back(marker); // May use embarkpush_back(std::move(marker)) for efficiency
        }
        return markers;
    }

    auto MarkerDetection::detect_corners(const std::vector<cv::Point>& contour) -> std::array<cv::Point2f, 4>{
        std::array<cv::Point2f, 4> corners{};

        for (std::size_t i = 0; i < 4; ++i){
            corners[i] = cv::Point2f(
                static_cast<float>(contour[i].x), 
                static_cast<float>(contour[i].y)
            );
        }
        
        return corners;
    }

    auto MarkerDetection::order_corners(const std::array<cv::Point2f, 4>& corners) -> std::array<cv::Point2f, 4>{
        std::array<cv::Point2f, 4> ordered_corners{};

        // Calculate the sum and difference of the corner points
        std::array<float, 4> sum{};
        std::array<float, 4> diff{};

        for (std::size_t i = 0; i < 4; ++i) {
            sum[i] = corners[i].x + corners[i].y;
            diff[i] = corners[i].y - corners[i].x;
        }

        // Top-left corner has the smallest sum
        ordered_corners[0] = corners[std::distance(sum.begin(), std::min_element(sum.begin(), sum.end()))];
        // Bottom-right corner has the largest sum
        ordered_corners[2] = corners[std::distance(sum.begin(), std::max_element(sum.begin(), sum.end()))];
        // Top-right corner has the smallest difference
        ordered_corners[1] = corners[std::distance(diff.begin(), std::min_element(diff.begin(), diff.end()))];
        // Bottom-left corner has the largest difference
        ordered_corners[3] = corners[std::distance(diff.begin(), std::max_element(diff.begin(), diff.end()))];

        return ordered_corners;
    }

    auto MarkerDetection::detect_rect(const std::vector<cv::Point>& contour) -> cv::Rect{ return cv::boundingRect(contour);}

}