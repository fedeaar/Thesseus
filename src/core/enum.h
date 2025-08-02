#pragma once

namespace core {

enum class code
{
  ERROR,
  SUCCESS,
  NOT_IMPLEMENTED,
  NOT_SUPPORTED,
  NOT_INITIALIZED,
  RETRYABLE_ERROR,
  IN_ERROR_STATE,
};

enum class status
{
  ERROR,
  NOT_INITIALIZED,
  INITIALIZED,
};

} // namespace core
