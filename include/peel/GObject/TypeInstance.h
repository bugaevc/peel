#pragma once

#include <peel/GObject/Type.h>
#include <peel/lang.h>
#include <peel/signal.h>
#include <glib-object.h>

peel_begin_header

namespace peel
{
namespace GObject
{

class TypeClass;
class TypeInterface;

class TypeInstance
{
private:
  ::GTypeInstance ti peel_no_warn_unused;

  TypeInstance () = delete;
  TypeInstance (const TypeInstance &) = delete;
  TypeInstance (TypeInstance &&) = delete;

  TypeInstance &
  operator = (const TypeInstance &) = delete;
  TypeInstance &
  operator = (TypeInstance &&) = delete;

protected:
  ~TypeInstance () = default;

public:
  bool
  check_type (Type tp) const noexcept
  {
    return !!G_TYPE_CHECK_INSTANCE_TYPE (this, static_cast<::GType> (tp));
  }

  template<typename T>
  bool
  check_type () const
  {
    return check_type (Type::of<T> ());
  }

  bool
  check_fundamental_type (Type tp) const noexcept
  {
    return !!G_TYPE_CHECK_INSTANCE_FUNDAMENTAL_TYPE (this, static_cast<::GType> (tp));
  }

  template<typename T>
  bool
  check_fundamental_type () const
  {
    return check_fundamental_type (Type::of<T> ());
  }

  void *
  cast (Type tp) noexcept
  {
#if defined (G_DISABLE_CAST_CHECKS) || defined (__OPTIMIZE__)
    (void) tp;
    return this;
#else
    return G_TYPE_CHECK_INSTANCE_CAST (this, static_cast<::GType> (tp), void);
#endif
  }

  template<typename T>
  T *
  cast ()
  {
#if defined (G_DISABLE_CAST_CHECKS) || defined (__OPTIMIZE__)
    return reinterpret_cast<T *> (this);
#else
    return reinterpret_cast<T *> (cast (Type::of<T> ()));
#endif
  }

  TypeClass *
  get_class () const noexcept
  {
    return G_TYPE_INSTANCE_GET_CLASS (this, this doesnt matter, TypeClass);
  }

  template<typename T>
  typename T::Class *
  get_class () const noexcept
  {
    TypeClass *klass = get_class ();
#if defined (G_DISABLE_CAST_CHECKS) || defined (__OPTIMIZE__)
    return reinterpret_cast<typename T::Class *> (klass);
#else
    return G_TYPE_CHECK_CLASS_CAST (klass, Type::of<T> (), typename T::Class);
#endif
  }

  TypeInterface *
  get_interface (Type tp) const noexcept
  {
    return G_TYPE_INSTANCE_GET_INTERFACE (this, static_cast<::GType> (tp), TypeInterface);
  }

  template<typename I>
  typename I::Iface *
  get_interface () const noexcept
  {
    return reinterpret_cast<typename I::Iface *> (get_interface (Type::of<I> ()));
  }

  Type
  get_type () const noexcept
  {
    return G_TYPE_FROM_INSTANCE (this);
  }

  const char *
  get_type_name () const noexcept
  {
    ::GTypeInstance *ti = reinterpret_cast<::GTypeInstance *> (const_cast<TypeInstance *> (this));
    return g_type_name_from_instance (ti);
  }

  template<typename Handler>
  typename SignalConnection::Token
  connect_signal (const char *detailed_name, Handler &&handler, bool after = false) noexcept
  {
    typedef typename std::remove_reference<Handler>::type H;
    typedef typename peel::internals::DynamicSignalTypeHelper1<H>::SignalType SignalType;
    typedef typename peel::internals::DynamicSignalTypeHelper1<H>::InstanceType InstanceType;
    return SignalType::_peel_connect_by_name (this->cast<InstanceType> (), detailed_name, std::forward<Handler> (handler), after);
  }

  template<typename Ret = void, typename... Args>
  Ret
  emit_signal (const char *detailed_name, Args... args) noexcept
  {
    return Signal<TypeInstance, Ret (Args...)>::_peel_emit_by_name (this, detailed_name, args...);
  }

protected:
  static TypeInstance *
  create (Type tp) noexcept
  {
    ::GTypeInstance *ti = g_type_create_instance (static_cast<::GType> (tp));
    return reinterpret_cast<TypeInstance *> (ti);
  }

  template<typename T>
  static T *
  create ()
  {
    return reinterpret_cast<T *> (create (Type::of<T> ()));
  }

  void
  free () noexcept
  {
    ::GTypeInstance *ti = reinterpret_cast<::GTypeInstance *> (this);
    g_type_free_instance (ti);
  }
};

static_assert (sizeof (TypeInstance) == sizeof (::GTypeInstance),
               "GObject::TypeInstance size mismatch");
static_assert (alignof (TypeInstance) == alignof (::GTypeInstance),
               "GObject::TypeInstance align mismatch");

} /* namespace GObject */
} /* namespace peel */

peel_end_header
