#pragma once

#include <format>
#include <sstream>
#include <string>
#include <vector>

#include "monitor.hpp"
#include "utils.hpp"

using mem::Monitor;
namespace ex {
void write_task(Monitor<std::vector<std::string>> &mon, const std::string &hash,
                const std::size_t ntimes) {
  std::stringstream ss{};
  utils::repeat(ntimes, [&ss, &hash, &mon] {
    ss << '{' << hash << '}';
    std::chrono::duration max_time{std::chrono::seconds(1)};
    utils::random_sleep(max_time);
    mon->push_back(ss.str());
    ss.str(std::string{});
  });
}

void read_task(Monitor<std::vector<std::string>> &mon, const std::string &hash,
               const std::size_t ntimes) {
  auto vec_not_empty = [](const std::vector<std::string> &vec) -> bool {
    return !vec.empty();
  };
  utils::repeat(ntimes, [&mon, &hash, &vec_not_empty] {
    std::chrono::duration max_time{std::chrono::seconds(1)};
    utils::random_sleep(max_time);
    auto vec{mon.wait_until(vec_not_empty)};
    utils::println(std::format("{}: {}", hash, vec->back()));
    vec->pop_back();
  });
}
}