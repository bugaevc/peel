# from peel_dbus_gen.method import Method

class Interface:
    def __init__(self, attrs):
        self.interface_name = attrs.get('name')
        self.methods = []
        self.properties = []
        self.signals = []

        # FIXME: This s wrong, but we have to start somewhere.
        self.gtype_name = self.own_name = self.emit_name = self.interface_name.split('.')[-1]

    def generate_header(self):
        l = [
            '#pragma once',
            '',
            '#include <peel/GObject/Object.h>',
            '#include <peel/Gio/DBusProxy.h>',
            '#include <peel/Gio/DBusProxyFlags.h>',
            '#include <peel/Gio/DBusCallFlags.h>',
            '#include <peel/Gio/DBusInterfaceSkeleton.h>',
            '',
            'namespace peel',
            '{',
            'namespace Gio',
            '{',
            'class DBusConnection;',
            '}',
            '}',
            '',
        ]
        # TODO: generate opening namespace blocks
        l.extend([
            'class /* interface */ {} : public ::peel::GObject::Object'.format(self.emit_name),
            '{',
            'private:',
            '  {} () = delete;'.format(self.own_name),
            '  {} (const {} &) = delete;'.format(self.own_name, self.own_name),
            '  {} ({} &&) = delete;'.format(self.own_name, self.own_name),
            '',
            '  {} &'.format(self.own_name),
            '  operator = (const {} &) = delete;'.format(self.own_name),
            '  {} &'.format(self.own_name),
            '  operator = ({} &&) = delete;'.format(self.own_name),
            '',
            'public:',
            '  class Proxy;',
            '  class Skeleton;',
            '',
            '  G_GNUC_CONST',
            '  static ::peel::GObject::Type',
            '  _peel_get_type () noexcept;',
            '',
            '  unsigned',
            '  export_on_bus (::peel::Gio::DBusConnection *connection, const char *object_path, ::peel::UniquePtr<::peel::GLib::Error> *error) noexcept;',
            '',
            '  static const ::peel::Gio::DBusInterface::Info *',
            '  get_interface_info () noexcept;',
            '',
        ])
        for method in self.methods:
            l.append(method.generate_header())
        for property in self.properties:
            l.append(property.generate_header())
        for signal in self.signals:
            l.append(signal.generate_header())

        l.extend([
            '  class Iface : public ::peel::GObject::TypeInterface',
            '  {',
            '    friend class {};'.format(self.own_name),
            '    Iface () = delete;',
            '    Iface (const Iface &) = delete;',
            '    Iface (const Iface &&) = delete;',
            '',
            '  public:',
        ])
        for method in self.methods:
            l.append(method.generate_vfunc_ptr())
        for signal in self.signals:
            l.append(signal.generate_vfunc_ptr())
        for method in self.methods:
            l.append(method.generate_override_vfunc_method())
        for signal in self.signals:
            l.append(signal.generate_override_vfunc_method())
        l.extend([
            '  };',
            '};',
            '',
            'class {}::Proxy final : public ::peel::Gio::DBusProxy'.format(self.emit_name),
            '/* implements {} */'.format(self.emit_name),
            '{',
            'private:',
            '  Proxy () = delete;',
            '  Proxy (const Proxy &) = delete;',
            '  Proxy (Proxy &&) = delete;',
            '',
            'public:',
            '  G_GNUC_CONST',
            '  static ::peel::GObject::Type',
            '  _peel_get_type () noexcept;',
            '',
            '  template<typename AsyncReadyCallback>',
            '  static void',
            '  create (::peel::Gio::DBusConnection *connection, const char *name, const char *object_path,',
            '    AsyncReadyCallback &&callback, ::peel::Gio::DBusProxy::Flags flags = ::peel::Gio::DBusProxy::Flags::NONE, ::peel::Gio::Cancellable *cancellable = nullptr) noexcept',
            '  {',
            '    gpointer _peel_user_data;',
            '    ::GAsyncReadyCallback _peel_callback = ::peel::internals::CallbackHelper<void, ::GObject *, ::GAsyncResult *>::wrap_async_callback (',
            '      static_cast<AsyncReadyCallback &&> (callback),',
            '      [] (::GObject *source_object, ::GAsyncResult *res, gpointer data) -> void',
            '      {',
            '        AsyncReadyCallback &callback = reinterpret_cast<AsyncReadyCallback &> (*reinterpret_cast<unsigned char *> (data));',
            '        ::peel::GObject::Object *_peel_source_object = reinterpret_cast<::peel::GObject::Object *> (source_object);',
            '        ::peel::Gio::AsyncResult *_peel_res = reinterpret_cast<::peel::Gio::AsyncResult *> (res);',
            '        static_cast<AsyncReadyCallback &&> (callback) (_peel_source_object, _peel_res);',
            '      },',
            '      &_peel_user_data);',
            '    g_async_initable_new_async (static_cast<::GType> (_peel_get_type ()), G_PRIORITY_DEFAULT,',
            '      reinterpret_cast<::GCancellable *> (cancellable), _peel_callback, _peel_user_data,',
            '      "g-connection", reinterpret_cast<::GDBusConnection *> (connection),',
            '      "g-flags", static_cast<::GDBusProxyFlags> (flags),',
            '      "g-name", name,',
            '      "g-object-path", object_path,',
            '      "g-interface-name", "{}",'.format(self.interface_name),
            '      "g-interface-info", {}::get_interface_info (),'.format(self.emit_name),
            '      nullptr);',
            '  }',
            '',
            '  template<typename AsyncReadyCallback>',
            '  static void',
            '  create (::peel::Gio::BusType bus_type, const char *name, const char *object_path,',
            '    AsyncReadyCallback &&callback, ::peel::Gio::DBusProxy::Flags flags = ::peel::Gio::DBusProxy::Flags::NONE, ::peel::Gio::Cancellable *cancellable = nullptr) noexcept',
            '  {',
            '    gpointer _peel_user_data;',
            '    ::GAsyncReadyCallback _peel_callback = ::peel::internals::CallbackHelper<void, ::GObject *, ::GAsyncResult *>::wrap_async_callback (',
            '      static_cast<AsyncReadyCallback &&> (callback),',
            '      [] (::GObject *source_object, ::GAsyncResult *res, gpointer data) -> void',
            '      {',
            '        AsyncReadyCallback &callback = reinterpret_cast<AsyncReadyCallback &> (*reinterpret_cast<unsigned char *> (data));',
            '        ::peel::GObject::Object *_peel_source_object = reinterpret_cast<::peel::GObject::Object *> (source_object);',
            '        ::peel::Gio::AsyncResult *_peel_res = reinterpret_cast<::peel::Gio::AsyncResult *> (res);',
            '        static_cast<AsyncReadyCallback &&> (callback) (_peel_source_object, _peel_res);',
            '      },',
            '      &_peel_user_data);',
            '    g_async_initable_new_async (static_cast<::GType> (_peel_get_type ()), G_PRIORITY_DEFAULT,',
            '      reinterpret_cast<::GCancellable *> (cancellable), _peel_callback, _peel_user_data,',
            '      "g-bus-type", static_cast<::GBusType> (bus_type),',
            '      "g-flags", static_cast<::GDBusProxyFlags> (flags),',
            '      "g-name", name,',
            '      "g-object-path", object_path,',
            '      "g-interface-name", "{}",'.format(self.interface_name),
            '      "g-interface-info", {}::get_interface_info (),'.format(self.emit_name),
            '      nullptr);',
            '  }',
            '',
            '  static ::peel::RefPtr<{}>'.format(self.emit_name),
            '  create_finish (::peel::Gio::AsyncResult *res, ::peel::UniquePtr<::peel::GLib::Error> *error) noexcept',
            '  {',
            '    ::GAsyncResult *_peel_res = reinterpret_cast<::GAsyncResult *> (res);',
            '    ::GError *_peel_error = nullptr;',
            '    ::GObject *source_object = g_async_result_get_source_object (_peel_res);',
            '    ::GObject *object = g_async_initable_new_finish (G_ASYNC_INITABLE (source_object), _peel_res, error ? &_peel_error : nullptr);',
            '    g_object_unref (source_object);',
            '    if (error)',
            '      {',
            '        *error = ::peel::UniquePtr<::peel::GLib::Error>::adopt_ref (reinterpret_cast<::peel::GLib::Error *> (_peel_error));',
            '        if (_peel_error)',
            '          peel_assume (object == nullptr);',
            '        else',
            '          peel_assume (object);',
            '      }',
            '    return ::peel::RefPtr<{}>::adopt_ref (reinterpret_cast<{} *> (object));'.format(self.emit_name, self.emit_name),
            '  }',
            '',
            '  static ::peel::RefPtr<{}>'.format(self.emit_name),
            '  create_sync (::peel::Gio::DBusConnection *connection, const char *name, const char *object_path,',
            '    ::peel::UniquePtr<::peel::GLib::Error> *error,',
            '    ::peel::Gio::DBusProxy::Flags flags = ::peel::Gio::DBusProxy::Flags::NONE, ::peel::Gio::Cancellable *cancellable = nullptr) noexcept',
            '  {',
            '    ::GError *_peel_error = nullptr;',
            '    gpointer object = g_initable_new (static_cast<::GType> (_peel_get_type ()),',
            '      reinterpret_cast<::GCancellable *> (cancellable), error ? &_peel_error : nullptr,',
            '      "g-connection", reinterpret_cast<::GDBusConnection *> (connection),',
            '      "g-flags", static_cast<::GDBusProxyFlags> (flags),',
            '      "g-name", name,',
            '      "g-object-path", object_path,',
            '      "g-interface-name", "{}",'.format(self.interface_name),
            '      "g-interface-info", {}::get_interface_info (),'.format(self.emit_name),
            '      nullptr);',
            '    if (error)',
            '      {',
            '        *error = ::peel::UniquePtr<::peel::GLib::Error>::adopt_ref (reinterpret_cast<::peel::GLib::Error *> (_peel_error));',
            '        if (_peel_error)',
            '          peel_assume (object == nullptr);',
            '        else',
            '          peel_assume (object);',
            '      }',
            '    return ::peel::RefPtr<{}>::adopt_ref (reinterpret_cast<{} *> (object));'.format(self.emit_name, self.emit_name),
            '  }',
            '',
            '  static ::peel::RefPtr<{}>'.format(self.emit_name),
            '  create_sync (::peel::Gio::BusType bus_type, const char *name, const char *object_path,',
            '    ::peel::UniquePtr<::peel::GLib::Error> *error,',
            '    ::peel::Gio::DBusProxy::Flags flags = ::peel::Gio::DBusProxy::Flags::NONE, ::peel::Gio::Cancellable *cancellable = nullptr) noexcept',
            '  {',
            '    ::GError *_peel_error = nullptr;',
            '    gpointer object = g_initable_new (static_cast<::GType> (_peel_get_type ()),',
            '      reinterpret_cast<::GCancellable *> (cancellable), error ? &_peel_error : nullptr,',
            '      "g-bus-type", static_cast<::GBusType> (bus_type),',
            '      "g-flags", static_cast<::GDBusProxyFlags> (flags),',
            '      "g-name", name,',
            '      "g-object-path", object_path,',
            '      "g-interface-name", "{}",'.format(self.interface_name),
            '      "g-interface-info", {}::get_interface_info (),'.format(self.emit_name),
            '      nullptr);',
            '    if (error)',
            '      {',
            '        *error = ::peel::UniquePtr<::peel::GLib::Error>::adopt_ref (reinterpret_cast<::peel::GLib::Error *> (_peel_error));',
            '        if (_peel_error)',
            '          peel_assume (object == nullptr);',
            '        else',
            '          peel_assume (object);',
            '      }',
            '    return ::peel::RefPtr<{}>::adopt_ref (reinterpret_cast<{} *> (object));'.format(self.emit_name, self.emit_name),
            '  }',
            '};',
            '',
            'class {}::Skeleton : public ::peel::Gio::DBusInterfaceSkeleton'.format(self.emit_name),
            '/* abstract */',
            '{',
            'public:',
            '  G_GNUC_CONST',
            '  static ::peel::GObject::Type',
            '  _peel_get_type () noexcept;',
            '',
            'protected:',
        ])
        for signal in self.signals:
           l.append(signal.generate_emit_method_decl())
        l.extend([
            '};',
            '',
            'template<>',
            'inline ::peel::GObject::Type',
            'peel::GObject::Type::of<{}> ()'.format(self.emit_name),
            '{',
            '  return {}::_peel_get_type ();'.format(self.emit_name),
            '}',
            '',
            'template<>',
            'inline ::peel::GObject::Type',
            'peel::GObject::Type::of<{}::Proxy> ()'.format(self.emit_name),
            '{',
            '  return {}::Proxy::_peel_get_type ();'.format(self.emit_name),
            '}',
            '',
            'template<>',
            'inline ::peel::GObject::Type',
            'peel::GObject::Type::of<{}::Skeleton> ()'.format(self.emit_name),
            '{',
            '  return {}::Skeleton::_peel_get_type ();'.format(self.emit_name),
            '}',
        ])
        return '\n'.join(l)

    def generate_cpp(self, generated_header_name):
        l = [
            '#ifdef __GNUC__',
            '#pragma GCC optimize "no-exceptions"',
            '#endif',
            '',
            '#include "{}"'.format(generated_header_name),
            '#include <peel/Gio/Task.h>',
            '#include <peel/GLib/MainContext.h>',
            '#include <peel/class.h>',
            '',
        ]
        # TODO: Open any namespaces here.
        l.extend([
            'static ::GType {}_type;'.format(self.emit_name),
            'static ::GType {}_proxy_type;'.format(self.emit_name),
            'static ::GType {}_skeleton_type;'.format(self.emit_name),
            '',
        ])
        method_infos = []
        for method in self.methods:
            name, s = method.generate_method_info()
            l.append(s)
            method_infos.append('  &{},'.format(name))
        signal_infos = []
        for signal in self.signals:
            name, s = signal.generate_signal_info()
            l.append(s)
            signal_infos.append('  &{},'.format(name))
        property_infos = []
        for property in self.properties:
            name, s = property.generate_property_info()
            l.append(s)
            property_infos.append('  &{},'.format(name))
        l.extend([
            'static const ::GDBusMethodInfo * const',
            '{}_method_infos[] ='.format(self.emit_name),
            '{',
            '\n'.join(method_infos),
            '  nullptr',
            '};',
            '',
            'static const ::GDBusSignalInfo * const',
            '{}_signal_infos[] ='.format(self.emit_name),
            '{',
            '\n'.join(signal_infos),
            '  nullptr',
            '};',
            '',
            'static const ::GDBusPropertyInfo * const',
            '{}_property_infos[] ='.format(self.emit_name),
            '{',
            '\n'.join(property_infos),
            '  nullptr',
            '};',
            '',
            'static const ::GDBusInterfaceInfo',
            '{}_interface_info ='.format(self.emit_name),
            '{',
            '  -1, /* ref_count */',
            '  const_cast<char *> ("{}"),'.format(self.interface_name),
            '  const_cast<::GDBusMethodInfo **> ({}_method_infos),'.format(self.emit_name),
            '  const_cast<::GDBusSignalInfo **> ({}_signal_infos),'.format(self.emit_name),
            '  const_cast<::GDBusPropertyInfo **> ({}_property_infos),'.format(self.emit_name),
            '  nullptr /* annotations */',
            '};',
            '',
            'const ::peel::Gio::DBusInterface::Info *',
            '{}::get_interface_info () noexcept'.format(self.emit_name),
            '{',
            '  return reinterpret_cast<const ::peel::Gio::DBusInterface::Info *> (&{}_interface_info);'.format(self.emit_name),
            '}',
            '',
        ])
        for method in self.methods:
            l.append(method.generate_default_vfuncs())
        for signal in self.signals:
            l.append(signal.generate_id_decl())
            l.append(signal.generate_skeleton_vfunc())
        l.extend([
            '',
            'static void',
            '{}_iface_init (gpointer g_class, gpointer)'.format(self.emit_name),
            '{',
            '  {}::Iface *iface = reinterpret_cast<{}::Iface *> (g_class);'.format(self.emit_name, self.emit_name),
        ])
        for method in self.methods:
            l.append(method.generate_iface_init('iface'))
        for property in self.properties:
            l.append(property.generate_iface_init('iface'))
        for signal in self.signals:
            l.append(signal.generate_iface_init('{}_type'.format(self.emit_name)))
        l.extend([
            '}',
            '',
            'static void',
            '{}_iface_base_init (gpointer g_iface)'.format(self.emit_name),
            '{',
            '  {}::Iface *iface = reinterpret_cast<{}::Iface *> (g_iface);'.format(self.emit_name, self.emit_name),
            '  ::GType tp = reinterpret_cast<::GTypeInterface *> (g_iface)->g_instance_type;',
            '  if (!tp || !{}_skeleton_type || !g_type_is_a (tp, {}_skeleton_type))'.format(self.emit_name, self.emit_name),
            '    return;',
        ])
        for signal in self.signals:
            l.append(signal.generate_iface_base_init('iface'))
        l.extend([
            '}',
            '',
            '::peel::GObject::Type',
            '{}::_peel_get_type () noexcept'.format(self.emit_name),
            '{',
            '  if (_peel_once_init_enter (&{}_type))'.format(self.emit_name),
            '    {',
            '      ::GTypeInfo type_info =',
            '      {',
            '        sizeof ({}::Iface),'.format(self.emit_name),
            '        {}_iface_base_init,'.format(self.emit_name),
            '        nullptr,',
            '        {}_iface_init,'.format(self.emit_name),
            '        nullptr,',
            '        nullptr,',
            '        0,',
            '        0,',
            '        nullptr,',
            '        nullptr,',
            '      };',
            '      ::GType tp = g_type_register_static (G_TYPE_INTERFACE,',
            '        g_intern_static_string ("{}"),'.format(self.gtype_name),
            '        &type_info, ::GTypeFlags (0));',
            #'      ::GType tp = g_type_register_static_simple (G_TYPE_INTERFACE,',
            #'        g_intern_static_string ("{}"),'.format(self.gtype_name),
            #'        sizeof ({}::Iface),'.format(self.emit_name),
            #'        {}_iface_init,'.format(self.emit_name),
            #'        0,',
            #'        nullptr,',
            #'        ::GTypeFlags (0));',
            '      g_type_interface_add_prerequisite (tp, G_TYPE_OBJECT);',
            '      _peel_once_init_leave (&{}_type, tp);'.format(self.emit_name),
            '    }',
            '  return {}_type;'.format(self.emit_name),
            '}',
            '',
            'enum',
            '{',
            '  {}_PROP_0,'.format(self.emit_name),
            '\n'.join('  {},'.format(property.generate_enum_member()) for property in self.properties),
            '};',
            '',
        ])
        for method in self.methods:
            l.append(method.generate_proxy_vfuncs())
        for property in self.properties:
            l.append(property.generate_proxy_accessors())
        l.extend([
            'static void',
            '{}_proxy_get_property (::GObject *object, guint prop_id, ::GValue *value, ::GParamSpec *pspec)'.format(self.emit_name),
            '{',
            '  {}::Proxy *proxy = reinterpret_cast<{}::Proxy *> (object);'.format(self.emit_name, self.emit_name),
            '',
            '  switch (prop_id)',
            '  {',
        ])
        for property in self.properties:
            call = property.generate_proxy_get_property_call(proxy_expr='proxy', value_expr='value')
            if call:
                l.append(call)
        l.extend([
            '  default:',
            '    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);',
            '    break;',
            '  }',
            '}',
            '',
            'static void',
            '{}_proxy_set_property (::GObject *object, guint prop_id, const ::GValue *value, ::GParamSpec *pspec)'.format(self.emit_name),
            '{',
            '  {}::Proxy *proxy = reinterpret_cast<{}::Proxy *> (object);'.format(self.emit_name, self.emit_name),
            '',
            '  switch (prop_id)',
            '  {',
        ])
        for property in self.properties:
            call = property.generate_proxy_set_property_call(proxy_expr='proxy', value_expr='value')
            if call:
                l.append(call)
        l.extend([
            '  default:',
            '    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);',
            '    break;',
            '  }',
            '}',
            '',
            'static void',
            '{}_proxy_properties_changed (::GDBusProxy *proxy, ::GVariant *changed_properties, const char * const *invalidated_properties)'.format(self.emit_name),
            '{',
            '  for (const char * const *prop = invalidated_properties; *prop; prop++)',
            '    {',
            '\n'.join(
                '      {}if (!strcmp (*prop, "{}"))\n'.format('' if index == 0 else 'else ', property.dbus_name) +
                '        g_object_notify (G_OBJECT (proxy), "{}");'.format(property.prop_name)
                for index, property in enumerate(self.properties)
            ),
            '  }',
            '  ::GVariantIter iter;',
            '  g_variant_iter_init (&iter, changed_properties);',
            '  const char *prop;',
            '  while (g_variant_iter_loop (&iter, "{&sv}", &prop, nullptr))',
            '    {',
            '\n'.join(
                '      {}if (!strcmp (prop, "{}"))\n'.format('' if index == 0 else 'else ', property.dbus_name) +
                '        g_object_notify (G_OBJECT (proxy), "{}");'.format(property.prop_name)
                for index, property in enumerate(self.properties)
            ),
            '    }',
            '}',
            '',
        ])
        for signal in self.signals:
            l.append(signal.generate_proxy_signal('{}_type'.format(self.emit_name)))
        l.extend([
            'static void',
            '{}_proxy_signal (::GDBusProxy *proxy, const char *sender_name, const char *signal_name, ::GVariant *parameters)'.format(self.emit_name),
            '{',
            '\n'.join(
                '  if (!strcmp (signal_name, "{}"))\n'.format(signal.dbus_name) +
                '    return {}_proxy_signal_{} (proxy, parameters);'.format(self.emit_name, signal.cpp_name)
                for signal in self.signals
            ),
            '',
            '  /* Otherwise, it\'s a new signal we don\'t yet know about */',
            '}',
            '',
            'static void',
            '{}_proxy_class_init (gpointer g_class, gpointer)'.format(self.emit_name),
            '{',
            '  ::GObjectClass *object_class = G_OBJECT_CLASS (g_class);',
            '  ::GDBusProxyClass *proxy_class = G_DBUS_PROXY_CLASS (g_class);',
            '',
            '  object_class->get_property = {}_proxy_get_property;'.format(self.emit_name),
            '  object_class->set_property = {}_proxy_set_property;'.format(self.emit_name),
            '',
            '  proxy_class->g_properties_changed = {}_proxy_properties_changed;'.format(self.emit_name),
            '  proxy_class->g_signal = {}_proxy_signal;'.format(self.emit_name),
        ])
        for property in self.properties:
            l.append(property.generate_proxy_override_pspec(class_expr='object_class'))
        l.extend([
            '}',
            '',
            'static void',
            '{}_proxy_iface_init (gpointer g_iface, gpointer)'.format(self.emit_name),
            '{',
            '  {}::Iface *iface = reinterpret_cast<{}::Iface *> (g_iface);'.format(self.emit_name, self.emit_name),
        ])
        for method in self.methods:
            l.append(method.generate_proxy_iface_init('iface'))
        l.extend([
            '}',
            '',
            '::peel::GObject::Type',
            '{}::Proxy::_peel_get_type () noexcept'.format(self.emit_name),
            '{',
            '  if (_peel_once_init_enter (&{}_proxy_type))'.format(self.emit_name),
            '    {',
            '      ::GType tp = g_type_register_static_simple (G_TYPE_DBUS_PROXY,',
            '        g_intern_static_string ("{}Proxy"),'.format(self.gtype_name),
            '        sizeof (GDBusProxyClass),',
            '        {}_proxy_class_init,'.format(self.emit_name),
            '        sizeof ({}::Proxy),'.format(self.emit_name),
            '        nullptr,',
            '#if GLIB_CHECK_VERSION (2, 70, 0)',
            '        ::GTypeFlags (G_TYPE_FLAG_FINAL)',
            '#else',
            '        ::GTypeFlags (0)',
            '#endif',
            '      );',
            '      const ::GInterfaceInfo iface_info {{ {}_proxy_iface_init, nullptr, nullptr }};'.format(self.emit_name),
            '      g_type_add_interface_static (tp, {}::_peel_get_type (), &iface_info);'.format(self.emit_name),
            '      _peel_once_init_leave (&{}_proxy_type, tp);'.format(self.emit_name),
            '    }',
            '  return {}_proxy_type;'.format(self.emit_name),
            '}'
            '',
        ])
        for method in self.methods:
            l.append(method.generate_vtable_method())
        l.extend([
            'static void',
            '{}_interface_vtable_method_call (::GDBusConnection *connection,'.format(self.emit_name),
            '  const char *sender, const char *object_path, const char *interface_name, const char *method_name,',
            '  ::GVariant *parameters, ::GDBusMethodInvocation *invocation, gpointer user_data)',
            '{',
            '  {} *self = G_TYPE_CHECK_INSTANCE_CAST (user_data, {}_type, {});'.format(self.emit_name, self.emit_name, self.emit_name),
            '',
            '\n'.join(
                '  if (!strcmp (method_name, "{}"))\n'.format(method.dbus_name) +
                '    return {}_vtable_method_{} (self, parameters, invocation);'.format(self.emit_name, method.cpp_name)
                for method in self.methods
            ),
            '',
            '  g_dbus_method_invocation_return_error (invocation, G_DBUS_ERROR, G_DBUS_ERROR_UNKNOWN_METHOD,',
            '    "Method %s is not implemented on interface %s", method_name, interface_name ? interface_name : "(none)");',
            '}',
            '',
            'static ::GVariant *',
            '{}_interface_vtable_get_property (::GDBusConnection *connection,'.format(self.emit_name),
            '  const char *sender, const char *object_path, const char *interface_name, const char *property_name,',
            '  ::GError **error, gpointer user_data)',
            '{',
            '  return nullptr;',
            '}',
            '',
            'static gboolean',
            '{}_interface_vtable_set_property (::GDBusConnection *connection,'.format(self.emit_name),
            '  const char *sender, const char *object_path, const char *interface_name, const char *property_name,',
            '  ::GVariant *value, ::GError **error, gpointer user_data)',
            '{',
            '  return false;',
            '}',
            '',
            'static const ::GDBusInterfaceVTable',
            '{}_interface_vtable ='.format(self.emit_name),
            '{',
            '  {}_interface_vtable_method_call,'.format(self.emit_name),
            '  {}_interface_vtable_get_property,'.format(self.emit_name),
            '  {}_interface_vtable_set_property,'.format(self.emit_name),
            '};',
            '',
            'static ::GDBusInterfaceVTable *',
            '{}_skeleton_get_vtable (::GDBusInterfaceSkeleton *self)'.format(self.emit_name),
            '{',
            '  return const_cast<::GDBusInterfaceVTable *> (&{}_interface_vtable);'.format(self.emit_name),
            '}',
            '',
            'static ::GDBusInterfaceInfo *',
            '{}_skeleton_get_info (::GDBusInterfaceSkeleton *self)'.format(self.emit_name),
            '{',
            '  return const_cast<::GDBusInterfaceInfo *> (&{}_interface_info);'.format(self.emit_name),
            '}',
            '',
            'static void',
            '{}_skeleton_class_init (gpointer g_class, gpointer)'.format(self.emit_name),
            '{',
            '  ::GObjectClass *object_class = G_OBJECT_CLASS (g_class);',
            '  ::GDBusInterfaceSkeletonClass *skeleton_class = G_DBUS_INTERFACE_SKELETON_CLASS (g_class);',
            '',
            '  skeleton_class->get_info = {}_skeleton_get_info;'.format(self.emit_name),
            '  skeleton_class->get_vtable = {}_skeleton_get_vtable;'.format(self.emit_name),
            '}',
            '',
            '::peel::GObject::Type',
            '{}::Skeleton::_peel_get_type () noexcept'.format(self.emit_name),
            '{',
            '  if (_peel_once_init_enter (&{}_skeleton_type))'.format(self.emit_name),
            '    {',
            '      ::GType tp = g_type_register_static_simple (G_TYPE_DBUS_INTERFACE_SKELETON,',
            '        g_intern_static_string ("{}Skeleton"),'.format(self.emit_name),
            '        sizeof ({}::Skeleton::Class),'.format(self.emit_name),
            '        {}_skeleton_class_init,'.format(self.emit_name),
            '        sizeof ({}::Skeleton),'.format(self.emit_name),
            '        nullptr,',
            '        G_TYPE_FLAG_ABSTRACT);',
            '      _peel_once_init_leave (&{}_skeleton_type, tp);'.format(self.emit_name),
            '    }',
            '  return {}_skeleton_type;'.format(self.emit_name),
            '}'
        ])
        for signal in self.signals:
            l.append(signal.generate_emit_method_body())
        return '\n'.join(l)
