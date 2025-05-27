#include "example1.hpp"
#include "example2.hpp"
#include "log.hpp"
#include "monitor.hpp"
#include "pch.hpp"

constexpr std::size_t producer_count{3};
constexpr std::size_t consumer_count{5};

std::shared_ptr<mem::Monitor<std::vector<std::string>>> mon_ptr{
    std::make_shared<mem::Monitor<std::vector<std::string>>>()};
std::shared_ptr<mem::Monitor<mem::Flag>> flg_ptr{
    std::make_shared<mem::Monitor<mem::Flag>>(producer_count, consumer_count)};

int main() {
  using mem::Monitor;

  utils::clog::set();

  utils::println(std::string(60, '-'));

  spdlog::info("example1 started");
  std::vector<std::string> vec{};
  Monitor<std::vector<std::string>> mon{vec};
  std::thread writer_0{example1::write_task, std::ref(mon), "a", 5};
  std::thread writer_1{example1::write_task, std::ref(mon), "b", 5};
  std::thread writer_2{example1::write_task, std::ref(mon), "c", 2};
  std::thread reader{example1::read_task, std::ref(mon), "reader", 12};
  writer_0.join();
  writer_1.join();
  writer_2.join();
  reader.join();
  spdlog::info("example1 completed");

  utils::println(std::string(60, '-'));

  spdlog::info("example2 started");
  std::vector<std::thread> producers{};
  producers.reserve(producer_count);
#if defined(_MSC_VER)
  std::size_t i = 0;
  utils::repeat(producer_count, [&producers, &i] {
    producers.emplace_back(example2::producer_task, std::to_string(i++), 2);
  });
#else
  utils::repeat(producer_count, [&producers, i = std::size_t{0}]() mutable {
    producers.emplace_back(example2::producer_task, std::to_string(i++), 2);
  });
#endif

  std::vector<std::thread> consumers{};
  consumers.reserve(consumer_count);
#if defined(_MSC_VER)
  std::size_t j = 0;
  utils::repeat(consumer_count, [&consumers, &j] {
    consumers.emplace_back(example2::consumer_task, std::to_string(j++));
  });
#else
  utils::repeat(consumer_count, [&consumers, j = std::size_t{0}]() mutable {
    consumers.emplace_back(example2::consumer_task, std::to_string(j++));
  });
#endif

  std::for_each(producers.begin(), producers.end(),
                [](std::thread &t) { t.join(); });
  std::for_each(consumers.begin(), consumers.end(),
                [](std::thread &t) { t.join(); });
  spdlog::info("example2 completed");

  utils::println(std::string(60, '-'));

  return EXIT_SUCCESS;
}