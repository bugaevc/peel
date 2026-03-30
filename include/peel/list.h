#pragma once

#include <peel/lang.h>
#include <glib.h>
#include <new>

peel_begin_header

namespace peel
{

namespace internals
{

template<typename T, typename = void>
struct ListPack
{
  static_assert (sizeof (T) <= sizeof (gpointer));

private:
  union U
    {
      T value;
      gpointer ptr;
      U () : ptr (nullptr) { }
      ~U () { }
    };

public:
  typedef T UnpackValueType;
  typedef T &UnpackRefType;

  static gpointer
  pack (T &&value) noexcept
  {
    if (std::is_pointer<T>::value)
      return (gpointer) value;
    else
      {
        U u;
        new (&u.value) T (static_cast<T &&> (value));
        return u.ptr;
      }
  }

  static T &
  unpack_ref (gpointer &ptr) noexcept
  {
    return reinterpret_cast<T &> (ptr);
  }

  static T
  unpack_value (gpointer ptr) noexcept
  {
    /* Make a copy */
    return reinterpret_cast<const T &> (ptr);
  }
};

template<typename T>
struct ListPack<T, typename std::enable_if<std::is_integral<T>::value>::type>
{
  static_assert (sizeof (T) <= sizeof (gpointer));

  typedef T UnpackValueType;
  typedef T UnpackRefType;

  static gpointer
  pack (T value) noexcept
  {
    if (std::is_signed<T>::value)
      return GINT_TO_POINTER (value);
    else
      return GUINT_TO_POINTER (value);
  }

  static T
  unpack_ref (gpointer &ptr) noexcept
  {
    return unpack_value (ptr);
  }

  static T
  unpack_value (gpointer ptr) noexcept
  {
    if (std::is_signed<T>::value)
      return GPOINTER_TO_INT (ptr);
    else
      return GPOINTER_TO_UINT (ptr);
  }
};

} /* namespace internals */

} /* namespace peel */

peel_end_header
