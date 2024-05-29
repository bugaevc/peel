#pragma once

#include <peel/class.h>
#include <peel/signal.h>
#include <peel/Gtk/Widget.h>
#include <peel/Gtk/BuilderCScope.h>

namespace peel
{
namespace internals
{

template<typename HandlerObject, typename Instance, typename Ret, typename... Args>
class BuilderClosure : public ::GClosure
{
private:
  // GClosure::data stores the HandlerObject pointer.
  Ret (HandlerObject::*handler_method) (Instance *, Args...);

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
    auto handler = [closure] (Instance *instance, Args... args) -> Ret
    {
      HandlerObject *obj = reinterpret_cast<HandlerObject *> (closure->data);
      Ret (HandlerObject::*handler_method) (Instance *, Args...) = reinterpret_cast<BuilderClosure *> (closure)->handler_method;
      return (obj->*handler_method) (instance, args...);
    };
    SignalHelper1<Ret, Args...>::template marshal_cpp<Instance, decltype (handler)> (instance, handler, return_value, param_values + 1);
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
    auto handler = [closure] (Instance *instance, Args... args) -> Ret
    {
      HandlerObject *obj = reinterpret_cast<HandlerObject *> (closure->data);
      Ret (HandlerObject::*handler_method) (Instance *, Args...) = reinterpret_cast<BuilderClosure *> (closure)->handler_method;
      return (obj->*handler_method) (instance, args...);
    };

    va_list args_copy;
    if (!std::is_void<Ret>::value)
      va_copy (args_copy, args);
    SignalHelper1<Ret, Args...>::template marshal_va_cpp<Instance, decltype (handler)> (
      instance, handler, return_value,
      std::is_void<Ret>::value ? args : args_copy);
    if (!std::is_void<Ret>::value)
      va_end (args_copy);
  }

public:
  peel_nothrow
  static BuilderClosure *
  make (void *obj, Ret (HandlerObject::*handler_method) (Instance *, Args...))
  {
    ::GClosure *g_closure = g_closure_new_simple (sizeof (BuilderClosure), obj);
    BuilderClosure *closure = reinterpret_cast<BuilderClosure *> (g_closure);
    closure->handler_method = handler_method;

    g_closure_set_marshal (g_closure, marshal);
    // _g_closure_set_va_marshal (g_closure, marshal_va);
    closure_set_va_marshal (g_closure, marshal_va);

    return closure;
  }
};

class BuilderClosureFactoryBase
{
protected:
  ::GClosure *(*make_closure_cb) (BuilderClosureFactoryBase *factory, void *obj);

public:
  ::GClosure *
  make_closure (void *obj)
  {
    return make_closure_cb (this, obj);
  }
};

template<typename HandlerObject, typename Instance, typename Ret, typename... Args>
class BuilderClosureFactory : public BuilderClosureFactoryBase
{
private:
  Ret (HandlerObject::*handler_method) (Instance *, Args...);

  static ::GClosure *
  make_closure (BuilderClosureFactoryBase *factory, void *obj)
  {
    BuilderClosureFactory *self = static_cast<BuilderClosureFactory *> (factory);
    return BuilderClosure<HandlerObject, Instance, Ret, Args...>::make (obj, self->handler_method);
  }

public:
  BuilderClosureFactory (Ret (HandlerObject::*handler_method) (Instance *, Args...))
    : handler_method (handler_method)
  {
    make_closure_cb = make_closure;
  }
};

} /* namespace internals */

class BuilderScope : public Gtk::Builder::CScope
{
  ::GHashTable *closure_factories;

  peel_nothrow
  void
  vfunc_dispose ()
  {
    g_clear_pointer (&closure_factories, g_hash_table_destroy);
    parent_vfunc_dispose<BuilderScope> ();
  }

  peel_nothrow
  static void
  class_init (gpointer g_class, gpointer class_data)
  {
    (void) class_data;
    reinterpret_cast<::GObjectClass *> (g_class)->dispose = +[] (::GObject *obj)
    {
      reinterpret_cast<BuilderScope *> (obj)->vfunc_dispose ();
    };
  }

  static ::GClosure *
  create_closure (::GtkBuilderScope *scope, ::GtkBuilder *builder,
                  const char *function_name, ::GtkBuilderClosureFlags flags,
                  ::GObject *object, ::GError **error)
  {
    BuilderScope *self = reinterpret_cast<BuilderScope *> (scope);

    if (self->closure_factories)
      {
        gpointer closure_factory = g_hash_table_lookup (self->closure_factories, function_name);
        if (G_LIKELY (closure_factory))
          {
            if (G_UNLIKELY (flags & GTK_BUILDER_CLOSURE_SWAPPED))
              {
                g_critical ("Swapped set for closure %s", function_name);
                return nullptr;
              }
            if (G_UNLIKELY (object != nullptr))
              {
                g_critical ("Object set for closure %s\n", function_name);
                return nullptr;
              }
            object = gtk_builder_get_current_object (builder);
            return reinterpret_cast<internals::BuilderClosureFactoryBase *> (closure_factory)->make_closure (object);
          }
      }

    gpointer iface = g_type_interface_peek (self->get_class (), GTK_TYPE_BUILDER_SCOPE);
    gpointer parent_iface = g_type_interface_peek_parent (iface);
    return reinterpret_cast<::GtkBuilderScopeInterface *> (parent_iface)->create_closure (scope, builder, function_name, flags, object, error);
  }

  static void
  init_iface (gpointer iface, gpointer data)
  {
    (void) data;
    reinterpret_cast<::GtkBuilderScopeInterface *> (iface)->create_closure = create_closure;
  }

public:
  peel_nothrow G_GNUC_CONST
  static Type
  get_type ()
  {
    static ::GType tp;
    if (_peel_once_init_enter (&tp))
      {
        ::GType actual_tp = g_type_register_static_simple (GTK_TYPE_BUILDER_CSCOPE,
          "PeelBuilderScope", sizeof (::GObjectClass), class_init,
          sizeof (BuilderScope), nullptr, GTypeFlags (0));
        const ::GInterfaceInfo iface_info { init_iface, nullptr, nullptr };
        g_type_add_interface_static (actual_tp, GTK_TYPE_BUILDER_SCOPE, &iface_info);
        _peel_once_init_leave (&tp, actual_tp);
      }
    return tp;
  }

  static RefPtr<BuilderScope>
  create ()
  {
    return Object::create<BuilderScope> ();
  }

  template<typename HandlerObject, typename Ret, typename Instance, typename... Args>
  void
  add_callback (const char *callback_name, Ret (HandlerObject::*handler_method) (Instance *, Args...))
  {
    if (!closure_factories)
      closure_factories = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);

    typedef internals::BuilderClosureFactory<HandlerObject, Instance, Ret, Args...> Factory;
    Factory *factory = reinterpret_cast<Factory *> (g_malloc (sizeof (Factory)));
    new (factory) Factory (handler_method);
    g_hash_table_insert (closure_factories, g_strdup (callback_name), factory);
  }
};

} /* namespace peel */

#define PEEL_WIDGET_TEMPLATE_BIND_CHILD(WidgetClass, name) this->::peel::Gtk::Widget::Class::bind_template_child_full (#name, false, G_STRUCT_OFFSET (WidgetClass, name))
