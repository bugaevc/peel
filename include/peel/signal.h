#pragma once

#include <peel/GObject/Type.h>
#include <peel/GObject/TypeInstance.h>
#include <peel/GObject/Value.h>
#include <peel/RefPtr.h>
#include <peel/UniquePtr.h>
#include <peel/lang.h>
#include <new>
#include <type_traits>
#include <glib-object.h>

namespace peel
{
namespace internals
{

peel_nothrow G_GNUC_CONST
static inline gpointer
value_peek_pointer (const ::GValue *value)
{
  return g_value_peek_pointer (value);
}

template<typename T, typename = void>
struct SignalTraits
{
  typedef T CType;
  typedef T PlainCppType;

  static T
  to_c (T value)
  {
    return value;
  }

  static T
  from_c (T value)
  {
    return value;
  }
};

template<>
struct SignalTraits<void>
{
  typedef void CType;
  typedef void PlainCppType;
};

template<>
struct SignalTraits<bool>
{
  typedef gboolean CType;
  typedef bool PlainCppType;

  static gboolean
  to_c (bool b)
  {
    return b;
  }

  static bool
  from_c (gboolean b)
  {
    return !!b;
  }
};

template<>
struct SignalTraits<const char *>
{
  typedef const char *CType;
  typedef const char *PlainCppType;

  static const char *
  to_c (const char *s)
  {
    return s;
  }

  static const char *
  from_c (const char *s)
  {
    return s;
  }
};

template<>
struct SignalTraits<char *>
{
  typedef char *CType;
  typedef char *PlainCppType;

  static char *
  to_c (char *s)
  {
    return s;
  }

  static char *
  from_c (char *s)
  {
    return s;
  }
};

template<>
struct SignalTraits<GObject::Type>
{
  typedef ::GType CType;
  typedef GObject::Type PlainCppType;

  static ::GType
  to_c (GObject::Type tp)
  {
    return static_cast<::GType> (tp);
  }

  static GObject::Type
  from_c (::GType tp)
  {
    return tp;
  }
};

template<typename T>
struct SignalTraits<RefPtr<T>>
{
  typedef T *CType;
  typedef T PlainCppType;

  // No conversion to C, as this should never be needed.

  static RefPtr<T>
  from_c (T *ptr)
  {
    return RefPtr<T>::adopt_ref (ptr);
  }
};

template<typename T>
struct SignalTraits<UniquePtr<T>>
{
  typedef T *CType;
  typedef T PlainCppType;

  // No conversion to C, as this should never be needed.

  static UniquePtr<T>
  from_c (T *ptr)
  {
    return UniquePtr<T>::adopt_ref (ptr);
  }
};

template<typename T>
struct SignalTraits<T * /*, peel::enable_if_derived<GObject::TypeInstance, T, void> */>
{
  typedef T *CType;
  typedef T PlainCppType;

  static T *
  to_c (T *obj)
  {
    return obj;
  }

  static T *
  from_c (T *obj)
  {
    return obj;
  }
};

template<typename Ret, typename... Args>
struct SignalHelper1;
template<typename Ret, typename... Args>
struct SignalHelper2;

template<typename Ret, typename Arg, typename... Args>
struct SignalHelper1<Ret, Arg, Args...>
{
  typedef typename SignalTraits<Arg>::CType ArgCType;
  typedef typename SignalTraits<Arg>::PlainCppType ArgPlainCppType;

  template<typename... UnpackedArgs>
  peel_nothrow peel_always_inline
  static void
  marshal (gpointer data1, gpointer data2, void *callback, ::GValue *return_value, const ::GValue *param_values, UnpackedArgs... args)
  {
    Arg arg_cpp = GObject::Value::Traits<ArgPlainCppType>::get (param_values);
    ArgCType arg_c = SignalTraits<Arg>::to_c (arg_cpp);
    SignalHelper1<Ret, Args...>::template marshal<UnpackedArgs..., ArgCType> (data1, data2, callback, return_value, param_values + 1, args..., arg_c);
  }

