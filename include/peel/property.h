#pragma once

#include <peel/GObject/Type.h>
#include <peel/GObject/Value.h>
#include <peel/lang.h>
#include <glib-object.h>

peel_begin_header

namespace peel
{

class String;

namespace GObject
{
class Object;
class ParamSpec;
enum class ParamFlags : std::underlying_type<::GParamFlags>::type;
}

template<typename T>
struct Property;

namespace internals
{
template<typename Subclass>
struct HdpHelper;

#if defined (__GNUC__) && !defined (__clang__)
#define peel_hdp_via_visibility
#else
#define peel_hdp_via_ptreq
#endif

#if defined (peel_hdp_via_visibility)

template<typename Subclass, typename = void>
struct PropertyHelper;
#define peel_friend_prop_helper(Subclass)                                      \
  friend struct ::peel::internals::PropertyHelper<Subclass>;                   \
  friend struct ::peel::internals::HdpHelper<Subclass>;

#elif defined (peel_hdp_via_ptreq)

template<typename Subclass, typename ParentClass, typename = void>
struct PropertyHelper;
#define peel_friend_prop_helper(Subclass)                                      \
  template<typename, typename, typename>                                       \
  friend struct ::peel::internals::PropertyHelper;                             \
  template<typename>                                                           \
  friend struct ::peel::internals::HdpHelper;

#endif

template<typename T>
struct PspecTraits;

struct PspecBasics
{
  const char *name;
  const char *nick;
  const char *blurb;
  ::GParamFlags flags;
};

template<>
struct PspecTraits<bool>
{
  bool default_value;

  constexpr PspecTraits (bool default_value)
    : default_value (default_value)
  { }

  ::GParamSpec *
  create_pspec (PspecBasics basics)
  {
    return g_param_spec_boolean (basics.name, basics.nick, basics.blurb, default_value, basics.flags);
  }
};

template<>
struct PspecTraits<int>
{
  int minimum;
  int maximum;
  int default_value;

  constexpr PspecTraits (int minimum, int maximum, int default_value)
    : minimum (minimum)
    , maximum (maximum)
    , default_value (default_value)
  { }

  ::GParamSpec *
  create_pspec (PspecBasics basics)
  {
    return g_param_spec_int (basics.name, basics.nick, basics.blurb, minimum, maximum, default_value, basics.flags);
  }
};

template<>
struct PspecTraits<unsigned>
{
  unsigned minimum;
  unsigned maximum;
  unsigned default_value;

  constexpr PspecTraits (unsigned minimum, unsigned maximum, unsigned default_value)
    : minimum (minimum)
    , maximum (maximum)
    , default_value (default_value)
  { }

  ::GParamSpec *
  create_pspec (PspecBasics basics)
  {
    return g_param_spec_uint (basics.name, basics.nick, basics.blurb, minimum, maximum, default_value, basics.flags);
  }
};

template<>
struct PspecTraits<GObject::Long>
{
  GObject::Long minimum;
  GObject::Long maximum;
  GObject::Long default_value;

  constexpr PspecTraits (GObject::Long minimum, GObject::Long maximum, GObject::Long default_value)
    : minimum (minimum)
    , maximum (maximum)
    , default_value (default_value)
  { }

  ::GParamSpec *
  create_pspec (PspecBasics basics)
  {
    return g_param_spec_long (basics.name, basics.nick, basics.blurb, minimum, maximum, default_value, basics.flags);
  }
};

template<>
struct PspecTraits<GObject::ULong>
{
  GObject::ULong minimum;
  GObject::ULong maximum;
  GObject::ULong default_value;

  constexpr PspecTraits (GObject::ULong minimum, GObject::ULong maximum, GObject::ULong default_value)
    : minimum (minimum)
    , maximum (maximum)
    , default_value (default_value)
  { }

  ::GParamSpec *
  create_pspec (PspecBasics basics)
  {
    return g_param_spec_ulong (basics.name, basics.nick, basics.blurb, minimum, maximum, default_value, basics.flags);
  }
};

template<>
struct PspecTraits<int64_t>
{
  int64_t minimum;
  int64_t maximum;
  int64_t default_value;

  constexpr PspecTraits (int64_t minimum, int64_t maximum, int64_t default_value)
    : minimum (minimum)
    , maximum (maximum)
    , default_value (default_value)
  { }

  ::GParamSpec *
  create_pspec (PspecBasics basics)
  {
    return g_param_spec_int64 (basics.name, basics.nick, basics.blurb, minimum, maximum, default_value, basics.flags);
  }
};

template<>
struct PspecTraits<uint64_t>
{
  uint64_t minimum;
  uint64_t maximum;
  uint64_t default_value;

