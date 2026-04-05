#pragma once

#include <coroutine>
#include <peel/lang.h>
#include <glib.h>

peel_begin_header

namespace peel
{
namespace coro
{

struct SimpleTask
{
  struct promise_type
  {
    void *
    operator new (size_t size) /* noexcept */
    {
      return g_malloc (size);
    }

    void
    operator delete (void *ptr, size_t size) noexcept
    {
#if GLIB_CHECK_VERSION (2, 76, 0)
      g_free_sized (ptr, size);
#else
      g_free (ptr);
#endif
    }

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