  template<typename... UnpackedArgs>
  peel_nothrow peel_always_inline
  static void
  marshal_va (gpointer data1, gpointer data2, void *callback, ::GValue *return_value, va_list va_args, UnpackedArgs... args)
  {
    ArgCType arg = va_arg (va_args, ArgCType);
    SignalHelper1<Ret, Args...>::template marshal_va<UnpackedArgs..., ArgCType> (data1, data2, callback, return_value, va_args, args..., arg);
  }

  template<typename Instance, typename Callback, typename... UnpackedArgs>
  peel_nothrow peel_always_inline
  static void
  marshal_cpp (Instance *instance, Callback &callback, ::GValue *return_value, const ::GValue *param_values, UnpackedArgs... args)
  {
    Arg arg = GObject::Value::Traits<ArgPlainCppType>::get (param_values);
    SignalHelper1<Ret, Args...>::template marshal_cpp<Instance, Callback, UnpackedArgs..., Arg> (instance, callback, return_value, param_values + 1, args..., arg);
  }

  template<typename Instance, typename Callback, typename... UnpackedArgs>
  peel_nothrow peel_always_inline
  static void
  marshal_va_cpp (Instance *instance, Callback &callback, ::GValue *return_value, va_list va_args, UnpackedArgs... args)
  {
    ArgCType arg_c = va_arg (va_args, ArgCType);
    Arg arg = SignalTraits<Arg>::from_c (arg_c);
    SignalHelper1<Ret, Args...>::template marshal_va_cpp<Instance, Callback, UnpackedArgs..., Arg> (instance, callback, return_value, va_args, args..., arg);
  }
};

template<typename Ret>
struct SignalHelper1<Ret>
{
  template<typename... UnpackedArgs>
  peel_nothrow peel_always_inline
  static void
  marshal (gpointer data1, gpointer data2, void *callback, ::GValue *return_value, const ::GValue *param_values, UnpackedArgs... args)
  {
    (void) param_values;
    SignalHelper2<Ret, UnpackedArgs...>::marshal (data1, data2, callback, return_value, args...);
  }

  template<typename... UnpackedArgs>
  peel_nothrow peel_always_inline
  static void
  marshal_va (gpointer data1, gpointer data2, void *callback, ::GValue *return_value, va_list va_args, UnpackedArgs... args)
  {
    (void) va_args;
    SignalHelper2<Ret, UnpackedArgs...>::marshal (data1, data2, callback, return_value, args...);
  }

  template<typename Instance, typename Callback, typename... UnpackedArgs>
  peel_nothrow peel_always_inline
  static void
  marshal_cpp (Instance *instance, Callback &callback, ::GValue *return_value, const ::GValue *param_values, UnpackedArgs... args)
  {
    (void) param_values;
    SignalHelper2<Ret, UnpackedArgs...>::template marshal_cpp<Instance, Callback> (instance, callback, return_value, args...);
  }

  template<typename Instance, typename Callback, typename... UnpackedArgs>
  peel_nothrow peel_always_inline
  static void
  marshal_va_cpp (Instance *instance, Callback &callback, ::GValue *return_value, va_list va_args, UnpackedArgs... args)
  {
    (void) va_args;
    SignalHelper2<Ret, UnpackedArgs...>::template marshal_cpp<Instance, Callback> (instance, callback, return_value, args...);
  }
};

template<typename Ret, typename... Args>
struct SignalHelper2
{
  peel_nothrow peel_always_inline
  static Ret
  emit (void *instance, gint id, ::GQuark detail, Args... args)
  {
    typename SignalTraits<Ret>::CType ret_c = 0;
    g_signal_emit (instance, id, detail, SignalTraits<Args>::to_c (args)..., &ret_c);
    return SignalTraits<Ret>::from_c (ret_c);
  }

