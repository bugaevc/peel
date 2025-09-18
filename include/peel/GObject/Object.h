#pragma once

#include <peel/GObject/Type.h>
#include <peel/GObject/Value.h>
#include <peel/GObject/TypeInstance.h>
#include <peel/GObject/TypeClass.h>
#include <peel/GObject/BindingFlags.h>
#include <peel/GObject/ParamSpec.h>
#include <peel/RefPtr.h>
#include <peel/FloatPtr.h>
#include <peel/lang.h>
#include <peel/signal.h>
#include <utility>
#include <glib-object.h>

peel_begin_header

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

  template<typename T1, typename T2, typename C, typename = void>
  struct BindingTransformHelper;

  template<typename F>
  constexpr static bool
  is_effectively_empty ()
  {
    return std::is_same<F, decltype (nullptr)>::value || std::is_empty<F>::value;
  }

  template<typename T, typename = void>
  struct GetPropertyHelper
  {
    typedef typename Value::Traits<T>::UnownedType GetType;

    static GetType
    get (const Value *value) noexcept
    {
      return value->get<T> ();
    }
  };

  template<typename T>
  struct GetPropertyHelper<T, void_t<typename Value::Traits<T>::OwnedType>>
  {
    typedef typename Value::Traits<T>::OwnedType GetType;

    static GetType
    get (const Value *value) noexcept
    {
      return value->dup<T> ();
    }
  };

