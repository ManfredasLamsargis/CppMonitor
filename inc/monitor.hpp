#ifndef LAB1_MONITOR_HPP
#define LAB1_MONITOR_HPP

#include "pch.hpp"

namespace lab1 {
template <typename T>
class Monitor {
 private:
  T m_cl;
  std::mutex m_mtx;
  std::condition_variable m_cv;

 public:
  template <typename... Args>
  Monitor(Args &&...args) : m_cl{std::forward<Args>(args)...} {}

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
  };

  Window operator->() { return Window{*this}; }

  Window pause() { return Window{*this}; }

  T &get_thread_unsafe_access() { return m_cl; }

  template <typename Predicate>
  Window wait_until(Predicate pred) {
    std::unique_lock<std::mutex> lock{m_mtx};
    m_cv.wait(lock, [&] { return pred(m_cl); });
    return Window{*this, std::move(lock)};
  }

  void inform_other() { m_cv.notify_one(); }
  void inform_others() { m_cv.notify_all(); }
};
}  // namespace lab1

#endif