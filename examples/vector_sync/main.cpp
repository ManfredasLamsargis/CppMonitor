#include <thread>

#include "example.hpp"

int main() {
  constexpr std::size_t repeat_n{5};
  Monitor<std::vector<std::string>> mon;
  std::jthread writer{ex::write_task, std::ref(mon), "writer", repeat_n};
  std::jthread reader{ex::read_task, std::ref(mon), "reader", repeat_n};
  return 0;
}