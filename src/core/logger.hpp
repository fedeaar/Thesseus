#pragma once

#include <fmt/base.h>
#include <fmt/format.h>

#include <chrono>

namespace core {

class Logger
{
private:
  std::string name_;

  template<typename... T>
  inline void log_(fmt::format_string<T...> fmt,
                   std::string const& level,
                   T&&... args)
  {
    auto now = std::chrono::duration_cast<std::chrono::nanoseconds>(
                 std::chrono::system_clock::now().time_since_epoch())
                 .count();
    fmt::print("[{}: {}]\ntimestamp:\n{}\nmessage:\n{}\n",
               level,
               name_,
               now,
               fmt::format(fmt, std::forward<T>(args)...));
  }

public:
  Logger(std::string const& name)
    : name_(name) {};
  ~Logger() { flush(); };

  template<typename... T>
  inline void log(fmt::format_string<T...> fmt, T&&... args)
  {
    log_(fmt, "LOG", std::forward<T>(args)...);
  }

  template<typename... T>
  inline void wrn(fmt::format_string<T...> fmt, T&&... args)
  {
    log_(fmt, "WARN", std::forward<T>(args)...);
    std::fflush(stdout);
  }

  template<typename... T>
  inline void err(fmt::format_string<T...> fmt, T&&... args)
  {
    log_(fmt, "ERROR", std::forward<T>(args)...);
    std::fflush(stdout);
  }

  void flush() { std::fflush(stdout); };
};
} // namespace core
