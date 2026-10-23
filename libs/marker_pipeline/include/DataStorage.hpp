#pragma once

#include <unordered_map>

#include "Marker.hpp"

namespace late{
class DataStorage{
    public:
    DataStorage(float maxFrameMovement);
    auto Insert(Marker& marker) -> void;
    auto GetAllMarkers() -> std::vector<Marker>;
    auto RemoveMarker(const Marker& marker) -> void;

    private:
    auto GetInstanceId(const Marker& marker) -> uint64_t;
    std::unordered_map<std::uint64_t, Marker> m_storedMarkers;
    std::uint64_t m_nextInstanceId = 1; // Start at 1 to avoid removing 0 if remove marker hasn't been assign yet
    float m_maxFrameMovement;
};
}  // namespace late