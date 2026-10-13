#pragma once
#include <array>
#include <mutex>
#include <optional>
#include <semaphore>
#include <type_traits>

namespace late {
enum class OverflowPolicy { DROP, OVERWRITE };

struct NoSem {};
template <OverflowPolicy policy, size_t size>
using Sem = std::conditional_t<
    policy == OverflowPolicy::DROP, std::counting_semaphore<size>, NoSem>;

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
  auto write(T&& data) -> void {
    if constexpr (policy == OverflowPolicy::DROP) {
      if (!m_empty.try_acquire()) return;
    }

    std::lock_guard lock{m_mutex};
    m_buffer[m_writerHead] = std::move(data);
    m_writerHead           = (m_writerHead + 1) % m_buffer.max_size();
    m_filled.release();
  };
  auto read() -> std::optional<T> {
    if (!m_filled.try_acquire()) return std::nullopt;

    std::lock_guard lock{m_mutex};
    auto data    = std::move(m_buffer[m_readerHead]);
    m_readerHead = (m_readerHead + 1) % m_buffer.max_size();

    if constexpr (policy == OverflowPolicy::DROP) {
      m_empty.release();
    }
    return data;
  };

 private:
  static auto make_empty_sem() -> Sem<policy, size> {
    if constexpr (policy != OverflowPolicy::OVERWRITE)
      return std::counting_semaphore<size>(size);
    else
      return NoSem{};
  }

 private:
  std::array<T, size> m_buffer{};
  size_t m_readerHead = 0;
  size_t m_writerHead = 0;
  std::mutex m_mutex{};
  std::counting_semaphore<size> m_filled{0};
  [[no_unique_address]] Sem<policy, size> m_empty = make_empty_sem();
};
}  // namespace late
