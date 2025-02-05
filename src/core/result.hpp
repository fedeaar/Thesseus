#pragma once

#include <cassert>
#include <utility>

namespace core {

template<typename TOkResult, typename TErrorResult>
class Result
{
private:
  union
  {
    TOkResult value_;
    TErrorResult error_;
  };
  bool is_value_;

public:
  Result(TOkResult const& value)
    : value_{ value }
    , is_value_{ true } {};
  Result(TOkResult&& value)
    : value_{ std::move(value) }
    , is_value_{ true }
  {
  }
  Result(TErrorResult const& error)
    : error_{ error }
    , is_value_{ false } {};
  Result(TErrorResult&& error)
    : error_{ std::move(error) }
    , is_value_{ false } {};
  Result(Result const& other)
    : is_value_{ other.is_value_ }
  {
    if (is_value_) {
      value_ = other.value_;
    } else {
      error_ = other.error_;
    }
  }

  ~Result()
  {
    if (is_value_) {
      value_.~TOkResult();
    } else {
      error_.~TErrorResult();
    }
  }

  bool has_value() { return is_value_; }

  TOkResult value()
  {
    assert(is_value_);
    return value_;
  }

  TErrorResult error()
  {
    assert(!is_value_);
    return error_;
  }
};
} // namespace core
