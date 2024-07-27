from peel_gen.defined_type import DefinedType
from peel_gen.node_handler import NodeHandler
from peel_gen.utils import escape_cpp_name
from peel_gen.specializations import generate_get_type_specialization, generate_value_traits_specialization
from peel_gen import api_tweaks

class Enumeration(DefinedType):
    def __init__(self, attrs, ns):
        super().__init__(attrs, ns)
        self.get_type = attrs.get('glib:get-type', None)
        self.members = []

    def start_child_element(self, name, attrs):
        if name == 'member':
            m = EnumMember(attrs)
            self.members.append(m)
            return m

    def resolve_stuff(self):
        if self.has_resolved_stuff:
            return
        super().resolve_stuff()

    def generate_extra_include_members(self):
        self.resolve_stuff()
        if self.nested_in:
            return { self.nested_in }
        return set()

    def generate_extra_forward_members(self):
        return set()

    def generate_extra_include_at_end_members(self):
        return set()

    def generate_forward_decl(self, for_nested=False):
        if self.nested_in and not for_nested:
            return None
        return 'enum class {} : std::underlying_type<::{}>::type;'.format(self.own_name, self.c_type)

    def generate(self):
        api_tweaks.skip_if_needed(self.c_type, self.ns)
        l = [
            'enum class {} : std::underlying_type<::{}>::type'.format(self.emit_def_name, self.c_type),
            '{'
        ]
        for member in self.members:
            l.append(member.generate())
        l.append('}}; /* enum {} */'.format(self.emit_name))
        return '\n'.join(l)

    def generate_specializations(self):
        api_tweaks.skip_if_needed(self.c_type, self.ns)
        type_name = self.emit_name_for_context(None)
        l = [generate_value_traits_specialization(
            type_name,
            type_name,
            'm',
            'static_cast<{}> (g_value_get_enum (value))'.format(type_name),
            'g_value_set_enum (value, static_cast<::{}> (m))'.format(self.c_type),
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