  constexpr PspecTraits (uint64_t minimum, uint64_t maximum, uint64_t default_value)
    : minimum (minimum)
    , maximum (maximum)
    , default_value (default_value)
  { }

  ::GParamSpec *
  create_pspec (PspecBasics basics)
  {
    return g_param_spec_uint64 (basics.name, basics.nick, basics.blurb, minimum, maximum, default_value, basics.flags);
  }
};

template<>
struct PspecTraits<float>
{
  float minimum;
  float maximum;
  float default_value;

  constexpr PspecTraits (float minimum, float maximum, float default_value)
    : minimum (minimum)
    , maximum (maximum)
    , default_value (default_value)
  { }

  ::GParamSpec *
  create_pspec (PspecBasics basics)
  {
    return g_param_spec_float (basics.name, basics.nick, basics.blurb, minimum, maximum, default_value, basics.flags);
  }
};

template<>
struct PspecTraits<double>
{
  double minimum;
  double maximum;
  double default_value;

  constexpr PspecTraits (double minimum, double maximum, double default_value)
    : minimum (minimum)
    , maximum (maximum)
    , default_value (default_value)
  { }

  ::GParamSpec *
  create_pspec (PspecBasics basics)
  {
    return g_param_spec_double (basics.name, basics.nick, basics.blurb, minimum, maximum, default_value, basics.flags);
  }
};

template<>
struct PspecTraits<const char *>
{
  const char *default_value;

  constexpr PspecTraits (const char *default_value)
    : default_value (default_value)
  { }

  ::GParamSpec *
  create_pspec (PspecBasics basics)
  {
    return g_param_spec_string (basics.name, basics.nick, basics.blurb, default_value, basics.flags);
  }
};

/* Allow both String and const char * as logical value types */
template<>
struct PspecTraits<String> : PspecTraits<const char *>
{
  using PspecTraits<const char *>::PspecTraits;
};

template<>
struct PspecTraits<void *>
{
  constexpr PspecTraits ()
  { }

  ::GParamSpec *
  create_pspec (PspecBasics basics)
  {
    return g_param_spec_pointer (basics.name, basics.nick, basics.blurb, basics.flags);
  }
};

template<>
struct PspecTraits<signed char>
{
  signed char minimum;
  signed char maximum;
  signed char default_value;

  constexpr PspecTraits (signed char minimum, signed char maximum, signed char default_value)
    : minimum (minimum)
    , maximum (maximum)
    , default_value (default_value)
  { }

  ::GParamSpec *
  create_pspec (PspecBasics basics)
  {
    return g_param_spec_char (basics.name, basics.nick, basics.blurb, minimum, maximum, default_value, basics.flags);
  }
};

template<>
struct PspecTraits<unsigned char>
{
  unsigned char minimum;
  unsigned char maximum;
  unsigned char default_value;

  constexpr PspecTraits (unsigned char minimum, unsigned char maximum, unsigned char default_value)
    : minimum (minimum)
    , maximum (maximum)
    , default_value (default_value)
  { }

  ::GParamSpec *
  create_pspec (PspecBasics basics)
  {
    return g_param_spec_uchar (basics.name, basics.nick, basics.blurb, minimum, maximum, default_value, basics.flags);
  }
};

template<>
struct PspecTraits<GObject::Type>
{
  GObject::Type is_a_type;

  constexpr PspecTraits (GObject::Type is_a_type = G_TYPE_NONE)
    : is_a_type (is_a_type)
  { }

  ::GParamSpec *
  create_pspec (PspecBasics basics)
  {
    return g_param_spec_gtype (basics.name, basics.nick, basics.blurb, is_a_type, basics.flags);
  }
};

template<>
struct PspecTraits<GObject::Object>
{
  GObject::Type object_type;

  constexpr PspecTraits (GObject::Type object_type)
    : object_type (object_type)
  { }

  ::GParamSpec *
  create_pspec (PspecBasics basics)
  {
    return g_param_spec_object (basics.name, basics.nick, basics.blurb, object_type, basics.flags);
  }
};

template<typename Self>
struct GSBase
{
  Self &
  nick (const char *)
  {
    return reinterpret_cast<Self &> (*this);
  }

  Self &
  blurb (const char *)
  {
    return reinterpret_cast<Self &> (*this);
  }

  Self &
  flags (GObject::ParamFlags)
  {
    return reinterpret_cast<Self &> (*this);
  }
};

template<typename Class, typename T>
struct Getter : public GSBase<Getter<Class, T>>
{
  template<typename C>
  friend struct GetVisitor;

private:
  Class *instance;
  GObject::Value *value;
  bool *found_ptr;

