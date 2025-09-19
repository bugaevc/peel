from peel_gen.defined_type import DefinedType
from peel_gen.function import Function
from peel_gen.node_handler import NodeHandler
from peel_gen.utils import escape_cpp_name, strip_c_symbol_prefix
from peel_gen.specializations import generate_get_type_specialization, generate_value_traits_specialization
from peel_gen.exceptions import UnsupportedForNowException
from peel_gen import api_tweaks

class Enumeration(DefinedType):
    def __init__(self, attrs, ns):
        super().__init__(attrs, ns)
        self.get_type = attrs.get('glib:get-type', None)
        self.members = []
        self.methods = []

    def start_child_element(self, name, attrs):
        if name == 'member':
            m = EnumMember(attrs)
            # See https://gitlab.gnome.org/GNOME/gobject-introspection/-/issues/264
            if any(m2.name == m.name for m2 in self.members):
                return
            self.members.append(m)
            return m
        elif name == 'function':
            f = Function(attrs, self.ns)
            # Prefix the function name with the type name as enum classes
            # don't support methods and this has to become a plain function.
            f.name = strip_c_symbol_prefix(f.c_ident, self.ns)
            self.methods.append(f)
            return f

    def resolve_stuff(self):
        if self.has_resolved_stuff:
            return
        super().resolve_stuff()

    def generate_extra_include_members(self):
        self.resolve_stuff()
        s = set()
        for member in self.methods:
            try:
                s.update(member.generate_extra_include_members())
            except UnsupportedForNowException:
                pass
        if self.nested_in:
            s.add(self.nested_in)
        # If self got into the set due to a nested type being mentioned,
        # get rid of it, since there's no point in including ourselves.
        s.discard(self)
        for nested_type in self.nested_types:
            assert(nested_type not in s)
        return s

    def generate_extra_forward_members(self):
        self.resolve_stuff()
        s = set()
        for member in self.methods:
            try:
                s.update(member.generate_extra_forward_members())
            except UnsupportedForNowException:
                pass
        # We already forward-declare self and nested types.
        s.discard(self)
        for nested_type in self.nested_types:
            s.discard(nested_type)
        # Forward-declare the underlying type for nested type aliases.
        for nested_type in self.nested_type_aliases:
            s.add(nested_type)
        return s

    def generate_extra_include_at_end_members(self):
        self.resolve_stuff()
        s = set()
        for member in self.methods:
            try:
                s.update(member.generate_extra_include_at_end_members())
            except UnsupportedForNowException:
                pass
        return s

    def generate_forward_decl(self, for_nested=False):
        if self.nested_in and not for_nested:
            return None
        return 'enum class {} : std::underlying_type<::{}>::type;'.format(self.own_name, self.c_type)

    def generate(self):
        assert(not self.nested_types)
        assert(not self.nested_type_aliases)
        api_tweaks.skip_if_needed(self.c_type, self.ns)
        l = [
            'enum class {} : std::underlying_type<::{}>::type'.format(self.emit_def_name, self.c_type),
            '{'
        ]
        for member in self.members:
            l.append(member.generate())
        l.append('}}; /* enum {} */'.format(self.emit_name))
        l.append('')

        for method in self.methods:
            try:
                l.append(method.generate())
            except UnsupportedForNowException as e:
                l.append('/* Unsupported for now: {}: {} */'.format(method.name, e.reason))
            l.append('')

        return '\n'.join(l)

    def generate_specializations(self):
        api_tweaks.skip_if_needed(self.c_type, self.ns)
        type_name = self.emit_name_for_context(None)
        l = [generate_value_traits_specialization(
            type_name,
            None,
            type_name,
            'm',
            'static_cast<{}> (g_value_get_enum (value))'.format(type_name),
            'g_value_set_enum (value, static_cast<::{}> (m))'.format(self.c_type),
            None,
            None,
            support_set_marshal_return=True,
        )]
        if self.get_type:
            l.append(generate_get_type_specialization(type_name, self.get_type + ' ()'))
            l.extend([
                'template<>',
                'struct peel::internals::PspecTraits<{}>'.format(type_name),
                '{',
                '  {} default_value;'.format(type_name),
                '',
                '  constexpr PspecTraits ({} default_value)'.format(type_name),
                '    : default_value (default_value)',
                '  { }',
                '',
                '  ::GParamSpec *',
                '  create_pspec (PspecBasics basics)',
                '  {',
                '    return g_param_spec_enum (basics.name, basics.nick, basics.blurb,',
                '                              {} (),'.format(self.get_type),
                '                              static_cast<::{}> (default_value),'.format(self.c_type),
                '                              basics.flags);',
                '  }',
                '};',
            ])

        return '\n'.join(l)

class EnumMember(NodeHandler):
    def __init__(self, attrs):
        self.name = attrs['name']
        self.c_ident = attrs.get('c:identifier', None)
        if self.c_ident is None:
            self.c_ident = attrs['glib:name']
        self.nick = attrs.get('glib:nick', None)

    def generate(self):
        return '  {} = {},'.format(escape_cpp_name(self.name.upper()), self.c_ident)

