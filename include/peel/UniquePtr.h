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
  peel_nothrow
  constexpr UniquePtr ()
    : ptr (nullptr)
  { }

  peel_nothrow
  constexpr UniquePtr (decltype (nullptr))
    : ptr (nullptr)
  { }

  UniquePtr (const UniquePtr &) = delete;

  peel_nothrow
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

  peel_nothrow
  static UniquePtr
  adopt_ref (T *ptr)
  {
    UniquePtr p;
    p.ptr = ptr;
    return p;
  }

  peel_nothrow
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
  size_t c;

  peel_nothrow
  void
  free ()
  {
    if (!std::is_trivially_destructible<T>::value)
      {
        for (size_t i = 0; i < c; i++)
          ptr[i].~T();
      }
    g_free (reinterpret_cast<gpointer> (ptr));
    ptr = nullptr;
    c = 0;
  }

public:
  peel_nothrow
  constexpr UniquePtr ()
    : ptr (nullptr)
    , c (0)
  { }

  peel_nothrow
  constexpr UniquePtr (decltype (nullptr))
    : ptr (nullptr)
    , c (0)
  { }

  peel_nothrow
  UniquePtr (UniquePtr &&other)
    : ptr (other.ptr)
    , c (other.c)
  {
    other.ptr = nullptr;
    other.c = 0;
  }

  UniquePtr (const UniquePtr &) = delete;

  peel_nothrow
  ~UniquePtr ()
  {
    free ();
  }

  peel_nothrow
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

  peel_nothrow
  static UniquePtr
  adopt_ref (T *ptr, size_t size)
  {
    UniquePtr p;
    p.ptr = ptr;
    p.c = size;
    return p;
  }

  constexpr size_t
  size () const
  {
    return c;
  }

  explicit constexpr
  operator bool () const
  {
    return ptr;
  }

  T &
  operator [] (size_t index)
  {
    return ptr[index];
  }

  constexpr const T &
  operator [] (size_t index) const
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