  Getter (Class *instance, ::GValue *value, bool *found_ptr)
    : instance (instance)
    , value (reinterpret_cast<GObject::Value *> (value))
    , found_ptr (found_ptr)
  { }

public:
  template<typename ReturnType>
  Getter &
  get (ReturnType (Class::*getter) () const) noexcept
  {
    if (!found_ptr)
      return *this;
    *found_ptr = true;
    value->set<T> ((instance->*getter) ());
    return *this;
  }

  template<typename ReturnType>
  Getter &
  get (ReturnType (Class::*getter) ()) noexcept
  {
    if (!found_ptr)
      return *this;
    *found_ptr = true;
    value->set<T> ((instance->*getter) ());
    return *this;
  }

  Getter &
  get (void (Class::*getter) (GObject::Value *)) noexcept
  {
    if (!found_ptr)
      return *this;
    *found_ptr = true;
    (instance->*getter) (value);
    return *this;
  }

  Getter &
  get (void (Class::*getter) (GObject::Value *) const) noexcept
  {
    if (!found_ptr)
      return *this;
    *found_ptr = true;
    (instance->*getter) (value);
    return *this;
  }

  Getter &
  set (...) noexcept
  {
    return *this;
  }
};

template<typename Class>
struct GetVisitor
{
  peel_friend_prop_helper (Class)

private:
  Class *instance;
  guint target_id;
  ::GValue *value;
  guint next_id;
  bool found;

  GetVisitor (Class *instance, guint target_id, ::GValue *value)
    : instance (instance)
    , target_id (target_id)
    , value (value)
    , next_id (1)
    , found (false)
  { }

public:
  template<typename T>
  Getter<Class, T>
  prop (Property<T>, ...)
  {
    if (next_id++ == target_id)
      return Getter<Class, T> { instance, value, &found };
    return Getter<Class, T> { nullptr, nullptr, nullptr };
  }

  template<typename T>
  Getter<Class, T>
  prop_with_custom_pspec (Property<T> p, ...)
  {
    return prop (p);
  }

  template<typename T>
  Getter<Class, T>
  override_prop (Property<T>)
  {
    if (next_id++ == target_id)
      return Getter<Class, T> { instance, value, &found };
    return Getter<Class, T> { nullptr, nullptr, nullptr };
  }
};

template<typename Class, typename T>
struct Setter : public GSBase<Setter<Class, T>>
{
  template<typename C>
  friend struct SetVisitor;

private:
  Class *instance;
  const ::GValue *value;
  bool *found_ptr;

  Setter (Class *instance, const ::GValue *value, bool *found_ptr)
    : instance (instance)
    , value (value)
    , found_ptr (found_ptr)
  { }

public:
  typedef typename GObject::Value::Traits<T>::UnownedType UnownedType;

  Setter &
  get (...) noexcept
  {
    return *this;
  }

  Setter &
  set (void (Class::*setter) (UnownedType)) noexcept
  {
    if (!found_ptr)
      return *this;
    *found_ptr = true;
    UnownedType v = GObject::Value::Traits<T>::get (value);
    (instance->*setter) (v);
    return *this;
  }

  // template<typename = peel::enable_if_derived<GObject::Object, T>>
  Setter &
  set (void (Class::*setter) (FloatPtr<T>)) noexcept
  {
    if (!found_ptr)
      return *this;
    *found_ptr = true;
    FloatPtr<T> obj = GObject::Value::Traits<T>::get (value);
    (instance->*setter) (std::move (obj));
    return *this;
  }
};

template<typename Class>
struct SetVisitor
{
  peel_friend_prop_helper (Class)

private:
  Class *instance;
  guint target_id;
  const ::GValue *value;
  guint next_id;
  bool found;

  SetVisitor (Class *instance, guint target_id, const ::GValue *value)
    : instance (instance)
    , target_id (target_id)
    , value (value)
    , next_id (1)
    , found (false)
  { }

public:
  template<typename T>
  Setter<Class, T>
  prop (Property<T>, ...)
  {
    if (next_id++ == target_id)
      return Setter<Class, T> { instance, value, &found };
    return Setter<Class, T> { nullptr, nullptr, nullptr };
  }

  template<typename T>
  Setter<Class, T>
  prop_with_custom_pspec (Property<T> p, ...)
  {
    return prop (p);
  }

