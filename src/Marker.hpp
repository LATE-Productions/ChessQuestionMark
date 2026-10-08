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

    struct Marker {
        PieceType type;
        PieceColor color;
        cv::Point3f position;
    };
}