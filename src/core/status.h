#pragma once

namespace core {

enum Status
{
  NOT_IMPLEMENTED = -4,
  RETRYABLE_ERROR = -3,
  NOT_INIT = -2,
  ERROR = -1,
  SUCCESS = 1,
};
} // namespace core
