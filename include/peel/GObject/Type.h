#pragma once

#include <peel/lang.h>
#include <glib-object.h>

namespace peel
{
namespace GObject
{

class Type
{
private:
  ::GType tp;

public:
  constexpr Type ()
    : tp (G_TYPE_INVALID)
  { }

  constexpr Type (::GType tp)
    : tp (tp)
  { }

  constexpr
  operator ::GType () const
  {
    return tp;
  }

  constexpr static Type
  invalid ()
  {
    return Type ();
  }

  template<typename T>
  static Type
  of ();

  constexpr bool
  operator == (const Type &other) const
  {
    return tp == other.tp;
  }

  constexpr bool
  operator != (const Type &other) const
  {
    return tp != other.tp;
  }

  peel_nothrow
  const char *
  name () const
  {
    return g_type_name (tp);
  }

  peel_nothrow
  static Type
  from_name (const char *name)
  {
    return g_type_from_name (name);
  }

  Type
  fundamental () const
  {
    return G_TYPE_FUNDAMENTAL (tp);
  }

  peel_nothrow
  Type
  parent () const
  {
    return g_type_parent (tp);
  }

  peel_nothrow
  unsigned int
  depth () const
  {
    return g_type_depth (tp);
  }

  // next_base

  peel_nothrow
  bool
  is_a (Type other) const
  {
    return !!g_type_is_a (tp, other.tp);
  }

  peel_nothrow
  int
  get_instance_count () const
  {
    return g_type_get_instance_count (tp);
  }

  peel_nothrow
  constexpr bool
  is_fundamental () const
  {
    return !!G_TYPE_IS_FUNDAMENTAL (tp);
  }

  peel_nothrow
  constexpr bool
  is_derived () const
  {
    return !!G_TYPE_IS_DERIVED (tp);
  }

  peel_nothrow
  bool
  is_interface () const
  {
    return !!G_TYPE_IS_INTERFACE (tp);
  }

  peel_nothrow
  bool
  is_classed () const
  {
    return !!G_TYPE_IS_CLASSED (tp);
  }

  peel_nothrow
  bool
  is_instantiatable () const
  {
    return !!G_TYPE_IS_INSTANTIATABLE (tp);
  }

  peel_nothrow
  bool
  is_derivable () const
  {
    return !!G_TYPE_IS_DERIVABLE (tp);
  }

  peel_nothrow
  bool
  is_abstract () const
  {
    return !!G_TYPE_IS_ABSTRACT (tp);
  }

  peel_nothrow
  bool
  is_value_type () const
  {
    return !!G_TYPE_IS_VALUE_TYPE (tp);
  }

  peel_nothrow
  bool
  has_value_table () const
  {
    return !!G_TYPE_HAS_VALUE_TABLE (tp);
  }

  peel_nothrow
  bool
  is_final () const
  {
    return !!G_TYPE_IS_FINAL (tp);
  }

#if defined (G_TYPE_IS_DEPRECATED)
  peel_nothrow
  bool
  is_deprecated () const
  {
    return !!G_TYPE_IS_DEPRECATED (tp);
  }
#else
  bool
  is_deprecated () const
  {
    return false;
  }
#endif

  peel_nothrow
  bool
  is_object () const
  {
    return !!G_TYPE_IS_OBJECT (tp);
  }

  peel_nothrow
  bool
  is_boxed () const
  {
    return !!G_TYPE_IS_BOXED (tp);
  }

  peel_nothrow
  void
  ensure () const
  {
#ifdef __GNUC__
    __asm__ volatile ("" :: "rm" (tp));
#else
    g_type_ensure (tp);
#endif
  }
};

template<>
constexpr inline Type
Type::of<void> ()
{
  return G_TYPE_NONE;
}

/* TODO: G_TYPE_INTERFACE */

template<>
constexpr inline Type
Type::of<signed char> ()
{
  return G_TYPE_CHAR;
}

template<>
constexpr inline Type
Type::of<unsigned char> ()
{
  return G_TYPE_UCHAR;
}

template<>
constexpr inline Type
Type::of<bool> ()
{
  return G_TYPE_BOOLEAN;
}

template<>
constexpr inline Type
Type::of<int> ()
{
  return G_TYPE_INT;
}

template<>
constexpr inline Type
Type::of<unsigned int> ()
{
  return G_TYPE_UINT;
}

template<>
constexpr inline Type
Type::of<long> ()
{
  return G_TYPE_LONG;
}

template<>
constexpr inline Type
Type::of<unsigned long> ()
{
  return G_TYPE_ULONG;
}

/* TODO: int64, uint64 might be same as long, what do we do? */

/* TODO: enum, flags */

template<>
constexpr inline Type
Type::of<float> ()
{
  return G_TYPE_FLOAT;
}

template<>
constexpr inline Type
Type::of<double> ()
{
  return G_TYPE_DOUBLE;
}

template<>
constexpr inline Type
Type::of<const char *> ()
{
  return G_TYPE_STRING;
}

template<>
constexpr inline Type
Type::of<void *> ()
{
  return G_TYPE_POINTER;
}

template<>
inline Type
Type::of<Type> ()
{
  return G_TYPE_GTYPE;
}

/* TODO: boxed */

} /* namespace GObject */

using GObject::Type;

} /* namespace peel */
