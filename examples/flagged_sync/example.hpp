#pragma once

#include <format>
#include <sstream>

#include "monitor.hpp"
#include "utils.hpp"

namespace ex {
class Flag {
 private:
  std::size_t m_prod;  // producer count
  std::size_t m_con;   // consumer count
 public:
  Flag(const std::size_t prod_cnt, const std::size_t con_cnt)
      : m_prod{prod_cnt}, m_con{con_cnt} {}
  void producer_declare_exit() { m_prod--; }
  void consumer_declare_exit() { m_con--; }
  bool task_completed() const { return m_prod == 0; }
};
}  // namespace ex

extern std::shared_ptr<mem::Monitor<std::vector<std::string>>> mon_ptr;
extern std::shared_ptr<mem::Monitor<ex::Flag>> flg_ptr;

namespace ex {

using mem::Monitor;

inline void producer_task(const std::string &hash, const std::size_t ntimes) {
  std::stringstream ss{};
  utils::repeat(ntimes, [&ss, &hash] {
    ss << "{producer:" << hash << '}';
    utils::random_sleep(std::chrono::seconds(1));
    mon_ptr->pause()->push_back(ss.str());
    ss.str(std::string{});
  });
  utils::println(std::format("pruducer {} exited", hash));
  flg_ptr->pause()->producer_declare_exit();
}

inline void consumer_task(const std::string &hash) {
  auto can_consume_or_done = [](const std::vector<std::string> &vec) -> bool {
    return !vec.empty() || flg_ptr->pause()->task_completed();
  };
  while (true) {
    utils::random_sleep(std::chrono::seconds(2));
    auto vec_window{mon_ptr->wait_until(can_consume_or_done)};
    if (!vec_window->empty()) {
      utils::println(
          std::format("consumer: {} => {}", hash, vec_window->back()));
    } else {
      break;
    }
    vec_window->pop_back();
  }
  utils::println(std::format("consumer {} exited", hash));
  flg_ptr->pause()->consumer_declare_exit();
}
}  // namespace ex