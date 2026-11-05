#pragma once

#include <unordered_map>
#include <vector>

#include "Marker.hpp"
#include "pipeline/Components.hpp"

namespace late {
class DataStorage : public pipeline::IStage<FinalPayload, std::vector<Marker>> {
 public:
  DataStorage(float maxFrameMovement);
  auto Insert(Marker& marker) -> void;
  auto GetAllMarkers() -> std::vector<Marker>;
  auto RemoveMarker(const Marker& marker) -> void;
  auto process(FinalPayload&& payload) -> std::vector<Marker> override;

 private:
  auto GetInstanceId(const Marker& marker) -> uint64_t;
  std::vector<std::vector<Marker>> m_markerVector;
  std::unordered_map<std::uint64_t, Marker> m_storedMarkers;
  std::uint64_t m_nextInstanceId = 1;  // Start at 1 to avoid removing 0 if
                                       // remove marker hasn't been assign yet
  float m_maxFrameMovement;
};
}  // namespace late