  peel_nothrow peel_always_inline
  static void
  marshal (gpointer data1, gpointer data2, void *callback, ::GValue *return_value, Args... args)
  {
    typedef typename SignalTraits<Ret>::CType (*CallbackType) (gpointer, Args..., gpointer);
    CallbackType cb = reinterpret_cast<CallbackType> (callback);
    typename SignalTraits<Ret>::CType ret = cb (data1, args..., data2);
    typedef typename SignalTraits<Ret>::PlainCppType RetPlainCppType;
    reinterpret_cast<GObject::Value *> (return_value)->set<RetPlainCppType> (SignalTraits<Ret>::from_c (ret));
  }

  template<typename Instance, typename Callback>
  peel_nothrow peel_always_inline
  static void
  marshal_cpp (Instance *instance, Callback &callback, ::GValue *return_value, Args... args)
  {
    Ret ret = callback (instance, args...);
    typedef typename SignalTraits<Ret>::PlainCppType RetPlainCppType;
    reinterpret_cast<GObject::Value *> (return_value)->set<RetPlainCppType> (static_cast<Ret &&> (ret));
  }
};

template<typename... Args>
struct SignalHelper2<void, Args...>
{
  peel_nothrow peel_always_inline
  static void
  emit (void *instance, gint id, ::GQuark detail, Args... args)
  {
    g_signal_emit (instance, id, detail, SignalTraits<Args>::to_c (args)...);
  }

  peel_nothrow peel_always_inline
  static void
  marshal (gpointer data1, gpointer data2, void *callback, ::GValue *return_value, Args... args)
  {
    (void) return_value;
    typedef void (*CallbackType) (gpointer, Args..., gpointer);
    CallbackType cb = reinterpret_cast<CallbackType> (callback);
    cb (data1, args..., data2);
  }

  template<typename Instance, typename Callback>
  peel_nothrow peel_always_inline
  static void
  marshal_cpp (Instance *instance, Callback &callback, ::GValue *return_value, Args... args)
  {
    (void) return_value;
    callback (instance, args...);
  }
};

template<typename Callback>
struct ClosureStoringCallback : public ::GClosure
{
  Callback storage;
};

template<typename Callback>
using ClosureStoringOptionalCallback = typename std::conditional<
  (sizeof (Callback) > sizeof (gpointer)),
  ClosureStoringCallback<Callback>,
  ::GClosure
>::type;

static void
closure_set_va_marshal (::GClosure *closure, ::GVaClosureMarshal marshal)
{
  // HACK: This is the private _GRealClosure / _g_closure_set_va_marshal API.
  struct RealClosure
  {
    gpointer stuff[3];
    ::GVaClosureMarshal va_marshal;
    ::GClosure closure;
  };

  unsigned char *ptr = reinterpret_cast<unsigned char *> (closure);
  ptr -= offsetof (RealClosure, closure);
  RealClosure *real_closure = reinterpret_cast<RealClosure *> (ptr);
  real_closure->va_marshal = marshal;
}

template<typename Instance, typename Callback, typename Ret, typename... Args>
class SignalClosure : public ClosureStoringOptionalCallback<Callback>
{
private:
  Callback *
  callback_ptr ()
  {
    if (sizeof (Callback) > sizeof (gpointer))
      return &reinterpret_cast<ClosureStoringCallback<Callback> *> (this)->storage;
    return reinterpret_cast<Callback *> (&reinterpret_cast<::GClosure *> (this)->data);
  }

  peel_nothrow
  static void
  notify_func (gpointer unused, GClosure *g_closure)
  {
    (void) unused;
    SignalClosure *closure = reinterpret_cast<SignalClosure *> (g_closure);
    closure->callback_ptr ()->~Callback ();
  }

  peel_nothrow
  static void
  marshal (::GClosure *closure, ::GValue *return_value, guint n_param_values, const ::GValue *param_values, gpointer invocation_hint, gpointer marshal_data)
  {
    g_return_if_fail (n_param_values == sizeof... (Args) + 1);
    (void) n_param_values;
    (void) param_values;
    (void) invocation_hint;
    (void) marshal_data;

    Instance *instance = reinterpret_cast<Instance *> (value_peek_pointer (param_values));
    Callback &callback = *reinterpret_cast<SignalClosure *> (closure)->callback_ptr ();
    SignalHelper1<Ret, Args...>::template marshal_cpp<Instance, Callback> (instance, callback, return_value, param_values + 1);
  }

