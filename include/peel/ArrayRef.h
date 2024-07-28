#pragma once

namespace peel
{

template<typename T>
class ArrayRef
{
  T *p;
  unsigned c;

public:
  // The default constructor is used by generated local copies.
  constexpr ArrayRef ()
    : p (nullptr)
    , c (0)
  { }

  constexpr ArrayRef (decltype (nullptr))
    : p (nullptr)
    , c (0)
  { }

  template<unsigned C>
  constexpr ArrayRef (T (&arr)[C])
    : p (arr)
    , c (C)
  { }

  constexpr ArrayRef (T *ptr, unsigned count)
    : p (ptr)
    , c (count)
  { }

  constexpr T *
  ptr () const
  {
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
    return p;
  }

  T &
  operator [] (unsigned index)
  {
    return p[index];
  }

  constexpr const T &
  operator [] (unsigned index) const
  {
    return p[index];
  }
};

}
