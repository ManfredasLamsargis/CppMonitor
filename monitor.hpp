#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>

namespace mem {
template <typename F, typename T>
concept PredicateOver = requires(F f, T &val) {
  { std::invoke(f, val) } -> std::same_as<bool>;
};

template <typename T>
class Monitor {
 private:
  T m_cl;
  std::mutex m_mtx;
  std::condition_variable m_cv;

 public:
  template <typename... Args>
  explicit Monitor(Args &&...args) : m_cl{std::forward<Args>(args)...} {}

  Monitor(const Monitor &) = delete;
  Monitor &operator=(const Monitor &) = delete;
  Monitor(Monitor &&) = delete;
  Monitor &operator=(Monitor &&) = delete;

  class Window {
   private:
    Monitor &m_mon;
    std::unique_lock<std::mutex> m_lock;

   public:
    Window(Monitor &mon) : m_mon{mon}, m_lock{mon.m_mtx} {}

    Window(Monitor &mon, std::unique_lock<std::mutex> &&lock)
        : m_mon{mon}, m_lock{std::move(lock)} {}

    Window(const Window &) = delete;

    Window &operator=(const Window &) = delete;

    T *operator->() { return &m_mon.m_cl; }

    ~Window() { m_mon.m_cv.notify_all(); }
  };

  Window operator->() { return Window{*this}; }

  Window pause() { return Window{*this}; }

  T &get_thread_unsafe_access() { return m_cl; }

  template <typename Predicate>
    requires PredicateOver<Predicate, const T>
  Window wait_until(Predicate pred) {
    std::unique_lock<std::mutex> lock{m_mtx};
    m_cv.wait(lock, [&pred, this] { return pred(m_cl); });
    return Window{*this, std::move(lock)};
  }
};
}  // namespace mem