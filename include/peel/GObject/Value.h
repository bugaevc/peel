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

  constexpr Value ()
    : value (G_VALUE_INIT)
  { }

  peel_nothrow
  Value (Type type)
    : value (G_VALUE_INIT)
  {
    g_value_init (&value, static_cast<::GType> (type));
  }

  peel_nothrow
  Value (const Value &other)
    : value (G_VALUE_INIT)
  {
    g_value_copy (&other.value, &value);
  }

  Value (Value &&other)
  {
    memcpy (&value, &other.value, sizeof (value));
    memset (&other.value, 0, sizeof (value));
  }

  peel_nothrow
  ~Value ()
  {
    g_value_unset (&value);
  }

  peel_nothrow
  Value &
  operator = (const Value &other)
  {
    g_value_copy (&other.value, &value);
    return *this;
  }

  peel_nothrow
  Value &
  operator = (Value &&other)
  {
    g_value_unset (&value);
    memcpy (&value, &other.value, sizeof (value));
    memset (&other.value, 0, sizeof (value));
    return *this;
  }

  peel_nothrow
  void
  reset ()
  {
    g_value_reset (&value);
  }

  peel_nothrow G_GNUC_CONST
  Type
  get_type () const
  {
    return G_VALUE_TYPE (&value);
  }

  peel_nothrow G_GNUC_CONST
  bool
  holds (Type type) const
  {
    return !!G_VALUE_HOLDS (&value, static_cast<::GType> (type));
  }

  template<typename T>
  bool
  holds () const
  {
    return holds (Type::of<T> ());
  }

  peel_nothrow G_GNUC_CONST
  bool
  is_interned_string () const
  {
    return !!G_VALUE_IS_INTERNED_STRING (&value);
  }

  template<typename T>
  peel_nothrow G_GNUC_CONST
  typename Traits<T>::UnownedType
  get () const
  {
    return Traits<T>::get (&this->value);
  }

  template<typename T>
  peel_nothrow
  void
  set (typename Traits<T>::UnownedType value)
  {
    Traits<T>::set (&this->value, value);
  }

  template<typename T>
  peel_nothrow
  void
  set (typename Traits<T>::OwnedType &&value)
  {
    Traits<T>::take (&this->value, std::move (value));
  }

  template<typename T, typename U>
  peel_nothrow
  peel::void_t<decltype (&Traits<T>::set_sink)>
  set (FloatPtr<U> &&value)
  {
    Traits<T>::set_sink (&this->value, std::move (value));
  }

  template<typename T>
  peel_nothrow
  void
  take (typename Traits<T>::OwnedType &&value)
  {
    Traits<T>::take (&this->value, std::move (value));
  }

  template<typename T>
  peel_nothrow
  static Value
  make (typename Traits<T>::UnownedType value)
  {
    Value v { Type::of<T> () };
    Traits<T>::set (&v.value, value);
    return v;
  }

  template<typename T>
  peel_nothrow
  static Value
  make (typename Traits<T>::OwnedType &&value)
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

  peel_nothrow G_GNUC_CONST
  static signed char
  get (const ::GValue *value)
  {
    return g_value_get_schar (value);
  }

  peel_nothrow
  static void
  set (::GValue *value, signed char ch)
  {
    g_value_set_schar (value, ch);
  }
};

template<>
struct Value::Traits<unsigned char>
{
  typedef unsigned char UnownedType;

  peel_nothrow G_GNUC_CONST
  static unsigned char
  get (const ::GValue *value)
  {
    return g_value_get_uchar (value);
  }

  peel_nothrow
  static void
  set (::GValue *value, unsigned char ch)
  {
    g_value_set_uchar (value, ch);
  }
};

template<>
struct Value::Traits<bool>
{
  typedef bool UnownedType;

  peel_nothrow G_GNUC_CONST
  static bool
  get (const ::GValue *value)
  {
    return !!g_value_get_boolean (value);
  }

  peel_nothrow
  static void
  set (::GValue *value, bool b)
  {
    g_value_set_boolean (value, b);
  }
};

template<>
struct Value::Traits<int>
{
  typedef int UnownedType;

  peel_nothrow G_GNUC_CONST
  static int
  get (const ::GValue *value)
  {
    return g_value_get_int (value);
  }

  peel_nothrow
  static void
  set (::GValue *value, int i)
  {
    g_value_set_int (value, i);
  }
};

template<>
struct Value::Traits<unsigned int>
{
  typedef unsigned int UnownedType;

  peel_nothrow G_GNUC_CONST
  static unsigned int
  get (const ::GValue *value)
  {
    return g_value_get_uint (value);
  }

  peel_nothrow
  static void
  set (::GValue *value, unsigned int i)
  {
    g_value_set_uint (value, i);
  }
};

template<>
struct Value::Traits<float>
{
  typedef float UnownedType;

  peel_nothrow G_GNUC_CONST
  static float
  get (const ::GValue *value)
  {
    return g_value_get_float (value);
  }

  peel_nothrow
  static void
  set (::GValue *value, float f)
  {
    g_value_set_float (value ,f);
  }
};

template<>
struct Value::Traits<double>
{
  typedef double UnownedType;

  peel_nothrow G_GNUC_CONST
  static double
  get (const ::GValue *value)
  {
    return g_value_get_double (value);
  }

  peel_nothrow
  static void
  set (::GValue *value, double d)
  {
    g_value_set_double (value, d);
  }
};

// long, unsigned long...

template<>
struct Value::Traits<const char *>
{
  typedef const char *UnownedType;
  typedef char *OwnedType;

  peel_nothrow G_GNUC_CONST
  static const char *
  get (const ::GValue *value)
  {
    return g_value_get_string (value);
  }

  peel_nothrow
  static void
  set (::GValue *value, const char *str)
  {
#ifdef __GNUC__
    if (__builtin_constant_p (__builtin_strlen (str)))
      g_value_set_static_string (value, str);
    else
#endif
      g_value_set_string (value, str);
  }

  peel_nothrow
  static void
  take (::GValue *value, char *str)
  {
    g_value_take_string (value, str);
  }
};

template<>
struct Value::Traits<Type>
{
  typedef Type UnownedType;

  peel_nothrow G_GNUC_CONST
  static Type
  get (const ::GValue *value)
  {
    return g_value_get_gtype (value);
  }

  peel_nothrow
  static void
  set (::GValue *value, Type type)
  {
    g_value_set_gtype (value, type);
  }
};

template<>
struct Value::Traits<void *>
{
  typedef void *UnownedType;

  peel_nothrow G_GNUC_CONST
  static void *
  get (const ::GValue *value)
  {
    return g_value_get_pointer (value);
  }

  peel_nothrow
  static void
  set (::GValue *value, void *p)
  {
    g_value_set_pointer (value, p);
  }
};

template<>
struct Value::Traits<Value>
{
  typedef Value *UnownedType;

  peel_nothrow G_GNUC_CONST
  static Value *
  get (const ::GValue *value)
  {
    return reinterpret_cast<Value *> (g_value_get_boxed (value));
  }

  peel_nothrow
  static void
  set (::GValue *value, Value *v)
  {
    g_value_set_boxed (value, reinterpret_cast<const void *> (v));
  }
};

} /* namespace GObject */

using GObject::Value;

} /* namespace peel */
