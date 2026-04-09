#pragma once

#include <coroutine>
#include <atomic>
#include <peel/lang.h>
#include <glib-object.h>

peel_begin_header

namespace peel
{

namespace GObject
{
class Object;
}
namespace Gio
{
class AsyncResult;
}

namespace coro
{

struct AsyncResult
{
private:
  std::atomic<void *> rendezvous { nullptr };

  AsyncResult (const AsyncResult &) = delete;
  AsyncResult (AsyncResult &&) = delete;

  AsyncResult &
  operator = (const AsyncResult &) = delete;
  AsyncResult &
  operator = (AsyncResult &&) = delete;

  static inline bool
  is_referenced (void *ptr)
  {
    return reinterpret_cast<uintptr_t> (ptr) & 1;
  }

  static inline void *
  unmask (void *ptr)
  {
    return reinterpret_cast<void *> (reinterpret_cast<uintptr_t> (ptr) & ~static_cast<uintptr_t> (1));
  }

  void
  destroy () noexcept
  {
    void *ptr = rendezvous.exchange (nullptr, std::memory_order_relaxed);
    if (!is_referenced (ptr)) [[likely]]
      return;
    g_object_unref (unmask (ptr));
  }

public:
  constexpr AsyncResult ()
  { }

  ~AsyncResult () noexcept
  {
    destroy ();
  }

  struct Callback
  {
  private:
    AsyncResult &async_result;

  public:
    Callback (AsyncResult &async_result) noexcept
      : async_result (async_result)
    { }

    void
    operator () (GObject::Object *obj, Gio::AsyncResult *res) noexcept
    {
      /* Peek whether a coroutine is waiting */
      void *peek = async_result.rendezvous.load (std::memory_order_relaxed);
      /* If no-one appears to be waiting, reference our result */
      if (!peek) [[unlikely]]
        {
          g_object_ref (res);
          res = reinterpret_cast<Gio::AsyncResult *> (reinterpret_cast<uintptr_t> (res) | 1);
        }
      /* Actually put it in */
      void *ptr = async_result.rendezvous.exchange (res, std::memory_order_acq_rel);
      /* If there was a coroutine awaiting, resume it now */
      if (ptr) [[likely]]
        std::coroutine_handle<>::from_address (ptr).resume ();
    }
  };

  Callback
  callback () noexcept
  {
    destroy ();
    return Callback { *this };
  }

  constexpr bool
  await_ready () const noexcept
  {
    return false;
  }

  bool
  await_suspend (std::coroutine_handle<> h) noexcept
  {
    void *ptr = rendezvous.exchange (h.address (), std::memory_order_acq_rel);
    if (!ptr) [[likely]]
      return true;
    rendezvous.store (ptr, std::memory_order_relaxed);
    return false;
  }

  Gio::AsyncResult *
  await_resume () noexcept
  {
    /* We're resumed either on the same thread as the callback, or after having
     * seen a result without waiting. In either case no more synchronization is
     * necessary.
     */
    void *ptr = rendezvous.load (std::memory_order_relaxed);
    return reinterpret_cast<Gio::AsyncResult *> (unmask (ptr));
  }
};

} /* namespace coro */
} /* namespace peel */

peel_end_header
