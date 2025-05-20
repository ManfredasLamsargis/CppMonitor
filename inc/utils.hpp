#ifndef UTILS_HPP
#define UTILS_HPP

#include "pch.hpp"

namespace utils {
inline void random_sleep(const std::chrono::microseconds min,
                         const std::chrono::microseconds max) {
  std::random_device rnd_dev;
  std::mt19937 gen{rnd_dev()};
  std::uniform_int_distribution<> dist{static_cast<int>(min.count()),
                                       static_cast<int>(max.count())};
  std::this_thread::sleep_for(std::chrono::microseconds(dist(gen)));
}

inline void random_sleep(const std::chrono::microseconds max) {
  random_sleep({}, max);
}

template <typename functor>
inline void repeat(const std::size_t n, functor f) {
  for (size_t i = 0; i < n; i++) {
    f();
  }
}
}  // namespace utils

#endif