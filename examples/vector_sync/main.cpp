#include <thread>

#include "example.hpp"

int main() {
  constexpr std::size_t repeat_ntimes{5};
  Monitor<std::vector<std::string>> monitor;
  std::jthread writer{ex::write_task, std::ref(monitor), "writer",
                      repeat_ntimes};
  std::jthread reader{ex::read_task, std::ref(monitor), "reader",
                      repeat_ntimes};
  return 0;
}