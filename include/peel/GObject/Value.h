#pragma once

#include <peel/GObject/Type.h>
#include <peel/GObject/EnumClass.h>
#include <peel/GObject/FlagsClass.h>
#include <peel/String.h>
#include <peel/Strv.h>
#include <peel/lang.h>
#include <glib-object.h>
#include <utility>

peel_begin_header

namespace peel
{

template<typename T>
class FloatPtr;

namespace GObject
{

struct Value final
{
private:
  ::GValue value;

public:
  template<typename T, typename = void>
  struct Traits;

  constexpr Value () noexcept
    : value (G_VALUE_INIT)
  { }

  Value (Type type) noexcept
    : value (G_VALUE_INIT)
  {
    g_value_init (&value, static_cast<::GType> (type));
  }

  Value (const Value &other) noexcept
    : value (G_VALUE_INIT)
  {
    g_value_copy (&other.value, &value);
  }

  Value (Value &&other) noexcept
  {
    memcpy (&value, &other.value, sizeof (value));
    memset (&other.value, 0, sizeof (value));
  }

  ~Value () noexcept
  {
    g_value_unset (&value);
  }

  Value &
  operator = (const Value &other) noexcept
  {
    g_value_copy (&other.value, &value);
    return *this;
  }

  Value &
  operator = (Value &&other) noexcept
  {
    g_value_unset (&value);
    memcpy (&value, &other.value, sizeof (value));
    memset (&other.value, 0, sizeof (value));
    return *this;
  }

  void
  reset () noexcept
  {
    g_value_reset (&value);
  }

  G_GNUC_CONST
  Type
  get_type () const noexcept
  {
    return G_VALUE_TYPE (&value);
  }

  G_GNUC_CONST
  bool
  holds (Type type) const noexcept
  {
    return !!G_VALUE_HOLDS (&value, static_cast<::GType> (type));
  }

  template<typename T>
  bool
  holds () const
  {
    return holds (Type::of<T> ());
  }

  G_GNUC_CONST
  bool
  is_interned_string () const noexcept
  {
    return !!G_VALUE_IS_INTERNED_STRING (&value);
  }

  template<typename T>
  G_GNUC_CONST
  typename Traits<T>::UnownedType
  get () const noexcept
  {
    return Traits<T>::get (&this->value);
  }

  template<typename T>
  typename Traits<T>::OwnedType
  dup () const noexcept
  {
    return Traits<T>::dup (&this->value);
  }

  template<typename T>
  void
  set (typename Traits<T>::UnownedType value) noexcept
  {
    Traits<T>::set (&this->value, value);
  }

  template<typename T>
  void
  set (typename Traits<T>::OwnedType &&value) noexcept
  {
    Traits<T>::take (&this->value, std::move (value));
  }

  template<typename T, typename U>
  peel::void_t<decltype (&Traits<T>::set_sink)>
  set (FloatPtr<U> &&value) noexcept
  {
    Traits<T>::set_sink (&this->value, std::move (value));
  }

  template<typename T>
  void
  take (typename Traits<T>::OwnedType &&value) noexcept
  {
    Traits<T>::take (&this->value, std::move (value));
  }

  template<typename T>
  static Value
  make (typename Traits<T>::UnownedType value) noexcept
  {
    Value v { Type::of<T> () };
    Traits<T>::set (&v.value, value);
    return v;
  }

  template<typename T>
  static Value
  make (typename Traits<T>::OwnedType &&value) noexcept
  {
    Value v { Type::of<T> () };
    Traits<T>::take (&v.value, std::move (value));
    return v;
  }
};

static_assert (sizeof (Value) == sizeof (::GValue),
               "GObject::Value size mismatch");
static_assert (alignof (Value) == alignof (::GValue),
               "GObject::Value align mismatch");

template<>
inline Type
Type::of<Value> ()
{
  return G_TYPE_VALUE;
}

template<>
struct Value::Traits<signed char>
{
  typedef signed char UnownedType;

  G_GNUC_CONST
  static signed char
  get (const ::GValue *value) noexcept
  {
    return g_value_get_schar (value);
  }

  static void
  set (::GValue *value, signed char ch) noexcept
  {
    g_value_set_schar (value, ch);
  }

  static void
  set_marshal_return (::GValue *value, signed char ch) noexcept
  {
    return set (value, ch);
  }

  static signed char
  cast_for_create (signed char ch) noexcept
  {
     return ch;
  }
};

template<>
struct Value::Traits<unsigned char>
{
  typedef unsigned char UnownedType;

  G_GNUC_CONST
  static unsigned char
  get (const ::GValue *value) noexcept
  {
    return g_value_get_uchar (value);
  }

  static void
  set (::GValue *value, unsigned char ch) noexcept
  {
    g_value_set_uchar (value, ch);
  }

  static void
  set_marshal_return (::GValue *value, unsigned char ch) noexcept
  {
    return set (value, ch);
  }

