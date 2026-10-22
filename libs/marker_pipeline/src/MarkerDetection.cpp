#include "MarkerDetection.hpp"
#include "MarkerPoseEstimator.hpp"

namespace late {

MarkerDetection::MarkerDetection(double minContourThreshold, double maxContourThreshold)
    : m_minContourThreshold(minContourThreshold * minContourThreshold), m_maxContourThreshold(maxContourThreshold * maxContourThreshold) {
  assert(m_minContourThreshold <= 1);
  assert(m_maxContourThreshold <= 1 && m_maxContourThreshold > m_minContourThreshold);
}

auto MarkerDetection::detect_candidate_markers(cv::Mat& frame)
    -> std::vector<CandidateMarker> {
  std::vector<CandidateMarker> markers{};
  std::array<cv::Point2f, 4> corners{};

  if (frame.empty()) {
    return markers;
  }

  std::vector<std::vector<cv::Point>> contours;

  cv::findContours(frame, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
  markers.reserve(contours.size());  // Reserve space for efficiency
  // cv::drawContours(frame, contours, 0, cv::Scalar(0, 0, 255), 2);

  std::size_t id_generator = 0;
  int minContourThreshold     = (frame.cols * frame.rows) * m_minContourThreshold;
  int maxContourThreshold     = (frame.cols * frame.rows) * m_maxContourThreshold;
  for (const auto& contour : contours) {
    if (cv::contourArea(contour) < minContourThreshold || cv::contourArea(contour) > maxContourThreshold) {
      continue;  // Skipping too small and too large contours
    }

    std::vector<cv::Point> approx;

    const double perimeter = cv::arcLength(contour, true);

    cv::approxPolyDP(contour, approx, 0.02 * perimeter, true);

    if (approx.size() != 4) {
      continue;  // Skipping non-rectangular contours
    }

    if (!cv::isContourConvex(approx)) {
      continue;  // Skipping non-convex contours
    }

    if (!equilateral_control(approx, 0.50)) {
      continue;
    }

    CandidateMarker marker;

    marker.m_corners      = order_corners(detect_corners(approx));
    marker.m_boundingBox = detect_rect(approx);
    marker.markerImage    = frame(marker.m_boundingBox).clone();
    MarkerPoseEstimator::estimate_pose(marker, 825.0f);
    marker.m_markerId = id_generator++;  // Assigning a unique ID to the marker
    marker.m_contours.push_back(contour);

    markers.push_back(
        marker
    );  // May use embarkpush_back(std::move(marker)) for efficiency

    std::cout << "Marker apperent size" << marker.m_apperentSize << std::endl;

    // std::println("Detected marker with corners: ");
    // for (const auto& corner : marker.m_corners) {
    //   std::println("Corner: {}, {}", corner.x, corner.y);
    // }
  }
  return markers;
}

auto MarkerDetection::draw_markers(
    const cv::Mat& frame, const std::vector<CandidateMarker> markers
) -> cv::Mat {
  cv::Mat display_frame;
  cv::cvtColor(frame, display_frame, cv::COLOR_GRAY2BGR);

  std::vector<std::vector<cv::Point>> contours;

  cv::Mat contour_input = frame.clone();

  
  for (const auto& marker : markers) {
    cv::Point center;

    cv::drawContours(
        display_frame, marker.m_contours, -1, cv::Scalar(0, 255, 0), 2
    );

    for (size_t i = 0; i < marker.m_corners.size(); i++){
      cv::Point corner = marker.m_corners[i];
      cv::circle(display_frame, corner, 5, cv::Scalar(0, 0, 255), -1);

      cv::putText(
        display_frame, std::to_string(i), corner + cv::Point(5, -5),
        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 0), 1
      );

      center += corner;
    }

    center = center * 0.25f;

    cv::circle(display_frame, center, 5, cv::Scalar(0, 255, 0), -1);
  }

  return display_frame;
}

auto MarkerDetection::detect_corners(const std::vector<cv::Point>& contour)
    -> std::array<cv::Point2f, 4> {
  std::array<cv::Point2f, 4> corners{};

  for (std::size_t i = 0; i < 4; ++i) {
    corners[i] = cv::Point2f(
        static_cast<float>(contour[i].x), static_cast<float>(contour[i].y)
    );
  }

  return corners;
}

auto MarkerDetection::order_corners(const std::array<cv::Point2f, 4>& corners)
    -> std::array<cv::Point2f, 4> {
  std::array<cv::Point2f, 4> ordered_corners{};

  // Calculate the sum and difference of the corner points
  std::array<float, 4> sum{};
  std::array<float, 4> diff{};

  for (std::size_t i = 0; i < 4; ++i) {
    sum[i]  = corners[i].x + corners[i].y;
    diff[i] = corners[i].y - corners[i].x;
  }

  // Top-left corner has the smallest sum
  ordered_corners[0] = corners[std::distance(
      sum.begin(), std::min_element(sum.begin(), sum.end())
  )];
  // Bottom-right corner has the largest sum
  ordered_corners[2] = corners[std::distance(
      sum.begin(), std::max_element(sum.begin(), sum.end())
  )];
  // Top-right corner has the smallest difference
  ordered_corners[1] = corners[std::distance(
      diff.begin(), std::min_element(diff.begin(), diff.end())
  )];
  // Bottom-left corner has the largest difference
  ordered_corners[3] = corners[std::distance(
      diff.begin(), std::max_element(diff.begin(), diff.end())
  )];

  return ordered_corners;
}

auto MarkerDetection::detect_rect(const std::vector<cv::Point>& contour)
    -> cv::Rect {
  return cv::boundingRect(contour);
}

auto MarkerDetection::equilateral_control(const std::vector<cv::Point>& contour, const double& threshold)
    -> bool {

      if (contour.size() != 4) {
        return false;
      }

      cv::Point v1 = contour[1] - contour[0];
      cv::Point v2 = contour[2] - contour[1];
      cv::Point v3 = contour[3] - contour[2];
      cv::Point v4 = contour[0] - contour[3];

      double l1 = cv::norm(v1);
      double l2 = cv::norm(v2);
      double l3 = cv::norm(v3);
      double l4 = cv::norm(v4);

      double lengths[] = {l1, l2, l3, l4};
      bool valid = true;

      for (int i = 0; i < 4; i++) {
        for (int j = i + 1; j < 4; j++) {
          if (std::abs(
                  lengths[i] - lengths[j] >
                  threshold * std::max(lengths[i], lengths[j])
              )) {
            valid = false;
            break;
          }
        }

        if (!valid) {
          break;
        }
      }

      return valid;
    }
}  // namespace late
