#include "example.hpp"

int main() {
  std::vector<std::string> vec{};
  Monitor<std::vector<std::string>> mon{vec};
  std::thread writer_0{ex::write_task, std::ref(mon), "a", 5};
  std::thread writer_1{ex::write_task, std::ref(mon), "b", 5};
  std::thread writer_2{ex::write_task, std::ref(mon), "c", 2};
  std::thread reader{ex::read_task, std::ref(mon), "reader", 12};
  writer_0.join();
  writer_1.join();
  writer_2.join();
  reader.join();
  return 0;
}