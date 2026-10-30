#pragma once
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <vector>

namespace late {

enum class PieceType { Board, Pawn, Rook, Knight, Bishop, Queen, King };

enum class PieceColor { White, Black };

struct GodotMarker {
  int m_typeId;
  int m_instanceId;
  float m_posX, m_posY, m_posZ;
  float m_rotX, m_rotY, m_rotZ;
};

struct Marker {
  std::uint64_t m_instanceId;
  int m_markerTypeId;
  cv::Vec3f m_markerRotation;
  cv::Point3f m_markerPos;
  float m_apperentSize;
  float m_estimatedDistance;
  cv::Mat m_markerImage;
};

struct CandidateMarker {
  std::array<cv::Point2f, 4> m_corners;
  std::vector<std::vector<cv::Point>> m_contours;
  cv::Rect m_boundingBox;
  cv::Mat m_markerImage;
  cv::Vec3f m_markerRotation;
  cv::Point3f m_markerPos;
  float m_apperentSize;
  float m_estimatedDistance;
  std::size_t m_markerId;
  std::size_t m_markerTemplateId;
};

struct ImagePayload {
  cv::Mat m_binaryFrame;
  cv::Mat m_greyscaleFrame;
};
struct CandidatePayload {
  cv::Mat m_frame;
  std::vector<CandidateMarker> m_markers;
};
struct FinalPayload {
  cv::Mat m_frame;
  std::vector<Marker> m_markers;
};
}  // namespace late
