class Type:
    def __init__(self, signature):
        self.signature = signature

    def generate_cpp_type(self, flavor='arg'):
        if self.signature == 'b':
            return 'bool'
        elif self.signature == 'y':
            return 'unsigned char'
        elif self.signature == 'n':
            return 'int16_t'
        elif self.signature == 'q':
            return 'uint16_t'
        elif self.signature == 'i':
            return 'int'
        elif self.signature == 'u':
            return 'unsigned int'
        elif self.signature == 'x':
            return 'int64_t'
        elif self.signature == 't':
            return 'uint64_t'
        elif self.signature == 'd':
            return 'double'
        elif self.signature in ('s', 'o', 'g'):
            return '::peel::String'
        else:
            # TODO
            if flavor in ('arg', 'signal'):
                return '::peel::GLib::Variant *'
            elif flavor == 'property':
                return '::peel::GLib::Variant'


    def generate_make_variant(self, cpp_expr):
        if self.signature == 'b':
            return 'g_variant_new_boolean ({})'.format(cpp_expr)
        elif self.signature == 'y':
            return 'g_variant_new_byte ({})'.format(cpp_expr)
        elif self.signature == 'n':
            return 'g_variant_new_int16 ({})'.format(cpp_expr)
        elif self.signature == 'q':
            return 'g_variant_new_uint16 ({})'.format(cpp_expr)
        elif self.signature == 'i':
            return 'g_variant_new_int ({})'.format(cpp_expr)
        elif self.signature == 'u':
            return 'g_variant_new_uint ({})'.format(cpp_expr)
        elif self.signature == 'x':
            return 'g_variant_new_int64 ({})'.format(cpp_expr)
        elif self.signature == 't':
            return 'g_variant_new_uint64 ({})'.format(cpp_expr)
        elif self.signature == 'd':
            return 'g_variant_new_double ({})'.format(cpp_expr)
        elif self.signature in ('s', 'g'):
            return 'g_variant_new_take_string (std::move ({}).release_string ())'.format(cpp_expr)
        elif self.signature == 'o':
            return 'g_variant_new_object_path ({})'.format(cpp_expr)
        else:
            # TODO
            return 'reinterpret_cast<::GVariant *> ({})'.format(cpp_expr)

    def generate_variant_get(self, variant_expr):
        if self.signature == 'b':
            return '!!g_variant_get_boolean ({})'.format(variant_expr)
        elif self.signature == 'y':
            return 'g_variant_get_byte ({})'.format(variant_expr)
        elif self.signature == 'n':
            return 'g_variant_get_int16 ({})'.format(variant_expr)
        elif self.signature == 'q':
            return 'g_variant_get_uint16 ({})'.format(variant_expr)
        elif self.signature == 'i':
            return 'g_variant_get_int ({})'.format(variant_expr)
        elif self.signature == 'u':
            return 'g_variant_get_uint ({})'.format(variant_expr)
        elif self.signature == 'x':
            return 'g_variant_get_int64 ({})'.format(variant_expr)
        elif self.signature == 't':
            return 'g_variant_get_uint64 ({})'.format(variant_expr)
        elif self.signature == 'd':
            return 'g_variant_get_double ({})'.format(variant_expr)
        elif self.signature in ('s', 'g', 'o'):
            return '::peel::String::adopt_string (g_variant_dup_string ({}, nullptr))'.format(variant_expr)
        else:
            # TODO
            return 'reinterpret_cast<::GVariant *> ({})'.format(variant_expr)
