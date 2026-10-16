#pragma once
#include <array>
#include <cstddef>
#include <mutex>
#include <optional>
#include <semaphore>
#include <type_traits>

#include "Utilities.hpp"

namespace late {
enum class OverflowPolicy { DROP, OVERWRITE };

template <OverflowPolicy policy, size_t size>
using conditional_semaphore = std::conditional_t<
    policy == OverflowPolicy::DROP, std::counting_semaphore<size>, Empty>;

template <typename T>
class IBuffer {
 public:
  virtual ~IBuffer()                      = default;
  virtual auto write(T&& data) -> void    = 0;
  virtual auto read() -> std::optional<T> = 0;
};

template <typename T, size_t size, OverflowPolicy policy>
class RingBuffer : public IBuffer<T> {
 public:
  RingBuffer()                                                      = default;
  RingBuffer(const RingBuffer<T, size, policy>& buffer)             = delete;
  RingBuffer& operator=(const RingBuffer<T, size, policy>& buffer)  = delete;
  RingBuffer(const RingBuffer<T, size, policy>&& buffer)            = delete;
  RingBuffer& operator=(const RingBuffer<T, size, policy>&& buffer) = delete;
  ~RingBuffer()                                                     = default;

  auto write(T&& data) -> void;
  auto read() -> std::optional<T>;

 private:
  static auto make_conditional_semaphore()
      -> conditional_semaphore<policy, size> {
    if constexpr (policy != OverflowPolicy::OVERWRITE)
      return std::counting_semaphore<size>(size);
    else
      return Empty{};
  }

 private:
  std::array<T, size> m_buffer{};
  size_t m_readerHead = 0;
  size_t m_writerHead = 0;
  std::mutex m_mutex{};
  std::counting_semaphore<size> m_filled{0};
  [[no_unique_address]] conditional_semaphore<policy, size> m_empty =
      make_conditional_semaphore();
};

template <typename T, size_t size, OverflowPolicy policy>
auto RingBuffer<T, size, policy>::write(T&& data) -> void {
  if constexpr (policy == OverflowPolicy::DROP) {
    if (!m_empty.try_acquire()) return;
  }

  {
    std::lock_guard lock{m_mutex};
    m_buffer[m_writerHead] = std::move(data);
    m_writerHead           = (m_writerHead + 1) % m_buffer.max_size();
  }

  m_filled.release();
};

template <typename T, size_t size, OverflowPolicy policy>
auto RingBuffer<T, size, policy>::read() -> std::optional<T> {
  if (!m_filled.try_acquire()) return std::nullopt;

  std::optional<T> data;
  {
    std::lock_guard lock{m_mutex};
    data         = std::move(m_buffer[m_readerHead]);
    m_readerHead = (m_readerHead + 1) % m_buffer.max_size();
  }

  if constexpr (policy == OverflowPolicy::DROP) {
    m_empty.release();
  }
  return data;
};
}  // namespace late