  peel_nothrow
  static void
  marshal_va (::GClosure *closure, ::GValue *return_value, gpointer c_instance, va_list args, gpointer marshal_data, int n_params, ::GType *param_types)
  {
    g_return_if_fail (n_params == sizeof... (Args));
    (void) n_params;
    (void) param_types;
    (void) marshal_data;

    Instance *instance = reinterpret_cast<Instance *> (c_instance);
    Callback &callback = *reinterpret_cast<SignalClosure *> (closure)->callback_ptr ();

    va_list args_copy;
    if (!std::is_void<Ret>::value)
      va_copy (args_copy, args);
    SignalHelper1<Ret, Args...>::template marshal_va_cpp<Instance, Callback> (
      instance, callback, return_value,
      std::is_void<Ret>::value ? args : args_copy);
    if (!std::is_void<Ret>::value)
      va_end (args_copy);
  }

public:
  peel_nothrow
  static SignalClosure *
  make (Callback &&callback)
  {
    ::GClosure *g_closure = g_closure_new_simple (sizeof (SignalClosure), nullptr);
    SignalClosure *closure = reinterpret_cast<SignalClosure *> (g_closure);
    new (closure->callback_ptr ()) Callback (static_cast<Callback &&> (callback));

    g_closure_set_marshal (g_closure, marshal);
    // _g_closure_set_va_marshal (g_closure, marshal_va);
    closure_set_va_marshal (g_closure, marshal_va);

    if (!std::is_trivially_destructible<Callback>::value)
      g_closure_add_finalize_notifier (g_closure, nullptr, notify_func);

    return closure;
  }
};

} /* namespace internals */

class peel_nodiscard ("immediately unblocks if leaked")
SignalBlockGuard
{
  friend class SignalConnection;

private:
  gpointer instance;
  gulong id;

public:
  peel_nothrow
  void
  unblock ()
  {
    g_signal_handler_unblock (instance, id);
    id = 0;
  }

  ~SignalBlockGuard ()
  {
    if (id > 0)
      unblock ();
  }

  SignalBlockGuard (gpointer instance, gulong id)
    : instance (instance)
    , id (id)
  { }

  SignalBlockGuard (const SignalBlockGuard &other) = delete;

  SignalBlockGuard (SignalBlockGuard &&other)
    : instance (other.instance)
    , id (other.id)
  {
    other.id = 0;
  }

  SignalBlockGuard &
  operator = (const SignalBlockGuard &other) = delete;

  SignalBlockGuard &
  operator = (SignalBlockGuard &&other)
  {
    if (this == &other)
      return *this;
    if (id > 0)
      unblock ();
    instance = other.instance;
    id = other.id;
    other.id = 0;
    return *this;
  }
};

class peel_nodiscard ("immediately disconnects if leaked")
SignalConnection
{
private:
  gpointer instance;
  gulong id;

public:
  struct Token
  {
    gpointer instance;
    gulong id;
  };

  void
  leave_connected_forever () &&
  {
    id = 0;
  }

  peel_nothrow
  void
  disconnect ()
  {
    if (id > 0)
      {
        g_signal_handler_disconnect (instance, id);
        id = 0;
      }
  }

  ~SignalConnection ()
  {
    disconnect ();
  }

  SignalConnection ()
    : instance (nullptr)
    , id (0)
  { }

  SignalConnection (gpointer instance, gulong id)
    : instance (instance)
    , id (id)
  { }

  SignalConnection (Token &&token)
    : instance (token.instance)
    , id (token.id)
  { }

  SignalConnection (const SignalConnection &other) = delete;

  SignalConnection (SignalConnection &&other)
    : instance (other.instance)
    , id (other.id)
  {
    other.id = 0;
  }

  SignalConnection &
  operator = (const SignalConnection &other) = delete;

  SignalConnection &
  operator = (SignalConnection &&other)
  {
    if (this == &other)
      return *this;
    disconnect ();
    instance = other.instance;
    id = other.id;
    other.id = 0;
    return *this;
  }

  peel_nothrow
  SignalBlockGuard
  block ()
  {
    g_signal_handler_block (instance, id);
    return SignalBlockGuard { instance, id };
  }

  explicit constexpr
  operator bool () const
  {
    return id > 0;
  }
};

template<typename Instance, typename F>
struct Signal;

template<typename Instance, typename Ret, typename... Args>
struct Signal<Instance, Ret (Args...)>
{
private:
  guint id;

