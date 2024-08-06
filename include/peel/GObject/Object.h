#pragma once

#include <peel/GObject/Type.h>
#include <peel/GObject/Value.h>
#include <peel/GObject/TypeInstance.h>
#include <peel/GObject/TypeClass.h>
#include <peel/GObject/BindingFlags.h>
#include <peel/RefPtr.h>
#include <peel/FloatPtr.h>
#include <peel/lang.h>
#include <peel/signal.h>
#include <utility>
#include <glib-object.h>

namespace peel
{

namespace internals
{
template<typename>
struct InstallVisitor;
}

// Hack.
namespace Gtk
{
class Window;
}

namespace GObject
{
class Object;
class ParamSpec;
class Binding;
}

template<typename T>
struct Property
{
private:
  template<typename C>
  friend struct internals::InstallVisitor;
  friend class GObject::Object;

  const char *name;
  GObject::ParamSpec **pspec_ptr;

public:
  constexpr Property (const char *name, GObject::ParamSpec **pspec_ptr = nullptr)
    : name (name)
    , pspec_ptr (pspec_ptr)
  { }

  constexpr
  const char *
  get_name () const
  {
    return name;
  }
};

namespace GObject
{

class Object : public TypeInstance
{
private:
  unsigned char _placeholder[sizeof (::GObject) - sizeof (TypeInstance)] peel_no_warn_unused;

  Object () = delete;
  Object (const Object &) = delete;
  Object (Object &&) = delete;

  Object &
  operator = (const Object &) = delete;
  Object &
  operator = (Object &&) = delete;

  // using TypeInstance::create;
  using TypeInstance::free;

#ifdef peel_hdp_via_ptreq
  template<typename, typename, typename>
  friend struct internals::PropertyHelper;
  template<typename>
  friend struct internals::HdpHelper;

  template<typename F>
  static void
  define_properties (F &)
  { }
#endif

public:
  struct ConstructParam
  {
    ParamSpec *psepc;
    Value *value;
  };
  static_assert (sizeof (ConstructParam) == sizeof (::GObjectConstructParam),
                 "GObject::ConstructParam size mismatch");
  static_assert (alignof (ConstructParam) == alignof (::GObjectConstructParam),
                 "GObject::ConstructParam align mismatch");

private:
  template<typename... Args>
  struct PropsCollector;

protected:
  ~Object () = default;

  peel_nothrow
  void
  notify (const char *property_name)
  {
    ::GObject *object = reinterpret_cast<::GObject *> (this);
    g_object_notify (object, property_name);
  }

  template<typename T>
  peel_nothrow
  void
  notify (Property<T> prop)
  {
    ::GObject *object = reinterpret_cast<::GObject *> (this);
    if (prop.pspec_ptr)
      {
        ::GParamSpec *pspec = reinterpret_cast<::GParamSpec *> (*prop.pspec_ptr);
        g_object_notify_by_pspec (object, pspec);
      }
    else
      g_object_notify (object, prop.name);
  }

  peel_nothrow
  void
  freeze_notify ()
  {
    ::GObject *object = reinterpret_cast<::GObject *> (this);
    g_object_freeze_notify (object);
  }

  peel_nothrow
  void
  thaw_notify ()
  {
    ::GObject *object = reinterpret_cast<::GObject *> (this);
    g_object_thaw_notify (object);
  }

  static void
  _peel_chain_finalize (::GObject *obj)
  {
    reinterpret_cast<::GObjectClass *> (TypeClass::peek_static<Object> ())->finalize (obj);
  }

  template<typename DerivedClass>
  peel_nothrow
  void
  parent_vfunc_dispose ()
  {
    ::GObjectClass *klass = reinterpret_cast<::GObjectClass *> (TypeClass::peek<DerivedClass> ()->peek_parent ());
    ::GObject *self = reinterpret_cast<::GObject *> (this);
    klass->dispose (self);
  }