  static unsigned char
  cast_for_create (unsigned char ch) noexcept
  {
    return ch;
  }
};

template<>
struct Value::Traits<bool>
{
  typedef bool UnownedType;

  G_GNUC_CONST
  static bool
  get (const ::GValue *value) noexcept
  {
    return !!g_value_get_boolean (value);
  }

  static void
  set (::GValue *value, bool b) noexcept
  {
    g_value_set_boolean (value, b);
  }

  static void
  set_marshal_return (::GValue *value, bool b) noexcept
  {
    return set (value, b);
  }

  static gboolean
  cast_for_create (bool b) noexcept
  {
     return b;
  }
};

template<>
struct Value::Traits<int>
{
  typedef int UnownedType;

  G_GNUC_CONST
  static int
  get (const ::GValue *value) noexcept
  {
    return g_value_get_int (value);
  }

  static void
  set (::GValue *value, int i) noexcept
  {
    g_value_set_int (value, i);
  }

  static void
  set_marshal_return (::GValue *value, int i) noexcept
  {
    return set (value, i);
  }

  static gint
  cast_for_create (int i) noexcept
  {
    return i;
  }
};

template<>
struct Value::Traits<unsigned int>
{
  typedef unsigned int UnownedType;

  G_GNUC_CONST
  static unsigned int
  get (const ::GValue *value) noexcept
  {
    return g_value_get_uint (value);
  }

  static void
  set (::GValue *value, unsigned int i) noexcept
  {
    g_value_set_uint (value, i);
  }

  static void
  set_marshal_return (::GValue *value, unsigned int i) noexcept
  {
    return set (value, i);
  }

  static guint
  cast_for_create (unsigned int i) noexcept
  {
    return i;
  }
};

template<>
struct Value::Traits<float>
{
  typedef float UnownedType;

  G_GNUC_CONST
  static float
  get (const ::GValue *value) noexcept
  {
    return g_value_get_float (value);
  }

  static void
  set (::GValue *value, float f) noexcept
  {
    g_value_set_float (value ,f);
  }

  static void
  set_marshal_return (::GValue *value, float f) noexcept
  {
    set (value, f);
  }

  static gfloat
  cast_for_create (float f) noexcept
  {
    return f;
  }
};

template<>
struct Value::Traits<double>
{
  typedef double UnownedType;

  G_GNUC_CONST
  static double
  get (const ::GValue *value) noexcept
  {
    return g_value_get_double (value);
  }

  static void
  set (::GValue *value, double d) noexcept
  {
    g_value_set_double (value, d);
  }

  static void
  set_marshal_return (::GValue *value, double d) noexcept
  {
    set (value, d);
  }

  static gdouble
  cast_for_create (double d) noexcept
  {
    return d;
  }
};

template<>
struct Value::Traits<Long>
{
  typedef Long UnownedType;

  G_GNUC_CONST
  static Long
  get (const ::GValue *value) noexcept
  {
    return Long (g_value_get_long (value));
  }

  static void
  set (::GValue *value, Long l) noexcept
  {
    g_value_set_long (value, l.value);
  }

  static void
  set_marshal_return (::GValue *value, Long l) noexcept
  {
    return set (value, l);
  }

  static gulong
  cast_for_create (Long l) noexcept
  {
    return l.value;
  }
};

template<>
struct Value::Traits<ULong>
{
  typedef ULong UnownedType;

  G_GNUC_CONST
  static ULong
  get (const ::GValue *value) noexcept
  {
    return ULong (g_value_get_ulong (value));
  }

  static void
  set (::GValue *value, ULong l) noexcept
  {
    g_value_set_ulong (value, l.value);
  }

  static void
  set_marshal_return (::GValue *value, ULong l) noexcept
  {
    return set (value, l);
  }

  static gulong
  cast_for_create (ULong l) noexcept
  {
    return l.value;
  }
};
template<>
struct Value::Traits<int64_t>
{
  typedef int64_t UnownedType;

  G_GNUC_CONST
  static int64_t
  get (const ::GValue *value) noexcept
  {
    return g_value_get_int64 (value);
  }

  static void
  set (::GValue *value, int64_t i) noexcept
  {
    g_value_set_int64 (value, i);
  }

  static void
  set_marshal_return (::GValue *value, int64_t i) noexcept
  {
    return set (value, i);
  }

  static gint64
  cast_for_create (int64_t i) noexcept
  {
    return i;
  }
};

template<>
struct Value::Traits<uint64_t>
{
  typedef uint64_t UnownedType;

  G_GNUC_CONST
  static uint64_t
  get (const ::GValue *value) noexcept
  {
    return g_value_get_uint64 (value);
  }

  static void
  set (::GValue *value, uint64_t i) noexcept
  {
    g_value_set_uint64 (value, i);
  }

  static void
  set_marshal_return (::GValue *value, uint64_t i) noexcept
  {
    return set (value, i);
  }

  static guint64
  cast_for_create (uint64_t i) noexcept
  {
    return i;
  }
};

template<>
struct Value::Traits<const char *>
{
  typedef const char *UnownedType;
  typedef String OwnedType;

