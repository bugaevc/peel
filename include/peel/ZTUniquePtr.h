#pragma once

#include <peel/lang.h>
#include <glib.h>

peel_begin_header

namespace peel
{

template<typename>
class ZTUniquePtr;

template<typename>
class ZTArrayRef;

class String;

template<typename T>
class ZTUniquePtr<T[]>
{
  T *ptr;

  void
  free () noexcept
  {
    /* Special-case path for strv */
    if (std::is_same<T, String>::value)
      {
#ifdef __GNUC__
        if (!__builtin_constant_p (ptr == nullptr) || (ptr != nullptr))
#endif
          g_strfreev (reinterpret_cast<gchar **> (reinterpret_cast<void *> (ptr)));
        ptr = nullptr;
        return;
      }

    if (!std::is_trivially_destructible<T>::value)
      {
        for (T *p = ptr; p && *p; p++)
          p->~T ();
      }
#ifdef __GNUC__
    /* It is safe to call g_free (nullptr), so don't emit an extra
       runtime check.  But also don't call g_free (nullptr) when we
       know at compile time that it's nullptr that we're dealing with. */
    if (!__builtin_constant_p (ptr == nullptr) || (ptr != nullptr))
#endif
      g_free (ptr);
    ptr = nullptr;
  }

public:
  constexpr ZTUniquePtr () noexcept
    : ptr (nullptr)
  { }

  constexpr ZTUniquePtr (decltype (nullptr)) noexcept
    : ptr (nullptr)
  { }

  ZTUniquePtr (ZTUniquePtr &&other) noexcept
    : ptr (other.ptr)
  {
    other.ptr = nullptr;
  }

  ZTUniquePtr (const ZTUniquePtr &) = delete;

  ~ZTUniquePtr () noexcept
  {
    free ();
  }

  peel_nodiscard ("the reference will leak if unused")
  T *
  release_ref () && noexcept
  {
    T *p = ptr;
    ptr = nullptr;
    return p;
  }

  ZTUniquePtr &
  operator = (const ZTUniquePtr &) = delete;

  ZTUniquePtr &
  operator = (ZTUniquePtr &&other) noexcept
  {
    if (this == &other)
      return *this;
    free ();
    ptr = other.ptr;
    other.ptr = nullptr;
    return *this;
  }

  static ZTUniquePtr
  adopt_ref (T *ptr) noexcept
  {
    ZTUniquePtr p;
    p.ptr = ptr;
    return p;
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

  size_t
  length () const noexcept
  {
    size_t l = 0;
    for (T *p = ptr; p; p++)
      l++;
    return l;
  }

  operator ZTArrayRef<T> () const & noexcept
  {
    return ZTArrayRef<T>::adopt (ptr);
  }

  operator ZTArrayRef<const T> () const & noexcept
  {
    return ZTArrayRef<const T>::adopt (ptr);
  }

  operator ZTArrayRef<T> () && = delete;
  operator ZTArrayRef<const T> () && = delete;

  typedef typename ZTArrayRef<T>::iterator iterator;
  typedef typename ZTArrayRef<const T>::iterator const_iterator;
  typedef T value_type;
  typedef T &reference;
  typedef const T &const_reference;

  iterator
  begin () const & noexcept
  {
    return ZTArrayRef<T> (*this).begin ();
  }

  const_iterator
  cbegin () const & noexcept
  {
    return ZTArrayRef<const T> (*this).begin ();
  }

  iterator
  end () const & noexcept
  {
    return ZTArrayRef<T> (*this).end ();
  }

  const_iterator
  cend () const & noexcept
  {
    return ZTArrayRef<const T> (*this).end ();
  }
};

} /* namespace peel */

peel_end_header
