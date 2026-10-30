#pragma once

#include <map>
#include <string>
#include <vector>

#include "Marker.hpp"
#include "MarkerPoseEstimator.hpp"
#include "MarkerTemplates.hpp"
#include "pipeline/Components.hpp"

namespace late {
class MarkerRecognition
    : public pipeline::IStage<CandidatePayload, FinalPayload> {
 public:
  MarkerRecognition(double markerMatchPercentage, const std::string& filePath);
  auto GetValidMarkers(const std::vector<CandidateMarker>& markers)
      -> std::vector<Marker>;

  static auto draw_markers(
      const cv::Mat& frame, const std::vector<Marker> markers
  ) -> cv::Mat;

  auto process(CandidatePayload&& payload) -> FinalPayload override;

 private:
  auto DecodeMarker(const CandidateMarker& candidateMarker, Marker& destination)
      -> bool;
  auto HammingDistance(const cv::Mat& a, const cv::Mat b) -> int;
  auto RotateMat(const cv::Mat& mat, int k) -> cv::Mat;
  auto GetCenter(const CandidateMarker& candidateMarker) -> cv::Point3f;
  auto CorrectCornerOrder(
      const std::array<cv::Point2f, 4>& corners, int bestRotation
  ) -> std::array<cv::Point2f, 4>;

 private:
  std::map<int, MarkerTemplate<6>> m_storedMarkers;
  double m_markerMatchPercentage;
  MarkerPoseEstimator m_poseEstimator;
};
}  // namespace late
