#pragma once

namespace core {

enum class code
{
  NOT_IMPLEMENTED = -4,
  RETRYABLE_ERROR = -3,
  NOT_INIT = -2,
  ERROR = -1,
  SUCCESS = 1,
};

enum class status
{
  ERROR = -2,
  NOT_INIT = -1,
  INIT = 1
};

} // namespace core
