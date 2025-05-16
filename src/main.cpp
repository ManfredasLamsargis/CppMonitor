#include "log.hpp"
#include <algorithm>
#include <mutex>
#include <random>
#include <source_location>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

template <typename T> class monitor {
private:
  /**
   * @brief Monitored member client.
   *
   */
  T m_cl;

  /**
   * @brief Mutex.
   *
   */
  std::mutex m_mtx;

public:
  template <typename... Args>
  monitor(Args &&...args) : m_cl{std::forward<Args>(args)...} {}

  /**
   * @brief Allows access to underlying member client in the monitor class while
   * locking other threads out.
   *
   * A helper object which locks data within monitor and then unlocks the
   * monitor's mutex on it's own destruction around a function call.
   *
   */
  class window {
  private:
    /**
     * @brief Reference to the Monitor the Window belongs to.
     *
     */
    monitor &m_mon;

    /**
     * @brief Unique lock of the Monitor's mutex.
     *
     */
    std::unique_lock<std::mutex> m_lock;

  public:
    window(monitor &mon) : m_mon{mon}, m_lock(mon.m_mtx) {}
    window(const window &) = delete;
    window &operator=(const window &) = delete;
    ~window() {}
    T &operator*() { return m_mon.m_cl; }
    T *operator->() { return &m_mon.m_cl; }
  };

  window operator->() { return window(*this); }
  window pause() { return window(*this); }
  T &get_thread_unsafe_access() { return m_cl; }
};

void rnd_sleep_ms(const std::chrono::microseconds max) {
  std::random_device rnd_dev;
  std::mt19937 gen{rnd_dev()};
  std::uniform_int_distribution<> dist{0, static_cast<int>(max.count())};
  std::this_thread::sleep_for(std::chrono::microseconds(dist(gen)));
}

template <typename functor> void repeat(const std::size_t n, functor f) {
  for (size_t i = 0; i < n; i++) {
    f();
  }
}

void task(monitor<std::vector<std::string>> &mon, const std::string &hash,
          const size_t ntimes) {
  std::stringstream ss{};
  repeat(ntimes, [&ss, &hash, &mon]() {
    ss << '{' << hash << '}';
    rnd_sleep_ms(std::chrono::microseconds(10));
    mon->push_back(ss.str());
    ss.str(std::string{});
  });
}

int main() {
  utils::clog::set();

  monitor<std::vector<std::string>> mon;

  constexpr size_t thd_cnt{3};
  constexpr size_t repeat_ntimes{5};
  std::array<std::thread, thd_cnt> thds;
  for (size_t i = 0; i < thds.size(); ++i) {
    thds.at(i) = std::thread{task, std::ref(mon), std::format("thread_{}", i),
                             repeat_ntimes};
  }
  std::for_each(thds.begin(), thds.end(), [](std::thread &thd) { thd.join(); });

  const std::vector<std::string> &vec{mon.get_thread_unsafe_access()};
  std::for_each(vec.begin(), vec.end(),
                [](const std::string_view sv) { spdlog::trace("{}", sv); });
  spdlog::debug("Total count: {}", vec.size());
  return EXIT_SUCCESS;
}