class Argument:
    def __init__(self, attrs, default_direction):
        self.name = attrs.get('name', None)
        self.direction = attrs.get('direction', default_direction)
        self.type = attrs.get('type')

    def generate_cpp_type(self):
        if self.direction == 'in':
            out_asterisk = ''
        else:
            out_asterisk = '*'

        if self.type == 'b':
            return 'bool ' + out_asterisk + self.name
        elif self.type in ('s', 'o'):
            return '::peel::String ' + out_asterisk + self.name
        else:
            # TODO
            return '::peel::GLib::Variant *' + out_asterisk + self.name

    def generate_make_variant(self):
        if self.type == 'b':
            return 'g_variant_new_boolean ({})'.format(self.name)
        elif self.type == 's':
            return 'g_variant_new_take_string (std::move ({}).release_string ())'.format(self.name)
        elif self.type == 'o':
            return 'g_variant_new_object_path ({})'.format(self.name)
        else:
            # TODO
            return 'reinterpret_cast<::GVariant *> ({})'.format(self.name)
