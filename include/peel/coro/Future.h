#pragma once

#include <coroutine>
#include <atomic>
#include <cstdint>
#include <peel/lang.h>

peel_begin_header

namespace peel
{
namespace coro
{

template<typename T>
class Future
{
public:
  class FinalSuspend;
  class promise_type;

private:
  static void *
  coro_completed_token ()
  {
    return reinterpret_cast<void *> (UINTPTR_MAX);
  }

  static void *
  future_gone_token ()
  {
    return reinterpret_cast<void *> (UINTPTR_MAX - 1);
  }

  promise_type *promise;

  explicit constexpr
  Future (promise_type *promise) noexcept
    : promise (promise)
  { }

public:
  ~Future () noexcept
  {
    if (!promise)
      return;
    /* Acquire so we can destroy the coroutine */
    void *ptr = promise->rendezvous.exchange (future_gone_token (), std::memory_order_acquire);
    if (ptr == coro_completed_token ()) [[likely]]
      std::coroutine_handle<promise_type>::from_promise (*promise).destroy ();
    /* If the coroutine is still running, leave it be, it
     * is responsible for destroying itself when completed.
     */
  }

  Future (const Future &) = delete;

  constexpr
  Future (Future &&other) noexcept
    : promise (other.promise)
  {
    other.promise = nullptr;
  }

  constexpr bool
  await_ready () const noexcept
  {
    return false;
  }

  bool
  await_suspend (std::coroutine_handle<> h) const noexcept
  {
    /* Acquire so we can read the result if completed, release
     * so the promise can later resume our coroutine if not.
     */
    void *ptr = promise->rendezvous.exchange (h.address (), std::memory_order_acq_rel);
    /* If we read nullptr, the coroutine is still running */
    if (!ptr) [[likely]]
      return true;
    /* We read coro_completed_token (), put it back and don't suspend */
    promise->rendezvous.store (ptr, std::memory_order_relaxed);
    return false;
  }

  std::add_rvalue_reference_t<T>
  await_resume () const noexcept
  requires (!std::is_void_v<T>)
  {
    return static_cast<T &&> (promise->value);
  }

  void
  await_resume () const noexcept
  requires (std::is_void_v<T>)
  { }
};

template<typename T>
class Future<T>::promise_type
{
  friend class Future;
  friend class FinalSuspend;

public:
  constexpr
  promise_type () noexcept
  { }

  ~promise_type ()
  {
    value.~T ();
  }

  Future
  get_return_object () noexcept
  {
    return Future (this);
  }

  constexpr std::suspend_never
  initial_suspend () const noexcept
  {
    return std::suspend_never { };
  }

  constexpr FinalSuspend
  final_suspend () noexcept
  {
    return FinalSuspend (this);
  }

  void
  unhandled_exception ()
  { }

  template<typename U>
  void
  return_value (U &&v)
  {
    new (&value) T (static_cast<U &&> (v));
  }

private:
  std::atomic<void *> rendezvous { nullptr };
  union
  {
    T value;
  };
};

template<>
class Future<void>::promise_type
{
  friend class Future;
  friend class FinalSuspend;

public:
  constexpr
  promise_type () noexcept
  { }

  ~promise_type () noexcept
  { }

  Future
  get_return_object () noexcept
  {
    return Future (this);
  }

  constexpr std::suspend_never
  initial_suspend () const noexcept
  {
    return std::suspend_never { };
  }

  constexpr FinalSuspend
  final_suspend () noexcept;

  void
  unhandled_exception ()
  { }

  constexpr void
  return_void () const noexcept
  { }

private:
  std::atomic<void *> rendezvous { nullptr };
};

template<typename T>
class Future<T>::FinalSuspend
{
  friend class promise_type;
  promise_type *promise;

  explicit constexpr
  FinalSuspend (promise_type *promise) noexcept
    : promise (promise)
  { }

public:
  constexpr bool
  await_ready () const noexcept
  {
    return false;
  }

  std::coroutine_handle<>
  await_suspend (std::coroutine_handle<> h) const noexcept
  {
    /* Acquire so we can resume the waiting coroutine, release so
     * our result can be later read if there is no coroutine yet.
     */
    void *ptr = promise->rendezvous.exchange (coro_completed_token (), std::memory_order_acq_rel);
    if (ptr == future_gone_token ()) [[unlikely]]
      /* Our future is gone, destroy ourselves */
      std::coroutine_handle<promise_type>::from_promise (*promise).destroy ();
    else if (ptr) [[likely]]
      return std::coroutine_handle<>::from_address (ptr);
    return std::noop_coroutine ();
  }

  void
  await_resume () const noexcept
  {
    peel_unreachable;
  }
};

constexpr Future<void>::FinalSuspend
Future<void>::promise_type::final_suspend () noexcept
{
  return FinalSuspend (this);
}

} /* namespace coro */
} /* namespace peel */

peel_end_header
