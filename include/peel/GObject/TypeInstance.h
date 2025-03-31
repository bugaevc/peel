#pragma once

#include <peel/GObject/Type.h>
#include <peel/lang.h>
#include <glib-object.h>

peel_begin_header

namespace peel
{
namespace GObject
{

class TypeClass;
class TypeInterface;

class TypeInstance
{
private:
  ::GTypeInstance ti peel_no_warn_unused;

  TypeInstance () = delete;
  TypeInstance (const TypeInstance &) = delete;
  TypeInstance (TypeInstance &&) = delete;

  TypeInstance &
  operator = (const TypeInstance &) = delete;
  TypeInstance &
  operator = (TypeInstance &&) = delete;

protected:
  ~TypeInstance () = default;

public:
  bool
  check_type (Type tp) const noexcept
  {
    return !!G_TYPE_CHECK_INSTANCE_TYPE (this, static_cast<::GType> (tp));
  }

  template<typename T>
  bool
  check_type () const
  {
    return check_type (Type::of<T> ());
  }

  bool
  check_fundamental_type (Type tp) const noexcept
  {
    return !!G_TYPE_CHECK_INSTANCE_FUNDAMENTAL_TYPE (this, static_cast<::GType> (tp));
  }

  template<typename T>
  bool
  check_fundamental_type () const
  {
    return check_fundamental_type (Type::of<T> ());
  }

  void *
  cast (Type tp) noexcept
  {
#if defined (G_DISABLE_CAST_CHECKS) || defined (__OPTIMIZE__)
    (void) tp;
    return this;
#else
    return G_TYPE_CHECK_INSTANCE_CAST (this, static_cast<::GType> (tp), void);
#endif
  }

  template<typename T>
  T *
  cast ()
  {
#if defined (G_DISABLE_CAST_CHECKS) || defined (__OPTIMIZE__)
    return reinterpret_cast<T *> (this);
#else
    return reinterpret_cast<T *> (cast (Type::of<T> ()));
#endif
  }

  TypeClass *
  get_class () const noexcept
  {
    return G_TYPE_INSTANCE_GET_CLASS (this, this doesnt matter, TypeClass);
  }

  TypeInterface *
  get_interface (Type tp) const noexcept
  {
    return G_TYPE_INSTANCE_GET_INTERFACE (this, static_cast<::GType> (tp), TypeInterface);
  }

  template<typename I>
  typename I::Iface *
  get_interface () const noexcept
  {
    return reinterpret_cast<typename I::Iface *> (get_interface (Type::of<I> ()));
  }

  Type
  get_type () const noexcept
  {
    return G_TYPE_FROM_INSTANCE (this);
  }

  const char *
  get_type_name () const noexcept
  {
    ::GTypeInstance *ti = reinterpret_cast<::GTypeInstance *> (const_cast<TypeInstance *> (this));
    return g_type_name_from_instance (ti);
  }

protected:
  static TypeInstance *
  create (Type tp) noexcept
  {
    ::GTypeInstance *ti = g_type_create_instance (static_cast<::GType> (tp));
    return reinterpret_cast<TypeInstance *> (ti);
  }

  template<typename T>
  static T *
  create ()
  {
    return reinterpret_cast<T *> (create (Type::of<T> ()));
  }

  void
  free () noexcept
  {
    ::GTypeInstance *ti = reinterpret_cast<::GTypeInstance *> (this);
    g_type_free_instance (ti);
  }
};

static_assert (sizeof (TypeInstance) == sizeof (::GTypeInstance),
               "GObject::TypeInstance size mismatch");
static_assert (alignof (TypeInstance) == alignof (::GTypeInstance),
               "GObject::TypeInstance align mismatch");

} /* namespace GObject */
} /* namespace peel */

peel_end_header
