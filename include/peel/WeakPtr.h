#pragma once

#include <glib-object.h>
#include <peel/lang.h>

namespace peel
{

namespace GObject
{
class Object;
};

template<typename T, peel::enable_if_derived<GObject::Object, T, int> = 0>
class WeakPtr final
{
private:
  T *ptr;

  peel_nothrow
  void
  add_weak_ptr ()
  {
    if (ptr)
      g_object_add_weak_pointer (reinterpret_cast<::GObject *> (ptr),
                                 reinterpret_cast<gpointer*> (&ptr));
  }

  peel_nothrow
  void
  remove_weak_ptr ()
  {
    if (ptr)
      g_object_remove_weak_pointer (reinterpret_cast<::GObject *> (ptr),
                                    reinterpret_cast<gpointer*> (&ptr));
  }

public:
  peel_nothrow
  constexpr WeakPtr ()
    : ptr (nullptr)
  { }

  peel_nothrow
  constexpr WeakPtr (decltype (nullptr))
    : ptr (nullptr)
  { }

  peel_nothrow
  WeakPtr (T *ptr)
    : ptr (ptr)
  {
    add_weak_ptr ();
  }

  peel_nothrow
  WeakPtr (const WeakPtr &other)
    : ptr (other.ptr)
  {
    add_weak_ptr ();
  }

  template<typename U, peel::enable_if_derived<T, U, int> = 0>
  peel_nothrow
  WeakPtr (const RefPtr<U> &ptr)
    : ptr (ptr)
  {
    add_weak_ptr ();
  }

  /* Upcast.  */
  template<typename U, peel::enable_if_derived<T, U, int> = 0>
  peel_nothrow
  WeakPtr (U *ptr)
    : ptr (ptr)
  {
    add_weak_ptr ();
  }

  /* Upcast.  */
  template<typename U, peel::enable_if_derived<T, U, int> = 0>
  peel_nothrow
  WeakPtr (const WeakPtr<U> &other)
    : ptr (static_cast<U *> (other))
  {
    add_weak_ptr ();
  }

  peel_nothrow
  ~WeakPtr ()
  {
    remove_weak_ptr ();
  }

  peel_nothrow
  WeakPtr &
  operator = (T *ptr)
  {
    if (ptr == this->ptr)
      return *this;
    remove_weak_ptr ();
    this->ptr = ptr;
    add_weak_ptr ();
    return *this;
  }

  peel_nothrow
  WeakPtr &
  operator = (const WeakPtr &other)
  {
    if (ptr == other.ptr)
      return *this;
    remove_weak_ptr ();
    ptr = other.ptr;
    add_weak_ptr ();
    return *this;
  }

  template<typename U, peel::enable_if_derived<T, U, int> = 0>
  peel_nothrow
  WeakPtr &
  operator = (const RefPtr<U> &other)
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

  peel_nothrow
  operator T * () &&
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

  peel_nothrow
  T *
  operator -> () &&
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
