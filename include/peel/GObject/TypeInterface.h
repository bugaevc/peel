#pragma once

#include <peel/lang.h>
#include <glib-object.h>

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
  TypeInterface *
  peek_parent () const noexcept
  {
    gpointer iface = const_cast<TypeInterface *> (this);
    return reinterpret_cast<TypeInterface *> (g_type_interface_peek_parent (iface));
  }
};

static_assert (sizeof (TypeInterface) == sizeof (::GTypeInterface),
               "GObject::TypeInterface size mismatch");
static_assert (alignof (TypeInterface) == alignof (::GTypeInterface),
               "GObject::TypeInterface align mismatch");

} /* namespace GObject */
} /* namespace peel */
