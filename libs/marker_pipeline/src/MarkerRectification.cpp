#include "MarkerRectification.hpp"

namespace late {

    auto MarkerRectification::rectify_markers(const cv::Mat& frame, std::vector<Marker>& markers)
        -> std::vector<Marker> {
        for (auto& marker : markers) {
            marker.markerImage = rectify_markerImage(frame, marker);
        }
        return markers;
    }

    auto MarkerRectification::rectify_markerImage(const cv::Mat& frame, const Marker& marker) -> cv::Mat {

        std::array<cv::Point2f, 4> dst_corners = {
            cv::Point2f(0, 0),
            cv::Point2f(marker.markerImage.cols - 1, 0),
            cv::Point2f(marker.markerImage.cols - 1, marker.markerImage.rows - 1),
            cv::Point2f(0, marker.markerImage.rows - 1)
        };

        cv::Mat homography = cv::getPerspectiveTransform(marker.m_corners, dst_corners);
        
        cv::Mat rectified_image;

        cv::warpPerspective(frame, rectified_image, homography, marker.markerImage.size());
        return rectified_image;
    }
}