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
  template<typename>
  struct Traits;

  class Type;
  struct Builder;
  typedef VariantIter Iter;

  peel_nothrow
  const Variant::Type *
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
    return g_variant_get_type_string (_peel_this);
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

  peel_nothrow
  bool
  is_container ()
  {
    ::GVariant *_peel_this = reinterpret_cast<::GVariant *> (this);
    gboolean _peel_return = g_variant_is_container (_peel_this);
    return !!_peel_return;
  }

  // ...

  template<typename ...Args>
  peel_nothrow
  static peel::FloatPtr<Variant>
  create (const char *format_string, Args... args)
  {
    ::GVariant *_peel_return = g_variant_new (format_string, Traits<Args>::cast_for_create (args)...);
    peel_assume (_peel_return);
    return peel::FloatPtr<Variant> (reinterpret_cast<Variant *> (_peel_return));
  }

  template<typename T>
  peel_nothrow
  static peel::FloatPtr<Variant>
  create (typename Traits<T>::CreateCppType value)
  {
    ::GVariant *v = Traits<T>::create (std::move (value));
    peel_assume (v);
    return peel::FloatPtr<Variant> (reinterpret_cast<Variant *> (v));
  }

  peel_nothrow
  static peel::FloatPtr<Variant>
  create_maybe (const Variant::Type *child_type, peel::FloatPtr<Variant> child)
  {
    const ::GVariantType *_peel_child_type = reinterpret_cast<const ::GVariantType *> (child_type);
    ::GVariant *_peel_child = reinterpret_cast<::GVariant *> (std::move (child).release_floating_ptr ());
    ::GVariant *_peel_return = g_variant_new_maybe (_peel_child_type, _peel_child);
    peel_assume (_peel_return);
    return peel::FloatPtr<Variant> (reinterpret_cast<Variant *> (_peel_return));
  }

  peel_nothrow
  static peel::FloatPtr<Variant>
  create_object_path (const char *object_path)
  {
    ::GVariant *_peel_return = g_variant_new_object_path (object_path);
    peel_assume (_peel_return);
    return peel::FloatPtr<Variant> (reinterpret_cast<Variant *> (_peel_return));
  }

  peel_nothrow
  static peel::FloatPtr<Variant>
  create_dict_entry (peel::FloatPtr<Variant> key, peel::FloatPtr<Variant> value)
  {
    ::GVariant *_peel_key = reinterpret_cast<::GVariant *> (std::move (key).release_floating_ptr ());
    ::GVariant *_peel_value = reinterpret_cast<::GVariant *> (std::move (value).release_floating_ptr ());
    ::GVariant *_peel_return = g_variant_new_dict_entry (_peel_key, _peel_value);
    peel_assume (_peel_return);
    return peel::FloatPtr<Variant> (reinterpret_cast<Variant *> (_peel_return));
  }

  peel_nothrow
  static peel::FloatPtr<Variant>
  create_tuple (peel::ArrayRef<Variant * const> children)
  {
    ::GVariant * const *_peel_children = reinterpret_cast<::GVariant * const *> (children.ptr ());
    gsize _peel_n_children = children.size ();
    ::GVariant *_peel_return = g_variant_new_tuple (_peel_children, _peel_n_children);
    peel_assume (_peel_return);
    return peel::FloatPtr<Variant> (reinterpret_cast<Variant *> (_peel_return));
  }

  // TODO: FloatPtr-enabled overload of create_tuple

  template<typename T>
  peel_nothrow
  typename Traits<T>::GetCppType
  get ()
  {
    ::GVariant *v = reinterpret_cast<::GVariant *> (this);
    return Traits<T>::get (v);
  }

  peel_nothrow
  peel::RefPtr<Variant>
  get_child_value (size_t index)
  {
    ::GVariant *v = reinterpret_cast<::GVariant *> (this);
    ::GVariant *_peel_return = g_variant_get_child_value (v, index);
    peel_assume (_peel_return);
    return peel::RefPtr<Variant>::adopt_ref (reinterpret_cast<Variant *> (_peel_return));
  }

  peel_nothrow
  peel::RefPtr<Variant>
  get_maybe ()
  {
    ::GVariant *_peel_this = reinterpret_cast<::GVariant *> (this);
    ::GVariant *_peel_return = g_variant_get_maybe (_peel_this);
    return peel::RefPtr<Variant>::adopt_ref (reinterpret_cast<Variant *> (_peel_return));
  }

  peel_nothrow
  static bool
  is_object_path (const char *string)
  {
    gboolean _peel_return = g_variant_is_object_path (string);
    return !!_peel_return;
  }

  peel_nothrow
  static bool
  is_signature (const char *string)
  {
    gboolean _peel_return = g_variant_is_signature (string);
    return !!_peel_return;
  }
};

template<>
struct Variant::Traits<bool>
{
  // "b"
  typedef bool CreateCppType;
  typedef bool GetCppType;

  static gboolean
  cast_for_create (bool b)
  {
    return static_cast<gboolean> (b);
  }

  static bool
  get (::GVariant *v)
  {
    return !!g_variant_get_boolean (v);
  }

