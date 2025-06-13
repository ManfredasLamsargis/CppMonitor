#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <optional>

namespace mem {
namespace concepts {
template <typename F, typename T>
concept PredicateOver = requires(F f, T &val) {
  { std::invoke(f, val) } -> std::same_as<bool>;
};
}  // namespace concepts

template <typename T>
class Monitor {
 public:
  class Window {
   public:
    enum class NotifyPolicy { notify_one, notify_all };

   private:
    Monitor &m_mon;
    std::unique_lock<std::mutex> m_lock;
    NotifyPolicy m_policy;

   public:
    Window(Monitor &mon)
        : m_mon{mon}, m_lock{mon.m_mtx}, m_policy{NotifyPolicy::notify_all} {}

    Window(Monitor &mon, NotifyPolicy policy)
        : m_mon{mon}, m_lock{mon.m_mtx}, m_policy{policy} {}

    Window(Monitor &mon, std::unique_lock<std::mutex> &&lock)
        : m_mon{mon},
          m_lock{std::move(lock)},
          m_policy{NotifyPolicy::notify_all} {}

    Window(Monitor &mon, std::unique_lock<std::mutex> &&lock,
           NotifyPolicy policy)
        : m_mon{mon}, m_lock{std::move(lock)}, m_policy{policy} {}

    Window(const Window &) = delete;

    Window &operator=(const Window &) = delete;

    T *operator->() { return &m_mon.m_cl; }

    ~Window() {
      if (m_policy == NotifyPolicy::notify_all) {
        m_mon.m_cv.notify_all();
      } else {
        m_mon.m_cv.notify_one();
      }
    }
  };

 private:
  T m_cl;
  std::mutex m_mtx;
  std::condition_variable m_cv;
  Window::NotifyPolicy m_default_policy;

 public:
  template <typename... Args>
  explicit Monitor(Args &&...args)
      : m_cl{std::forward<Args>(args)...},
        m_default_policy{Window::NotifyPolicy::notify_all} {}

  template <typename... Args>
  explicit Monitor(Args &&...args, Window::NotifyPolicy default_policy)
      : m_cl{std::forward<Args>(args)...}, m_default_policy{default_policy} {}

  Monitor(const Monitor &) = delete;
  Monitor &operator=(const Monitor &) = delete;
  Monitor(Monitor &&) = delete;
  Monitor &operator=(Monitor &&) = delete;

  Window operator->() { return Window{*this, m_default_policy}; }

  Window pause() { return Window{*this, m_default_policy}; }

  Window pause(Window::NotifyPolicy tmp_policy) {
    return Window(*this, tmp_policy);
  }

  T &get_thread_unsafe_access() { return m_cl; }

  template <typename Predicate>
    requires concepts::PredicateOver<Predicate, const T>
  Window wait_until(Predicate pred) {
    std::unique_lock<std::mutex> lock{m_mtx};
    m_cv.wait(lock, [&pred, this] { return pred(m_cl); });
    return Window{*this, std::move(lock), m_default_policy};
  }
};
}  // namespace mem