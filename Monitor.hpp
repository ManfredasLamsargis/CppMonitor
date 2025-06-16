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

template <typename F, typename T>
concept ReadOnlyActionOn = requires(F f, const T &t) {
  { std::invoke(f, t) } -> std::same_as<void>;
};
}  // namespace concepts

template <typename T>
class Monitor {
 public:
  class AccessGuard {
   public:
    enum class NotifyPolicy { none, notify_one, notify_all };

   private:
    Monitor &m_monitor_ref;
    std::unique_lock<std::mutex> m_lock;
    NotifyPolicy m_notify_policy;

   public:
    explicit AccessGuard(Monitor &mon, NotifyPolicy notify_policy)
        : m_monitor_ref{mon},
          m_lock{mon.m_mutex},
          m_notify_policy{notify_policy} {}

    explicit AccessGuard(Monitor &mon, std::unique_lock<std::mutex> &&lock,
                         NotifyPolicy notify_policy)
        : m_monitor_ref{mon},
          m_lock{std::move(lock)},
          m_notify_policy{notify_policy} {}

    AccessGuard(AccessGuard &&) noexcept = default;
    AccessGuard &operator=(AccessGuard &&) noexcept = default;

    AccessGuard(const AccessGuard &) = delete;
    AccessGuard &operator=(const AccessGuard &) = delete;

    T &operator*() { return m_monitor_ref.m_shared_resource; }
    const T &operator*() const { return m_monitor_ref.m_shared_resource; }

    T *operator->() { return &m_monitor_ref.m_shared_resource; }
    const T *operator->() const { return &m_monitor_ref.m_shared_resource; }

    T &resource() { return m_monitor_ref.m_shared_resource; }
    const T &resource() const { return m_monitor_ref.m_shared_resource; }

    bool owns_resource() const noexcept { return m_lock.owns_lock(); }
    explicit operator bool() const noexcept { return owns_resource(); }

    template <typename F>
      requires concepts::ActionOn<F, T>
    void execute(F f) && {
      f(this->m_monitor_ref.m_shared_resource);
    }

    template <typename F>
      requires concepts::ActionOn<F, T>
    [[nodiscard(
        "Chainable method used as a terminal operation. Consider using "
        "execute() "
        "or before_release().")]] AccessGuard &&
    then(F f) && {
      f(this->m_monitor_ref.m_shared_resource);
      return std::move(*this);
    }

    template <typename F>
      requires concepts::ReadOnlyActionOn<F, T>
    void check(F f) && {
      f(this->m_monitor_ref.m_shared_resource);
    }

    template <typename F>
      requires concepts::ReadOnlyActionOn<F, T>
    [[nodiscard(
        "Chainable method used as a terminal operation. Consider using "
        "check().")]] AccessGuard &&
    then_check(F f) && {
      f(this->m_monitor_ref.m_shared_resource);
      return std::move(*this);
    }

    template <typename F>
      requires concepts::ActionOn<F, T>
    void before_release(F f) && {
      f(this->m_monitor_ref.m_shared_resource);
    }

    void release() && {}

    ~AccessGuard() {
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
  AccessGuard::NotifyPolicy m_def_notify_policy;

 public:
  template <typename... Args>
  explicit Monitor(Args &&...args)
      : m_shared_resource{std::forward<Args>(args)...},
        m_def_notify_policy{AccessGuard::NotifyPolicy::notify_all} {}

  explicit Monitor(AccessGuard::NotifyPolicy default_notify_policy)
      : m_shared_resource{}, m_def_notify_policy{default_notify_policy} {}

  Monitor(const Monitor &) = delete;
  Monitor &operator=(const Monitor &) = delete;
  Monitor(Monitor &&) = delete;
  Monitor &operator=(Monitor &&) = delete;

  AccessGuard operator->() { return AccessGuard{*this, m_def_notify_policy}; }

  AccessGuard acquire() { return AccessGuard{*this, m_def_notify_policy}; }

  AccessGuard acquire(AccessGuard::NotifyPolicy notify_policy) {
    return AccessGuard(*this, notify_policy);
  }

  T &get_thread_unsafe_access() { return m_shared_resource; }

  AccessGuard::NotifyPolicy default_notify_policy() const {
    return m_def_notify_policy;
  }

 private:
  template <typename Predicate>
    requires concepts::PredicateOver<Predicate, const T>
  AccessGuard wait_handle(Predicate pred,
                          AccessGuard::NotifyPolicy notify_policy) {
    std::unique_lock<std::mutex> lock{m_mutex};
    m_convar.wait(lock, [&pred, this] { return pred(m_shared_resource); });
    return AccessGuard{*this, std::move(lock), notify_policy};
  }

 public:
  template <typename Predicate>
    requires concepts::PredicateOver<Predicate, const T>
  AccessGuard acquire_when(Predicate pred) {
    return wait_handle(pred, m_def_notify_policy);
  }

  template <typename Predicate>
    requires concepts::PredicateOver<Predicate, const T>
  AccessGuard acquire_when(Predicate pred,
                           AccessGuard::NotifyPolicy notify_policy) {
    return wait_handle(pred, notify_policy);
  }
};
}  // namespace mem