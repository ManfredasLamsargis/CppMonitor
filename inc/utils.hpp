#ifndef UTILS_HPP
#define UTILS_HPP

#include "pch.hpp"

namespace utils {
inline void random_sleep(const std::chrono::microseconds min,
                         const std::chrono::microseconds max) {
  std::random_device rnd_dev;
  std::mt19937 gen{rnd_dev()};
  std::uniform_int_distribution<std::size_t> dist{
      static_cast<std::size_t>(min.count()),
      static_cast<std::size_t>(max.count())};
  std::this_thread::sleep_for(std::chrono::microseconds(dist(gen)));
}

inline void random_sleep(const std::chrono::microseconds max) {
  random_sleep({}, max);
}

template <typename F>
concept VoidFunction = requires(F f) {
  { std::invoke(f) } -> std::same_as<void>;
};

template <typename F>
  requires VoidFunction<F>
inline void repeat(const std::size_t n, F f) {
  for (std::size_t i = 0; i < n; i++) {
    f();
  }
}

inline void println(const std::string_view sv) { std::cout << sv << std::endl; }
}  // namespace utils

#endif