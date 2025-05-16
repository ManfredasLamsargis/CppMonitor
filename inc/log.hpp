#ifndef PRJ_LOG_HPP
#define PRJ_LOG_HPP

#include <optional>
#include <source_location>

#include "spdlog/spdlog.h"

namespace utils {
/**
 * @brief Console Logging.
 */
namespace clog {
inline void set() {
  spdlog::set_level(spdlog::level::trace);
  spdlog::set_pattern("[%x %H:%M:%S:%e][%t][%^%-8l%$]: %v");
}

inline void
flag(const spdlog::level::level_enum level = spdlog::level::trace,
     const std::source_location &src = std::source_location::current()) {
  const std::string_view file_path{src.file_name()};
  const std::string_view file_name{
      file_path.substr(file_path.find_last_of('/') + 1)};
  const std::string &str{
      spdlog::fmt_lib::format("FLAG: {}({}:{}) `{}`", file_name, src.line(),
                              src.column(), src.function_name())};
  switch (level) {
  case spdlog::level::trace:
    spdlog::trace(str);
    break;
  case spdlog::level::debug:
    spdlog::debug(str);
    break;
  case spdlog::level::info:
    spdlog::info(str);
    break;
  case spdlog::level::warn:
    spdlog::warn(str);
    break;
  case spdlog::level::err:
    spdlog::error(str);
    break;
  case spdlog::level::critical:
    spdlog::critical(str);
    break;
  default:
    spdlog::trace(str);
  }
}

class error {
private:
  std::string msg;
  std::optional<std::string> str;
  std::source_location src;

public:
  error(const std::string_view msg,
        const std::optional<const std::string_view> str,
        const std::source_location &src = std::source_location::current())
      : msg{msg}, src{src} {
    if (str) {
      this->str = *str;
    }
  }

  error &message(const std::string_view msg) {
    this->msg = msg;
    return *this;
  }

  const std::string_view message() const { return this->msg; }

  error &reason(const std::string_view str) {
    this->str.emplace(str);
    return *this;
  }

  const std::string reason() const { return str.value_or("unknown"); }

  error &source(const std::source_location &src) {
    this->src = src;
    return *this;
  }

  const std::string_view source_function() const { return src.function_name(); }

  const std::string_view source_file_name() const {
    // Take just the file name, because std::source_location::file_name()
    // returns a full path
    const std::string_view file_path{src.file_name()};
    return std::string_view{file_path.substr(file_path.find_last_of('/') + 1)};
  }

  const std::string_view source_file() const { return src.file_name(); }

  std::uint32_t source_line() const {
    return static_cast<std::uint32_t>(src.line());
  }

  std::uint32_t source_column() const {
    return static_cast<std::uint32_t>(src.column());
  }

  void print() const { print(*this); }

  error &print() {
    print(*this);
    return *this;
  }

  static void print(const error &err) {
    spdlog::error("{}({}:{}) `{}`: {}, reason: '{}'", err.source_file_name(),
                  err.source_line(), err.source_column(), err.source_function(),
                  err.message(), err.reason());
  }
};
} // namespace clog
} // namespace utils

#endif