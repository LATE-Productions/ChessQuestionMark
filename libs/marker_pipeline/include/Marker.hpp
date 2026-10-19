#pragma once
#include <opencv2/core.hpp>

namespace late {

    enum class PieceType {
        Board,
        Pawn,
        Rook,
        Knight,
        Bishop,
        Queen,
        King
    };

    enum class PieceColor {
        White,
        Black
    };

    struct CandidateMarker {
        PieceType type;
        PieceColor color;
        cv::Point3f position;
    };

    class Marker {

        public:

            std::array<cv::Point2f, 4> m_corners;
            std::vector<std::vector<cv::Point>> m_contours;
            cv::Rect m_bounding_box;
            cv::Mat markerImage;
            std::size_t m_markerId;
            std::size_t m_markerTemplateId;
    };
}