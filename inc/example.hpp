#ifndef LAB1_EXAMPLE_HPP
#define LAB1_EXAMPLE_HPP

#include "log.hpp"
#include "monitor.hpp"
#include "pch.hpp"
#include "utils.hpp"

namespace lab1::example {

inline void task(monitor<std::vector<std::string>> &mon,
                 const std::string &hash, const size_t ntimes) {
  std::stringstream ss{};
  utils::repeat(ntimes, [&ss, &hash, &mon]() {
    ss << '{' << hash << '}';
    utils::thd_rnd_sleep_us(std::chrono::microseconds(100));
    mon->push_back(ss.str());
    ss.str(std::string{});
  });
}
} // namespace lab1::example

#endif