protected:
  ~Object () = default;

  void
  notify (const char *property_name) noexcept
  {
    ::GObject *object = reinterpret_cast<::GObject *> (this);
    g_object_notify (object, property_name);
  }

  template<typename T>
  void
  notify (Property<T> prop) noexcept
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

  void
  freeze_notify () noexcept
  {
    ::GObject *object = reinterpret_cast<::GObject *> (this);
    g_object_freeze_notify (object);
  }

  void
  thaw_notify () noexcept
  {
    ::GObject *object = reinterpret_cast<::GObject *> (this);
    g_object_thaw_notify (object);
  }

  static void
  _peel_chain_finalize (::GObject *obj) noexcept
  {
    reinterpret_cast<::GObjectClass *> (TypeClass::peek_static<Object> ())->finalize (obj);
  }

  template<typename DerivedClass>
  void
  parent_vfunc_dispose () noexcept
  {
    ::GObjectClass *klass = reinterpret_cast<::GObjectClass *> (TypeClass::peek<DerivedClass> ()->peek_parent ());
    ::GObject *self = reinterpret_cast<::GObject *> (this);
    klass->dispose (self);
  }

  template<typename DerivedClass>
  static RefPtr<Object>
  parent_vfunc_constructor (Type type, ArrayRef<ConstructParam> params) noexcept
  {
    ::GObjectClass *klass = reinterpret_cast<::GObjectClass *> (TypeClass::peek<DerivedClass> ()->peek_parent ());
    ::GObjectConstructParam *peel_params = reinterpret_cast<::GObjectConstructParam *> (params.data ());
    ::GObject *obj = klass->constructor (type, params.size (), peel_params);
    return RefPtr<Object>::adopt_ref (reinterpret_cast<Object *> (obj));
  }

  template<typename DerivedClass>
  void
  parent_vfunc_constructed () noexcept
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
  static peel::enable_if_derived<Object, Class, typename Traits<Class>::CreateType>
  create (Args &&...args) noexcept
  {
    static_assert (sizeof... (Args) % 2 == 0,
                   "Must pass property / value pairs");

    ::GType type = static_cast<::GType> (Type::of<Class> ());
    ::GObject *obj = PropsCollector<Args...>::create (type, std::forward<Args> (args)...);
    return Traits<Class>::created (obj);
  }

  static RefPtr<Object>
  create (Type tp, const char *first_prop_name, ...) noexcept
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

  void
  get_property (const char *name, Value *value) noexcept
  {
    ::GObject *object = reinterpret_cast<::GObject *> (this);
    ::GValue *_peel_value = reinterpret_cast<::GValue *> (value);
    g_object_get_property (object, name, _peel_value);
  }

  template<typename T>
  typename GetPropertyHelper<T>::GetType
  get_property (Property<T> prop) noexcept
  {
    Value value { Type::of<T> () };
    get_property (prop.name, &value);
    return GetPropertyHelper<T>::get (&value);
  }

  template<typename T>
  typename GetPropertyHelper<T>::GetType
  get_property (const char *name) noexcept
  {
    Type tp;

    if (std::is_same<T, Enum>::value || std::is_same<T, Flags>::value)
      {
        ::GParamSpec *pspec = g_object_class_find_property (G_OBJECT_GET_CLASS (this), name);
        if (pspec)
          tp = static_cast<Type> (pspec->value_type);
        else
          {
            if (std::is_same<T, Enum>::value)
              tp = G_TYPE_ENUM;
            else if (std::is_same<T, Flags>::value)
              tp = G_TYPE_FLAGS;
          }
      }
    else
      tp = Type::of<T> ();

    Value value { tp };
    get_property (name, &value);
    return GetPropertyHelper<T>::get (&value);
  }

  void
  set_property (const char *name, const Value *value) noexcept
  {
    ::GObject *object = reinterpret_cast<::GObject *> (this);
    const ::GValue *_peel_value = reinterpret_cast<const ::GValue *> (value);
    g_object_set_property (object, name, _peel_value);
  }

  template<typename T, typename U>
  void
  set_property (Property<T> prop, U &&value) noexcept
  {
    Value v { Type::of<T> () };
    v.set<T> (std::forward<U> (value));
    set_property (prop.name, &v);
  }

  template<typename T, typename U>
  void
  set_property (const char *name, U &&value) noexcept
  {
    Type tp;

    if (std::is_same<T, Enum>::value || std::is_same<T, Flags>::value)
      {
        ::GParamSpec *pspec = g_object_class_find_property (G_OBJECT_GET_CLASS (this), name);
        if (pspec)
          tp = static_cast<Type> (pspec->value_type);
        else
          {
            if (std::is_same<T, Enum>::value)
              tp = G_TYPE_ENUM;
            else if (std::is_same<T, Flags>::value)
              tp = G_TYPE_FLAGS;
          }
      }
    else
      tp = Type::of<T> ();

    Value v { tp };
    v.set<T> (std::forward<U> (value));
    set_property (name, &v);
  }

  // TODO: variadic get/set

  template<typename T1, typename T2, typename TransformTo = decltype (nullptr), typename TransformFrom = decltype (nullptr)>
  static Binding *
  bind_property (
    Object *source, Property<T1> source_property,
    Object *target, Property<T2> target_property,
    BindingFlags flags = BindingFlags (0),
    TransformTo &&transform_to = nullptr,
    TransformFrom &&transform_from = nullptr) noexcept
  {
    ::GBindingTransformFunc _peel_transform_to;
    ::GBindingTransformFunc _peel_transform_from;
    gpointer _peel_user_data;
    ::GDestroyNotify _peel_notify;

    typedef typename std::remove_reference<TransformTo>::type CTo;
    typedef typename std::remove_reference<TransformFrom>::type CFrom;
    size_t c_to_size = is_effectively_empty<CTo> () ? 0 : sizeof (CTo);
    size_t c_from_size = is_effectively_empty<CFrom> () ? 0 : sizeof (CFrom);
    if (c_to_size + c_from_size <= sizeof (gpointer))
      {
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif
        union U
        {
          CTo transform_to;
          CFrom transform_from;
          gpointer data;
          U (gpointer data) : data (data) { }
          ~U () { }
        } u { nullptr };
        union U2
        {
          CFrom transform_from;
          unsigned char storage[sizeof (CFrom)];
          U2 () : storage { 0 } { }
          ~U2 () { }
        } u2;
        if (is_effectively_empty<CTo> ())
          {
            new (&u.transform_to) CTo (static_cast<TransformTo &&> (transform_to));
            new (&u.transform_from) CFrom (static_cast<TransformFrom &&> (transform_from));
          }
        else if (is_effectively_empty<CFrom> ())
          {
            new (&u.transform_from) CFrom (static_cast<TransformFrom &&> (transform_from));
            new (&u.transform_to) CTo (static_cast<TransformTo &&> (transform_to));
          }
        else
          {
            new (&u.transform_to) CTo (static_cast<TransformTo &&> (transform_to));
            new (&u2.transform_from) CFrom (static_cast<TransformFrom &&> (transform_from));
            memcpy (&u.transform_to + 1, &u2.transform_from, sizeof (CFrom));
          }
        _peel_user_data = u.data;
        if (std::is_trivially_destructible<CTo>::value && std::is_trivially_destructible<CFrom>::value)
          _peel_notify = nullptr;
        else
          _peel_notify = +[] (gpointer data)
          {
            U u { data };
            if (is_effectively_empty<CTo> () || is_effectively_empty<CFrom> ())
              {
                u.transform_from.~CFrom ();
                u.transform_to.~CTo ();
              }
            else
              {
                U2 u2;
                memcpy (&u2.transform_from, &u.transform_to + 1, sizeof (CFrom));
                u.transform_to.~CTo ();
                u2.transform_from.~CFrom ();
              }
          };
        _peel_transform_to = std::is_same<CTo, decltype (nullptr)>::value ? nullptr : +[] (::GBinding *binding, const ::GValue *from_value, ::GValue *to_value, gpointer data) -> gboolean
        {
          Binding *_peel_binding = reinterpret_cast<Binding *> (binding);
          const Value *_peel_from_value = reinterpret_cast<const Value *> (from_value);
          Value *_peel_to_value = reinterpret_cast<Value *> (to_value);
          U u { data };
          bool transformed = BindingTransformHelper<T1, T2, CTo>::transform (u.transform_to, _peel_binding, _peel_from_value, _peel_to_value);
          return static_cast<gboolean> (transformed);
        };
        _peel_transform_from = std::is_same<CFrom, decltype (nullptr)>::value ? nullptr : +[] (::GBinding *binding, const ::GValue *from_value, ::GValue *to_value, gpointer data) -> gboolean
        {
          Binding *_peel_binding = reinterpret_cast<Binding *> (binding);
          const Value *_peel_from_value = reinterpret_cast<const Value *> (from_value);
          Value *_peel_to_value = reinterpret_cast<Value *> (to_value);
          U u { data };
          U2 u2;
          CFrom *transform_from;
          if (is_effectively_empty<CTo> () || is_effectively_empty<CFrom> ())
            transform_from = &u.transform_from;
          else
            {
              memcpy (&u2.transform_from, &u.transform_to + 1, sizeof (CFrom));
              transform_from = &u2.transform_from;
            }
          bool transformed = BindingTransformHelper<T2, T1, CFrom>::transform (*transform_from, _peel_binding, _peel_from_value, _peel_to_value);
          return static_cast<gboolean> (transformed);
        };
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
      }
    else
      {
        struct TransformClosure
        {
          CTo transform_to;
          CFrom transform_from;
        };
        TransformClosure *closure = reinterpret_cast<TransformClosure *> (g_malloc (sizeof (TransformClosure)));
        new (closure) TransformClosure
        {
          static_cast<TransformTo &&> (transform_to),
          static_cast<TransformFrom &&> (transform_from)
        };
        _peel_user_data = closure;
        _peel_notify = +[] (gpointer data)
        {
          peel_assume (data);
          reinterpret_cast<TransformClosure *> (data)->~TransformClosure ();
#if GLIB_CHECK_VERSION (2, 76, 0)
          g_free_sized (data, sizeof (TransformClosure));
#else
          g_free (data);
#endif
        };
        _peel_transform_to = std::is_same<CTo, decltype (nullptr)>::value ? nullptr : +[] (::GBinding *binding, const ::GValue *from_value, ::GValue *to_value, gpointer data) -> gboolean
        {
          Binding *_peel_binding = reinterpret_cast<Binding *> (binding);
          const Value *_peel_from_value = reinterpret_cast<const Value *> (from_value);
          Value *_peel_to_value = reinterpret_cast<Value *> (to_value);
          TransformClosure *closure = reinterpret_cast<TransformClosure *> (data);
          bool transformed = BindingTransformHelper<T1, T2, CTo>::transform (closure->transform_to, _peel_binding, _peel_from_value, _peel_to_value);
          return static_cast<gboolean> (transformed);
        };
        _peel_transform_from = std::is_same<CFrom, decltype (nullptr)>::value ? nullptr : +[] (::GBinding *binding, const ::GValue *from_value, ::GValue *to_value, gpointer data) -> gboolean
        {
          Binding *_peel_binding = reinterpret_cast<Binding *> (binding);
          const Value *_peel_from_value = reinterpret_cast<const Value *> (from_value);
          Value *_peel_to_value = reinterpret_cast<Value *> (to_value);
          TransformClosure *closure = reinterpret_cast<TransformClosure *> (data);
          bool transformed = BindingTransformHelper<T2, T1, CFrom>::transform (closure->transform_from, _peel_binding, _peel_from_value, _peel_to_value);
          return static_cast<gboolean> (transformed);
        };
      }

    ::GBinding *raw_binding = g_object_bind_property_full (
      source, source_property.name,
      target, target_property.name,
      static_cast<::GBindingFlags> (flags),
      _peel_transform_to,
      _peel_transform_from,
      _peel_user_data,
      _peel_notify);
    return reinterpret_cast<Binding *> (raw_binding);
  }

  void
  run_dispose () noexcept
  {
    g_object_run_dispose (reinterpret_cast<::GObject *> (this));
  }

  template<typename T, typename Handler>
  SignalConnection::Token
  connect_notify (Property<T> prop, Handler &&handler, bool after = false) noexcept
  {
    Signal<Object, void (ParamSpec *)> notify_signal = Signal<Object, void (ParamSpec *)>::lookup ("notify");
    ::GQuark quark = g_quark_from_string (prop.name);
    return notify_signal.template connect<Handler> (this, quark, static_cast<Handler &&> (handler), after);
  }

  template<typename Handler>
  SignalConnection::Token
  connect_notify (Handler &&handler, bool after = false) noexcept
  {
    Signal<Object, void (ParamSpec *)> notify_signal = Signal<Object, void (ParamSpec *)>::lookup ("notify");
    return notify_signal.template connect<Handler> (this, 0, static_cast<Handler &&> (handler), after);
  }

  template<typename T, typename HandlerObject>
  SignalConnection::Token
  connect_notify
  (
    Property<T> prop,
    HandlerObject *object,
    void (HandlerObject::*handler_method) (Object *, ParamSpec *),
    bool after = false
  ) noexcept
  {
    Signal<Object, void (ParamSpec *)> notify_signal = Signal<Object, void (ParamSpec *)>::lookup ("notify");
    ::GQuark quark = g_quark_from_string (prop.name);
    return notify_signal.template connect<HandlerObject> (this, quark, object, handler_method, after);
  }

  template<typename HandlerObject>
  SignalConnection::Token
  connect_notify
  (
    HandlerObject *object,
    void (HandlerObject::*handler_method) (Object *, ParamSpec *),
    bool after = false
  ) noexcept
  {
    Signal<Object, void (ParamSpec *)> notify_signal = Signal<Object, void (ParamSpec *)>::lookup ("notify");
    return notify_signal.template connect<HandlerObject> (this, 0, object, handler_method, after);
  }

  template<typename T>
  void
  add_weak_pointer (T **weak_pointer_location) noexcept
  {
    ::GObject *obj = reinterpret_cast<::GObject *> (this);
    gpointer *ptr = reinterpret_cast<gpointer *> (weak_pointer_location);
    g_object_add_weak_pointer (obj, ptr);
  }

  template<typename T>
  void
  remove_weak_pointer (T **weak_pointer_location) noexcept
  {
    ::GObject *obj = reinterpret_cast<::GObject *> (this);
    gpointer *ptr = reinterpret_cast<gpointer *> (weak_pointer_location);
    g_object_remove_weak_pointer (obj, ptr);
  }

  // TODO: the generic add_weak_ref () API

  void *
  get_data (const char *key) noexcept
  {
    ::GObject *obj = reinterpret_cast<::GObject *> (this);
    return g_object_get_data (obj, key);
  }

  void *
  get_data (::GQuark quark) noexcept
  {
    ::GObject *obj = reinterpret_cast<::GObject *> (this);
    return g_object_get_qdata (obj, quark);
  }

  void
  set_data (const char *key, void *data, ::GDestroyNotify destroy_notify = nullptr) noexcept
  {
    ::GObject *obj = reinterpret_cast<::GObject *> (this);
    g_object_set_data_full (obj, key, data, destroy_notify);
  }

  void
  set_data (::GQuark quark, void *data, ::GDestroyNotify destroy_notify = nullptr) noexcept
  {
    ::GObject *obj = reinterpret_cast<::GObject *> (this);
    g_object_set_qdata_full (obj, quark, data, destroy_notify);
  }

  void *
  steal_data (const char *key) noexcept
  {
    ::GObject *obj = reinterpret_cast<::GObject *> (this);
    return g_object_steal_data (obj, key);
  }

  void *
  steal_data (::GQuark quark) noexcept
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
    ParamSpec *
    find_property (const char *property_name) noexcept
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
    void
    install_property (unsigned id, FloatPtr<ParamSpec> &&pspec) noexcept
    {
      // The argument must be of type FloatPtr && (as opposed to a by-value
      // FloatPtr) to prevent trying to instantiate RefTraits<ParamSpec>
      // for the forward-declared ParamSpec type.
      ::GObjectClass *klass = reinterpret_cast<::GObjectClass *> (this);
      ::GParamSpec *p = reinterpret_cast<::GParamSpec *> (std::move (pspec).release_floating_ptr ());
      g_object_class_install_property (klass, id, p);
    }

    template<typename DerivedClass>
    void
    override_vfunc_constructor ()
    {
      ::GObjectClass *klass = reinterpret_cast<::GObjectClass *> (this);
      klass->constructor = +[] (::GType tp, guint n, ::GObjectConstructParam *raw_params) -> ::GObject *
      {
        Type type = tp;
        ConstructParam *params = reinterpret_cast<ConstructParam *> (raw_params);
        RefPtr<Object> obj = DerivedClass::vfunc_constructor (type, ArrayRef<ConstructParam> { params, n });
        return reinterpret_cast<::GObject *> (std::move (obj).release_ref ());
      };
    }

    template<typename DerivedClass>
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

  G_GNUC_CONST
  static T *
  get (const ::GValue *value) noexcept
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

  static RefPtr<T>
  dup (const ::GValue *value) noexcept
  {
    void *obj = g_value_dup_object (value);
    if (std::is_same<T, Object>::value)
      return RefPtr<T>::adopt_ref (reinterpret_cast<T *> (obj));
#if defined (G_DISABLE_CAST_CHECKS) || defined (__OPTIMIZE__)
    return RefPtr<T>::adopt_ref (reinterpret_cast<T *> (obj));
#else
    ::GType tp = static_cast<::GType> (Type::of<T> ());
    return RefPtr<T>::adopt_ref (G_TYPE_CHECK_INSTANCE_CAST (obj, tp, T));
#endif
  }

  static void
  set (::GValue *value, T *object) noexcept
  {
    g_value_set_object (value, object);
  }

  static void
  take (::GValue *value, RefPtr<T> &&object) noexcept
  {
    g_value_take_object (value, std::move (object).release_ref ());
  }

  static void
  set_sink (::GValue *value, FloatPtr<T> &&object) noexcept
  {
    T *obj = std::move (object).release_floating_ptr ();
    g_value_take_object (value, g_object_ref_sink (obj));
  }

  static void
  set_marshal_return (::GValue *value, RefPtr<T> &&object) noexcept
  {
    g_value_take_object (value, std::move (object).release_ref ());
  }

  static void
  set_marshal_return (::GValue *value, T *object) noexcept
  {
    /* Pretend to have a reference.  */
    g_value_take_object (value, object);
  }

  static ::GObject *
  cast_for_create (T *object) noexcept
  {
    return reinterpret_cast<::GObject *> (object);
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
  collect (const char **names, ::GValue *values, Property<T> prop, U &&prop_value, Args... args) noexcept
  {
    names[0] = prop.get_name ();
    g_value_init (&values[0], static_cast<::GType> (Type::of<T> ()));
    Value *value = reinterpret_cast<Value *> (&values[0]);
    value->set<T> (std::forward<U> (prop_value));

    Object::PropsCollector<Args...>::collect (names + 1, values + 1, std::forward<Args> (args)...);
  }

  static ::GObject *
  create (::GType type, Property<T> prop, U &&prop_value, Args... args) noexcept
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

template<typename T1, typename T2, typename C, typename>
struct Object::BindingTransformHelper
{
  static bool
  transform (C &callback, Binding *binding, const Value *from_value, Value *to_value)
  {
    /* The callback accepts the binding, plus the two GValues. */
    return callback (binding, from_value, to_value);
  }
};

template<typename T1, typename T2, typename C>
struct Object::BindingTransformHelper<T1, T2, C, decltype (std::declval<Value> ().set<T2> (std::declval<C> () (std::declval<typename Value::Traits<T1>::UnownedType> ())))>
{
  static bool
  transform (C &callback, Binding *binding, const Value *from_value, Value *to_value)
  {
    /* The callback accepts the unpacked "from value", and returns the unpacked "to value". */
    (void) binding;
    to_value->set<T2> (callback (from_value->get<T1> ()));
    return true;
  }
};

template<typename T1, typename T2>
struct Object::BindingTransformHelper<T1, T2, decltype (nullptr), void>
{
  static bool
  transform (...)
  {
    peel_unreachable;
    return false;
  }
};

} /* namespace GObject */

