#pragma once

#include <peel/GObject/Type.h>
#include <peel/lang.h>
#include <peel/property.h>
#include <type_traits>
#include <glib-object.h>

namespace peel
{
namespace internals
{

template<typename Subclass, typename = void>
struct InstanceInitHelper
{
  static ::GInstanceInitFunc
  get_instance_init ()
  {
    return nullptr;
  }

  static void
  call_instance_init (::GTypeInstance *instance, gpointer g_class)
  {
  }
};

template<typename Subclass>
struct InstanceInitHelper<Subclass, typename std::enable_if<has_own_member<Subclass> (&Subclass::init)>::type>
{
  static ::GInstanceInitFunc
  get_instance_init ()
  {
    return call_instance_init;
  }

  peel_nothrow
  static void
  call_instance_init (::GTypeInstance *instance, gpointer g_class)
  {
    Subclass *self = reinterpret_cast<Subclass *> (instance);
    typename Subclass::Class *klass = reinterpret_cast<typename Subclass::Class *> (g_class);
    self->Subclass::init (klass);
  }
};

template<typename Subclass>
struct ClassHelper
{
  peel_nothrow
  static void
  finalize_vfunc (::GObject *obj)
  {
    Subclass *self = reinterpret_cast<Subclass *> (obj);
    // Call the C++ destructor; this destructs all the C++ classes hierarchy,
    // but does nothing for the base C classes.
    self->~Subclass ();
    // Chain to the C finalize vfunc of the most derived C class.
    self->_peel_chain_finalize (obj);
  }

  peel_nothrow
  static void
  class_init (gpointer g_class, gpointer class_data)
  {
    // static_assert (std::is_trivially_copyable<typename Subclass::Class>::value);
    typename Subclass::Class *klass = reinterpret_cast<typename Subclass::Class *> (g_class);
    ::GObjectClass *object_class = reinterpret_cast<::GObjectClass *> (g_class);
    if (!std::is_trivially_destructible<Subclass>::value)
      object_class->finalize = finalize_vfunc;
    PropertyHelper<Subclass>::init_props (object_class);
    klass->Subclass::Class::init ();
  }

  G_NO_INLINE peel_nothrow
  static ::GType
  register_type_static (::GType parent_type, const char *type_name)
  {
    return g_type_register_static_simple (
        parent_type,
        g_intern_static_string (type_name),
        sizeof (typename Subclass::Class),
        class_init,
        sizeof (Subclass),
        InstanceInitHelper<Subclass>::get_instance_init (),
        // TODO: add a way to pass final/abstract here
        GTypeFlags(0));
    // TODO: add private, interfaces
  }
};

} /* namespace internals */

// FIXME: Maybe this shouldn't be the primary template?
template<typename T>
GObject::Type
GObject::Type::of ()
{
  return T::get_type ();
}

} /* namespace p */

#define PEEL_CLASS(Subclass)                                                   \
  friend class ::peel::GObject::Object;                                        \
  friend class ::peel::GObject::TypeClass;                                     \
  friend struct ::peel::internals::ClassHelper<Subclass>;                      \
  template<typename, typename> /* workaround a Clang bug */                    \
  friend struct ::peel::internals::InstanceInitHelper;                         \
  friend struct ::peel::internals::PropertyHelper<Subclass>;                   \
                                                                               \
public:                                                                        \
  peel_nothrow G_GNUC_CONST                                                    \
  static ::peel::GObject::Type                                                 \
  get_type ();                                                                 \
                                                                               \
protected:                                                                     \
  class Class;                                                                 \
                                                                               \
private:                                                                       \
  Subclass () = delete;                                                        \
  Subclass (const Subclass &) = delete;                                        \
  Subclass (Subclass &&) = delete;                                             \
                                                                               \
  Subclass &                                                                   \
  operator = (const Subclass &) = delete;                                      \
  Subclass &                                                                   \
  operator = (Subclass &&) = delete;                                           \
/* end of PEEL_CLASS */

#define PEEL_SIMPLE_CLASS(Subclass, ParentClass)                               \
  PEEL_CLASS (Subclass)                                                        \
                                                                               \
protected:                                                                     \
  class Class : public ParentClass::Class                                      \
  {                                                                            \
    friend struct ::peel::internals::ClassHelper<Subclass>;                    \
                                                                               \
  private:                                                                     \
    void                                                                       \
    init ();                                                                   \
  };                                                                           \
                                                                               \
private:                                                                       \
/* end of PEEL_SIMPLE_CLASS */

#define PEEL_CLASS_IMPL(Subclass, type_name, ParentClass)                      \
peel_nothrow G_GNUC_CONST                                                      \
::peel::GObject::Type                                                          \
Subclass::get_type ()                                                          \
{                                                                              \
  static ::GType _peel_tp;                                                     \
                                                                               \
  if (g_once_init_enter_pointer (&_peel_tp))                                   \
    {                                                                          \
      ::GType _peel_actual_tp = ::peel::internals::ClassHelper<Subclass>::     \
        register_type_static (::peel::Type::of<ParentClass> (), (type_name));  \
      g_once_init_leave_pointer (&_peel_tp, _peel_actual_tp);                  \
    }                                                                          \
                                                                               \
  return _peel_tp;                                                             \
}                                                                              \
/* end of PEEL_CLASS_IMPL */
