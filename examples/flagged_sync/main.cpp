#include <memory>

#include "example.hpp"

using mem::Monitor;

constexpr std::size_t producer_count{3};
constexpr std::size_t consumer_count{5};

std::shared_ptr<Monitor<std::vector<std::string>>> mon_ptr{
    std::make_shared<Monitor<std::vector<std::string>>>()};
std::shared_ptr<Monitor<ex::Flag>> flg_ptr{
    std::make_shared<Monitor<ex::Flag>>(producer_count, consumer_count)};

int main() {
  std::vector<std::thread> producers{};
  producers.reserve(producer_count);
#if defined(_MSC_VER)
  std::size_t i = 0;
  utils::repeat(producer_count, [&producers, &i] {
    producers.emplace_back(example2::producer_task, std::to_string(i++), 2);
  });
#else
  utils::repeat(producer_count, [&producers, i = std::size_t{0}]() mutable {
    producers.emplace_back(ex::producer_task, std::to_string(i++), 2);
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
    consumers.emplace_back(ex::consumer_task, std::to_string(j++));
  });
#endif
  std::for_each(producers.begin(), producers.end(),
                [](std::thread &t) { t.join(); });
  std::for_each(consumers.begin(), consumers.end(),
                [](std::thread &t) { t.join(); });
  return 0;
}