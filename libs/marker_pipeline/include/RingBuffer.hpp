#pragma once
#include <array>
#include <atomic>
#include <cstddef>
#include <optional>
#include <utility>

namespace late {
enum class OverflowPolicy { DROP, OVERWRITE };

template <typename T>
class IBuffer {
 public:
  virtual ~IBuffer()                      = default;
  virtual auto write(T&& data) -> void    = 0;
  virtual auto read() -> std::optional<T> = 0;
};

template <typename T, size_t size, OverflowPolicy policy>
class RingBuffer : public IBuffer<T> {
  static_assert(
      (size & (size - 1)) == 0 && size > 0, "size must be a power of 2"
  );

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
  using size_atomic = std::atomic<std::size_t>;
  std::array<T, size> m_buffer{};
  std::atomic<std::size_t> m_idxRead{0};
  std::atomic<std::size_t> m_idxWrite{0};
};

template <typename T, size_t size, OverflowPolicy policy>
auto RingBuffer<T, size, policy>::write(T&& data) -> void {
  const std::size_t w = m_idxWrite.load(std::memory_order_relaxed);

  if constexpr (policy == OverflowPolicy::DROP) {
    const std::size_t r = m_idxRead.load(std::memory_order_acquire);
    if ((w - r) == size) return;
  }

  m_buffer[w & (size - 1)] = std::move(data);
  m_idxWrite.store(w + 1, std::memory_order_release);
};

template <typename T, size_t size, OverflowPolicy policy>
auto RingBuffer<T, size, policy>::read() -> std::optional<T> {
  std::size_t r       = m_idxRead.load(std::memory_order_relaxed);
  const std::size_t w = m_idxWrite.load(std::memory_order_acquire);

  if (w == r) return std::nullopt;
  if constexpr (policy == OverflowPolicy::OVERWRITE) {
    if (w - r > size) {
      r = w - size;
      m_idxRead.store(r, std::memory_order_relaxed);
    }
  }
  std::optional<T> data = std::move(m_buffer[r & (size - 1)]);
  m_idxRead.store(r + 1, std::memory_order_release);
  return data;
};
}  // namespace late
