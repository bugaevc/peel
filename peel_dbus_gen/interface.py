# from peel_dbus_gen.method import Method

class Interface:
    def __init__(self, attrs):
        self.interface_name = attrs.get('name')
        self.methods = []
        self.properties = []
        self.signals = []

        # FIXME: This s wrong, but we hae to start somewhere.
        self.gtype_name = self.own_name = self.emit_name = self.interface_name.split('.')[-1]

    def generate_header(self):
        l = [
            '#pragma once',
            '',
            '#include <peel/GObject/Object.h>',
            '#include <peel/Gio/DBusProxy.h>',
            '#include <peel/Gio/DBusProxyFlags.h>',
            '#include <peel/Gio/DBusCallFlags.h>',
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
        # TODO: override for signals too?
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
            # TODO: "g-interface-info"
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
            # TODO: "g-interface-info"
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
            # TODO: "g-interface-info"
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
            # TODO: "g-interface-info"
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
        ])
        return '\n'.join(l)

    def generate_cpp(self, generated_header_name):
        l = [
            '#include "{}"'.format(generated_header_name),
            '#include <peel/Gio/Task.h>',
            '#include <peel/GLib/MainContext.h>',
            '#include <peel/class.h>',
            '',
        ]
        # TODO: Open any namespaces here.
        l.extend([
            'static ::GType {}_type;'.format(self.emit_name),
            'static ::GType {}_Proxy_type;'.format(self.emit_name),
            '',
        ])
        for method in self.methods:
            l.append(method.generate_default_vfuncs())
        l.extend([
            '',
            'static void',
            '{}_iface_init (gpointer g_class, gpointer)'.format(self.emit_name),
            '{',
            '  {}::Iface *iface = reinterpret_cast<{}::Iface *> (g_class);'.format(self.emit_name, self.emit_name),
        ])
        for method in self.methods:
            l.append(method.generate_iface_init('iface'))
        l.extend([
            '}',
            '',
            '::peel::GObject::Type',
            '{}::_peel_get_type () noexcept'.format(self.emit_name),
            '{',
            '  if (_peel_once_init_enter (&{}_type))'.format(self.emit_name),
            '    {',
            '      ::GType tp = g_type_register_static_simple (G_TYPE_INTERFACE,',
            '        g_intern_static_string ("{}"),'.format(self.gtype_name),
            '        sizeof ({}::Iface),'.format(self.emit_name),
            '        {}_iface_init,'.format(self.emit_name),
            '        0,',
            '        nullptr,',
            '        ::GTypeFlags (0));',
            '      g_type_interface_add_prerequisite (tp, G_TYPE_OBJECT);',
            '      _peel_once_init_leave (&{}_type, tp);'.format(self.emit_name),
            '    }',
            '  return {}_type;'.format(self.emit_name),
            '}',
            '',
            'static void',
            '{}_proxy_properties_changed (::GDBusProxy *proxy, ::GVariant *changed_properties, const char * const *invalidated_properties)'.format(self.emit_name),
            '{',
            # TODO: notify, or proxy->dispatch_properties_changed
            '}',
            '',
            'static void',
            '{}_proxy_signal (::GDBusProxy *proxy, const char *sender_name, const char *signal_name, ::GVariant *parameters)'.format(self.emit_name),
            '{',
            # TODO: switch on signal, emit the right one
            '}',
            '',
            'static void',
            '{}_proxy_class_init (gpointer g_class, gpointer)'.format(self.emit_name),
            '{',
            '  ::GObjectClass *object_class = G_OBJECT_CLASS (g_class);',
            '  ::GDBusProxyClass *proxy_class = G_DBUS_PROXY_CLASS (g_class);',
            '  proxy_class->g_properties_changed = {}_proxy_properties_changed;'.format(self.emit_name),
            '  proxy_class->g_signal = {}_proxy_signal;'.format(self.emit_name),
            # TODO: override interface properties here
            '}',
            '',
        ])
        for method in self.methods:
            l.append(method.generate_proxy_vfuncs())
        l.extend([
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
            '  if (_peel_once_init_enter (&{}_Proxy_type))'.format(self.emit_name),
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
            '      _peel_once_init_leave (&{}_Proxy_type, tp);'.format(self.emit_name),
            '    }',
            '  return {}_Proxy_type;'.format(self.emit_name),
            '}'
            '',
        ])
        return '\n'.join(l)
