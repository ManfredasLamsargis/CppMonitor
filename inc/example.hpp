#ifndef LAB1_EXAMPLE_HPP
#define LAB1_EXAMPLE_HPP

#include "log.hpp"
#include "monitor.hpp"
#include "pch.hpp"
#include "utils.hpp"

namespace lab1::example {

inline void write_task(Monitor<std::vector<std::string>> &mon,
                       const std::string &hash, const size_t ntimes) {
  std::stringstream ss{};
  utils::repeat(ntimes, [&ss, &hash, &mon]() {
    ss << '{' << hash << '}';
    utils::thd_rnd_sleep_us(std::chrono::microseconds(100));
    mon->push_back(ss.str());
    ss.str(std::string{});
    mon.inform_others();
  });
}

inline void read_task(Monitor<std::vector<std::string>> &mon,
                      const std::string &hash, const size_t ntimes) {
  size_t count{ntimes};
  auto vec_is_empty = [](const std::vector<std::string> &vec) -> bool {
    return !vec.empty();
  };
  while (count > 0) {
    auto vec{mon.wait_until(vec_is_empty)};
    spdlog::debug("{}: {}", hash, vec->back());
    vec->pop_back();
    mon.inform_others();
    count--;
  }
}
}  // namespace lab1::example

#endif