#pragma once

#include <coroutine>

peel_begin_header

namespace peel
{
namespace coro
{

struct SimpleTask
{
  struct promise_type
  {
    SimpleTask
    get_return_object ()
    {
      return SimpleTask { };
    }

    std::suspend_never
    initial_suspend ()
    {
      return std::suspend_never { };
    }

    std::suspend_never
    final_suspend () noexcept
    {
      return std::suspend_never { };
    }

    void
    return_void ()
    { }

    void
    unhandled_exception ()
    { }
  };
};

} /* namespace coro */
} /* namespace peel */

peel_end_header
