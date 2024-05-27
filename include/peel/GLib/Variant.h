#pragma once

#include <peel/lang.h>
#include <peel/GObject/Type.h>
#include <peel/GObject/Value.h>
#include <glib.h>

namespace peel
{
namespace GLib
{

class VariantIter;

class Variant
{
public:
  class Type;
  class Builder;
  typedef VariantIter Iter;
};

} /* namespace GLib */

template<>
inline GObject::Type
GObject::Type::of<GLib::Variant> ()
{
  return G_TYPE_VARIANT;
}

template<>
struct GObject::Value::Traits<GLib::Variant>
{
  typedef GLib::Variant *UnownedType;
  typedef RefPtr<GLib::Variant> OwnedType;

  static GLib::Variant *
  get (const ::GValue *value)
  {
    return reinterpret_cast<GLib::Variant *> (g_value_get_variant (value));
  }

  static void
  set (::GValue *value, GLib::Variant *v)
  {
    g_value_set_variant (value, reinterpret_cast<::GVariant *> (v));
  }

  static void
  take (::GValue *value, RefPtr<GLib::Variant> &&v)
  {
    ::GVariant *raw = reinterpret_cast<::GVariant *> (std::move (v).release_ref ());
    g_value_take_variant (value, raw);
  }
};

template<>
struct RefTraits<GLib::Variant, void>
{
  static void
  ref (GLib::Variant *v)
  {
    g_variant_ref (reinterpret_cast<::GVariant *> (v));
  }

  static void
  unref (GLib::Variant *v)
  {
    g_variant_unref (reinterpret_cast<::GVariant *> (v));
  }

  static void
  ref_sink (GLib::Variant *v)
  {
    g_variant_ref_sink (reinterpret_cast<::GVariant *> (v));
  }

  static void
  sink_unref (GLib::Variant *v)
  {
    g_variant_ref_sink (reinterpret_cast<::GVariant *> (v));
    g_variant_unref (reinterpret_cast<::GVariant *> (v));
  }
};

} /* namespace peel */
