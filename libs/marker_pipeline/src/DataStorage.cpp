#include "DataStorage.hpp"

#include <print>

namespace late{
auto DataStorage::Insert(const Marker& marker) -> void{
    m_storedMarkers[marker.m_markerId] = marker;
    std::print("Sucessfuly added marker {0}\n", marker.m_markerId);
};

auto DataStorage::GetMarker(const int& id) -> Marker{
    std::print("Returning marker {0}\n", id);
    return m_storedMarkers.at(id);
};

auto DataStorage::RemoveId(const int& id) -> void{
    m_storedMarkers.erase(id);
    std::print("Sucessfuly removed marker {0}\n", id);
};

auto DataStorage::Contains(const int& id) -> bool{
    std::print("Checking if storage contains marker id {0}\n", id);
    return m_storedMarkers.contains(id);
};
}