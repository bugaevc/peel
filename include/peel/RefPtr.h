#pragma once

#include <peel/lang.h>

namespace peel
{

template<typename T, typename = void>
struct RefTraits;
/*
{
  static void
  ref (T *);

  constexpr static
  bool can_ref_null;

  static void
  unref (T *);

  constexpr static
  bool can_unref_null;

  static void
  ref_sink (T *);

  static void
  sink (T *);
}
*/

template<typename T>
class FloatPtr;

template<typename T>
class RefPtr final
{
private:
  template<typename U>
  friend class RefPtr;

  T *ptr;

  peel_always_inline
  static void
  do_ref (T *ptr)
  {
#ifdef __GNUC__
    if (__builtin_constant_p (ptr == nullptr) && (ptr == nullptr))
      return;
    if (RefTraits<T>::can_ref_null || ptr)
      RefTraits<T>::ref (ptr);
#else
    if (ptr)
      RefTraits<T>::ref (ptr);
#endif
  }

  peel_always_inline
  static void
  do_unref (T *ptr)
  {
#ifdef __GNUC__
    if (__builtin_constant_p (ptr == nullptr) && (ptr == nullptr))
      return;
    if (RefTraits<T>::can_unref_null || ptr)
      RefTraits<T>::unref (ptr);
#else
    if (ptr)
      RefTraits<T>::unref (ptr);
#endif
  }

public:
  constexpr RefPtr () noexcept
    : ptr (nullptr)
  { }

  constexpr RefPtr (decltype (nullptr)) noexcept
    : ptr (nullptr)
  { }

  RefPtr (T *ptr) noexcept
    : ptr (ptr)
  {
    do_ref (ptr);
  }

  RefPtr (const RefPtr &other) noexcept
    : ptr (other.ptr)
  {
    do_ref (ptr);
  }

  RefPtr (RefPtr &&other) noexcept
    : ptr (other.ptr)
  {
    other.ptr = nullptr;
  }

  /* FIXME: This causes weird compilation errors w/ libdex
   * due to trying to evaluate
   * peel::enable_if_derived<peel::Dex::Future, ::DexFuture>
   * for some unclear reason, where struct _DexFuture is only
   * forward-declared.  Was this constructor even useful for
   * anything, given that raw pointers upcast automatically?
   * As a better overload resolution candidate compared to
   * something else perhaps?

  template<typename U, peel::enable_if_derived<T, U, int> = 0>
  RefPtr (U *ptr) noexcept
    : ptr (ptr)
  {
    do_ref (ptr);
  }
  */

  /* Upcast.  */
  template<typename U, peel::enable_if_derived<T, U, int> = 0>
  RefPtr (const RefPtr<U> &other) noexcept
    : ptr (other.ptr)
  {
    do_ref (ptr);
  }

  /* Upcast.  */
  template<typename U, peel::enable_if_derived<T, U, int> = 0>
  RefPtr (RefPtr<U> &&other) noexcept
    : ptr (other.ptr)
  {
    other.ptr = nullptr;
  }

  template<typename U, peel::enable_if_derived<T, U, int> = 0>
  RefPtr (const FloatPtr<U> &f) noexcept
    : RefPtr ((T *) (U *) f)
  { }

  template<typename U, peel::enable_if_derived<T, U, int> = 0>
  RefPtr (FloatPtr<U> &&f) noexcept
    : RefPtr (static_cast<FloatPtr<U> &&> (f).ref_sink ())
  { }

  ~RefPtr () noexcept
  {
    do_unref (ptr);
  }

  static RefPtr
  adopt_ref (T *ptr) noexcept
  {
    RefPtr p;
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

  template<typename U, peel::enable_if_derived<T, U, int> = 0>
  RefPtr &
  operator = (U *ptr) noexcept
  {
    do_ref (ptr);
    do_unref (this->ptr);
    this->ptr = ptr;
    return *this;
  }

  RefPtr &
  operator = (const RefPtr &other) noexcept
  {
    do_ref (other.ptr);
    do_unref (this->ptr);
    this->ptr = other.ptr;
    return *this;
  }

  /* upcast */
  template<typename U, peel::enable_if_derived<T, U, int> = 0>
  RefPtr &
  operator = (const RefPtr<U> &other) noexcept
  {
    do_ref (other.ptr);
    do_unref (this->ptr);
    this->ptr = other.ptr;
    return *this;
  }

  RefPtr &
  operator = (RefPtr &&other) & noexcept
  {
    do_unref (ptr);
    ptr = other.ptr;
    other.ptr = nullptr;
    return *this;
  }

  /* upcast */
  template<typename U, peel::enable_if_derived<T, U, int> = 0>
  RefPtr &
  operator = (RefPtr<U> &&other) & noexcept
  {
    do_unref (ptr);
    ptr = other.ptr;
    other.ptr = nullptr;
    return *this;
  }

  template<typename U, peel::enable_if_derived<T, U, int> = 0>
  RefPtr &
  operator = (FloatPtr<U> &other) & noexcept
  {
    *this = (T *) (U *) other;
    return *this;
  }

  template<typename U, peel::enable_if_derived<T, U, int> = 0>
  RefPtr &
  operator = (FloatPtr<U> &&other) & noexcept
  {
    *this = static_cast<FloatPtr<U> &&> (other).ref_sink ();
    return *this;
  }

  RefPtr &
  operator = (decltype (nullptr)) noexcept
  {
    do_unref (this->ptr);
    this->ptr = nullptr;
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
  operator -> () const &
  {
    return ptr;
  }

  T *
  operator -> () && = delete;

  explicit constexpr
  operator bool () const
  {
    return ptr != nullptr;
  }

  template<typename U>
  RefPtr<U>
  cast () && noexcept
  {
    U *u_ptr = ptr->template cast<U> ();
    ptr = nullptr;
    return RefPtr<U>::adopt_ref (u_ptr);
  }
};

} /* namespace peel */
