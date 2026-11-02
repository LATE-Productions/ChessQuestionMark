#include "DataStorage.hpp"

#include <cstdint>
#include <limits>
#include <opencv2/opencv.hpp>

namespace late {
DataStorage::DataStorage(float maxFrameMovement)
    : m_maxFrameMovement(maxFrameMovement) {}

auto DataStorage::Insert(Marker& marker) -> void {
  marker.m_instanceId = GetInstanceId(marker);
  m_storedMarkers.insert_or_assign(marker.m_instanceId, marker);
};

auto DataStorage::GetAllMarkers() -> std::vector<Marker> {
  // std::print("Returning markers\n");
  std::vector<Marker> markers;

  for (const auto& pair : m_storedMarkers) {
    markers.push_back(pair.second);
  }

  return markers;
};

auto DataStorage::RemoveMarker(const Marker& marker) -> void {
  m_storedMarkers.erase(marker.m_instanceId);
};

auto DataStorage::GetInstanceId(const Marker& marker) -> uint64_t {
  std::vector<uint64_t> matchingIds;

  for (const auto& [instanceId, storedMarker] : m_storedMarkers) {
    if (storedMarker.m_markerTypeId == marker.m_markerTypeId) {
      matchingIds.push_back(instanceId);
    }
  }

  if (matchingIds.size() == 0) {
    return m_nextInstanceId++;
  }

  std::uint64_t bestInstanceId;
  double shortestDistance = std::numeric_limits<int>::max();

  for (const uint64_t& index : matchingIds) {
    cv::Point3f point1   = marker.m_markerPos;
    cv::Point3f point2   = m_storedMarkers[index].m_markerPos;
    double movedDistance = cv::norm(point1 - point2);
    if (movedDistance < shortestDistance) {
      shortestDistance = movedDistance;
      bestInstanceId   = index;
    }
  }

  if (shortestDistance < m_maxFrameMovement) {
    return bestInstanceId;
  }

  return m_nextInstanceId++;
};
auto DataStorage::process(FinalPayload&& payload) -> std::vector<Marker> {
  m_storedMarkers.clear();
  
  for (auto& marker : payload.m_markers) {
    Insert(marker);
  }
  return GetAllMarkers();
}
}  // namespace late
