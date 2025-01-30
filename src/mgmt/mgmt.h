#pragma once

#include "../core/destructor-queue.h"
#include "../core/logger.h"
#include "../core/result.hpp"
#include "../core/types.h"

namespace ResourceManagement {

extern std::string const& namespace_;

enum Status
{
  NOT_INIT = -2,
  ERROR = -1,
  SUCCESS = 1
};

};