  G_GNUC_CONST
  static const char *
  get (const ::GValue *value) noexcept
  {
    return g_value_get_string (value);
  }

  static String
  dup (const ::GValue *value) noexcept
  {
    return String::adopt_string (g_value_dup_string (value));
  }

  static void
  set (::GValue *value, const char *str) noexcept
  {
#ifdef __GNUC__
    if ((__builtin_constant_p (!str) && !str) || __builtin_constant_p (__builtin_strlen (str)))
      g_value_set_static_string (value, str);
    else
#endif
      g_value_set_string (value, str);
  }

  static void
  take (::GValue *value, String &&str) noexcept
  {
    g_value_take_string (value, std::move (str).release_string ());
  }

  static void
  set_marshal_return (::GValue *value, String &&str) noexcept
  {
    take (value, std::move (str));
  }

  static void
  set_marshal_return (::GValue *value, const char *str) noexcept
  {
    /* whee! */
    g_value_set_static_string (value, str);
  }

  static const char *
  cast_for_create (const char *c) noexcept
  {
    return c;
  }
};

/* Allow both String and const char * as logical value types */
template<>
struct Value::Traits<String> : Value::Traits<const char *>
{ };

template<>
struct Value::Traits<Strv>
{
  typedef StrvRef UnownedType;
  typedef Strv OwnedType;

  G_GNUC_CONST
  static StrvRef
  get (const ::GValue *value) noexcept
  {
    void *p = g_value_get_boxed (value);
    return StrvRef::adopt (reinterpret_cast<const char * const *> (p));
  }

  static Strv
  dup (const ::GValue *value) noexcept
  {
    return Strv::adopt_ref (reinterpret_cast<String *> (g_value_dup_boxed (value)));
  }

  static void
  set (::GValue *value, StrvRef strv) noexcept
  {
    g_value_set_boxed (value, strv.data ());
  }

  static void
  take (::GValue *value, Strv &&strv) noexcept
  {
    g_value_take_boxed (value, std::move (strv).release_ref ());
  }

  static const char * const *
  cast_for_create (StrvRef strv) noexcept
  {
    return strv.data ();
  }
};

template<>
struct Value::Traits<Type>
{
  typedef Type UnownedType;

  G_GNUC_CONST
  static Type
  get (const ::GValue *value) noexcept
  {
    return g_value_get_gtype (value);
  }

  static void
  set (::GValue *value, Type type) noexcept
  {
    g_value_set_gtype (value, type);
  }

  static void
  set_marshal_return (::GValue *value, Type type) noexcept
  {
    set (value, type);
  }

  static ::GType
  cast_for_create (Type type) noexcept
  {
    return type;
  }
};

template<>
struct Value::Traits<void *>
{
  typedef void *UnownedType;

  G_GNUC_CONST
  static void *
  get (const ::GValue *value) noexcept
  {
    return g_value_get_pointer (value);
  }

  static void
  set (::GValue *value, void *p) noexcept
  {
    g_value_set_pointer (value, p);
  }

  static void
  set_marshal_return (::GValue *value, void *p) noexcept
  {
    set (value, p);
  }

  static gpointer
  cast_for_create (void *p) noexcept
  {
    return p;
  }
};

template<>
struct Value::Traits<Value>
{
  typedef Value *UnownedType;

  G_GNUC_CONST
  static Value *
  get (const ::GValue *value) noexcept
  {
    return reinterpret_cast<Value *> (g_value_get_boxed (value));
  }

  static void
  set (::GValue *value, const Value *v) noexcept
  {
    g_value_set_boxed (value, reinterpret_cast<const void *> (v));
  }

  static const ::GValue *
  cast_for_create (const Value *v) noexcept
  {
    return reinterpret_cast<const ::GValue *> (v);
  }
};

template<>
struct GObject::Value::Traits<Enum>
{
  typedef Enum UnownedType;

  static Enum
  get (const ::GValue *value)
  {
    return Enum (g_value_get_enum (value));
  }

  static void
  set (::GValue *value, Enum m)
  {
    g_value_set_enum (value, m.value);
  }

  static void
  set_marshal_return (::GValue *value, Enum m)
  {
    set (value, m);
  }

  static Enum
  cast_for_create (Enum m) noexcept
  {
    return m;
  }
};

template<>
struct GObject::Value::Traits<Flags>
{
  typedef Flags UnownedType;

  static Flags
  get (const ::GValue *value)
  {
    return Flags (g_value_get_flags (value));
  }

  static void
  set (::GValue *value, Flags m)
  {
    g_value_set_flags (value, m.value);
  }

  static void
  set_marshal_return (::GValue *value, Flags m)
  {
    set (value, m);
  }

  static Flags
  cast_for_create (Flags m) noexcept
  {
    return m;
  }
};

} /* namespace GObject */

using GObject::Value;

} /* namespace peel */

peel_end_header
