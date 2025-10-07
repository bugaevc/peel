from peel_dbus_gen.utils import camel_case_to_underscore, escape_cpp_name

class Signal:
    def __init__(self, attrs, iface):
        self.iface = iface
        self.dbus_name = attrs.get('name')
        self.cpp_name = camel_case_to_underscore(self.dbus_name)
        self.signal_name = self.cpp_name.replace('_', '-')
        # self.cpp_name = escape_cpp_name(self.cpp_name)
        self.arguments = []

    def generate_header(self):
        arg_types = [arg.generate_cpp_type(bare=True) for arg in self.arguments]
        signal_type = '::peel::Signal<{}, void ({})>'.format(self.iface.emit_name, ', '.join(arg_types))
        return '\n'.join([
            '  template<typename Handler>',
            '  ::peel::SignalConnection::Token',
            '  connect_{} (Handler &&handler, bool after = false) noexcept'.format(self.cpp_name),
            '  {',
            '    return {}::_peel_connect_by_name (this, "{}", static_cast<Handler &&> (handler), after);'.format(
                signal_type,
                self.signal_name,
            ),
            '  }',
            '',
            '  template<typename HandlerObject>',
            '  ::peel::enable_if_derived<::peel::GObject::Object, HandlerObject, ::peel::SignalConnection::Token>',
            '  connect_{} (HandlerObject *object, void (HandlerObject::*handler_method) ({} *{}), bool after = false) noexcept'.format(
                self.cpp_name,
                self.iface.emit_name,
                ''.join(', ' + tp for tp in arg_types),
            ),
            '  {',
            '    return {}::_peel_connect_by_name (this, "{}", object, handler_method, after);'.format(
                signal_type,
                self.signal_name,
            ),
            '  }',
        ])

    def generate_vfunc_ptr(self):
        args = ''.join(', ' + arg.generate_cpp_type() for arg in self.arguments)
        return '    void (*_peel_vfunc_{}) ({} *self{});'.format(self.cpp_name, self.iface.emit_name, args)

    def generate_override_vfunc_method(self):
        args = ''.join(', ' + arg.generate_cpp_type() for arg in self.arguments)
        args_forward = ', '.join(arg.cpp_name for arg in self.arguments)
        return '\n'.join([
            '    template<typename DerivedClass>',
            '    void',
            '    override_vfunc_{} ()'.format(self.cpp_name),
            '    {',
            '      _peel_vfunc_{} = +[] ({} *self{})'.format(self.cpp_name, self.iface.emit_name, args),
            '        {',
            '          DerivedClass *_peel_this = reinterpret_cast<DerivedClass *> (self);',
            '          _peel_this->DerivedClass::vfunc_{} ({});'.format(self.cpp_name, args_forward),
            '        };',
            '    }',
        ])

    def generate_id_decl(self):
        return 'static guint {}_{}_signal_id;'.format(self.iface.emit_name, self.cpp_name)

    def generate_iface_init(self, gtype_expr):
        args = ''.join(', ::peel::Type::of<{}> ()'.format(arg.type.generate_cpp_type(flavor='plain')) for arg in self.arguments)
        return '\n'.join([
            '  _peel_diagnostic_push_ignored_offsetof',
            '  {}_{}_signal_id = g_signal_new ('.format(self.iface.emit_name, self.cpp_name),
            '    "{}", {}, G_SIGNAL_RUN_LAST,'.format(self.signal_name, gtype_expr),
            '    G_STRUCT_OFFSET ({}::Iface, _peel_vfunc_{}),'.format(self.iface.emit_name, self.cpp_name),
            '    nullptr, nullptr, /* accumulator */',
            '    nullptr, /* TODO: marshaller */',
            '    G_TYPE_NONE, {}{});'.format(len(self.arguments), args),
            '  _peel_diagnostic_pop',
        ])

    def generate_signal_info(self):
        arg_infos = []
        l = []
        for arg in self.arguments:
            name, s = arg.generate_arg_info()
            l.append(s)
            arg_infos.append('  &{},'.format(name))
        name = '{}_{}_signal_info'.format(self.iface.emit_name, self.cpp_name)
        l.extend([
            'static const ::GDBusArgInfo * const',
            '{}_{}_arg_infos[] ='.format(self.iface.emit_name, self.cpp_name),
            '{',
            '\n'.join(arg_infos),
            '  nullptr',
            '};',
            '',
            'static const ::GDBusSignalInfo',
            '{} ='.format(name),
            '{',
            '  -1, /* ref_count */',
            '  const_cast<char *> ("{}"),'.format(self.dbus_name),
            '  const_cast<::GDBusArgInfo **> ({}_{}_arg_infos),'.format(self.iface.emit_name, self.cpp_name),
            '  nullptr /* annotations */',
            '};',
        ])
        return name, '\n'.join(l)

    def generate_proxy_signal(self, gtype_expr):
        l = [
            'static void',
            '{}_proxy_signal_{} (::GDBusProxy *proxy, ::GVariant *parameters)'.format(self.iface.emit_name, self.cpp_name),
            '{',
            '  ::GValue instance_and_params[{}];'.format(len(self.arguments) + 1),
            '  memset (instance_and_params, 0, sizeof (instance_and_params));',
            '  g_value_init (&instance_and_params[0], {});'.format(gtype_expr),
            '  g_value_set_object (&instance_and_params[0], proxy);',
            '  ::GVariant *parameter;',
        ]
        index = 0
        for arg in self.arguments:
            tp = arg.type.generate_cpp_type(flavor='plain')
            value_expr = '&instance_and_params[{}]'.format(index + 1)
            set_value = arg.type.generate_set_value_from_variant(value_expr, 'parameter')
            l.extend([
                '  parameter = g_variant_get_child_value (parameters, {});'.format(index),
                '  g_value_init ({}, ::peel::Type::of<{}> ());'.format(value_expr, tp),
                '  {};'.format(set_value),
                '  g_variant_unref (parameter);',
            ])
            index += 1
        l.append(
            '  g_signal_emitv (instance_and_params, {}_{}_signal_id, 0, nullptr);'.format(self.iface.emit_name, self.cpp_name),
        )
        for index in range(len(self.arguments) + 1):
            l.append('  g_value_unset (&instance_and_params[{}]);'.format(index))
        l.append('}')
        return '\n'.join(l)
