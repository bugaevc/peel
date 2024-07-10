from peel_gen.defined_type import DefinedType
from peel_gen.method import Method
from peel_gen.constructor import Constructor
from peel_gen.field import Field
from peel_gen.specializations import (
    generate_get_type_specialization,
    generate_value_traits_specialization,
    generate_ref_traits_specialization,
    generate_unique_traits_specialization
)
from peel_gen.exceptions import UnsupportedForNowException
from peel_gen.utils import intern_get_type_map
from peel_gen import api_tweaks

class Record(DefinedType):
    def __init__(self, attrs, ns):
        super().__init__(attrs, ns)
        self.get_type = attrs.get('glib:get-type', None)
        self.ref_func = None
        self.unref_func = None
        self.free_func = None
        self.ref_sink_func = None
        self.fields = []
        self.methods = []
        self.constructors = []
        self.incomplete = True # Will be unset if we see any fields
        self.opaque = attrs.get('opaque', None) == '1'
        self.is_pointer_type = False
        self.onstack = False
        self.is_initially_floating = False
        self.all_fields_supported = True
        self.struct_kw = 'class /* record */'
        assert('glib:is-gtype-struct-for' not in attrs)

    def is_passed_by_ref(self):
        return True

    def can_be_allocated_by_value(self):
        return self.onstack

    def start_child_element(self, name, attrs):
        if name == 'constructor':
            c = Constructor(attrs, self)
            self.constructors.append(c)
            return c
        elif name in ('method', 'function'):
            m_name = attrs['name']
            c_ident = attrs['c:identifier']
            if m_name == 'ref':
                self.ref_func = c_ident
                return
            elif m_name == 'unref':
                self.unref_func = c_ident
                return
            elif m_name == 'free':
                self.free_func = c_ident
                return
            elif m_name == 'ref_sink':
                self.ref_sink_func = c_ident
                return
            m = Method(attrs, self)
            self.methods.append(m)
            return m
        elif name == 'field':
            self.incomplete = False
            if not self.opaque:
                self.struct_kw = 'struct'
            f = Field(attrs, self)
            self.fields.append(f)
            return f
        elif name == 'union':
            self.incomplete = False
            self.all_fields_supported = False
            if not self.opaque:
                self.struct_kw = 'struct'

    def resolve_stuff(self):
        if self.has_resolved_stuff:
            return
        super().resolve_stuff()
        for tweak in api_tweaks.lookup(self.c_type):
            if tweak[0] == 'no-autoref':
                self.ref_func = None
                self.unref_func = None
            elif tweak[0] == 'no-autofree':
                self.free_func = None
            elif tweak[0] == 'type-struct':
                self.type_struct_for_name = tweak[1]
            elif tweak[0] == 'pointer':
                self.is_pointer_type = True
            elif tweak[0] == 'onstack':
                self.onstack = True
            elif tweak[0] == 'opaque':
                self.opaque = True
            elif tweak[0] == 'float':
                self.is_initially_floating = True

        self.is_refcounted = bool(self.ref_func)
        self.all_fields_supported = self.all_fields_supported and all(f.we_support_this for f in self.fields)

    def should_generate_fields(self):
        return self.all_fields_supported and not self.opaque and not self.incomplete

    def generate_extra_include_members(self):
        self.resolve_stuff()
        s = set()
        for member in self.constructors + self.methods + (self.fields if self.should_generate_fields() else []):
            try:
                if self.opaque and isinstance(member, Field):
                    continue
                s.update(member.generate_extra_include_members())
            except UnsupportedForNowException:
                pass
        if self.nested_in:
            s.add(self.nested_in)
        # If self got into the set (e.g. due to being passed inside RefPtr<>),
        # get rid of it, since there's no point in including ourselves.
        s.discard(self)
        for nested_type in self.nested_types:
            s.discard(nested_type)
        return s

    def generate_extra_forward_members(self):
        self.resolve_stuff()
        s = set()
        for member in self.constructors + self.methods + (self.fields if self.should_generate_fields() else []):
            try:
                if self.opaque and isinstance(member, Field):
                    continue
                s.update(member.generate_extra_forward_members())
            except UnsupportedForNowException:
                pass
        s.discard(self)
        for nested_type in self.nested_types:
            s.discard(nested_type)
        return s

    def generate_extra_include_at_end_members(self):
        self.resolve_stuff()
        s = set()
        for member in self.constructors + self.methods + (self.fields if self.should_generate_fields() else []):
            try:
                if self.opaque and isinstance(member, Field):
                    continue
                s.update(member.generate_extra_include_at_end_members())
            except UnsupportedForNowException:
                pass
        if self.nested_in:
            s.add(self.nested_in)
        s.discard(self)
        for nested_type in self.nested_types:
            s.discard(nested_type)
        return s

    def generate_forward_decl(self, for_nested=False):
        api_tweaks.skip_if_needed(self.c_type, self.ns)
        if self.nested_in and not for_nested:
            return None
        return '{} {};'.format(self.struct_kw, self.own_name)

    def generate(self):
        api_tweaks.skip_if_needed(self.c_type, self.ns)
        assert(not (self.onstack and self.incomplete))

        l = [
            '{} {}'.format(self.struct_kw, self.emit_def_name),
            '{',
            'private:',
        ]
        s = api_tweaks.ifdef_for_non_opaque(self.c_type)
        if s:
            l.append(s)
        if (self.opaque or not self.all_fields_supported) and not self.incomplete:
            l.extend([
                '  ::{} inner peel_no_warn_unused;'.format(self.c_type),
                ''
            ])
        s = api_tweaks.endif_for_non_opaque(self.c_type)
        if s:
            l.append(s)
        if not self.onstack:
            l.extend([
                '  {} () = delete;'.format(self.own_name),
                '  {} (const {} &) = delete;'.format(self.own_name, self.own_name),
                '  {} ({} &&) = delete;'.format(self.own_name, self.own_name),
                '  ~{} ();'.format(self.own_name),
            ])
        l.extend([
            '',
            'public:'
        ])
        for nested_type in self.nested_types:
            l.append('  ' + nested_type.generate_forward_decl(for_nested=True))
        if self.nested_types:
            l.append('')
        if self.should_generate_fields():
            for field in self.fields:
                l.append(field.generate())
            if self.fields:
                l.append('')
        for constructor in self.constructors:
            constructor.resolve_stuff()
            try:
                l.append(constructor.generate())
            except UnsupportedForNowException as e:
                l.append('  /* Unsupported for now: {}: {} */'.format(constructor.name, e.reason))
            l.append('')
        for method in self.methods:
            try:
                l.append(method.generate(indent='  '))
            except UnsupportedForNowException as e:
                l.append('  /* Unsupported for now: {}: {} */'.format(method.name, e.reason))
            l.append('')
        if not l[-1]:
            l.pop()
        l.append('};' + ' /* record {} */'.format(self.emit_name))
        s = api_tweaks.ifdef_for_non_opaque(self.c_type)
        if not self.incomplete:
            l.append('')
        if s:
            l.append(s)
        if not self.incomplete:
            l.extend([
                'static_assert (sizeof ({}) == sizeof (::{}),'.format(self.emit_def_name, self.c_type),
                '               "{} size mismatch");'.format(self.emit_def_name),
                'static_assert (alignof ({}) == alignof (::{}),'.format(self.emit_def_name, self.c_type),
                '               "{} align mismatch");'.format(self.emit_def_name),
            ])
        s = api_tweaks.endif_for_non_opaque(self.c_type)
        if s:
            l.append(s)

        return '\n'.join(l)

    def generate_specializations(self):
        self.resolve_stuff()
        api_tweaks.skip_if_needed(self.c_type, self.ns)
        full_name = self.emit_name_for_context(None)
        if self.get_type == 'intern':
            get_type = generate_get_type_specialization(full_name, intern_get_type_map[self.gir_name])
        elif self.get_type:
            get_type = generate_get_type_specialization(full_name, self.get_type + ' ()')
        else:
            get_type = ''
        s = get_type
        if self.is_pointer_type:
            s += '\n' + generate_value_traits_specialization(
                full_name,
                full_name + '*',
                'r',
                'reinterpret_cast<{} *> (g_value_get_pointer (value))'.format(full_name),
                'g_value_set_pointer (value, reinterpret_cast<void *> (r))',
            )
            s += '\n\n' + '\n'.join([
                'template<>',
                'struct peel::internals::PspecTraits<{}>'.format(full_name),
                '{',
                '  constexpr PspecTraits ()',
                '  { }',
                '',
                '  ::GParamSpec *',
                '  create_pspec (PspecBasics basics)',
                '  {',
                '    return g_param_spec_pointer (basics.name, basics.nick, basics.blurb, basics.flags);',
                '  }',
                '};',
            ])
        elif self.get_type:
            # XXX: We assume that structures that have get_type are boxed
            s += '\n' + generate_value_traits_specialization(
                full_name,
                full_name + '*',
                'r',
                'reinterpret_cast<{} *> (g_value_get_boxed (value))'.format(full_name),
                'g_value_set_boxed (value, reinterpret_cast<const void *> (r))',
            )
            s += '\n\n' + '\n'.join([
                'template<>',
                'struct peel::internals::PspecTraits<{}>'.format(full_name),
                '{',
                '  constexpr PspecTraits ()',
                '  { }',
                '',
                '  ::GParamSpec *',
                '  create_pspec (PspecBasics basics)',
                '  {',
                '    return g_param_spec_boxed (basics.name, basics.nick, basics.blurb, GObject::Type::of<{}> (), basics.flags);'.format(full_name),
                '  }',
                '};',
            ])
        if self.ref_func or self.unref_func:
            assert(not self.free_func)
            s += '\n\n' + generate_ref_traits_specialization(
                full_name,
                self.c_type,
                self.ref_func,
                self.unref_func,
                self.ref_sink_func,
                template_derived=False,
            )
        elif self.free_func:
            s += '\n\n' + generate_unique_traits_specialization(full_name, self.c_type, self.free_func)
        return s

