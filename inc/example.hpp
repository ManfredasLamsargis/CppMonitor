#ifndef LAB1_EXAMPLE_HPP
#define LAB1_EXAMPLE_HPP

#include "log.hpp"
#include "monitor.hpp"
#include "pch.hpp"
#include "utils.hpp"

namespace mem::example {
inline void write_task(Monitor<std::vector<std::string>> &mon,
                       const std::string &hash, const std::size_t ntimes) {
  std::stringstream ss{};
  utils::repeat(ntimes, [&ss, &hash, &mon] {
    ss << '{' << hash << '}';
    std::chrono::duration max_time{std::chrono::seconds(2)};
    utils::random_sleep(max_time);
    mon->push_back(ss.str());
    ss.str(std::string{});
  });
}

inline void read_task(Monitor<std::vector<std::string>> &mon,
                      const std::string &hash, const std::size_t ntimes) {
  auto vec_not_empty = [](const std::vector<std::string> &vec) -> bool {
    return !vec.empty();
  };
  utils::repeat(ntimes, [&mon, &hash, &vec_not_empty] {
    std::chrono::duration max_time{std::chrono::seconds(2)};
    utils::random_sleep(max_time);
    auto vec{mon.wait_until(vec_not_empty)};
    spdlog::debug("{}: {}", hash, vec->back());
    vec->pop_back();
  });
}
}  // namespace mem::example

#endif