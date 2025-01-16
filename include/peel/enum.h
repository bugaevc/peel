#pragma once

#include <peel/GObject/Type.h>
#include <peel/GObject/Value.h>
#include <peel/property.h>
#include <peel/class.h>
#include <peel/lang.h>
#include <type_traits>
#include <glib-object.h>

#define PEEL_ENUM(Enum)                                                        \
template<>                                                                     \
peel::GObject::Type                                                            \
peel::GObject::Type::of<Enum> ();                                              \
                                                                               \
template<>                                                                     \
struct peel::GObject::Value::Traits<Enum>                                      \
{                                                                              \
  typedef Enum UnownedType;                                                    \
                                                                               \
  static Enum                                                                  \
  get (const ::GValue *value) noexcept                                         \
  {                                                                            \
    return static_cast<Enum> (g_value_get_enum (value));                       \
  }                                                                            \
                                                                               \
  static void                                                                  \
  set (::GValue *value, Enum m) noexcept                                       \
  {                                                                            \
    g_value_set_enum (value,                                                   \
                      static_cast<std::underlying_type<Enum>::type> (m));      \
  }                                                                            \
                                                                               \
  static void                                                                  \
  set_marshal_return (::GValue *value, Enum m)                                 \
  {                                                                            \
    set (value, m);                                                            \
  }                                                                            \
};                                                                             \
                                                                               \
template<>                                                                     \
struct peel::internals::PspecTraits<Enum>                                      \
{                                                                              \
  Enum default_value;                                                          \
                                                                               \
  constexpr PspecTraits (Enum default_value)                                   \
    : default_value (default_value)                                            \
  { }                                                                          \
                                                                               \
  ::GParamSpec *                                                               \
  create_pspec (::peel::internals::PspecBasics basics)                         \
  {                                                                            \
    return g_param_spec_enum (basics.name, basics.nick, basics.blurb,          \
                              ::peel::Type::of<Enum> (),                       \
                              static_cast<gint> (default_value),               \
                              basics.flags);                                   \
  }                                                                            \
};                                                                             \
/* end of PEEL_ENUM */


#define PEEL_ENUM_IMPL(Enum, type_name, ...)                                   \
template<>                                                                     \
peel::GObject::Type                                                            \
peel::GObject::Type::of<Enum> ()                                               \
{                                                                              \
  static ::GType _peel_tp;                                                     \
                                                                               \
  if (_peel_once_init_enter (&_peel_tp))                                       \
    {                                                                          \
      static const ::GEnumValue enum_values[] =                                \
      {                                                                        \
        __VA_ARGS__,                                                           \
        { 0, nullptr, nullptr }                                                \
      };                                                                       \
      ::GType _peel_actual_tp = g_enum_register_static (                       \
        g_intern_static_string (type_name), enum_values);                      \
      _peel_once_init_leave (&_peel_tp, _peel_actual_tp);                      \
    }                                                                          \
                                                                               \
  return _peel_tp;                                                             \
}                                                                              \
/* end of PEEL_ENUM_IMPL */

#define PEEL_ENUM_VALUE(value, nick)                                           \
::GEnumValue { static_cast<gint> (value), #value, nick }
