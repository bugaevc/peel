from peel_dbus_gen.type import Type
from peel_dbus_gen.utils import escape_cpp_name

class Argument:
    def __init__(self, attrs, function):
        from peel_dbus_gen.method import Method
        from peel_dbus_gen.signal import Signal

        self.function = function
        self.dbus_name = attrs.get('name', None)
        if self.dbus_name:
            self.cpp_name = escape_cpp_name(self.dbus_name)
        else:
            self.cpp_name = 'unnamed_arg_{}'.format(len(self.function.args) + 1)
        if isinstance(function, Method):
            default_direction = 'in'
        else:
            default_direction = 'out'
        self.direction = attrs.get('direction', default_direction)
        self.type = Type(attrs.get('type'))

    def generate_cpp_type(self, bare=False, ownership=None):
        from peel_dbus_gen.method import Method
        from peel_dbus_gen.signal import Signal

        if self.direction == 'in' or isinstance(self.function, Signal):
            out_asterisk = ''
        else:
            out_asterisk = '*'

        if ownership is None:
            if self.direction == 'in':
                ownership = 'none'
            else:
                ownership = 'full'
        if isinstance(self.function, Method):
            flavor = 'method'
        else:
            flavor = 'signal'
        s = self.type.generate_cpp_type(flavor=flavor, ownership=ownership)
        if bare:
            return s + out_asterisk
        elif s.endswith('*'):
            return s + out_asterisk + self.cpp_name
        else:
            return '{} {}{}'.format(s, out_asterisk, self.cpp_name)

    def generate_make_variant(self, ownership='full'):
        return self.type.generate_make_variant(self.cpp_name, ownership)

    def generate_set_from_variant(self, variant_expr):
        return '*{} = {}'.format(self.cpp_name, self.type.generate_variant_get(variant_expr))

    def generate_arg_info(self):
        name = '{}_{}_{}_arg_info'.format(self.function.iface.emit_name, self.function.cpp_name, self.cpp_name)
        l = [
            'static const ::GDBusArgInfo',
            '{} ='.format(name),
            '{',
            '  -1, /* ref_count */',
            '  const_cast<char *> ("{}"),'.format(self.dbus_name),
            '  const_cast<char *> ("{}"),'.format(self.type.signature),
            '  nullptr /* annotations */',
            '};',
        ]
        return name, '\n'.join(l)
