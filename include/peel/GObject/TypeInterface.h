#pragma once

#include <peel/lang.h>
#include <glib-object.h>

peel_begin_header

namespace peel
{
namespace GObject
{

class TypeInterface
{
  TypeInterface () = delete;
  TypeInterface (const TypeInterface &) = delete;
  TypeInterface (TypeInterface &&) = delete;

  ::GTypeInterface ti peel_no_warn_unused;

protected:
  ~TypeInterface () { }

public:
  Type
  get_type () const noexcept
  {
    return G_TYPE_FROM_INTERFACE (this);
  }

  TypeInterface *
  peek_parent () const noexcept
  {
    gpointer iface = const_cast<TypeInterface *> (this);
    return reinterpret_cast<TypeInterface *> (g_type_interface_peek_parent (iface));
  }

  static TypeInterface *
  peek_default (Type tp)
  {
    ::GType type = static_cast<::GType> (tp);
    return reinterpret_cast<TypeInterface *> (g_type_default_interface_peek (type));
  }

  template<typename I>
  static typename I::Iface *
  peek_default ()
  {
    return reinterpret_cast<typename I::Iface *> (peek_default (Type::of<I> ()));
  }
};

static_assert (sizeof (TypeInterface) == sizeof (::GTypeInterface),
               "GObject::TypeInterface size mismatch");
static_assert (alignof (TypeInterface) == alignof (::GTypeInterface),
               "GObject::TypeInterface align mismatch");

} /* namespace GObject */
} /* namespace peel */

peel_end_header
