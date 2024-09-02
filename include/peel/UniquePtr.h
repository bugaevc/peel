#pragma once

#include <peel/lang.h>
#include <glib.h>

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
class UniquePtr final
{
private:
  template<typename U>
  friend class UniquePtr;

  T *ptr;

public:
  constexpr UniquePtr () noexcept
    : ptr (nullptr)
  { }

  constexpr UniquePtr (decltype (nullptr)) noexcept
    : ptr (nullptr)
  { }

  UniquePtr (const UniquePtr &) = delete;

  UniquePtr (UniquePtr &&other) noexcept
    : ptr (other.ptr)
  {
    other.ptr = nullptr;
  }

  ~UniquePtr () noexcept
  {
    if (ptr)
      UniqueTraits<T>::free (ptr);
  }

  static UniquePtr
  adopt_ref (T *ptr) noexcept
  {
    UniquePtr p;
    p.ptr = ptr;
    return p;
  }

  peel_nodiscard ("the reference will leak if unused")
  T *
  release_ref () && noexcept
  {
    T *p = ptr;
    ptr = nullptr;
    return p;
  }

  UniquePtr &
  operator = (const UniquePtr &) = delete;

  UniquePtr &
  operator = (UniquePtr &&other) noexcept
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
class ArrayRef;

template<typename T>
class UniquePtr<T[]> final
{
private:
  T *ptr;
  size_t c;

  void
  free () noexcept
  {
    if (!std::is_trivially_destructible<T>::value)
      {
        for (size_t i = 0; i < c; i++)
          ptr[i].~T();
      }
#ifdef __GNUC__
    /* It is safe to call g_free (nullptr), so don't emit an extra
       runtime check.  But also don't call g_free (nullptr) when we
       know at compile time that it's nullptr that we're dealing with. */
    if (!__builtin_constant_p (ptr == nullptr) || (ptr != nullptr))
#endif
    g_free (reinterpret_cast<gpointer> (ptr));
    ptr = nullptr;
    c = 0;
  }

public:
  constexpr UniquePtr () noexcept
    : ptr (nullptr)
    , c (0)
  { }

  constexpr UniquePtr (decltype (nullptr)) noexcept
    : ptr (nullptr)
    , c (0)
  { }

  UniquePtr (UniquePtr &&other) noexcept
    : ptr (other.ptr)
    , c (other.c)
  {
    other.ptr = nullptr;
    other.c = 0;
  }

  UniquePtr (const UniquePtr &) = delete;

  ~UniquePtr () noexcept
  {
    free ();
  }

  peel_nodiscard ("the reference will leak if unused")
  T *
  release_ref () && noexcept
  {
    T *p = ptr;
    ptr = nullptr;
    c = 0;
    return p;
  }

  UniquePtr &
  operator = (const UniquePtr &) = delete;

  UniquePtr &
  operator = (UniquePtr &&other) noexcept
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
  adopt_ref (T *ptr, size_t size) noexcept
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

  constexpr T &
  operator [] (size_t index) const
  {
    return ptr[index];
  }

  constexpr
  operator ArrayRef<T> () const &
  {
    return ArrayRef<T> { ptr, c };
  }

  operator ArrayRef<T> () && = delete;
  operator ArrayRef<const T> () && = delete;

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
