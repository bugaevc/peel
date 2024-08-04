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
private:
  Variant () = delete;
  Variant (const Variant &) = delete;
  Variant (Variant &&) = delete;
  Variant &
  operator = (const Variant &) = delete;
  Variant &
  operator = (Variant &&) = delete;

public:
  class Type;
  struct Builder;
  typedef VariantIter Iter;

  peel_nothrow
  const Type *
  get_type ()
  {
    ::GVariant *_peel_this = reinterpret_cast<::GVariant *> (this);
    const ::GVariantType *_peel_return = g_variant_get_type (_peel_this);
    return reinterpret_cast<const Type *> (_peel_return);
  }

  peel_nothrow
  const char *
  get_type_string ()
  {
    ::GVariant *_peel_this = reinterpret_cast<::GVariant *> (this);
    return g_variant_get_type (_peel_this);
  }

  peel_nothrow
  bool
  is_of_type (const GVariantType *type)
  {
    ::GVariant *_peel_this = reinterpret_cast<::GVariant *> (this);
    const ::GVariantType *_peel_type = reinterpret_cast<const ::GVariantType *> (type);
    gboolean _peel_return = g_variant_is_of_type (_peel_this, _peel_type);
    return !!_peel_return;
  }

  // ...

  template<typename ...Args>
  peel_nothrow
  static peel::FloatPtr<Variant>
  create (const char *format_string, Args... args)
  {
    // TODO: this should handle floating refs properly
    ::GVariant *_peel_return = g_variant_new (format_string, args...);
    return peel::FloatPtr<Variant> (reinterpret_cast<Variant *> (_peel_return));
  }
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
