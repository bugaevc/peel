#pragma once

#include <peel/lang.h>

namespace peel
{

template<typename T>
struct UniqueTraits;
/*
{
  static void
  free (T *);
}
*/

template<typename T>
class UniquePtr
{
private:
  template<typename U>
  friend class UniquePtr;

  T *ptr;

public:
  constexpr UniquePtr ()
    : ptr (nullptr)
  { }

  constexpr UniquePtr (decltype (nullptr))
    : ptr (nullptr)
  { }

  UniquePtr (const UniquePtr &) = delete;

  UniquePtr (UniquePtr &&other)
    : ptr (other.ptr)
  {
    other.ptr = nullptr;
  }

  peel_nothrow
  ~UniquePtr ()
  {
    if (ptr)
      UniqueTraits<T>::free (ptr);
  }

  static UniquePtr
  adopt_ref (T *ptr)
  {
    UniquePtr p;
    p.ptr = ptr;
    return p;
  }

  peel_nodiscard ("the reference will leak if unused")
  T *
  release_ref () &&
  {
    T *p = ptr;
    ptr = nullptr;
    return p;
  }

  UniquePtr &
  operator = (const UniquePtr &) = delete;

  peel_nothrow
  UniquePtr &
  operator = (UniquePtr &&other)
  {
    if (this == &other)
      return *this;
    if (ptr)
      UniqueTraits<T>::free (ptr);
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
  operator -> () const
  {
    return ptr;
  }

  explicit constexpr
  operator bool () const
  {
    return ptr != nullptr;
  }
};

}
