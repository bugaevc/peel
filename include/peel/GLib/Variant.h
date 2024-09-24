#pragma once

#include <peel/lang.h>
#include <peel/GObject/Type.h>
#include <peel/GObject/Value.h>
#include <glib.h>

namespace peel
{
namespace GLib
{

struct Error;
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

  const Variant::Type *
  get_type () noexcept
  {
    ::GVariant *_peel_this = reinterpret_cast<::GVariant *> (this);
    const ::GVariantType *_peel_return = g_variant_get_type (_peel_this);
    peel_assume (_peel_return);
    return reinterpret_cast<const Type *> (_peel_return);
  }

  const char *
  get_type_string () noexcept
  {
    ::GVariant *_peel_this = reinterpret_cast<::GVariant *> (this);
    return g_variant_get_type_string (_peel_this);
  }

  peel_nonnull_args (2)
  bool
  is_of_type (const GVariantType *type) noexcept
  {
    ::GVariant *_peel_this = reinterpret_cast<::GVariant *> (this);
    const ::GVariantType *_peel_type = reinterpret_cast<const ::GVariantType *> (type);
    gboolean _peel_return = g_variant_is_of_type (_peel_this, _peel_type);
    return !!_peel_return;
  }

  bool
  is_container () noexcept
  {
    ::GVariant *_peel_this = reinterpret_cast<::GVariant *> (this);
    gboolean _peel_return = g_variant_is_container (_peel_this);
    return !!_peel_return;
  }

  // ...

  template<typename ...Args>
  peel_nonnull_args (1)
  static peel::FloatPtr<Variant>
  create (const char *format_string, Args &&...args) noexcept
  {
    ::GVariant *_peel_return = g_variant_new (format_string, Traits<typename std::decay<Args>::type>::cast_for_create (std::forward<Args> (args))...);
    peel_assume (_peel_return);
    return peel::FloatPtr<Variant> (reinterpret_cast<Variant *> (_peel_return));
  }

  template<typename T>
  static peel::FloatPtr<Variant>
  create (typename Traits<T>::CreateCppType value) noexcept
  {
    ::GVariant *v = Traits<T>::create (std::move (value));
    peel_assume (v);
    return peel::FloatPtr<Variant> (reinterpret_cast<Variant *> (v));
  }

  template<typename ...Args>
  peel_nonnull_args (1)
  static peel::FloatPtr<Variant>
  create_parsed (const char *format_string, Args &&...args) noexcept
  {
    ::GVariant *_peel_return = g_variant_new_parsed (format_string, Traits<typename std::decay<Args>::type>::cast_for_create (std::forward<Args> (args))...);
    peel_assume (_peel_return);
    return peel::FloatPtr<Variant> (reinterpret_cast<Variant *> (_peel_return));
  }

  static peel::FloatPtr<Variant>
  create_maybe (const Variant::Type *child_type, peel::FloatPtr<Variant> child) noexcept
  {
    const ::GVariantType *_peel_child_type = reinterpret_cast<const ::GVariantType *> (child_type);
    ::GVariant *_peel_child = reinterpret_cast<::GVariant *> (std::move (child).release_floating_ptr ());
    ::GVariant *_peel_return = g_variant_new_maybe (_peel_child_type, _peel_child);
    peel_assume (_peel_return);
    return peel::FloatPtr<Variant> (reinterpret_cast<Variant *> (_peel_return));
  }

  static peel::FloatPtr<Variant>
  create_object_path (const char *object_path) noexcept
  {
    ::GVariant *_peel_return = g_variant_new_object_path (object_path);
    peel_assume (_peel_return);
    return peel::FloatPtr<Variant> (reinterpret_cast<Variant *> (_peel_return));
  }

