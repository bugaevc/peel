from peel_dbus_gen.type import Type

class Argument:
    def __init__(self, attrs, default_direction):
        self.name = attrs.get('name', None)
        self.direction = attrs.get('direction', default_direction)
        self.type = Type(attrs.get('type'))

    def generate_cpp_type(self):
        if self.direction == 'in':
            out_asterisk = ''
        else:
            out_asterisk = '*'

        if self.direction == 'in':
            ownership = 'none'
        else:
            ownership = 'full'
        s = self.type.generate_cpp_type(flavor='arg', ownership=ownership)
        if s.endswith('*'):
            return s + out_asterisk + self.name
        else:
            return '{} {}{}'.format(s, out_asterisk, self.name)

    def generate_make_variant(self):
        return self.type.generate_make_variant(self.name)


    def generate_set_from_variant(self, variant_expr):
        return '*{} = {}'.format(self.name, self.type.generate_variant_get(variant_expr))
