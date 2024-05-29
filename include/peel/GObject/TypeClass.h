#pragma once

#include <peel/RefPtr.h>
#include <peel/GObject/Type.h>
#include <peel/lang.h>
#include <glib-object.h>

namespace peel
{
namespace GObject
{

class TypeInterface;

class TypeClass {
private:
  TypeClass () = delete;
  TypeClass (const TypeClass &) = delete;
  TypeClass (TypeClass &&) = delete;

  ::GTypeClass tc peel_no_warn_unused;

protected:
  ~TypeClass() { }

public:
  peel_nothrow
  bool
  check_type (Type tp) const
  {
    return !!G_TYPE_CHECK_CLASS_TYPE (this, static_cast<::GType> (tp));
  }

  template<typename T>
  bool
  check_type () const
  {
    return check_type (Type::of<T> ());
  }

  peel_nothrow
  void *
  cast (Type tp) const
  {
    return G_TYPE_CHECK_CLASS_CAST (this, static_cast<::GType> (tp), void);
  }

  template<typename T>
  typename T::Class *
  cast ()
  {
#if defined (G_DISABLE_CAST_CHECKS) || defined (__OPTIMIZE__)
    return reinterpret_cast<typename T::Class *> (this);
#else
    return reinterpret_cast<typename T::Class *> (cast (Type::of<T> ()));
#endif
  }

  peel_nothrow
  Type
  get_type () const
  {
    return G_TYPE_FROM_CLASS (this);
  }

  peel_nothrow
  static RefPtr<TypeClass>
  get (Type tp)
  {
    ::GType type = static_cast<::GType> (tp);
    return reinterpret_cast<TypeClass *> (g_type_class_ref (type));
  }

  peel_nothrow
  static TypeClass *
  peek (Type tp)
  {
    ::GType type = static_cast<::GType> (tp);
    return reinterpret_cast<TypeClass *> (g_type_class_peek (type));
  }

  template<typename T>
  static typename T::Class *
  peek ()
  {
    return reinterpret_cast<typename T::Class *> (peek (Type::of<T> ()));
  }

  peel_nothrow
  static TypeClass *
  peek_static (Type tp)
  {
    ::GType type = static_cast<::GType> (tp);
    return reinterpret_cast<TypeClass *> (g_type_class_peek_static (type));
  }

  template<typename T>
  static typename T::Class *
  peek_static ()
  {
    return reinterpret_cast<typename T::Class *> (peek_static (Type::of<T> ()));
  }

  peel_nothrow
  TypeClass *
  peek_parent () const
  {
    gpointer klass = const_cast<TypeClass *> (this);
    return reinterpret_cast<TypeClass *> (g_type_class_peek_parent (klass));
  }

  peel_nothrow
  TypeInterface *
  peek_interface (Type iface_type) const
  {
    gpointer klass = const_cast<TypeClass *> (this);
    ::GType iface_tp = static_cast<::GType> (iface_type);
    return reinterpret_cast<TypeInterface *> (g_type_interface_peek (klass, iface_tp));
  }

  template<typename I>
  typename I::Interface *
  peek_interface () const
  {
    return reinterpret_cast<typename I::Interface *> (peek_interface (Type::of<I> ()));
  }
};

static_assert (sizeof (TypeClass) == sizeof (::GTypeClass),
               "GObject::TypeClass size mismatch");
static_assert (alignof (TypeClass) == alignof (::GTypeClass),
               "GObject::TypeClass align mismatch");

} /* namespace GObject */

template<typename T>
struct RefTraits<T, peel::enable_if_derived<GObject::TypeClass, T, void>>
{
  static void
  ref (T *klass)
  {
    void *tc = g_type_class_ref (klass->get_type ());
    peel_assume (klass == tc);
  }

  static void
  unref (T *klass)
  {
    g_type_class_unref (klass);
  }
};

} /* namespace peel */
