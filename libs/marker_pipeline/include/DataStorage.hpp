#pragma once

#include "Marker.hpp"

namespace late{
class DataStorage{
    public:
    auto Insert(const Marker& marker) -> void;
    auto GetMarker(const int& id) -> Marker;
    auto RemoveId(const int& id) -> void;
    auto Contains(const int& id) -> bool;

    private:
    std::unordered_map<int, Marker> m_storedMarkers;
};
}  // namespace late