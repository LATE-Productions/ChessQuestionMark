#include "MarkerDetection.hpp"

namespace late {

    auto MarkerDetection::detect_candidate_markers(cv::Mat& frame) -> std::vector<Marker>{
        std::vector<Marker> markers{};

        // Do detections for each marker, checking their corners and rects

        // Use embarkpushback to add markers

        for (auto m : markers){

            detect_corners(frame);
            detect_rect(frame);

            
        }


        // Add candidates to markers.

        // Recognition will later check and know which candidates are allowed.

        return markers;
    }

    auto MarkerDetection::detect_corners(cv::Mat& frame) -> std::array<cv::Point2f, 4>{
        std::array<cv::Point2f, 4> corners{};

        // Find contours

        
        return corners;
    }

    auto MarkerDetection::detect_rect(cv::Mat& frame) -> cv::Rect{
        cv::Rect bounding_box


        return bounding_box;
    }

}