  template<typename DerivedClass>
  peel_nothrow
  static RefPtr<Object>
  parent_vfunc_constructor (Type type, size_t n_construct_params, ConstructParam params[])
  {
    ::GObjectClass *klass = reinterpret_cast<::GObjectClass *> (TypeClass::peek<DerivedClass> ()->peek_parent ());
    ::GObjectConstructParam *peel_params = reinterpret_cast<::GObjectConstructParam *> (params);
    ::GObject *obj = klass->constructor (type, n_construct_params, peel_params);
    return RefPtr<Object>::adopt_ref (reinterpret_cast<Object *> (obj));
  }

  template<typename DerivedClass>
  peel_nothrow
  void
  parent_vfunc_constructed ()
  {
    ::GObjectClass *klass = reinterpret_cast<::GObjectClass *> (TypeClass::peek<DerivedClass> ()->peek_parent ());
    ::GObject *self = reinterpret_cast<::GObject *> (this);
    klass->constructed (self);
  }

public:
  template<typename Class, typename = void>
  struct Traits
  {
    typedef RefPtr<Class> CreateType;

    static CreateType
    created (::GObject *obj)
    {
      return RefPtr<Class>::adopt_ref (reinterpret_cast<Class *> (obj));
    }
  };

  template<typename Class, typename... Args>
  peel_nothrow
  static peel::enable_if_derived<Object, Class, typename Traits<Class>::CreateType>
  create (Args &&...args)
  {
    static_assert (sizeof... (Args) % 2 == 0,
                   "Must pass property / value pairs");

    ::GType type = static_cast<::GType> (Type::of<Class> ());
    ::GObject *obj = PropsCollector<Args...>::create (type, std::forward<Args> (args)...);
    return Traits<Class>::created (obj);
  }

  peel_nothrow
  static RefPtr<Object>
  create (Type tp, const char *first_prop_name, ...)
  {
    va_list ap;
    va_start (ap, first_prop_name);
    ::GType type = static_cast<::GType> (tp);
    void *obj = g_object_new_valist (type, first_prop_name, ap);
    va_end (ap);
    if (!obj)
      return nullptr;
    if (G_IS_INITIALLY_UNOWNED (obj))
      g_object_ref_sink (obj);
    return RefPtr<Object>::adopt_ref (reinterpret_cast<Object *> (obj));
  }

  peel_nothrow
  void
  get_property (const char *name, Value *value)
  {
    ::GObject *object = reinterpret_cast<::GObject *> (this);
    ::GValue *_peel_value = reinterpret_cast<::GValue *> (value);
    g_object_get_property (object, name, _peel_value);
  }

  template<typename T>
  typename Value::Traits<T>::UnownedType
  get_property (Property<T> prop)
  {
    Value value { Type::of<T> () };
    get_property (prop.name, &value);
    return value.get<T> ();
  }

  peel_nothrow
  void
  set_property (const char *name, const Value *value)
  {
    ::GObject *object = reinterpret_cast<::GObject *> (this);
    const ::GValue *_peel_value = reinterpret_cast<const ::GValue *> (value);
    g_object_set_property (object, name, _peel_value);
  }

  template<typename T, typename U>
  void
  set_property (Property<T> prop, U &&v)
  {
    Value value { Type::of<T> () };
    value.set<T> (std::forward<U> (v));
    set_property (prop.name, &value);
  }

  // TODO: variadic get/set

  template<typename T1, typename T2>
  peel_nothrow
  static Binding *
  bind_property (
    Object *source, Property<T1> source_property,
    Object *target, Property<T2> target_property,
    BindingFlags flags = BindingFlags (0))
  {
    ::GBinding *raw_binding = g_object_bind_property (
      source, source_property.name,
      target, target_property.name,
      static_cast<::GBindingFlags> (flags));
    return reinterpret_cast<Binding *> (raw_binding);
  }

  peel_nothrow
  void
  run_dispose ()
  {
    g_object_run_dispose (reinterpret_cast<::GObject *> (this));
  }