  static ::GVariant *
  create (bool b)
  {
    return g_variant_new_boolean (b);
  }
};

template<>
struct Variant::Traits<unsigned char>
{
  // "y"
  typedef unsigned char CreateCppType;
  typedef unsigned char GetCppType;

  static unsigned char
  cast_for_create (unsigned char c)
  {
    return c;
  }

  static unsigned char
  get (::GVariant *v)
  {
    return g_variant_get_byte (v);
  }

  static ::GVariant *
  create (unsigned char c)
  {
    return g_variant_new_byte (c);
  }
};

template<>
struct Variant::Traits<double>
{
  // "d"
  typedef double CreateCppType;
  typedef double GetCppType;

  static gdouble
  cast_for_create (double d)
  {
    return d;
  }

  static double
  get (::GVariant *v)
  {
    return g_variant_get_double (v);
  }

  static ::GVariant *
  create (double d)
  {
    return g_variant_new_double (d);
  }
};

template<>
struct Variant::Traits<int16_t>
{
  // "n"
  typedef int16_t CreateCppType;
  typedef int16_t GetCppType;

  static gint16
  cast_for_create (int16_t i)
  {
    return i;
  }

  static int16_t
  get (::GVariant *v)
  {
    return g_variant_get_int16 (v);
  }

  static ::GVariant *
  create (int16_t i)
  {
    return g_variant_new_int16 (i);
  }
};

template<>
struct Variant::Traits<uint16_t>
{
  // "q"
  typedef uint16_t CreateCppType;
  typedef uint16_t GetCppType;

  static guint16
  cast_for_create (uint16_t i)
  {
    return i;
  }

  static uint16_t
  get (::GVariant *v)
  {
    return g_variant_get_uint16 (v);
  }

  static ::GVariant *
  create (uint16_t i)
  {
    return g_variant_new_uint16 (i);
  }
};

template<>
struct Variant::Traits<int32_t>
{
  // "i"
  typedef int32_t CreateCppType;
  typedef int32_t GetCppType;

  static gint32
  cast_for_create (int32_t i)
  {
    return i;
  }

  static int32_t
  get (::GVariant *v)
  {
    return g_variant_get_int32 (v);
  }

  static ::GVariant *
  create (int32_t i)
  {
    return g_variant_new_int32 (i);
  }
};

template<>
struct Variant::Traits<uint32_t>
{
  // "u"
  typedef uint32_t CreateCppType;
  typedef uint32_t GetCppType;

  static guint32
  cast_for_create (uint32_t i)
  {
    return i;
  }

  static uint32_t
  get (::GVariant *v)
  {
    return g_variant_get_uint32 (v);
  }

  static ::GVariant *
  create (uint32_t i)
  {
    return g_variant_new_uint32 (i);
  }
};

template<>
struct Variant::Traits<int64_t>
{
  // "x"
  typedef int64_t CreateCppType;
  typedef int64_t GetCppType;

  static gint64
  cast_for_create (int64_t i)
  {
    return i;
  }

  static int64_t
  get (::GVariant *v)
  {
    return g_variant_get_int64 (v);
  }

  static ::GVariant *
  create (int64_t i)
  {
    return g_variant_new_int64 (i);
  }
};

template<>
struct Variant::Traits<uint64_t>
{
  // "t"
  typedef uint64_t CreateCppType;
  typedef uint64_t GetCppType;

  static guint64
  cast_for_create (uint64_t i)
  {
    return i;
  }

  static uint64_t
  get (::GVariant *v)
  {
    return g_variant_get_uint64 (v);
  }

  static ::GVariant *
  create (uint64_t i)
  {
    return g_variant_new_uint64 (i);
  }
};

template<>
struct Variant::Traits<const char *>
{
  // "s"
  typedef const char *CreateCppType;
  typedef const char *GetCppType;

  static const char *
  cast_for_create (const char *s)
  {
    return s;
  }

  static const char *
  get (::GVariant *v)
  {
    return g_variant_get_string (v, nullptr);
  }

  peel_nonnull_args (1)
  static ::GVariant *
  create (const char *s)
  {
    return g_variant_new_string (s);
  }
};

template<>
struct Variant::Traits<Variant>
{
  // "v"
  typedef peel::FloatPtr<Variant> CreateCppType;
  typedef peel::RefPtr<Variant> GetCppType;

  static ::GVariant *
  cast_for_create (peel::FloatPtr<Variant> v)
  {
    return reinterpret_cast<::GVariant *> (std::move (v).release_floating_ptr ());
  }

  static peel::RefPtr<Variant>
  get (::GVariant *v)
  {
    ::GVariant *child = g_variant_get_variant (v);
    return peel::RefPtr<Variant>::adopt_ref (reinterpret_cast<Variant *> (child));
  }

  static ::GVariant *
  create (peel::FloatPtr<Variant> child)
  {
    ::GVariant *_peel_child = reinterpret_cast<::GVariant *> (std::move (child).release_floating_ptr ());
    return g_variant_new_variant (_peel_child);
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
