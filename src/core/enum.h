#pragma once

namespace core {

enum class code
{
  NOT_IMPLEMENTED,
  RETRYABLE_ERROR,
  NOT_INIT,
  IN_ERROR_STATE,
  ERROR,
  SUCCESS,
};

enum class status
{
  ERROR,
  NOT_INIT,
  INIT,
};

} // namespace core
