#include "example.hpp"
#include "log.hpp"
#include "monitor.hpp"
#include "pch.hpp"

int main() {
  using lab1::Monitor;
  using lab1::example::read_task;
  using lab1::example::write_task;

  utils::clog::set();

  Monitor<std::vector<std::string>> mon;
  std::thread writer{write_task, std::ref(mon), "writer_0", 10};
  std::thread reader{read_task, std::ref(mon), "reader_0", 10};
  writer.join();
  reader.join();
  return EXIT_SUCCESS;
}