  static peel::FloatPtr<Variant>
  create_dict_entry (peel::FloatPtr<Variant> key, peel::FloatPtr<Variant> value) noexcept
  {
    ::GVariant *_peel_key = reinterpret_cast<::GVariant *> (std::move (key).release_floating_ptr ());
    ::GVariant *_peel_value = reinterpret_cast<::GVariant *> (std::move (value).release_floating_ptr ());
    ::GVariant *_peel_return = g_variant_new_dict_entry (_peel_key, _peel_value);
    peel_assume (_peel_return);
    return peel::FloatPtr<Variant> (reinterpret_cast<Variant *> (_peel_return));
  }

  static peel::FloatPtr<Variant>
  create_tuple (peel::ArrayRef<Variant * const> children) noexcept
  {
    ::GVariant * const *_peel_children = reinterpret_cast<::GVariant * const *> (children.ptr ());
    gsize _peel_n_children = children.size ();
    ::GVariant *_peel_return = g_variant_new_tuple (_peel_children, _peel_n_children);
    peel_assume (_peel_return);
    return peel::FloatPtr<Variant> (reinterpret_cast<Variant *> (_peel_return));
  }

  // TODO: FloatPtr-enabled overload of create_tuple

  template<typename T>
  typename Traits<T>::GetCppType
  get () noexcept
  {
    ::GVariant *v = reinterpret_cast<::GVariant *> (this);
    return Traits<T>::get (v);
  }

  peel::RefPtr<Variant>
  get_child_value (size_t index) noexcept
  {
    ::GVariant *v = reinterpret_cast<::GVariant *> (this);
    ::GVariant *_peel_return = g_variant_get_child_value (v, index);
    peel_assume (_peel_return);
    return peel::RefPtr<Variant>::adopt_ref (reinterpret_cast<Variant *> (_peel_return));
  }

  peel::RefPtr<Variant>
  get_maybe () noexcept
  {
    ::GVariant *_peel_this = reinterpret_cast<::GVariant *> (this);
    ::GVariant *_peel_return = g_variant_get_maybe (_peel_this);
    return peel::RefPtr<Variant>::adopt_ref (reinterpret_cast<Variant *> (_peel_return));
  }

  peel_nonnull_args (1)
  static bool
  is_object_path (const char *string) noexcept
  {
    gboolean _peel_return = g_variant_is_object_path (string);
    return !!_peel_return;
  }

  peel_nonnull_args (1)
  static bool
  is_signature (const char *string) noexcept
  {
    gboolean _peel_return = g_variant_is_signature (string);
    return !!_peel_return;
  }

  peel_nonnull_args (2)
  static peel::RefPtr<Variant>
  parse (const Variant::Type *type, const char *text, const char *limit, const char **endptr, peel::UniquePtr<GLib::Error> *error) noexcept
  {
    ::GError *_peel_error = nullptr;
    const ::GVariantType *_peel_type = reinterpret_cast<const ::GVariantType *> (type);
    ::GVariant *_peel_return = g_variant_parse (_peel_type, text, limit, endptr, error ? &_peel_error : nullptr);
    if (error)
      *error = peel::UniquePtr<GLib::Error>::adopt_ref (reinterpret_cast<GLib::Error *> (_peel_error));
    return peel::RefPtr<Variant>::adopt_ref (reinterpret_cast<Variant *> (_peel_return));
  }

  /* owned */ char *
  print (bool type_annotate) noexcept
  {
    ::GVariant *_peel_this = reinterpret_cast<::GVariant *> (this);
    gboolean _peel_type_annotate = static_cast<gboolean> (type_annotate);
    return g_variant_print (_peel_this, _peel_type_annotate);
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
  create (peel::FloatPtr<Variant> &&child)
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

  static void
  set_marshal_return (::GValue *value, GLib::Variant *v) noexcept
  {
    /* Pretend to have a reference.  */
    g_value_take_variant (value, reinterpret_cast<::GVariant *> (v));
  }

  static void
  set_marshal_return (::GValue *value, RefPtr<GLib::Variant> &&v) noexcept
  {
    take (value, std::move (v));
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
  sink (GLib::Variant *v)
  {
    g_variant_ref_sink (reinterpret_cast<::GVariant *> (v));
    g_variant_unref (reinterpret_cast<::GVariant *> (v));
  }
};

} /* namespace peel */
