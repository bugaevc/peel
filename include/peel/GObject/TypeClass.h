#pragma once

#include <peel/RefPtr.h>
#include <peel/GObject/Type.h>
#include <peel/lang.h>
#include <glib-object.h>

peel_begin_header

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
  ~TypeClass () { }

public:
  bool
  check_type (Type tp) const noexcept
  {
    return !!G_TYPE_CHECK_CLASS_TYPE (this, static_cast<::GType> (tp));
  }

  template<typename T>
  bool
  check_type () const
  {
    return check_type (Type::of<T> ());
  }

  void *
  cast (Type tp) noexcept
  {
#if defined (G_DISABLE_CAST_CHECKS) || defined (__OPTIMIZE__)
    (void) tp;
    return this;
#else
    return G_TYPE_CHECK_CLASS_CAST (this, static_cast<::GType> (tp), void);
#endif
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

  Type
  get_type () const noexcept
  {
    return G_TYPE_FROM_CLASS (this);
  }

  static RefPtr<TypeClass>
  get (Type tp) noexcept
  {
    ::GType type = static_cast<::GType> (tp);
    return reinterpret_cast<TypeClass *> (g_type_class_ref (type));
  }

  static TypeClass *
  peek (Type tp) noexcept
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

  static TypeClass *
  peek_static (Type tp) noexcept
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

  TypeClass *
  peek_parent () const noexcept
  {
    gpointer klass = const_cast<TypeClass *> (this);
    return reinterpret_cast<TypeClass *> (g_type_class_peek_parent (klass));
  }

  TypeInterface *
  peek_interface (Type iface_type) const noexcept
  {
    gpointer klass = const_cast<TypeClass *> (this);
    ::GType iface_tp = static_cast<::GType> (iface_type);
    return reinterpret_cast<TypeInterface *> (g_type_interface_peek (klass, iface_tp));
  }

  template<typename I>
  typename I::Iface *
  peek_interface () const
  {
    return reinterpret_cast<typename I::Iface *> (peek_interface (Type::of<I> ()));
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

  static constexpr
  bool can_ref_null = false;

  static void
  unref (T *klass)
  {
    g_type_class_unref (klass);
  }

  static constexpr
  bool can_unref_null = false;
};

} /* namespace peel */

peel_end_header
