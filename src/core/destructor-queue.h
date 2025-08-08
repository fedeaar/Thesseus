#pragma once

#include <deque>
#include <functional>

class DestructorQueue
{
private:
  std::deque<std::function<void()>> functions;

public:
  DestructorQueue()
    : functions()
  {
  }

  ~DestructorQueue() { flush(); }

  void push(std::function<void()>&& fn) { functions.push_back(fn); }

  void flush()
  {
    for (auto it = functions.rbegin(); it != functions.rend(); ++it) {
      (*it)();
    }
    functions.clear();
  }
};
