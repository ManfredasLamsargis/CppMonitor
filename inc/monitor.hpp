#ifndef LAB1_MONITOR_HPP
#define LAB1_MONITOR_HPP

#include "pch.hpp"

namespace lab1 {
template <typename T> class monitor {
private:
  T m_cl;
  std::mutex m_mtx;

public:
  template <typename... Args>
  monitor(Args &&...args) : m_cl{std::forward<Args>(args)...} {}

  class window {
  private:
    monitor &m_mon;
    std::unique_lock<std::mutex> m_lock;

  public:
    window(monitor &mon) : m_mon{mon}, m_lock{mon.m_mtx} {}
    window(const window &) = delete;
    window &operator=(const window &) = delete;
    T *operator->() { return &m_mon.m_cl; }
  };

  window operator->() { return window(*this); }

  window pause() { return window(*this); }

  T &get_thread_unsafe_access() { return m_cl; }
};
} // namespace lab1

#endif