#pragma once

#include <glib-object.h>
#include <peel/lang.h>

namespace peel
{

namespace GObject
{
class Object;
};

template<typename T /*, peel::enable_if_derived<GObject::Object, T, int> = 0*/>
class WeakPtr final
{
private:
  T *ptr;

  void
  add_weak_ptr () noexcept
  {
    if (ptr)
      g_object_add_weak_pointer (reinterpret_cast<::GObject *> (ptr),
                                 reinterpret_cast<gpointer*> (&ptr));
  }

  void
  remove_weak_ptr () noexcept
  {
    if (ptr)
      g_object_remove_weak_pointer (reinterpret_cast<::GObject *> (ptr),
                                    reinterpret_cast<gpointer*> (&ptr));
  }

public:
  constexpr WeakPtr () noexcept
    : ptr (nullptr)
  { }

  constexpr WeakPtr (decltype (nullptr)) noexcept
    : ptr (nullptr)
  { }

  WeakPtr (T *ptr) noexcept
    : ptr (ptr)
  {
    add_weak_ptr ();
  }

  WeakPtr (const WeakPtr &other) noexcept
    : ptr (other.ptr)
  {
    add_weak_ptr ();
  }

  template<typename U, peel::enable_if_derived<T, U, int> = 0>
  WeakPtr (const RefPtr<U> &ptr) noexcept
    : ptr (ptr)
  {
    add_weak_ptr ();
  }

  /* Upcast.  */
  template<typename U, peel::enable_if_derived<T, U, int> = 0>
  WeakPtr (U *ptr) noexcept
    : ptr (ptr)
  {
    add_weak_ptr ();
  }

  /* Upcast.  */
  template<typename U, peel::enable_if_derived<T, U, int> = 0>
  WeakPtr (const WeakPtr<U> &other) noexcept
    : ptr (static_cast<U *> (other))
  {
    add_weak_ptr ();
  }

  ~WeakPtr () noexcept
  {
    remove_weak_ptr ();
  }

  WeakPtr &
  operator = (T *ptr) noexcept
  {
    if (ptr == this->ptr)
      return *this;
    remove_weak_ptr ();
    this->ptr = ptr;
    add_weak_ptr ();
    return *this;
  }

  WeakPtr &
  operator = (const WeakPtr &other) noexcept
  {
    if (ptr == other.ptr)
      return *this;
    remove_weak_ptr ();
    ptr = other.ptr;
    add_weak_ptr ();
    return *this;
  }

  template<typename U, peel::enable_if_derived<T, U, int> = 0>
  WeakPtr &
  operator = (const RefPtr<U> &other) noexcept
  {
    if (ptr == static_cast<U *> (other))
      return *this;
    remove_weak_ptr ();
    ptr = other;
    add_weak_ptr ();
    return *this;
  }

  constexpr
  operator T * () const &
  {
    return ptr;
  }

  operator T * () && noexcept
  {
    remove_weak_ptr ();
    T *p = ptr;
    ptr = nullptr;
    return p;
  }

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
  operator -> () && noexcept
  {
    peel_assume (ptr);
    remove_weak_ptr ();
    T *p = ptr;
    ptr = nullptr;
    return p;
  }

  explicit constexpr
  operator bool () const
  {
    return ptr != nullptr;
  }
};

} /* namespace peel */
