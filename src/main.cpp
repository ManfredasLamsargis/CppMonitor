#include <algorithm>
#include <array>
#include <sstream>
#include <string>
#include <thread>

#include "Monitor.hpp"
#include "utils.hpp"

using utils::println;
using utils::repeat;
using StringVecMonitor = mem::Monitor<std::vector<std::string>>;
StringVecMonitor monitor{"x", "y", "z"};

std::string get_thread_id() {
  std::stringstream ss;
  ss << std::this_thread::get_id();
  return ss.str();
}

void provide() {
  constexpr std::size_t ntimes{1'000'000};
  println("new provider created");
  repeat(ntimes, [] { monitor->push_back(get_thread_id()); });
  println("provider exited");
}

void consume() {
  constexpr std::size_t ntimes{1'000'000};
  println("new consumer created");
  repeat(ntimes, [] {
    monitor
        .wait(
            [](const std::vector<std::string> &vec) { return vec.size() > 0; })
        ->pop_back();
  });
  println("consumer exited");
}

void run_providers() {
  std::array<std::jthread, 3> providers;
  std::for_each(providers.begin(), providers.end(), [](std::jthread &provider) {
    provider = std::jthread{provide};
  });
}

void run_consumers() {
  std::array<std::jthread, 3> consumers;
  std::for_each(consumers.begin(), consumers.end(), [](std::jthread &consumer) {
    consumer = std::jthread{consume};
  });
}

int main() {
  std::jthread providers_runner{run_providers};
  std::jthread consumers_runner{run_consumers};
  return EXIT_SUCCESS;
}