  template<typename T>
  Setter<Class, T>
  override_prop (Property<T>)
  {
    if (next_id++ == target_id)
      return Setter<Class, T> { instance, value, &found };
    return Setter<Class, T> { nullptr, nullptr, nullptr };
  }
};

template<typename T>
struct Installer
{
private:
  ::GObjectClass *klass;
  GObject::ParamSpec **pspec_ptr;
  PspecBasics basics;
  PspecTraits<T> traits;
  guint id;

public:
  template<typename... Args>
  Installer (::GObjectClass *klass, const char *name, guint id, GObject::ParamSpec **pspec_ptr, Args... args)
    : klass (klass)
    , pspec_ptr (pspec_ptr)
    , traits (args...)
    , id (id)
  {
    basics.name = name;
    basics.nick = nullptr;
    basics.blurb = nullptr;
    basics.flags = ::GParamFlags (G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);
  }

  ~Installer () noexcept
  {
    ::GParamSpec *pspec = traits.create_pspec (basics);
    *pspec_ptr = reinterpret_cast<GObject::ParamSpec *> (pspec);
    g_object_class_install_property (klass, id, pspec);
  }

  Installer &
  get (...)
  {
    basics.flags = ::GParamFlags (basics.flags | G_PARAM_READABLE);
    return *this;
  }

  Installer &
  set (...)
  {
    basics.flags = ::GParamFlags (basics.flags | G_PARAM_WRITABLE);
    return *this;
  }

  Installer &
  nick (const char *nick)
  {
    basics.nick = nick;
    return *this;
  }

  Installer &
  blurb (const char *blurb)
  {
    basics.blurb = blurb;
    return *this;
  }

  Installer &
  flags (GObject::ParamFlags flags)
  {
    basics.flags = ::GParamFlags (basics.flags | static_cast<::GParamFlags> (flags));
    return *this;
  }
};

struct OverrideInstaller
{
private:
  ::GObjectClass *klass;
  const char *name;
  guint id;

public:
  template<typename... Args>
  OverrideInstaller (::GObjectClass *klass, const char *name, guint id)
    : klass (klass)
    , name (name)
    , id (id)
  { }

  ~OverrideInstaller () noexcept
  {
    g_object_class_override_property (klass, id, name);
  }

  OverrideInstaller &
  get (...)
  {
    return *this;
  }

  OverrideInstaller &
  set (...)
  {
    return *this;
  }

  OverrideInstaller &
  nick (...)
  {
    return *this;
  }

  OverrideInstaller &
  blurb (...)
  {
    return *this;
  }

  OverrideInstaller &
  flags (...)
  {
    return *this;
  }
};

template<typename Class>
struct InstallVisitor
{
  peel_friend_prop_helper (Class)

private:
  ::GObjectClass *klass;
  guint next_id;

  InstallVisitor (::GObjectClass *klass)
    : klass (klass)
    , next_id (1)
  { }

public:
  template<typename T, typename... Args>
  Installer<T>
  prop (Property<T> p, Args... args)
  {
    return Installer<T> { klass, p.get_name (), next_id++, p.pspec_ptr, args... };
  }

  template<typename T>
  peel::enable_if_derived<GObject::Object, T, Installer<GObject::Object>>
  prop (Property<T> p)
  {
    ::GType tp = GObject::Type::of<T> ();
    return Installer<GObject::Object> { klass, p.get_name (), next_id++, p.pspec_ptr, tp };
  }

  template<typename T>
  OverrideInstaller
  override_prop (Property<T> p)
  {
    return OverrideInstaller { klass, p.get_name (), next_id++ };
  }
};

struct DummyVisitorRet
{
  DummyVisitorRet &
  get (...)
  {
    return *this;
  }

  DummyVisitorRet &
  set (...)
  {
    return *this;
  }

  DummyVisitorRet &
  nick (...)
  {
    return *this;
  }

  DummyVisitorRet &
  blurb (...)
  {
    return *this;
  }

  DummyVisitorRet &
  flags (...)
  {
    return *this;
  }
};

struct DummyVisitor
{
  DummyVisitorRet
  prop (...)
  {
    return DummyVisitorRet { };
  }

  DummyVisitorRet
  override_prop (...)
  {
    return DummyVisitorRet { };
  }
};

} /* namespace internals */
} /* namespace peel */

#define PEEL_PROPERTY(type, name, name_str)                                    \
  static ::peel::Property<type> prop_ ## name ()                               \
  {                                                                            \
    static ::peel::GObject::ParamSpec *pspec;                                  \
    return ::peel::Property<type> { (name_str), &pspec };                      \
  }                                                                            \
/* end of PEEL_PROPERTY */

peel_end_header
