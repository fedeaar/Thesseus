#pragma once

#include <fmt/base.h>
#include <fmt/format.h>

#include <chrono>

namespace core {

class Logger
{
private:
  template<typename... T>
  static inline void log_(fmt::format_string<T...> fmt,
                          std::string const& level,
                          std::string const& where,
                          T&&... args)
  {
    auto now = std::chrono::duration_cast<std::chrono::nanoseconds>(
                 std::chrono::system_clock::now().time_since_epoch())
                 .count();
    fmt::print("[{}: {}]\ntimestamp:\n{}\nmessage:\n{}\n",
               level,
               where,
               now,
               fmt::format(fmt, std::forward<T>(args)...));
  }

public:
  template<typename... T>
  static inline void log(std::string const& where,
                         fmt::format_string<T...> fmt,
                         T&&... args)
  {
    log_(fmt, "LOG", where, std::forward<T>(args)...);
  }

  template<typename... T>
  static inline void wrn(std::string const& where,
                         fmt::format_string<T...> fmt,
                         T&&... args)
  {
    log_(fmt, "WARN", where, std::forward<T>(args)...);
    std::fflush(stdout);
  }

  template<typename... T>
  static inline void err(std::string const& where,
                         fmt::format_string<T...> fmt,
                         T&&... args)
  {
    log_(fmt, "ERROR", where, std::forward<T>(args)...);
    std::fflush(stdout);
  }

  static void flush() { std::fflush(stdout); };
};

} // namespace core

#define LOG(...) core::Logger::log(FUNCTION_NAME, __VA_ARGS__);
#define WRN(...) core::Logger::log(FUNCTION_NAME, __VA_ARGS__);
#define ERR(...) core::Logger::err(FUNCTION_NAME, __VA_ARGS__);
