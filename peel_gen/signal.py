from peel_gen.alias import chase_type_aliases
from peel_gen.function_like import FunctionLike
from peel_gen.type import PlainType
from peel_gen.exceptions import UnsupportedForNowException

class Signal(FunctionLike):
    def __init__(self, attrs, containing_type):
        super().__init__(attrs, containing_type.ns)
        self.containing_type = containing_type
        self.detailed = attrs.get('detailed', '0') != '0'
        self.tweak_ident = containing_type.c_type + '::signal_' + self.name

    def __repr__(self):
        return 'Signal({}.{})'.format(self.containing_type, self.name)

    def generate(self):
        connect_signal_name = self.name.replace('-', '_')

        for param in ([self.rv] if self.rv else []) + (self.params.params if self.params else []):
            tp = chase_type_aliases(param.type)
            if isinstance(tp, PlainType) and tp.gname in ['glong', 'gulong']:
                raise UnsupportedForNowException('Signals with glong / gulong return type or parameters not supported')

        rv_cpp_type = self.rv.generate_cpp_type(name=None, context=self.containing_type)
        if self.params is not None:
            param_types = [p.generate_cpp_type(name=None, context=self.containing_type) for p in self.params.params]
        else:
            param_types = []
        signal_type = 'Signal<{}, {} ({})>'.format(
            self.containing_type.emit_name,
            rv_cpp_type,
            ', '.join(param_types)
        )
        l = [
            '  template<typename Handler>',
            '  peel::SignalConnection::Token',
            '  connect_{} (Handler &&handler, bool after = false) noexcept'.format(connect_signal_name),
            '  {',
            '    return {}::_peel_connect_by_name (this, "{}", static_cast<Handler &&> (handler), after);'.format(
                signal_type,
                self.name,
            ),
            '  }',
            '',
            '  template<typename HandlerObject>',
            '  peel::enable_if_derived<GObject::Object, HandlerObject, peel::SignalConnection::Token>',
            '  connect_{} (HandlerObject *object, {} (HandlerObject::*handler_method) ({}), bool after = false) noexcept'.format(
                connect_signal_name,
                rv_cpp_type,
                ', '.join([self.containing_type.emit_name + ' *'] + param_types)
            ),
            '  {',
            '    return {}::_peel_connect_by_name (this, "{}", object, handler_method, after);'.format(
                signal_type,
                self.name,
            ),
            '  }',
        ]
        return '\n'.join(l)
