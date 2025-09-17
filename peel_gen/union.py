from peel_gen.constructor import Constructor
from peel_gen.defined_type import DefinedType
from peel_gen.method import Method
from peel_gen.specializations import generate_get_type_specialization, generate_ref_traits_specialization, generate_unique_traits_specialization, generate_value_traits_specialization
from peel_gen.utils import VisibilityTracker
from peel_gen.exceptions import UnsupportedForNowException
from peel_gen import api_tweaks

class Union(DefinedType):
    def __init__(self, attrs, ns):
        super().__init__(attrs, ns)
        self.get_type = attrs.get('glib:get-type', None)
        self.ref_func = None
        self.unref_func = None
        self.free_func = None
        self.ref_sink_func = None
        self.sink_func = None
        self.methods = []
        self.constructors = []
        self.pointer = attrs.get('pointer', None) == '1'
        self.is_pointer_type = False
        self.onstack = False
        assert('glib:is-gtype-struct-for' not in attrs)
        self.is_private = self.gir_name is not None and self.gir_name.endswith('Private')

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
            elif m_name == 'unref':
                self.unref_func = c_ident
            elif m_name == 'free':
                self.free_func = c_ident
            elif m_name == 'ref_sink':
                self.ref_sink_func = c_ident
            elif m_name == 'sink':
                self.sink_func = c_ident
            m = Method(attrs, self)
            self.methods.append(m)
            return m

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
            elif tweak[0] == 'free':
                self.free_func = tweak[1]
            elif tweak[0] == 'pointer':
                self.is_pointer_type = True
            elif tweak[0] == 'onstack':
                self.onstack = True

        self.is_refcounted = bool(self.ref_func)

    def is_passed_by_ref(self):
        return True

    def generate_extra_include_members(self):
        self.resolve_stuff()
        s = set()
        if self.is_private:
            return s
        for member in self.constructors + self.methods:
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
        if self.is_private:
            return s
        for member in self.constructors + self.methods:
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
        if self.is_private:
            return s
        for member in self.constructors + self.methods:
            try:
                s.update(member.generate_extra_include_at_end_members())
            except UnsupportedForNowException:
                pass
        # If self got into the set (e.g. due to being passed inside RefPtr<>),
        # get rid of it, since there's no point in including ourselves.
        s.discard(self)
        return s

    def generate_forward_decl(self, for_nested=False):
        self.resolve_stuff()
        if self.is_private or self.nested_in and not for_nested:
            return None
        return 'union {};'.format(self.own_name)

    def generate(self):
        api_tweaks.skip_if_needed(self.c_type, self.ns)

        if not self.c_type:
            raise UnsupportedForNowException('No C type')
        assert(not self.is_private)

        l = [
            'union {}'.format(self.emit_def_name),
            '{',
            'private:',
            '  ::{} inner peel_no_warn_unused;'.format(self.c_type),
        ]

        if not self.onstack:
            l.extend([
                '  {} () = delete;'.format(self.own_name),
                '  {} (const {} &) = delete;'.format(self.own_name, self.own_name),
                '  {} ({} &&) = delete;'.format(self.own_name, self.own_name),
                '  ~{} () = delete;'.format(self.own_name),
            ])

        visibility = VisibilityTracker(l, 'private')

        for nested_type in self.nested_types:
            visibility.switch('public')
            l.append('  ' + nested_type.generate_forward_decl(for_nested=True))
        if self.nested_types:
            l.append('')


        visibility.switch('public')
        for constructor in self.constructors:
            constructor.resolve_stuff()
            try:
                l.append(constructor.generate())
            except UnsupportedForNowException as e:
                l.append('  /* Unsupported for now: {}: {} */'.format(constructor.name, e.reason))
            l.append('')
        for method in self.methods:
            if method.c_ident in (self.ref_func, self.unref_func, self.ref_sink_func, self.sink_func):
                l.append('  /* {} bound as RefTraits */'.format(method.name))
                l.append('')
                continue
            elif method.c_ident == self.free_func:
                l.append('  /* {} bound as UniqueTraits */'.format(method.name))
                l.append('')
                continue
            try:
                l.append(method.generate(indent='  '))
            except UnsupportedForNowException as e:
                l.append('  /* Unsupported for now: {}: {} */'.format(method.name, e.reason))
            l.append('')

        l.extend([
            '};' + ' /* union {} */'.format(self.emit_name),
            '',
            'static_assert (sizeof ({}) == sizeof (::{}),'.format(self.emit_def_name, self.c_type),
            '               "{} size mismatch");'.format(self.emit_def_name),
            'static_assert (alignof ({}) == alignof (::{}),'.format(self.emit_def_name, self.c_type),
            '               "{} align mismatch");'.format(self.emit_def_name),
        ])
        return '\n'.join(l)

    def generate_specializations(self):
        self.resolve_stuff()
        api_tweaks.skip_if_needed(self.c_type, self.ns)
        assert(not self.is_private)
        full_name = self.emit_name_for_context(None)
        assert(self.get_type != 'intern')
        if self.get_type:
            get_type = generate_get_type_specialization(full_name, self.get_type + ' ()')
        else:
            get_type = ''
        s = get_type
        if self.is_pointer_type:
            s += '\n' + generate_value_traits_specialization(
                full_name,
                None,
                full_name + ' *',
                'r',
                'reinterpret_cast<{} *> (g_value_get_pointer (value))'.format(full_name),
                'g_value_set_pointer (value, reinterpret_cast<void *> (r))',
                None,
                None,
                support_set_marshal_return=True,
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
            constness = '' if self.ref_func or self.unref_func else 'const '
            # XXX: We assume that unions that have get_type are boxed.
            if self.ref_func or self.unref_func:
                owned_type = 'RefPtr<{}>'.format(full_name)
            elif self.free_func:
                owned_type = 'UniquePtr<{}>'.format(full_name)
            else:
                owned_type = None

            if owned_type is not None:
                dup_expr = '{}::adopt_ref (reinterpret_cast<{} *> (g_value_dup_boxed (value)))'.format(owned_type, full_name)
                take_expr = 'g_value_take_boxed (value, reinterpret_cast<const void *> (std::move (r).release_ref ()))'
            else:
                dup_expr = None
                take_expr = None

            unowned_type = constness + full_name + ' *'
            s += '\n' + generate_value_traits_specialization(
                full_name,
                owned_type,
                unowned_type,
                'r',
                'reinterpret_cast<{} *> (g_value_get_boxed (value))'.format(constness + full_name),
                'g_value_set_boxed (value, reinterpret_cast<const void *> (r))',
                dup_expr,
                take_expr,
                support_set_marshal_return=False, # TODO
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
            can_ref_null = can_unref_null = False
            for method in self.methods:
                if method.c_ident == self.ref_func:
                    can_ref_null = any(p.is_instance and p.nullable for p in method.params.params)
                elif method.c_ident == self.unref_func:
                    can_unref_null = any(p.is_instance and p.nullable for p in method.params.params)
            s += '\n\n' + generate_ref_traits_specialization(
                full_name,
                self.c_type,
                self.ref_func,
                self.unref_func,
                self.ref_sink_func,
                self.sink_func,
                template_derived=False,
                can_ref_null=can_ref_null,
                can_unref_null=can_unref_null,
            )
        elif self.free_func:
            can_free_null = False
            for method in self.methods:
                if method.c_ident == self.free_func:
                    can_free_null = any(p.is_instance and p.nullable for p in method.params.params)
            s += '\n\n' + generate_unique_traits_specialization(
                full_name,
                self.c_type,
                self.free_func,
                can_free_null=can_free_null,
            )
        return s