  template<typename T, typename Handler>
  peel_nothrow
  SignalConnection::Token
  connect_notify (Property<T> prop, Handler &&handler, bool after = false)
  {
    Signal<Object, void (ParamSpec *)> notify_signal = Signal<Object, void (ParamSpec *)>::lookup ("notify");
    ::GQuark quark = g_quark_from_string (prop.name);
    return notify_signal.template connect<Handler> (this, quark, static_cast<Handler &&> (handler), after);
  }

  template<typename T, typename Handler>
  peel_nothrow
  SignalConnection::Token
  connect_notify (Handler &&handler, bool after = false)
  {
    Signal<Object, void (ParamSpec *)> notify_signal = Signal<Object, void (ParamSpec *)>::lookup ("notify");
    return notify_signal.template connect<Handler> (this, static_cast<Handler &&> (handler), after);
  }

  template<typename T, typename HandlerObject>
  peel_nothrow
  SignalConnection::Token
  connect_notify
  (
    Property<T> prop,
    HandlerObject *object,
    void (HandlerObject::*handler_method) (Object *, ParamSpec *),
    bool after = false
  )
  {
    Signal<Object, void (ParamSpec *)> notify_signal = Signal<Object, void (ParamSpec *)>::lookup ("notify");
    ::GQuark quark = g_quark_from_string (prop.name);
    return notify_signal.template connect<HandlerObject> (this, quark, object, handler_method, after);
  }

  template<typename T, typename HandlerObject>
  peel_nothrow
  SignalConnection::Token
  connect_notify
  (
    HandlerObject *object,
    void (HandlerObject::*handler_method) (Object *, ParamSpec *),
    bool after = false
  )
  {
    Signal<Object, void (ParamSpec *)> notify_signal = Signal<Object, void (ParamSpec *)>::lookup ("notify");
    return notify_signal.template connect<HandlerObject> (this, object, handler_method, after);
  }

  template<typename T>
  peel_nothrow
  void
  add_weak_pointer (T **weak_pointer_location)
  {
    ::GObject *obj = reinterpret_cast<::GObject *> (this);
    gpointer *ptr = reinterpret_cast<gpointer *> (weak_pointer_location);
    g_object_add_weak_pointer (obj, ptr);
  }

  template<typename T>
  peel_nothrow
  void
  remove_weak_pointer (T **weak_pointer_location)
  {
    ::GObject *obj = reinterpret_cast<::GObject *> (this);
    gpointer *ptr = reinterpret_cast<gpointer *> (weak_pointer_location);
    g_object_remove_weak_pointer (obj, ptr);
  }

  // TODO: the generic add_weak_ref () API

  peel_nothrow
  void *
  get_data (const char *key)
  {
    ::GObject *obj = reinterpret_cast<::GObject *> (this);
    return g_object_get_data (obj, key);
  }

  peel_nothrow
  void *
  get_data (::GQuark quark)
  {
    ::GObject *obj = reinterpret_cast<::GObject *> (this);
    return g_object_get_qdata (obj, quark);
  }

  peel_nothrow
  void
  set_data (const char *key, void *data, ::GDestroyNotify destroy_notify = nullptr)
  {
    ::GObject *obj = reinterpret_cast<::GObject *> (this);
    g_object_set_data_full (obj, key, data, destroy_notify);
  }

  peel_nothrow
  void
  set_data (::GQuark quark, void *data, ::GDestroyNotify destroy_notify = nullptr)
  {
    ::GObject *obj = reinterpret_cast<::GObject *> (this);
    g_object_set_qdata_full (obj, quark, data, destroy_notify);
  }

  peel_nothrow
  void *
  steal_data (const char *key)
  {
    ::GObject *obj = reinterpret_cast<::GObject *> (this);
    return g_object_steal_data (obj, key);
  }

  peel_nothrow
  void *
  steal_data (::GQuark quark)
  {
    ::GObject *obj = reinterpret_cast<::GObject *> (this);
    return g_object_steal_qdata (obj, quark);
  }

  class Class : public TypeClass
  {
  private:
    Class () = delete;
    Class (const Class &) = delete;
    Class (Class &&) = delete;

