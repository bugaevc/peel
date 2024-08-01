#pragma once

#include <peel/GObject/Type.h>
#include <peel/lang.h>
#include <glib-object.h>

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
  peel_nothrow
  bool
  check_type (Type tp) const
  {
    return !!G_TYPE_CHECK_INSTANCE_TYPE (this, static_cast<::GType> (tp));
  }

  template<typename T>
  bool
  check_type () const
  {
    return check_type (Type::of<T> ());
  }

  peel_nothrow
  bool
  check_fundamental_type (Type tp) const
  {
    return !!G_TYPE_CHECK_INSTANCE_FUNDAMENTAL_TYPE (this, static_cast<::GType> (tp));
  }

  template<typename T>
  bool
  check_fundamental_type () const
  {
    return check_fundamental_type (Type::of<T> ());
  }

  peel_nothrow
  void *
  cast (Type tp)
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

  peel_nothrow
  TypeClass *
  get_class () const
  {
    return G_TYPE_INSTANCE_GET_CLASS (this, this doesnt matter, TypeClass);
  }

  peel_nothrow
  TypeInterface *
  get_interface (Type tp) const
  {
    return G_TYPE_INSTANCE_GET_INTERFACE (this, static_cast<::GType> (tp), TypeInterface);
  }

  template<typename I>
  peel_nothrow
  typename I::Iface *
  get_interface () const
  {
    return reinterpret_cast<typename I::Iface *> (get_interface (Type::of<I> ()));
  }

  peel_nothrow
  Type
  get_type () const
  {
    return G_TYPE_FROM_INSTANCE (this);
  }

  peel_nothrow
  const char *
  get_type_name () const
  {
    ::GTypeInstance *ti = reinterpret_cast<::GTypeInstance *> (const_cast<TypeInstance *> (this));
    return g_type_name_from_instance (ti);
  }

protected:
  peel_nothrow
  static TypeInstance *
  create (Type tp)
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

  peel_nothrow
  void
  free ()
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
