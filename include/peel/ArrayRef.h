#pragma once

#include <peel/lang.h>

namespace peel
{

template<typename T>
class ArrayRef final
{
  T *p;
  size_t c;

public:
  // The default constructor is used by generated local copies.
  constexpr ArrayRef () noexcept
    : p (nullptr)
    , c (0)
  { }

  constexpr ArrayRef (decltype (nullptr)) noexcept
    : p (nullptr)
    , c (0)
  { }

  template<size_t C>
  constexpr ArrayRef (T (&arr)[C]) noexcept
    : p (arr)
    , c (C)
  { }

  constexpr ArrayRef (T *ptr, size_t size) noexcept
    : p (ptr)
    , c (size)
  { }

  constexpr T *
  ptr () const noexcept
  {
    return p;
  }

  constexpr size_t
  size () const noexcept
  {
    return c;
  }

  explicit constexpr
  operator bool () const noexcept
  {
    return p;
  }

  constexpr T &
  operator [] (size_t index) const noexcept
  {
    return p[index];
  }

  constexpr ArrayRef
  slice (size_t start, size_t length) const noexcept
  {
    return ArrayRef { p + start, length };
  }

  typedef T value_type;
  typedef T &reference;
  typedef const T &const_reference;
  typedef T *iterator;
  typedef const T *const_iterator;

  constexpr T *
  begin () const noexcept
  {
    return p;
  }

  constexpr const T *
  cbegin () const noexcept
  {
    return p;
  }

  constexpr T *
  end () const noexcept
  {
    return p + c;
  }

  constexpr const T *
  cend () const noexcept
  {
    return p + c;
  }
};

} /* namespace peel */
