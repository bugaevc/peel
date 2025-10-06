class Type:
    def __init__(self, signature):
        self.signature = signature

    def generate_cpp_type(self, flavor='arg', ownership='none'):
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
                if ownership == 'none':
                    return '::peel::GLib::Variant *'
                else:
                    return '::peel::RefPtr<::peel::GLib::Variant>'
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
            return 'g_variant_new_int32 ({})'.format(cpp_expr)
        elif self.signature == 'u':
            return 'g_variant_new_uint32 ({})'.format(cpp_expr)
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
            return 'g_variant_get_int32 ({})'.format(variant_expr)
        elif self.signature == 'u':
            return 'g_variant_get_uint32 ({})'.format(variant_expr)
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
            return 'reinterpret_cast<::peel::GLib::Variant *> ({})'.format(variant_expr)

    def generate_set_value_from_variant(self, value_expr, variant_expr):
        if self.signature == 'b':
            return 'g_value_set_boolean ({}, g_variant_get_boolean ({}))'.format(value_expr, variant_expr)
        elif self.signature == 'y':
            return 'g_value_set_uchar ({}, g_variant_get_byte ({}))'.format(value_expr, variant_expr)
        elif self.signature == 'n':
            return 'g_value_set_int ({}, g_variant_get_int16 ({}))'.format(value_expr, variant_expr)
        elif self.signature == 'q':
            return 'g_value_set_uint ({}, g_variant_get_uint16 ({}))'.format(value_expr, variant_expr)
        elif self.signature == 'i':
            return 'g_value_set_int ({}, g_variant_get_int32 ({}))'.format(value_expr, variant_expr)
        elif self.signature == 'u':
            return 'g_value_set_uint ({}, g_variant_get_uint32 ({}))'.format(value_expr, variant_expr)
        elif self.signature == 'x':
            return 'g_value_set_int64 ({}, g_variant_get_int64 ({}))'.format(value_expr, variant_expr)
        elif self.signature == 't':
            return 'g_value_set_uint64 ({}, g_variant_get_uint64 ({}))'.format(value_expr, variant_expr)
        elif self.signature == 'd':
            return 'g_value_set_double ({}, g_variant_get_double ({}))'.format(value_expr, variant_expr)
        elif self.signature in ('s', 'g', 'o'):
            return 'g_value_take_string ({}, g_variant_dup_string ({}, nullptr))'.format(value_expr, variant_expr)
        elif self.signature == 'as':
            return '{{ const char **strv = g_variant_get_strv ({}, nullptr); g_value_set_boxed ({}, strv); g_free (strv); }}'.format(variant_expr, value_expr)
        else:
            # TODO
            return 'g_value_set_variant ({}, {})'.format(value_expr, variant_expr)

    def generate_make_pspec(self, name, nick, blurb, flags):
        if self.signature == 'b':
            return 'g_param_spec_boolean ({}, {}, {}, FALSE, {})'.format(name, nick, blurb, flags)
        elif self.signature == 'y':
            return 'g_param_spec_uchar ({}, {}, {}, 0, 255, 0, {})'.format(name, nick, blurb, flags)
        elif self.signature == 'n':
            return 'g_param_spec_int ({}, {}, {}, G_MININT16, G_MAXINT16, 0, {})'.format(name, nick, blurb, flags)
        elif self.signature == 'q':
            return 'g_param_spec_uint ({}, {}, {}, 0, G_MAXUINT16, 0, {})'.format(name, nick, blurb, flags)
        elif self.signature == 'i':
            return 'g_param_spec_int ({}, {}, {}, G_MININT, G_MAXINT, 0, {})'.format(name, nick, blurb, flags)
        elif self.signature == 'u':
            return 'g_param_spec_uint ({}, {}, {}, 0, G_MAXUINT, 0, {})'.format(name, nick, blurb, flags)
        elif self.signature == 'x':
            return 'g_param_spec_int64 ({}, {}, {}, G_MININT64, G_MAXINT64, 0, {})'.format(name, nick, blurb, flags)
        elif self.signature == 't':
            return 'g_param_spec_uint64 ({}, {}, {}, 0, G_MAXUINT64, 0, {})'.format(name, nick, blurb, flags)
        elif self.signature == 'd':
            return 'g_param_spec_double ({}, {}, {}, -G_MAXDOUBLE, G_MAXDOUBLE, 0.0, {})'.format(name, nick, blurb, flags)
        elif self.signature in ('s', 'g', 'o'):
            return 'g_param_spec_string ({}, {}, {}, nullptr, {})'.format(name, nick, blurb, flags)
        elif self.signature == 'as':
            return 'g_param_spec_boxed ({}, {}, {}, G_TYPE_STRV, {})'.format(name, nick, blurb, flags)
        else:
            # TODO
            return 'g_param_spec_variant ({}, {}, {}, "{}", nullptr, {})'.format(name, nick, blurb, self.signature, flags)
