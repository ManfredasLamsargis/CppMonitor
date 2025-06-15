#include <thread>

#include "example.hpp"

int main() {
  using StringVecMonitor = mem::Monitor<std::vector<std::string>>;
  constexpr std::size_t repeat_ntimes{5};
  StringVecMonitor monitor{
      StringVecMonitor::AccessGuard::NotifyPolicy::notify_one};
  std::jthread writer{ex::write_task, std::ref(monitor), "writer",
                      repeat_ntimes};
  std::jthread reader{ex::read_task, std::ref(monitor), "reader",
                      repeat_ntimes};
  return 0;
}