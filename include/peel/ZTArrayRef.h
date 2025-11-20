#pragma once

#include <peel/lang.h>

peel_begin_header

namespace peel
{

template<typename T>
class ZTArrayRef
{
   T *ptr;

public:
  constexpr ZTArrayRef () noexcept
    : ptr (nullptr)
  { }

  constexpr ZTArrayRef (decltype (nullptr)) noexcept
    : ptr (nullptr)
  { }

  template<size_t C>
  ZTArrayRef (T (&arr)[C]) noexcept
    : ptr (arr)
  {
    g_assert (C > 0);
    g_assert (!arr[C - 1]);
  }

  constexpr T *
  data () const noexcept
  {
    return ptr;
  }

  explicit constexpr
  operator bool () const noexcept
  {
    return ptr && ptr[0];
  }

  constexpr T &
  operator [] (size_t index) const noexcept
  {
    return ptr[index];
  }

  static ZTArrayRef
  adopt (T *ptr) noexcept
  {
    ZTArrayRef arr;
    arr.ptr = ptr;
    return arr;
  }

  size_t
  length () const noexcept
  {
    size_t l = 0;
    for (T *p = ptr; p; p++)
      l++;
    return l;
  }

  operator ZTArrayRef<const T> () const noexcept
  {
    return ZTArrayRef<const T>::adopt (ptr);
  }

  ZTArrayRef
  slice (size_t start) const noexcept
  {
    return adopt (ptr + start);
  }

  constexpr ArrayRef<T>
  slice (size_t start, size_t length) const noexcept
  {
    return ArrayRef<T> { ptr + start, length };
  }

  /* C++ iteration */

  class iterator
  {
    friend class ZTArrayRef;
    T *ptr;

    constexpr
    iterator (T *ptr) noexcept
      : ptr (ptr)
    { }

  public:
    constexpr T &
    operator * () const noexcept
    {
      return *ptr;
    }

    constexpr T *
    operator -> () const noexcept
    {
      return ptr;
    }

    iterator &
    operator ++ () noexcept
    {
      ++ptr;
      return *this;
    }

    iterator
    operator ++ (int) noexcept
    {
      iterator other { ptr };
      ptr++;
      return other;
    }

    bool
    operator == (const iterator &other) const noexcept
    {
      if (ptr == nullptr && other.ptr == nullptr)
        return true;
      else if (ptr == nullptr)
        return !*other.ptr;
      else if (other.ptr == nullptr)
        return !*ptr;
      else
        return ptr == other.ptr;
    }

    bool
    operator != (const iterator &other) const noexcept
    {
      return !(*this == other);
    }
  };

  typedef typename ZTArrayRef<const T>::iterator const_iterator;
  typedef T value_type;
  typedef T &reference;
  typedef const T &const_reference;

  iterator
  begin () const noexcept
  {
    return iterator { ptr };
  }

  const_iterator
  cbegin () const noexcept
  {
    return const_iterator { ptr };
  }

  iterator
  end () const noexcept
  {
    return iterator { nullptr };
  }

  const_iterator
  cend () const noexcept
  {
    return const_iterator { nullptr };
  }
};

} /* namespace peel */

peel_end_header
