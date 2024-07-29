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

template<typename T>
class UniquePtr<T[]>
{
private:
  T *ptr;
  unsigned c;

  void
  free ()
  {
    if (!std::is_trivially_destructible<T>::value)
      {
        for (unsigned i = 0; i < c; i++)
          ptr[i].~T();
      }
    g_free (ptr);
    ptr = nullptr;
    c = 0;
  }

public:
  constexpr UniquePtr ()
    : ptr (nullptr)
    , c (0)
  { }

  constexpr UniquePtr (decltype (nullptr))
    : ptr (nullptr)
    , c (0)
  { }

  UniquePtr (UniquePtr &&other)
    : ptr (other.ptr)
    , c (other.c)
  {
    other.ptr = nullptr;
    other.c = 0;
  }

  UniquePtr (const UniquePtr &) = delete;

  ~UniquePtr ()
  {
    free ();
  }

  peel_nodiscard ("the reference will leak if unused")
  T *
  release_ref () &&
  {
    T *p = ptr;
    ptr = nullptr;
    c = 0;
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
    free ();
    ptr = other.ptr;
    c = other.c;
    other.ptr = nullptr;
    other.c = 0;
    return *this;
  }

  static UniquePtr
  adopt_ref (T *ptr, unsigned c)
  {
    UniquePtr p;
    p.ptr = ptr;
    p.c = c;
    return p;
  }

  constexpr unsigned
  count () const
  {
    return c;
  }

  explicit constexpr
  operator bool () const
  {
    return ptr;
  }

  T &
  operator [] (unsigned index)
  {
    return ptr[index];
  }

  constexpr const T &
  operator [] (unsigned index) const
  {
    return ptr[index];
  }

  typedef T value_type;
  typedef T &reference;
  typedef const T &const_reference;
  typedef T *iterator;
  typedef const T *const_iterator;

  constexpr T *
  begin () const &
  {
    return ptr;
  }

  constexpr const T *
  cbegin () const &
  {
    return ptr;
  }

  constexpr T *
  end () const &
  {
    return ptr + c;
  }

  constexpr const T *
  cend () const &
  {
    return ptr + c;
  }
};

} /* namespace peel */
