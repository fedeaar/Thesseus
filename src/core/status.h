#pragma once

namespace core {

enum Status
{
  RETRYABLE_ERROR = -3,
  NOT_INIT = -2,
  ERROR = -1,
  SUCCESS = 1,
};
} // namespace core
