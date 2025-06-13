#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>

namespace mem {
namespace concepts {
template <typename F, typename T>
concept PredicateOver = requires(F f, T &t) {
  { std::invoke(f, t) } -> std::same_as<bool>;
};
template <typename F, typename T>
concept ActionOn = requires(F f, T &t) {
  { std::invoke(f, t) } -> std::same_as<void>;
};
}  // namespace concepts

template <typename T>
class Monitor {
 public:
  class Window {
   public:
    enum class NotifyPolicy { none, notify_one, notify_all };

   private:
    Monitor &m_monitor_ref;
    std::unique_lock<std::mutex> m_lock;
    NotifyPolicy m_notify_policy;

   public:
    Window(Monitor &mon, NotifyPolicy notify_policy)
        : m_monitor_ref{mon},
          m_lock{mon.m_mutex},
          m_notify_policy{notify_policy} {}

    Window(Monitor &mon, std::unique_lock<std::mutex> &&lock,
           NotifyPolicy notify_policy)
        : m_monitor_ref{mon},
          m_lock{std::move(lock)},
          m_notify_policy{notify_policy} {}

    Window(Window &&) noexcept = default;
    Window &operator=(Window &&) noexcept = default;

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    T *operator->() { return &m_monitor_ref.m_shared_resource; }

    bool owns_resource() const noexcept { return m_lock.owns_lock(); }

    template <typename F>
      requires concepts::ActionOn<F, T>
    Window &&then(F f) {
      f(this->m_monitor_ref.m_shared_resource);
      return std::move(*this);
    }

    template <typename F>
      requires concepts::ActionOn<F, T>
    void finally(F f) {
      f(this->m_monitor_ref.m_shared_resource);
    }

    ~Window() {
      if (!m_lock.owns_lock()) {
        return;
      }
      m_lock.unlock();
      switch (m_notify_policy) {
        case NotifyPolicy::none:
          return;
        case NotifyPolicy::notify_one:
          m_monitor_ref.m_convar.notify_one();
          return;
        case NotifyPolicy::notify_all:
          m_monitor_ref.m_convar.notify_all();
          return;
      }
    }
  };

 private:
  T m_shared_resource;
  std::mutex m_mutex;
  std::condition_variable m_convar;
  Window::NotifyPolicy m_def_notify_policy;

 public:
  template <typename... Args>
  explicit Monitor(Args &&...args)
      : m_shared_resource{std::forward<Args>(args)...},
        m_def_notify_policy{Window::NotifyPolicy::notify_all} {}

  explicit Monitor(Window::NotifyPolicy default_notify_policy)
      : m_shared_resource{}, m_def_notify_policy{default_notify_policy} {}

  Monitor(const Monitor &) = delete;
  Monitor &operator=(const Monitor &) = delete;
  Monitor(Monitor &&) = delete;
  Monitor &operator=(Monitor &&) = delete;

  Window operator->() { return Window{*this, m_def_notify_policy}; }

  Window pause() { return Window{*this, m_def_notify_policy}; }

  Window pause(Window::NotifyPolicy notify_policy) {
    return Window(*this, notify_policy);
  }

  T &get_thread_unsafe_access() { return m_shared_resource; }

 private:
  template <typename Predicate>
    requires concepts::PredicateOver<Predicate, const T>
  Window wait_handle(Predicate pred, Window::NotifyPolicy notify_policy) {
    std::unique_lock<std::mutex> lock{m_mutex};
    m_convar.wait(lock, [&pred, this] { return pred(m_shared_resource); });
    return Window{*this, std::move(lock), notify_policy};
  }

 public:
  template <typename Predicate>
    requires concepts::PredicateOver<Predicate, const T>
  Window wait(Predicate pred) {
    return wait_handle(pred, m_def_notify_policy);
  }

  template <typename Predicate>
    requires concepts::PredicateOver<Predicate, const T>
  Window wait(Predicate pred, Window::NotifyPolicy notify_policy) {
    return wait_handle(pred, notify_policy);
  }
};
}  // namespace mem