    // TODO: Should be public fields?
    unsigned char _placeholder[sizeof (::GObjectClass) - sizeof (TypeClass)] peel_no_warn_unused;

  public:
    peel_nothrow
    ParamSpec *
    find_property (const char *property_name)
    {
      ::GObjectClass *klass = reinterpret_cast<::GObjectClass *> (this);
      ::GParamSpec *pspec = g_object_class_find_property (klass, property_name);
      return reinterpret_cast<ParamSpec *> (pspec);
    }

    template<typename T>
    ParamSpec *
    find_property (Property<T> prop)
    {
      return find_property (prop.get_name ());
    }

  protected:
    /* Don't call this if using the define_properties () mechanism */
    peel_nothrow
    void
    install_property (unsigned id, FloatPtr<ParamSpec> &&pspec)
    {
      // The argument must be of type FloatPtr && (as opposed to a by-value
      // FloatPtr) to prevent trying to instantiate RefTraits<ParamSpec>
      // for the forward-declared ParamSpec type.
      ::GObjectClass *klass = reinterpret_cast<::GObjectClass *> (this);
      ::GParamSpec *p = reinterpret_cast<::GParamSpec *> (std::move (pspec).release_floating_ptr ());
      g_object_class_install_property (klass, id, p);
    }

    template<typename DerivedClass>
    peel_nothrow
    void
    override_vfunc_constructor ()
    {
      ::GObjectClass *klass = reinterpret_cast<::GObjectClass *> (this);
      klass->constructor = +[] (::GType tp, guint n, ::GObjectConstructParam *raw_params) -> ::GObject *
      {
        Type type = tp;
        size_t num = n;
        ConstructParam *params = reinterpret_cast<ConstructParam *> (raw_params);
        RefPtr<Object> obj = DerivedClass::vfunc_constructor (type, num, params);
        return reinterpret_cast<::GObject *> (std::move (obj).release_ref ());
      };
    }

    template<typename DerivedClass>
    peel_nothrow
    void
    override_vfunc_constructed ()
    {
      ::GObjectClass *klass = reinterpret_cast<::GObjectClass *> (this);
      klass->constructed = +[] (::GObject *obj)
      {
        DerivedClass *self = reinterpret_cast<DerivedClass *> (obj);
        self->DerivedClass::vfunc_constructed ();
      };
    }

    template<typename DerivedClass>
    peel_nothrow
    void
    override_vfunc_dispose ()
    {
      ::GObjectClass *klass = reinterpret_cast<::GObjectClass *> (this);
      klass->dispose = +[] (::GObject *obj)
      {
        DerivedClass *self = reinterpret_cast<DerivedClass *> (obj);
        self->DerivedClass::vfunc_dispose ();
      };
    }
  };
};

static_assert (sizeof (Object) == sizeof (::GObject),
               "GObject::Object size mismatch");
static_assert (alignof (Object) == alignof (::GObject),
               "GObject::Object align mismatch");
static_assert (sizeof (Object::Class) == sizeof (::GObjectClass),
               "GObject::Object::Class size mismatch");
static_assert (alignof (Object::Class) == alignof (::GObjectClass),
               "GObject::Object::Class align mismatch");

template<>
constexpr inline Type
Type::of<Object> ()
{
  return G_TYPE_OBJECT;
}

template<typename T>
struct Value::Traits<T, peel::enable_if_derived<Object, T, void>>
{
  typedef T *UnownedType;
  typedef RefPtr<T> OwnedType;

  peel_nothrow G_GNUC_CONST
  static T *
  get (const ::GValue *value)
  {
    void *obj = g_value_get_object (value);
    if (std::is_same<T, Object>::value)
      return reinterpret_cast<T *> (obj);
#if defined (G_DISABLE_CAST_CHECKS) || defined (__OPTIMIZE__)
    return reinterpret_cast<T *> (obj);
#else
    ::GType tp = static_cast<::GType> (Type::of<T> ());
    return G_TYPE_CHECK_INSTANCE_CAST (obj, tp, T);
#endif
  }