  peel_nothrow
  static void
  marshal (::GClosure *closure, ::GValue *return_value, guint n_param_values, const ::GValue *param_values, gpointer invocation_hint, gpointer marshal_data)
  {
    ::GCClosure *cc = reinterpret_cast<::GCClosure *> (closure);
    gpointer data1, data2;

    g_return_if_fail (n_param_values == sizeof... (Args) + 1);
    (void) n_param_values;
    (void) invocation_hint;

    if (G_CCLOSURE_SWAP_DATA (closure))
      {
        data1 = closure->data;
        data2 = internals::value_peek_pointer (param_values);
      }
    else
      {
        data1 = internals::value_peek_pointer (param_values);
        data2 = closure->data;
      }

    void *cb = marshal_data ? marshal_data : cc->callback;
    internals::SignalHelper1<Ret, Args...>::template marshal<> (data1, data2, cb, return_value, param_values + 1);
  }

  peel_nothrow
  static void
  marshal_va (::GClosure *closure, ::GValue *return_value, gpointer instance, va_list args, gpointer marshal_data, int n_params, ::GType *param_types)
  {
    ::GCClosure *cc = reinterpret_cast<::GCClosure *> (closure);
    gpointer data1, data2;

    g_return_if_fail (n_params == sizeof... (Args));
    (void) n_params;
    (void) param_types;

    if (G_CCLOSURE_SWAP_DATA (closure))
      {
        data1 = closure->data;
        data2 = instance;
      }
    else
      {
        data1 = instance;
        data2 = closure->data;
      }

    va_list args_copy;
    if (!std::is_void<Ret>::value)
      va_copy (args_copy, args);
    void *cb = marshal_data ? marshal_data : cc->callback;
    internals::SignalHelper1<Ret, Args...>::template marshal_va<> (
      data1, data2, cb, return_value,
      std::is_void<Ret>::value ? args : args_copy);
    if (!std::is_void<Ret>::value)
      va_end (args_copy);
  }

public:
  peel_nothrow
  void
  register_ (const char *name, GSignalFlags flags /* TODO: class closure, accumulator */)
  {
    ::GType instance_type = GObject::Type::of<Instance> ();
    ::GType return_type = GObject::Type::of<typename internals::SignalTraits<Ret>::PlainCppType> ();
    ::GType param_types[] = { GObject::Type::of<typename internals::SignalTraits<Args>::PlainCppType> ()..., G_TYPE_INVALID };
    id = g_signal_newv (name, instance_type, flags,
                        nullptr, nullptr, nullptr,
                        marshal, return_type,
                        sizeof... (Args), param_types);
    g_signal_set_va_marshaller (id, instance_type, marshal_va);
  }

  peel_nothrow
  static Signal
  lookup (const char *name)
  {
    ::GType instance_type = GObject::Type::of<Instance> ();
    Signal sig;
    sig.id = g_signal_lookup (name, instance_type);
    return sig;
  }

  peel_nothrow
  Ret
  emit (Instance *instance, ::GQuark detail, Args... args)
  {
    return internals::SignalHelper2<Ret, Args...>::emit (instance, id, detail, args...);
  }

  peel_nothrow
  Ret
  emit (Instance *instance, Args... args)
  {
    return emit (instance, 0, args...);
  }

  template<typename Handler>
  peel_nothrow
  SignalConnection::Token
  connect (Instance *instance, ::GQuark detail, Handler handler, bool after = false)
  {
    typedef internals::SignalClosure<Instance, Handler, Ret, Args...> ClosureType;
    ClosureType *closure = ClosureType::make (static_cast<Handler &&> (handler));
    // Sinks the closure reference.
    gulong conn_id = g_signal_connect_closure_by_id (reinterpret_cast<::GObject *> (instance), id, detail, closure, after);
    return SignalConnection::Token { instance, conn_id };
  }

