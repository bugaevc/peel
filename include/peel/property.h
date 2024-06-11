#pragma once

#include <peel/GObject/Type.h>
#include <peel/GObject/Value.h>
#include <peel/lang.h>
#include <glib-object.h>

namespace peel
{

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
template<typename Class, typename = void>
struct PropertyHelper;

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
  ::GValue *value;
  bool *found_ptr;

  Getter (Class *instance, ::GValue *value, bool *found_ptr)
    : instance (instance)
    , value (value)
    , found_ptr (found_ptr)
  { }

public:
  typedef typename GObject::Value::Traits<T>::UnownedType UnownedType;

  peel_nothrow
  Getter &
  get (UnownedType (Class::*getter) () const)
  {
    if (!found_ptr)
      return *this;
    *found_ptr = true;
    UnownedType v = (instance->*getter) ();
    GObject::Value::Traits<T>::set (value, v);
    return *this;
  }

  peel_nothrow
  Getter &
  get (UnownedType (Class::*getter) ())
  {
    if (!found_ptr)
      return *this;
    *found_ptr = true;
    UnownedType v = (instance->*getter) ();
    GObject::Value::Traits<T>::set (value, v);
    return *this;
  }

  peel_nothrow
  Getter &
  get (void (Class::*getter) (GObject::Value *value))
  {
    if (!found_ptr)
      return *this;
    *found_ptr = true;
    (instance->*getter) (reinterpret_cast<GObject::Value *> (value));
    return *this;
  }

  peel_nothrow
  Getter &
  get (void (Class::*getter) (GObject::Value *value) const)
  {
    if (!found_ptr)
      return *this;
    *found_ptr = true;
    (instance->*getter) (reinterpret_cast<GObject::Value *> (value));
    return *this;
  }

  template<typename F>
  peel_nothrow
  Getter &
  get (F f)
  {
    if (!found_ptr)
      return *this;
    *found_ptr = true;
    UnownedType v = f (instance);
    GObject::Value::Traits<T>::set (value, v);
    return *this;
  }

  peel_nothrow
  Getter &
  set (...)
  {
    return *this;
  }
};

template<typename Class>
struct GetVisitor
{
  friend struct PropertyHelper<Class>;
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
};

template<typename Class, typename T>
struct Setter : public GSBase<Setter<Class, T>>
{
  template<typename C>
  friend class SetVisitor;

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

  peel_nothrow
  Setter &
  get (...)
  {
    return *this;
  }

  peel_nothrow
  Setter &
  set (void (Class::*setter) (UnownedType))
  {
    if (!found_ptr)
      return *this;
    *found_ptr = true;
    UnownedType v = GObject::Value::Traits<T>::get (value);
    (instance->*setter) (v);
    return *this;
  }

  template<typename F>
  peel_nothrow
  Setter &
  set (F f)
  {
    if (!found_ptr)
      return *this;
    *found_ptr = true;
    UnownedType v = GObject::Value::Traits<T>::get (value);
    f (instance, v);
    return *this;
  }
};

template<typename Class>
struct SetVisitor
{
  friend struct PropertyHelper<Class>;

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

  peel_nothrow
  ~Installer ()
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
    basics.flags = ::GParamFlags(basics.flags | static_cast<::GParamFlags> (flags));
    return *this;
  }
};

template<typename Class>
struct InstallVisitor
{
  friend struct PropertyHelper<Class>;

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
};

template<typename Class>
using if_has_define_properties = decltype (Class::define_properties (std::declval<int &> ()));

template<typename Class, typename>
struct PropertyHelper
{
  peel_nothrow
  static void
  init_props (::GObjectClass *)
  {
    /* The base implementation does nothing */
  }
};

template<typename Class>
struct PropertyHelper<Class, if_has_define_properties<Class>>
{
  peel_nothrow
  static void
  get_property (::GObject *object, guint prop_id, ::GValue *value, ::GParamSpec *pspec)
  {
    Class *instance = reinterpret_cast<Class *> (object);
    GetVisitor<Class> visitor { instance, prop_id, value };
    Class::define_properties (visitor);
    if (G_UNLIKELY (!visitor.found))
      {
        typedef ::GObject GObject;
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
  }

  peel_nothrow
  static void
  set_property (::GObject *object, guint prop_id, const ::GValue *value, ::GParamSpec *pspec)
  {
    Class *instance = reinterpret_cast<Class *> (object);
    SetVisitor<Class> visitor { instance, prop_id, value };
    Class::define_properties (visitor);
    if (G_UNLIKELY (!visitor.found))
      {
        typedef ::GObject GObject;
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
  }

  peel_nothrow
  static void
  init_props (::GObjectClass *klass)
  {
    klass->get_property = get_property;
    klass->set_property = set_property;

    InstallVisitor<Class> visitor { klass };
    Class::define_properties (visitor);
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
