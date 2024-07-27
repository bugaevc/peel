#pragma once

#include <coroutine>
#include <atomic>

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

public:
  constexpr AsyncResult ()
  { }

  struct Callback
  {
  private:
    AsyncResult &async_result;

  public:
    Callback (AsyncResult &async_result)
      : async_result (async_result)
    { }

    void
    operator () (GObject::Object *obj, Gio::AsyncResult *res)
    {
      void *ptr = async_result.rendezvous.exchange (res, std::memory_order_acq_rel);
      if (ptr) [[likely]]
        std::coroutine_handle<>::from_address (ptr).resume ();
    }
  };

  Callback
  callback ()
  {
    return Callback { *this };
  }

  constexpr bool
  await_ready () const
  {
    return false;
  }

  bool
  await_suspend (std::coroutine_handle<> h)
  {
    void *ptr = rendezvous.exchange (h.address (), std::memory_order_acq_rel);
    if (!ptr) [[likely]]
      return true;
    rendezvous.store (ptr, std::memory_order_relaxed);
    return false;
  }

  Gio::AsyncResult *
  await_resume ()
  {
    void *ptr = rendezvous.exchange (nullptr, std::memory_order_relaxed);
    return reinterpret_cast<Gio::AsyncResult *> (ptr);
  }
};

} /* namespace coro */
} /* namespace peel */