  template<typename HandlerObject>
  peel_nothrow
  SignalConnection::Token
  connect
  (
    Instance *instance,
    ::GQuark detail,
    HandlerObject *object,
    Ret (HandlerObject::*handler_method) (Instance *, Args...),
    bool after = false
  )
  {
    auto handler = [object, handler_method]
      (Instance *instance, Args... args) -> Ret
      {
        return (object->*handler_method) (instance, args...);
      };
    typedef internals::SignalClosure<Instance, decltype (handler), Ret, Args...> ClosureType;
    ClosureType *closure = ClosureType::make (std::move (handler));
    g_object_watch_closure (reinterpret_cast<::GObject *> (object), closure);
    // Sinks the closure reference.
    gulong conn_id = g_signal_connect_closure_by_id (reinterpret_cast<::GObject *> (instance), id, detail, closure, after);
    return SignalConnection::Token { instance, conn_id };
  }

  template<typename Handler>
  static peel_nothrow
  SignalConnection::Token
  _peel_connect_by_name (Instance *instance, const char *detailed_name, Handler handler, bool after = false)
  {
    typedef internals::SignalClosure<Instance, Handler, Ret, Args...> ClosureType;
    ClosureType *closure = ClosureType::make (static_cast<Handler &&> (handler));
    // Sinks the closure reference.
    gulong conn_id = g_signal_connect_closure (reinterpret_cast<::GObject *> (instance), detailed_name, closure, after);
    return SignalConnection::Token { instance, conn_id };
  }

  template<typename HandlerObject>
  static peel_nothrow
  SignalConnection::Token
  _peel_connect_by_name
  (
    Instance *instance,
    const char *detailed_name,
    HandlerObject *object,
    Ret (HandlerObject::*handler_method) (Instance *, Args...),
    bool after = false
  )
  {
    auto handler = [object, handler_method]
      (Instance *instance, Args... args) -> Ret
      {
        return (object->*handler_method) (instance, args...);
      };
    typedef internals::SignalClosure<Instance, decltype (handler), Ret, Args...> ClosureType;
    ClosureType *closure = ClosureType::make (std::move (handler));
    g_object_watch_closure (reinterpret_cast<::GObject *> (object), closure);
    // Sinks the closure reference.
    gulong conn_id = g_signal_connect_closure (reinterpret_cast<::GObject *> (instance), detailed_name, closure, after);
    return SignalConnection::Token { instance, conn_id };
  }
};

} /* namespace peel */

#define PEEL_SIGNAL_CONNECT_METHOD(signal_name, signal_obj)                    \
  template<typename _SignalHandler>                                            \
  peel_nothrow                                                                 \
  ::peel::SignalConnection::Token                                              \
  connect_ ## signal_name                                                      \
  (_SignalHandler signal_handler, ::GQuark detail = 0, bool after = false)     \
  {                                                                            \
    return (signal_obj).connect (this, detail,                                 \
      static_cast<_SignalHandler &&> (signal_handler), after);                 \
  }                                                                            \
                                                                               \
  template<typename _HandlerObject, typename _HandlerMethod>                   \
  peel_nothrow                                                                 \
  peel::enable_if_derived<                                                     \
    ::peel::GObject::Object,                                                   \
    _HandlerObject,                                                            \
    ::peel::SignalConnection::Token                                            \
  >                                                                            \
  connect_ ## signal_name                                                      \
  (                                                                            \
    _HandlerObject *object,                                                    \
    _HandlerMethod _HandlerObject::*handler_method,                            \
    ::GQuark detail = 0,                                                       \
    bool after = false                                                         \
  )                                                                            \
  {                                                                            \
    return (signal_obj).connect (this, detail, object, handler_method, after); \
  }                                                                            \
/* end of PEEL_SIGNAL_CONNECT_METHOD */
