#pragma once

#include <peel/lang.h>

namespace peel
{

template<typename T>
class FloatPtr
{
private:
  template<typename U>
  friend class FloatPtr;

  T *ptr;

  peel_always_inline peel_nothrow
  void
  do_dispose ()
  {
    if (!ptr)
      return;

    RefTraits<T>::sink_unref (ptr);
    ptr = nullptr;
  }

public:
  peel_nothrow
  constexpr FloatPtr ()
    : ptr (nullptr)
  { }

  peel_nothrow
  constexpr FloatPtr (decltype (nullptr))
    : ptr (nullptr)
  { }

  peel_nothrow
  constexpr FloatPtr (T *ptr)
    : ptr (ptr)
  { }

  peel_nothrow
  constexpr FloatPtr (const FloatPtr &other)
    : ptr (other.ptr)
  { }

  peel_nothrow
  FloatPtr (FloatPtr &&other)
    : ptr (other.ptr)
  {
    other.ptr = nullptr;
  }

  /* Upcast.  */
  template<typename U, peel::enable_if_derived<T, U, int> = 0>
  peel_nothrow
  FloatPtr (FloatPtr<U> &&other)
    : ptr (other.ptr)
  {
    other.ptr = nullptr;
  }

  peel_nothrow
  ~FloatPtr ()
  {
    do_dispose ();
  }

  peel_nothrow
  FloatPtr &
  operator = (T *ptr)
  {
    if (this->ptr == ptr)
      return *this;
    do_dispose ();
    this->ptr = ptr;
    return *this;
  }

  peel_nothrow
  FloatPtr &
  operator = (const FloatPtr &other)
  {
    if (ptr == other.ptr)
      return *this;
    do_dispose ();
    ptr = other.ptr;
    return *this;
  }

  peel_nothrow
  FloatPtr &
  operator = (FloatPtr &&other)
  {
    if (ptr == other.ptr)
      return *this;
    do_dispose ();
    ptr = other.ptr;
    other.ptr = nullptr;
    return *this;
  }

  peel_nothrow
  RefPtr<T>
  sink () &&
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
