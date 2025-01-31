#pragma once

#include <fmt/base.h>
#include <fmt/format.h>

namespace core {

class Logger
{
private:
  std::string name_;

public:
  Logger(std::string const& name)
    : name_(name) {};

  inline void log(std::string const& fmt)
  {
    fmt::print("[LOG: {}]\n{}\n", name_, fmt);
  }

  inline void error(std::string const& fmt)
  {
    fmt::print("[ERROR: {}]\n{}\n", name_, fmt);
    std::fflush(stdout);
  }
};
} // namespace core
