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

#define PEEL_FLAGS(Flags)                                                      \
template<>                                                                     \
peel::GObject::Type                                                            \
peel::GObject::Type::of<Flags> ();                                             \
                                                                               \
template<>                                                                     \
struct peel::GObject::Value::Traits<Flags>                                     \
{                                                                              \
  typedef Flags UnownedType;                                                   \
                                                                               \
  static Flags                                                                 \
  get (const ::GValue *value) noexcept                                         \
  {                                                                            \
    return static_cast<Flags> (g_value_get_flags (value));                     \
  }                                                                            \
                                                                               \
  static void                                                                  \
  set (::GValue *value, Flags m) noexcept                                      \
  {                                                                            \
    g_value_set_flags (value,                                                  \
                      static_cast<std::underlying_type<Flags>::type> (m));     \
  }                                                                            \
                                                                               \
  static void                                                                  \
  set_marshal_return (::GValue *value, Flags m)                                \
  {                                                                            \
    set (value, m);                                                            \
  }                                                                            \
};                                                                             \
                                                                               \
template<>                                                                     \
struct peel::internals::PspecTraits<Flags>                                     \
{                                                                              \
  Flags default_value;                                                         \
                                                                               \
  constexpr PspecTraits (Flags default_value)                                  \
    : default_value (default_value)                                            \
  { }                                                                          \
                                                                               \
  ::GParamSpec *                                                               \
  create_pspec (::peel::internals::PspecBasics basics)                         \
  {                                                                            \
    return g_param_spec_flags (basics.name, basics.nick, basics.blurb,         \
                               ::peel::Type::of<Flags> (),                     \
                               static_cast<gint> (default_value),              \
                               basics.flags);                                  \
  }                                                                            \
};                                                                             \
                                                                               \
static constexpr inline Flags                                                  \
operator | (Flags lhs, Flags rhs)                                              \
{                                                                              \
  return Flags (                                                               \
    static_cast<typename std::underlying_type<Flags>::type> (lhs)              \
    |                                                                          \
    static_cast<typename std::underlying_type<Flags>::type> (rhs));            \
}                                                                              \
static constexpr inline Flags                                                  \
operator & (Flags lhs, Flags rhs)                                              \
{                                                                              \
  return Flags (                                                               \
    static_cast<typename std::underlying_type<Flags>::type> (lhs)              \
    &                                                                          \
    static_cast<typename std::underlying_type<Flags>::type> (rhs));            \
}                                                                              \
static constexpr inline Flags                                                  \
operator ^ (Flags lhs, Flags rhs)                                              \
{                                                                              \
  return Flags (                                                               \
    static_cast<typename std::underlying_type<Flags>::type> (lhs)              \
    ^                                                                          \
    static_cast<typename std::underlying_type<Flags>::type> (rhs));            \
}                                                                              \
static constexpr inline Flags                                                  \
operator ~ (Flags lhs)                                                         \
{                                                                              \
  return Flags (                                                               \
    ~ static_cast<typename std::underlying_type<Flags>::type> (lhs));          \
}                                                                              \
static inline Flags &                                                          \
operator |= (Flags &lhs, Flags rhs)                                            \
{                                                                              \
  lhs = lhs | rhs;                                                             \
  return lhs;                                                                  \
}                                                                              \
static inline Flags &                                                          \
operator &= (Flags &lhs, Flags rhs)                                            \
{                                                                              \
  lhs = lhs & rhs;                                                             \
  return lhs;                                                                  \
}                                                                              \
static inline Flags &                                                          \
operator ^= (Flags &lhs, Flags rhs)                                            \
{                                                                              \
  lhs = lhs ^ rhs;                                                             \
  return lhs;                                                                  \
}                                                                              \
static constexpr inline bool                                                   \
operator ! (Flags lhs)                                                         \
{                                                                              \
  return !static_cast<typename std::underlying_type<Flags>::type> (lhs);       \
}                                                                              \
static constexpr inline bool                                                   \
operator + (Flags lhs)                                                         \
{                                                                              \
  return !!static_cast<typename std::underlying_type<Flags>::type> (lhs);      \
}                                                                              \
/* end of PEEL_FLAGS */


#define PEEL_FLAGS_IMPL(Flags, type_name, ...)                                 \
template<>                                                                     \
peel::GObject::Type                                                            \
peel::GObject::Type::of<Flags> ()                                              \
{                                                                              \
  static ::GType _peel_tp;                                                     \
                                                                               \
  if (_peel_once_init_enter (&_peel_tp))                                       \
    {                                                                          \
      static const ::GFlagsValue flags_values[] =                              \
      {                                                                        \
        __VA_ARGS__,                                                           \
        { 0, nullptr, nullptr }                                                \
      };                                                                       \
      ::GType _peel_actual_tp = g_flags_register_static (                      \
        g_intern_static_string (type_name), flags_values);                     \
      _peel_once_init_leave (&_peel_tp, _peel_actual_tp);                      \
    }                                                                          \
                                                                               \
  return _peel_tp;                                                             \
}                                                                              \
/* end of PEEL_FLAGS_IMPL */

#define PEEL_FLAGS_VALUE(value, nick)                                          \
::GFlagsValue { static_cast<gint> (value), #value, nick }
