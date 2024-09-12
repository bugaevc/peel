#pragma once

#include <peel/lang.h>

namespace peel
{

template<typename T>
class FloatPtr final
{
private:
  template<typename U>
  friend class FloatPtr;

  T *ptr;

  peel_always_inline
  void
  do_dispose () noexcept
  {
    if (!ptr)
      return;

    RefTraits<T>::sink (ptr);
    ptr = nullptr;
  }

public:
  constexpr FloatPtr () noexcept
    : ptr (nullptr)
  { }

  constexpr FloatPtr (decltype (nullptr)) noexcept
    : ptr (nullptr)
  { }

  constexpr FloatPtr (T *ptr) noexcept
    : ptr (ptr)
  { }

  /* Deny copying FloatPtr; please move them instead.
   * Otherwise, it's way too easy to deallocate the initial
   * floating reference, leading to a use-after-free.
   * If you know what you're doing and still want to copy
   * FloatPtr's, roundtrip through a plain (T *).
   */
  FloatPtr (const FloatPtr &) = delete;

  template<typename U>
  FloatPtr (const FloatPtr<U> &) = delete;

  FloatPtr (FloatPtr &&other) noexcept
    : ptr (other.ptr)
  {
    other.ptr = nullptr;
  }

  /* Upcast.  */
  template<typename U, peel::enable_if_derived<T, U, int> = 0>
  FloatPtr (FloatPtr<U> &&other) noexcept
    : ptr (other.ptr)
  {
    other.ptr = nullptr;
  }

  ~FloatPtr () noexcept
  {
    do_dispose ();
  }

  FloatPtr &
  operator = (T *ptr) noexcept
  {
    if (this->ptr == ptr)
      return *this;
    do_dispose ();
    this->ptr = ptr;
    return *this;
  }

  FloatPtr &
  operator = (const FloatPtr &other) noexcept
  {
    if (ptr == other.ptr)
      return *this;
    do_dispose ();
    ptr = other.ptr;
    return *this;
  }

  FloatPtr &
  operator = (FloatPtr &&other) noexcept
  {
    if (ptr == other.ptr)
      return *this;
    do_dispose ();
    ptr = other.ptr;
    other.ptr = nullptr;
    return *this;
  }

  RefPtr<T>
  ref_sink () && noexcept
  {
    if (!ptr)
      return nullptr;
    RefTraits<T>::ref_sink (ptr);
    RefPtr<T> p = RefPtr<T>::adopt_ref (ptr);
    ptr = nullptr;
    return p;
  }

  constexpr
  operator T * () const &
  {
    return ptr;
  }

  operator T * () && = delete;

  constexpr T *
  operator -> () const &
  {
    return ptr;
  }

  T *
  operator -> () && = delete;

  peel_nodiscard ("the floating reference will leak if unused")
  T *
  release_floating_ptr () &&
  {
    T *p = ptr;
    ptr = nullptr;
    return p;
  }
};

} /* namespace peel */
