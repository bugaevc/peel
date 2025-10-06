from peel_dbus_gen.type import Type
from peel_dbus_gen.utils import camel_case_to_underscore

class Property:
    def __init__(self, attrs, iface):
        self.iface = iface
        self.dbus_name = attrs.get('name')
        self.cpp_name = camel_case_to_underscore(self.dbus_name)
        self.prop_name = self.cpp_name.replace('_', '-')
        self.type = Type(attrs.get('type'))
        self.access = attrs.get('access')

    def generate_header(self):
        plain_type = self.type.generate_cpp_type(flavor='property')
        tp = self.type.generate_cpp_type(ownership='full')
        l = [
            '  static ::peel::Property<{}>'.format(plain_type),
            '  prop_{} ()'.format(self.cpp_name),
            '  {',
            '    return ::peel::Property<{}> ("{}");'.format(plain_type, self.prop_name),
            '  }',
        ]
        if 'read' in self.access:
            l.extend([
                '',
                '  {}'.format(tp),
                '  get_{} ()'.format(self.cpp_name),
                '  {',
                '    return get_property (prop_{} ());'.format(self.cpp_name),
                '  }',
            ])
        if 'write' in self.access:
            l.extend([
                '',
                '  void',
                '  set_{} ({} _peel_value)'.format(self.cpp_name, tp),
                '  {',
                '    set_property (prop_{} (), std::move (_peel_value));'.format(self.cpp_name),
                '  }',
            ])
        return '\n'.join(l)

    def generate_iface_init(self, iface_expr):
        flags = 'G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS'
        if 'read' in self.access:
            flags += ' | G_PARAM_READABLE'
        if 'write' in self.access:
            flags += ' | G_PARAM_WRITABLE'
        # TODO: deprecated
        pspec_expr = self.type.generate_make_pspec(
            name='"{}"'.format(self.prop_name),
            nick='nullptr',
            blurb='nullptr',
            flags='::GParamFlags ({})'.format(flags),
        )
        return '  g_object_interface_install_property ({}, {});'.format(iface_expr, pspec_expr)

    def generate_proxy_iface_init(self, iface_expr):
        pass

    def generate_proxy_accessors(self):
        l = []
        if 'read' in self.access:
            l.extend([
                'static void',
                '{}_proxy_get_{} ({}::Proxy *self, ::GValue *value)'.format(self.iface.emit_name, self.cpp_name, self.iface.emit_name),
                '{',
                '  ::GDBusProxy *_peel_self = reinterpret_cast<::GDBusProxy *> (self);',
                '  ::GVariant *v = g_dbus_proxy_get_cached_property (_peel_self, "{}");'.format (self.dbus_name),
                '  if (v == nullptr)',
                '    {',
                '      ::GVariant *reply = g_dbus_proxy_call_sync (',
                '        _peel_self, "org.freedesktop.DBus.Properties.Get",',
                '        g_variant_new ("(ss)", "{}", "{}"),'.format(self.iface.interface_name, self.dbus_name),
                '        G_DBUS_CALL_FLAGS_NONE, -1, nullptr, nullptr);',
                '      if (reply == nullptr)',
                '        return;',
                '      g_variant_get_child (reply, 0, "v", &v);',
                '      g_variant_unref (reply);',
                '    }',
                '  {};'.format(self.type.generate_set_value_from_variant(value_expr='value', variant_expr='v')),
                '  g_variant_unref (v);',
                '}',
            ])
        if 'write' in self.access:
            l.extend([
                'static void',
                '{}_proxy_set_{} ({}::Proxy *self, const ::GValue *value)'.format(self.iface.emit_name, self.cpp_name, self.iface.emit_name),
                '{',
                '  // TODO',
                '}',
            ])
        return '\n'.join(l)

    def generate_enum_member(self):
        return '{}_PROP_{}'.format(self.iface.emit_name, self.cpp_name.upper())

    def generate_proxy_get_property_call(self, proxy_expr, value_expr):
        if 'read' not in self.access:
            return None
        return '\n'.join([
            '  case {}:'.format(self.generate_enum_member()),
            '    {}_proxy_get_{} ({}, {});'.format(self.iface.emit_name, self.cpp_name, proxy_expr, value_expr),
            '    break;',
        ])

    def generate_proxy_set_property_call(self, proxy_expr, value_expr):
        if 'write' not in self.access:
            return None
        return '\n'.join([
            '  case {}:'.format(self.generate_enum_member()),
            '    {}_proxy_set_{} ({}, {});'.format(self.iface.emit_name, self.cpp_name, proxy_expr, value_expr),
            '    break;',
        ])

    def generate_proxy_override_pspec(self, class_expr):
        return '  g_object_class_override_property ({}, {}, "{}");'.format(class_expr, self.generate_enum_member(), self.prop_name)
