#include <thread>

#include "example.hpp"

int main() {
  std::vector<std::string> vec{};
  Monitor<std::vector<std::string>> mon{vec};
  std::jthread writer_0{ex::write_task, std::ref(mon), "a", 5};
  std::jthread writer_1{ex::write_task, std::ref(mon), "b", 5};
  std::jthread writer_2{ex::write_task, std::ref(mon), "c", 2};
  std::jthread reader{ex::read_task, std::ref(mon), "reader", 12};
  return 0;
}