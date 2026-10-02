#include <optional>
#include <unordered_map>
#include "MarkerRecognition.hpp"

namespace late {
void MarkerRecognition::recognizeMarker(int markerId) {
    auto it = markerDictionary.find(markerId);
    if (it != markerDictionary.end()) {
        return it->second;
    } else {
        return std::nullopt;
    }
}

void MarkerRecognition::initializeMarkerDictionary() {
    markerDictionary = {
        {0, {PieceType::Board, PieceColor::White}},
        {1, {PieceType::Pawn, PieceColor::White}},
        {2, {PieceType::Rook, PieceColor::White}},
        {3, {PieceType::Knight, PieceColor::White}},
        {4, {PieceType::Bishop, PieceColor::White}},
        {5, {PieceType::Queen, PieceColor::White}},
        {6, {PieceType::King, PieceColor::White}},
        {7, {PieceType::Pawn, PieceColor::Black}},
        {8, {PieceType::Rook, PieceColor::Black}},
        {9, {PieceType::Knight, PieceColor::Black}},
        {10, {PieceType::Bishop, PieceColor::Black}},
        {11, {PieceType::Queen, PieceColor::Black}},
        {12, {PieceType::King, PieceColor::Black}}
    };
}
}