  peel_nothrow
  static void
  set (::GValue *value, T *object)
  {
    g_value_set_object (value, object);
  }

  peel_nothrow
  static void
  take (::GValue *value, RefPtr<T> &&object)
  {
    g_value_take_object (value, std::move (object).release_ref ());
  }

  peel_nothrow
  static void
  set_sink (::GValue *value, FloatPtr<T> &&object)
  {
    T *obj = std::move (object).release_floating_ptr ();
    g_value_take_object (value, g_object_ref_sink (obj));
  }
};

class InitiallyUnowned;

template<typename T>
struct Object::Traits<T, typename std::enable_if<std::is_base_of<InitiallyUnowned, T>::value && !std::is_base_of<Gtk::Window, T>::value, void>::type>
{
  typedef FloatPtr<T> CreateType;

  static CreateType
  created (::GObject *obj)
  {
    return FloatPtr<T> (reinterpret_cast<T *> (obj));
  }
};

template<>
struct Object::PropsCollector<>
{
  static void
  collect (const char *names[], ::GValue values[])
  {
    (void) names;
    (void) values;
  }

  static ::GObject *
  create (::GType tp)
  {
    return g_object_new_with_properties (tp, 0, nullptr, nullptr);
  }
};

template<typename T, typename U, typename... Args>
struct Object::PropsCollector<Property<T>, U, Args...>
{
  static void
  collect (const char **names, ::GValue *values, Property<T> prop, U &&prop_value, Args... args)
  {
    names[0] = prop.get_name ();
    g_value_init (&values[0], static_cast<::GType> (Type::of<T> ()));
    Value *value = reinterpret_cast<Value *> (&values[0]);
    value->set<T> (std::forward<U> (prop_value));

    Object::PropsCollector<Args...>::collect (names + 1, values + 1, std::forward<Args> (args)...);
  }

  peel_nothrow
  static ::GObject *
  create (::GType type, Property<T> prop, U &&prop_value, Args... args)
  {
    constexpr int n = 1 + sizeof... (Args) / 2;
    const char *names[n];
    ::GValue values[n];
    memset (values, 0, sizeof (values));

    collect (names, values, prop, std::forward<U> (prop_value), std::forward<Args> (args)...);

    ::GObject *obj = g_object_new_with_properties (type, n, names, values);

    for (size_t i = 0; i < n; i++)
      g_value_unset (&values[i]);

    return obj;
  }
};

} /* namespace GObject */

using GObject::Object;

#define PEEL_USE_COMPAT_CONTROL_HACK

#ifdef PEEL_USE_COMPAT_CONTROL_HACK
/* Note: this function is very intentionally marked inline,
   but not static inline. */
peel_always_inline peel_nothrow
void
peel_sink_unref_gobject_if_floating (void *object)
{
  static guint (*floating_flag_handler)(::GObject *, gint);

  G_GNUC_BEGIN_IGNORE_DEPRECATIONS
  /* Could use relaxed atomics here to avoid the technical UB.  */
  if (G_UNLIKELY (floating_flag_handler == nullptr))
    g_object_compat_control (3, &floating_flag_handler);
  G_GNUC_END_IGNORE_DEPRECATIONS

  if (floating_flag_handler ((::GObject *) object, -1))
    g_object_unref (object);
}
#endif

template<typename T>
struct RefTraits<T, peel::enable_if_derived<GObject::Object, T, void>>
{
  static void
  ref (T *obj)
  {
    void *r = g_object_ref (obj);
    peel_assume (r == obj);
  }

  static void
  unref (T *obj)
  {
    g_object_unref (obj);
  }

  static void
  ref_sink (T *obj)
  {
    void *r = g_object_ref_sink (obj);
    peel_assume (r == obj);
  }

  peel_always_inline
  static void
  sink_unref (T *obj)
  {
#ifdef PEEL_USE_COMPAT_CONTROL_HACK
    peel_sink_unref_gobject_if_floating (obj);
#else
    g_object_ref_sink (obj);
    g_object_unref (obj);
#endif
  }
};

} /* namespace peel */
