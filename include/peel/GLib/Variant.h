#pragma once

#include <peel/lang.h>
#include <peel/String.h>
#include <peel/GObject/Type.h>
#include <peel/GObject/Value.h>
#include <peel/GLib/Error.h>
#include <glib.h>

peel_begin_header

namespace peel
{
namespace GLib
{
class /* record */ Bytes;

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
  struct Iter;
  enum class Class : std::underlying_type<::GVariantClass>::type;

  peel_returns_nonnull G_GNUC_PURE
  const Variant::Type *
  get_type () noexcept
  {
    ::GVariant *v = reinterpret_cast<::GVariant *> (this);
    const ::GVariantType *tp = g_variant_get_type (v);
    peel_assume (tp);
    return reinterpret_cast<const Type *> (tp);
  }

  peel_returns_nonnull G_GNUC_PURE
  const char *
  get_type_string () noexcept
  {
    ::GVariant *v = reinterpret_cast<::GVariant *> (this);
    return g_variant_get_type_string (v);
  }

  peel_nonnull_args (2) G_GNUC_PURE
  bool
  is_of_type (const Variant::Type *type) noexcept
  {
    ::GVariant *v = reinterpret_cast<::GVariant *> (this);
    const ::GVariantType *tp = reinterpret_cast<const ::GVariantType *> (type);
    gboolean r = g_variant_is_of_type (v, tp);
    return !!r;
  }

  G_GNUC_PURE
  bool
  is_container () noexcept
  {
    ::GVariant *v = reinterpret_cast<::GVariant *> (this);
    gboolean r = g_variant_is_container (v);
    return !!r;
  }

  // ...

  RefPtr<Variant>
  byteswap () noexcept
  {
    ::GVariant *v = reinterpret_cast<::GVariant *> (this);
    ::GVariant *r = g_variant_byteswap (v);
    peel_assume (r);
    return RefPtr<Variant>::adopt_ref (reinterpret_cast<Variant *> (r));
  }

  RefPtr<Variant>
  get_normal_form () noexcept
  {
    ::GVariant *v = reinterpret_cast<::GVariant *> (this);
    ::GVariant *r = g_variant_get_normal_form (v);
    peel_assume (r);
    return RefPtr<Variant>::adopt_ref (reinterpret_cast<Variant *> (r));
  }

  peel_nonnull_args (1, 2)
  static FloatPtr<Variant>
  create_from_bytes (const Variant::Type *type, GLib::Bytes *bytes, bool trusted) noexcept
  {
    const ::GVariantType *tp = reinterpret_cast<const ::GVariantType *> (type);
    ::GBytes *b = reinterpret_cast<::GBytes *> (bytes);
    ::GVariant *v = g_variant_new_from_bytes (tp, b, trusted);
    peel_assume (v);
    return FloatPtr<Variant> (reinterpret_cast<Variant *> (v));
  }

  template<typename ...Args>
  peel_nonnull_args (1)
  static FloatPtr<Variant>
  create (const char *format_string, Args &&...args) noexcept
  {
    ::GVariant *v = g_variant_new (format_string, Traits<typename std::decay<Args>::type>::cast_for_create (std::forward<Args> (args))...);
    peel_assume (v);
    return FloatPtr<Variant> (reinterpret_cast<Variant *> (v));
  }

  template<typename T>
  static FloatPtr<Variant>
  create (typename Traits<T>::CreateCppType value) noexcept
  {
    ::GVariant *v = Traits<T>::create (std::move (value));
    peel_assume (v);
    return FloatPtr<Variant> (reinterpret_cast<Variant *> (v));
  }

  template<typename ...Args>
  peel_nonnull_args (1)
  static FloatPtr<Variant>
  create_parsed (const char *format_string, Args &&...args) noexcept
  {
    ::GVariant *v = g_variant_new_parsed (format_string, Traits<typename std::decay<Args>::type>::cast_for_create (std::forward<Args> (args))...);
    peel_assume (v);
    return FloatPtr<Variant> (reinterpret_cast<Variant *> (v));
  }

