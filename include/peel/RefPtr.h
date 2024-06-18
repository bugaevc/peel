#pragma once

#include <peel/lang.h>

namespace peel
{

template<typename T, typename = void>
struct RefTraits;
/*
{
  static void
  ref (T *);

  static void
  unref (T *);

  static void
  ref_sink (T *);

  static void
  sink_unref (T *);
}
*/

template<typename T>
class FloatPtr;

template<typename T>
class RefPtr
{
private:
  template<typename U>
  friend class RefPtr;

  T *ptr;

public:
  constexpr RefPtr ()
    : ptr (nullptr)
  { }

  constexpr RefPtr (decltype (nullptr))
    : ptr (nullptr)
  { }

  peel_nothrow
  RefPtr (T *ptr)
    : ptr (ptr)
  {
    if (ptr)
      RefTraits<T>::ref (ptr);
  }

  peel_nothrow
  RefPtr (const RefPtr &other)
    : ptr (other.ptr)
  {
    if (ptr)
      RefTraits<T>::ref (ptr);
  }

  RefPtr (RefPtr &&other)
    : ptr (other.ptr)
  {
    other.ptr = nullptr;
  }

  RefPtr (const FloatPtr<T> &f)
    : RefPtr ((T *) f)
  { }

  peel_nothrow
  RefPtr (FloatPtr<T> &&f)
    : RefPtr (static_cast<FloatPtr<T> &&> (f).sink ())
  { }

  /* Upcast.  */
  template<typename U, peel::enable_if_derived<T, U, int> = 0>
  peel_nothrow
  RefPtr (U *ptr)
    : ptr (ptr)
  {
    if (ptr)
      RefTraits<T>::ref (ptr);
  }

  /* Upcast.  */
  template<typename U, peel::enable_if_derived<T, U, int> = 0>
  RefPtr (const RefPtr<U> &other)
    : ptr (other.ptr)
  {
    if (ptr)
      RefTraits<T>::ref (ptr);
  }

  /* Upcast.  */
  template<typename U, peel::enable_if_derived<T, U, int> = 0>
  RefPtr (RefPtr<U> &&other)
    : ptr (other.ptr)
  {
    other.ptr = nullptr;
  }

  peel_nothrow
  ~RefPtr ()
  {
    if (ptr)
      RefTraits<T>::unref (ptr);
  }

  static RefPtr
  adopt_ref (T *ptr)
  {
    RefPtr p;
    p.ptr = ptr;
    return p;
  }

/*
  peel_nothrow
  static RefPtr
  sink_ref (T *ptr)
  {
    RefPtr p;
    p.ptr = g_object_ref_sink (ptr);
    return p;
  }
*/

  peel_nodiscard ("the reference will leak if unused")
  T *
  release_ref () &&
  {
    T *p = ptr;
    ptr = nullptr;
    return p;
  }

  peel_nothrow
  RefPtr &
  operator = (T *ptr)
  {
    if (ptr == this->ptr)
      return *this;
    if (this->ptr)
      RefTraits<T>::unref (this->ptr);
    this->ptr = ptr;
    if (ptr)
      RefTraits<T>::ref (ptr);
    return *this;
  }

  peel_nothrow
  RefPtr &
  operator = (const RefPtr &other)
  {
    if (ptr == other.ptr)
      return *this;
    if (ptr)
      RefTraits<T>::unref (ptr);
    ptr = other.ptr;
    if (ptr)
      RefTraits<T>::ref (ptr);
    return *this;
  }

  peel_nothrow
  RefPtr &
  operator = (RefPtr &&other)
  {
    if (ptr == other.ptr)
      return *this;
    if (ptr)
      RefTraits<T>::unref (ptr);
    ptr = other.ptr;
    other.ptr = nullptr;
    return *this;
  }

  constexpr
  operator T * () const &
  {
    return ptr;
  }

  operator T * () && = delete;

  constexpr T &
  operator * () const
  {
    return *ptr;
  }

  constexpr T *
  operator -> () const &
  {
    return ptr;
  }

  T *
  operator -> () && = delete;

  explicit constexpr
  operator bool () const
  {
    return ptr != nullptr;
  }

  template<typename U>
  RefPtr<U>
  cast () &&
  {
    U *u_ptr = ptr->template cast<U> ();
    ptr = nullptr;
    return RefPtr<U>::adopt_ref (u_ptr);
  }
};

} /* namespace peel */
