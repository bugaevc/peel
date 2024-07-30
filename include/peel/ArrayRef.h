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

  constexpr ArrayRef (T *ptr, unsigned size)
    : p (ptr)
    , c (size)
  { }

  constexpr T *
  ptr () const
  {
    return p;
  }

  constexpr unsigned
  size () const
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

  typedef T value_type;
  typedef T &reference;
  typedef const T &const_reference;
  typedef T *iterator;
  typedef const T *const_iterator;

  constexpr T *
  begin () const
  {
    return p;
  }

  constexpr const T *
  cbegin () const
  {
    return p;
  }

  constexpr T *
  end () const
  {
    return p + c;
  }

  constexpr const T *
  cend () const
  {
    return p + c;
  }
};

} /* namespace peel */