  static FloatPtr<Variant>
  create_maybe (const Variant::Type *child_type, FloatPtr<Variant> child) noexcept
  {
    const ::GVariantType *tp = reinterpret_cast<const ::GVariantType *> (child_type);
    ::GVariant *c = reinterpret_cast<::GVariant *> (std::move (child).release_floating_ptr ());
    ::GVariant *v = g_variant_new_maybe (tp, c);
    peel_assume (v);
    return FloatPtr<Variant> (reinterpret_cast<Variant *> (v));
  }

  peel_nonnull_args (1)
  static FloatPtr<Variant>
  create_object_path (const char *object_path) noexcept
  {
    ::GVariant *v = g_variant_new_object_path (object_path);
    peel_assume (v);
    return FloatPtr<Variant> (reinterpret_cast<Variant *> (v));
  }

  static FloatPtr<Variant>
  create_dict_entry (FloatPtr<Variant> key, FloatPtr<Variant> value) noexcept
  {
    ::GVariant *k = reinterpret_cast<::GVariant *> (std::move (key).release_floating_ptr ());
    ::GVariant *v = reinterpret_cast<::GVariant *> (std::move (value).release_floating_ptr ());
    ::GVariant *r = g_variant_new_dict_entry (k, v);
    peel_assume (r);
    return FloatPtr<Variant> (reinterpret_cast<Variant *> (r));
  }

  static FloatPtr<Variant>
  create_array (const Variant::Type *child_type, ArrayRef<Variant * const> children) noexcept
  {
    const ::GVariantType *tp = reinterpret_cast<const ::GVariantType *> (child_type);
    ::GVariant * const *ch = reinterpret_cast<::GVariant * const *> (children.data ());
    ::GVariant *v = g_variant_new_array (tp, ch, children.size ());
    peel_assume (v);
    return FloatPtr<Variant> (reinterpret_cast<Variant *> (v));
  }

  static FloatPtr<Variant>
  create_tuple (ArrayRef<Variant * const> children) noexcept
  {
    ::GVariant * const *ch = reinterpret_cast<::GVariant * const *> (children.data ());
    ::GVariant *v = g_variant_new_tuple (ch, children.size ());
    peel_assume (v);
    return FloatPtr<Variant> (reinterpret_cast<Variant *> (v));
  }

  // TODO: FloatPtr-enabled overloads of create_tuple, create_array

  template<typename T>
  typename Traits<T>::GetCppType
  get () noexcept
  {
    ::GVariant *v = reinterpret_cast<::GVariant *> (this);
    return Traits<T>::get (v);
  }

  RefPtr<Variant>
  get_child_value (size_t index) noexcept
  {
    ::GVariant *v = reinterpret_cast<::GVariant *> (this);
    ::GVariant *r = g_variant_get_child_value (v, index);
    peel_assume (r);
    return RefPtr<Variant>::adopt_ref (reinterpret_cast<Variant *> (r));
  }

  RefPtr<Variant>
  get_maybe () noexcept
  {
    ::GVariant *v = reinterpret_cast<::GVariant *> (this);
    ::GVariant *r = g_variant_get_maybe (v);
    return RefPtr<Variant>::adopt_ref (reinterpret_cast<Variant *> (r));
  }

  peel_nonnull_args (1) G_GNUC_PURE
  static bool
  is_object_path (const char *string) noexcept
  {
    gboolean r = g_variant_is_object_path (string);
    return !!r;
  }

  peel_nonnull_args (1) G_GNUC_PURE
  static bool
  is_signature (const char *string) noexcept
  {
    gboolean r = g_variant_is_signature (string);
    return !!r;
  }

  peel_nonnull_args (2)
  static RefPtr<Variant>
  parse (const Variant::Type *type, const char *text, const char *limit, const char **endptr, UniquePtr<GLib::Error> *error) noexcept
  {
    ::GError *_peel_error = nullptr;
    const ::GVariantType *tp = reinterpret_cast<const ::GVariantType *> (type);
    ::GVariant *v = g_variant_parse (tp, text, limit, endptr, error ? &_peel_error : nullptr);
    if (error)
      *error = UniquePtr<GLib::Error>::adopt_ref (reinterpret_cast<GLib::Error *> (_peel_error));
    return RefPtr<Variant>::adopt_ref (reinterpret_cast<Variant *> (v));
  }

