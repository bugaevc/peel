#pragma once

#include <peel/lang.h>

peel_begin_header

namespace peel
{

template<typename T>
class ZArrayRef
{
   T *p;

public:
  constexpr ZArrayRef () noexcept
    : p (nullptr)
  { }

  constexpr ZArrayRef (decltype (nullptr)) noexcept
    : p (nullptr)
  { }

  template<size_t C>
  constexpr ZArrayRef (T (&arr)[C]) noexcept
    : p (arr)
  {
    g_assert (C > 0);
    g_assert (arr[C - 1] == 0);
  }

  constexpr T *
  data () const noexcept
  {
    return p;
  }

  explicit constexpr
  operator bool () const noexcept
  {
    return p && p[0];
  }

  constexpr T &
  operator [] (size_t index) const noexcept
  {
    return p[index];
  }

  static ZArrayRef
  adopt (T *ptr) noexcept
  {
    ZArrayRef<T> arr;
    arr.p = ptr;
    return arr;
  }

  operator ZArrayRef<const T> () const noexcept
  {
    return ZArrayRef<const T>::adopt (p);
  }

  ZArrayRef
  slice (size_t start) const noexcept
  {
    return adopt (p + start);
  }

  constexpr
  ArrayRef<T>
  slice (size_t start, size_t length) const noexcept
  {
    return ArrayRef<T> { p + start, length };
  }

  /* TODO: C++ iteration */
};

} /* namespace peel */

peel_end_header
