#include "example.hpp"
#include "log.hpp"
#include "monitor.hpp"
#include "pch.hpp"

int main() {
  using lab1::Monitor;
  using lab1::example::task;

  utils::clog::set();

  Monitor<std::vector<std::string>> mon;

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
  spdlog::debug("Expected total count: {}, actual: {}", thd_cnt * repeat_ntimes,
                vec.size());
  return EXIT_SUCCESS;
}