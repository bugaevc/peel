#pragma once

#include <cstdint>

#include <peel/lang.h>
#include <peel/UniquePtr.h>
#include <glib-object.h>

peel_begin_header

namespace peel
{

class String;

namespace GObject
{

struct Long final
{
  long value;

  constexpr
  Long (long value) noexcept
    : value (value)
  { }

  constexpr
  operator long () const noexcept
  {
    return value;
  }
};

struct ULong final
{
  unsigned long value;

  constexpr
  ULong (unsigned long value) noexcept
    : value (value)
  { }

  constexpr
  operator unsigned long () const noexcept
  {
    return value;
  }
};

class Type final
{
private:
  ::GType tp;

public:
  constexpr Type () noexcept
    : tp (G_TYPE_INVALID)
  { }

  constexpr Type (::GType tp) noexcept
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

  constexpr static Type
  interface_ ()
  {
    return G_TYPE_INTERFACE;
  }

  constexpr static Type
  boxed ()
  {
    return G_TYPE_BOXED;
  }

  constexpr static Type
  enum_ ()
  {
    return G_TYPE_ENUM;
  }

  constexpr static Type
  flags ()
  {
    return G_TYPE_FLAGS;
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

  const char *
  name () const noexcept
  {
    return g_type_name (tp);
  }

  static Type
  from_name (const char *name) noexcept
  {
    return g_type_from_name (name);
  }

  Type
  fundamental () const
  {
    return G_TYPE_FUNDAMENTAL (tp);
  }

  Type
  parent () const noexcept
  {
    return g_type_parent (tp);
  }

  unsigned int
  depth () const noexcept
  {
    return g_type_depth (tp);
  }

  Type
  next_base (Type base_type) const noexcept
  {
    return g_type_next_base (tp, base_type);
  }

  bool
  is_a (Type other) const noexcept
  {
    return !!g_type_is_a (tp, other.tp);
  }

  template<typename T>
  bool
  is_a () const
  {
    return is_a (of<T> ());
  }

  int
  get_instance_count () const noexcept
  {
    return g_type_get_instance_count (tp);
  }

  constexpr bool
  is_fundamental () const noexcept
  {
    return !!G_TYPE_IS_FUNDAMENTAL (tp);
  }

  constexpr bool
  is_derived () const noexcept
  {
    return !!G_TYPE_IS_DERIVED (tp);
  }

  bool
  is_interface () const noexcept
  {
    return !!G_TYPE_IS_INTERFACE (tp);
  }

  bool
  is_classed () const noexcept
  {
    return !!G_TYPE_IS_CLASSED (tp);
  }

  bool
  is_instantiatable () const noexcept
  {
    return !!G_TYPE_IS_INSTANTIATABLE (tp);
  }

  bool
  is_derivable () const noexcept
  {
    return !!G_TYPE_IS_DERIVABLE (tp);
  }

  bool
  is_abstract () const noexcept
  {
    return !!G_TYPE_IS_ABSTRACT (tp);
  }

  bool
  is_value_type () const noexcept
  {
    return !!G_TYPE_IS_VALUE_TYPE (tp);
  }

  bool
  has_value_table () const noexcept
  {
    return !!G_TYPE_HAS_VALUE_TABLE (tp);
  }

  bool
  is_final () const noexcept
  {
    return !!G_TYPE_IS_FINAL (tp);
  }

#if defined (G_TYPE_IS_DEPRECATED)
  bool
  is_deprecated () const noexcept
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

  bool
  is_object () const noexcept
  {
    return !!G_TYPE_IS_OBJECT (tp);
  }

  bool
  is_boxed () const noexcept
  {
    return !!G_TYPE_IS_BOXED (tp);
  }

  void
  ensure () const noexcept
  {
#ifdef __GNUC__
    __asm__ volatile ("" :: "rm" (tp));
#else
    g_type_ensure (tp);
#endif
  }

  UniquePtr<Type[]>
  children () const noexcept
  {
    guint n_children = 0;
    ::GType *ch = g_type_children (tp, &n_children);
    return UniquePtr<Type[]>::adopt_ref (reinterpret_cast<Type *> (ch), n_children);
  }

  UniquePtr<Type[]>
  interfaces () const noexcept
  {
    guint n_interfaces = 0;
    ::GType *is = g_type_interfaces (tp, &n_interfaces);
    return UniquePtr<Type[]>::adopt_ref (reinterpret_cast<Type *> (is), n_interfaces);
  }
};

template<>
constexpr inline Type
Type::of<void> ()
{
  return G_TYPE_NONE;
}

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
Type::of<Long> ()
{
  return G_TYPE_LONG;
}

template<>
constexpr inline Type
Type::of<ULong> ()
{
  return G_TYPE_ULONG;
}

template<>
constexpr inline Type
Type::of<int64_t> ()
{
  return G_TYPE_INT64;
}

template<>
constexpr inline Type
Type::of<uint64_t> ()
{
  return G_TYPE_UINT64;
}

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
Type::of<String> ()
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

} /* namespace GObject */

using GObject::Type;

} /* namespace peel */

peel_end_header
