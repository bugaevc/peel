#pragma once

#include <peel/lang.h>

namespace peel
{

template<typename T>
class ArrayRef
{
  T *p;
  size_t c;

public:
  // The default constructor is used by generated local copies.
  peel_nothrow
  constexpr ArrayRef ()
    : p (nullptr)
    , c (0)
  { }

  peel_nothrow
  constexpr ArrayRef (decltype (nullptr))
    : p (nullptr)
    , c (0)
  { }

  template<size_t C>
  peel_nothrow
  constexpr ArrayRef (T (&arr)[C])
    : p (arr)
    , c (C)
  { }

  peel_nothrow
  constexpr ArrayRef (T *ptr, size_t size)
    : p (ptr)
    , c (size)
  { }

  peel_nothrow
  constexpr T *
  ptr () const
  {
    return p;
  }

  peel_nothrow
  constexpr size_t
  size () const
  {
    return c;
  }

  peel_nothrow
  explicit constexpr
  operator bool () const
  {
    return p;
  }

  peel_nothrow
  T &
  operator [] (size_t index)
  {
    return p[index];
  }

  peel_nothrow
  constexpr const T &
  operator [] (size_t index) const
  {
    return p[index];
  }

  typedef T value_type;
  typedef T &reference;
  typedef const T &const_reference;
  typedef T *iterator;
  typedef const T *const_iterator;

  peel_nothrow
  constexpr T *
  begin () const
  {
    return p;
  }

  peel_nothrow
  constexpr const T *
  cbegin () const
  {
    return p;
  }

  peel_nothrow
  constexpr T *
  end () const
  {
    return p + c;
  }

  peel_nothrow
  constexpr const T *
  cend () const
  {
    return p + c;
  }
};

} /* namespace peel */