using GObject::Object;

// #define PEEL_USE_COMPAT_CONTROL_HACK

#ifdef PEEL_USE_COMPAT_CONTROL_HACK
/* Note: this function is very intentionally marked inline,
   but not static inline. */
peel_always_inline
void
peel_sink_gobject_if_floating (void *object) noexcept
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

  static constexpr
  bool can_ref_null = false;

  static void
  unref (T *obj)
  {
    g_object_unref (obj);
  }

  static constexpr
  bool can_unref_null = false;

  static void
  ref_sink (T *obj)
  {
    void *r = g_object_ref_sink (obj);
    peel_assume (r == obj);
  }

  peel_always_inline
  static void
  sink (T *obj)
  {
#ifdef PEEL_USE_COMPAT_CONTROL_HACK
    peel_sink_gobject_if_floating (obj);
#else
    g_object_ref_sink (obj);
    g_object_unref (obj);
#endif
  }
};

namespace internals
{
template<typename...>
struct ObjectCreateHelper;

template<>
struct ObjectCreateHelper<>
{
  template<typename Ret, typename CCalleeType, typename... UnpackedArgs>
  peel_always_inline
  static Ret
  invoke (CCalleeType c_callee, UnpackedArgs... unpacked_args) noexcept
  {
    return c_callee (unpacked_args..., nullptr);
  }
};

template<typename T, typename U, typename... Args>
struct ObjectCreateHelper<Property<T>, U, Args...>
{
  template<typename Ret, typename CCalleeType, typename... UnpackedArgs>
  peel_always_inline
  static Ret
  invoke (CCalleeType c_callee, UnpackedArgs... unpacked_args, Property<T> prop, U &&value, Args &&...args) noexcept
  {
    typedef decltype (GObject::Value::Traits<T>::cast_for_create (std::forward<U> (value))) CArg;
    return ObjectCreateHelper<Args...>::template invoke<
      Ret, CCalleeType, UnpackedArgs..., const char *, CArg> (
        c_callee,
        unpacked_args..., prop.get_name (),
        GObject::Value::Traits<T>::cast_for_create (std::forward<U> (value)),
        std::forward<Args> (args)...);
  }
};

} /* namespace internals */

} /* namespace peel */

peel_end_header
