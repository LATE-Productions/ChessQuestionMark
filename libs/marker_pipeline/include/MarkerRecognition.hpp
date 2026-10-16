#pragma once

#include <optional>
#include <unordered_map>

#include "Marker.hpp"
#include "MarkerRectification.hpp"

namespace late {
class MarkerRecognition {
 public:
  std::optional<std::pair<PieceType, PieceColor>> recognizeMarker(int markerId);

 private:
  void initializeMarkerDictionary();
  std::unordered_map<int, std::pair<PieceType, PieceColor>> markerDictionary;
};
}  // namespace late
