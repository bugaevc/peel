#pragma once

#include <peel/GObject/Type.h>
#include <peel/lang.h>
#include <glib-object.h>
#include <utility>

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
    Traits<T>::take (&v.value, std::move(value));
    return v;
  }
};

static_assert (sizeof (Value) == sizeof (::GValue),
               "GObject::Value size mismatch");
static_assert (alignof (Value) == alignof (::GValue),
               "GObject::Value align mismatch");

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
  set_marshal_return (::GValue *value, signed char ch) noexcept
  {
    return set (value, ch);
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
};

// long, unsigned long...

template<>
struct Value::Traits<const char *>
{
  typedef const char *UnownedType;
  typedef char *OwnedType;

  G_GNUC_CONST
  static const char *
  get (const ::GValue *value) noexcept
  {
    return g_value_get_string (value);
  }

  static void
  set (::GValue *value, const char *str) noexcept
  {
#ifdef __GNUC__
    if (__builtin_constant_p (__builtin_strlen (str)))
      g_value_set_static_string (value, str);
    else
#endif
      g_value_set_string (value, str);
  }

  static void
  take (::GValue *value, char *str) noexcept
  {
    g_value_take_string (value, str);
  }

  static void
  set_marshal_return (::GValue *value, char *str) noexcept
  {
    g_value_take_string (value, str);
  }

/*
  TODO: This is bound to break. Unlike with objects, I don't think
  any signal is actually doing this?

  static void
  set_marshal_return (::GValue *value, const char *str) noexcept
  {
    g_value_take_string (value, const_cast<char *> (str));
  }
*/
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
  set (::GValue *value, Value *v) noexcept
  {
    g_value_set_boxed (value, reinterpret_cast<const void *> (v));
  }
};

} /* namespace GObject */

using GObject::Value;

} /* namespace peel */
