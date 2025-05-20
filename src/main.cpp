#include "example.hpp"
#include "log.hpp"
#include "monitor.hpp"
#include "pch.hpp"

int main() {
  using mem::Monitor;
  using mem::example::read_task;
  using mem::example::write_task;

  utils::clog::set();

  std::vector<std::string> vec{};
  Monitor<std::vector<std::string>> mon{vec};
  std::thread writer_0{write_task, std::ref(mon), "a", 5};
  std::thread writer_1{write_task, std::ref(mon), "b", 5};
  std::thread writer_2{write_task, std::ref(mon), "c", 2};
  std::thread reader{read_task, std::ref(mon), "reader", 12};
  writer_0.join();
  writer_1.join();
  writer_2.join();
  reader.join();
  auto shared_ptr{std::make_shared<Monitor<std::vector<std::string>>>()};
  return EXIT_SUCCESS;
}