  peel::String
  print (bool type_annotate) noexcept
  {
    ::GVariant *v = reinterpret_cast<::GVariant *> (this);
    return peel::String::adopt_string (g_variant_print (v, type_annotate));
  }

  RefPtr<Bytes>
  get_data_as_bytes () noexcept
  {
    ::GVariant *v = reinterpret_cast<::GVariant *> (this);
    ::GBytes *r = g_variant_get_data_as_bytes (v);
    return RefPtr<Bytes>::adopt_ref (reinterpret_cast<Bytes *> (r));
  }

  G_GNUC_PURE
  ArrayRef<const uint8_t>
  get_data () noexcept
  {
    ::GVariant *v = reinterpret_cast<::GVariant *> (this);
    gconstpointer data = g_variant_get_data (v);
    gsize size = g_variant_get_size (v);
    return ArrayRef<const uint8_t> (reinterpret_cast<const uint8_t *> (data), size);
  }

  G_GNUC_PURE
  size_t
  get_size () noexcept
  {
    ::GVariant *v = reinterpret_cast<::GVariant *> (this);
    return g_variant_get_size (v);
  }

  G_GNUC_PURE
  size_t
  n_children () noexcept
  {
    ::GVariant *v = reinterpret_cast<::GVariant *> (this);
    return g_variant_n_children (v);
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

  G_GNUC_PURE
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

  G_GNUC_PURE
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

  G_GNUC_PURE
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

  G_GNUC_PURE
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

  G_GNUC_PURE
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

  G_GNUC_PURE
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

  G_GNUC_PURE
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

  G_GNUC_PURE
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

  G_GNUC_PURE
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

  G_GNUC_PURE
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

/* Allow both String and const char * as types */
template<>
struct Variant::Traits<peel::String> : Variant::Traits<const char *>
{ };

template<>
struct Variant::Traits<Variant>
{
  // "v"
  typedef FloatPtr<Variant> CreateCppType;
  typedef RefPtr<Variant> GetCppType;

  static ::GVariant *
  cast_for_create (FloatPtr<Variant> v)
  {
    return reinterpret_cast<::GVariant *> (std::move (v).release_floating_ptr ());
  }

  /* not marked as pure */
  static RefPtr<Variant>
  get (::GVariant *v)
  {
    ::GVariant *child = g_variant_get_variant (v);
    return RefPtr<Variant>::adopt_ref (reinterpret_cast<Variant *> (child));
  }

  static ::GVariant *
  create (FloatPtr<Variant> &&child)
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

  G_GNUC_PURE
  static GLib::Variant *
  get (const ::GValue *value)
  {
    return reinterpret_cast<GLib::Variant *> (g_value_get_variant (value));
  }

  static RefPtr<GLib::Variant>
  dup (const ::GValue *value) noexcept
  {
    ::GVariant *vv = g_value_dup_variant (value);
    return RefPtr<GLib::Variant>::adopt_ref (reinterpret_cast<GLib::Variant *> (vv));
  }

  static void
  set (::GValue *value, GLib::Variant *v)
  {
    g_value_set_variant (value, reinterpret_cast<::GVariant *> (v));
  }

  static void
  set_sink (::GValue *value, FloatPtr<GLib::Variant> &&v) noexcept
  {
    GLib::Variant *vv = std::move (v).release_floating_ptr ();
    g_value_set_variant (value, reinterpret_cast<::GVariant *> (vv));
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
    /* Pretend to have a reference */
    g_value_take_variant (value, reinterpret_cast<::GVariant *> (v));
  }

  static void
  set_marshal_return (::GValue *value, RefPtr<GLib::Variant> &&v) noexcept
  {
    take (value, std::move (v));
  }

  static ::GVariant *
  cast_for_create (FloatPtr<GLib::Variant> v) noexcept
  {
    GLib::Variant *vv = std::move (v).release_floating_ptr ();
    return reinterpret_cast<::GVariant *> (vv);
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

  constexpr static
  bool can_ref_null = false;

  static void
  unref (GLib::Variant *v)
  {
    g_variant_unref (reinterpret_cast<::GVariant *> (v));
  }

  constexpr static
  bool can_unref_null = false;

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

peel_end_header

#include <peel/GLib/Bytes.h>
