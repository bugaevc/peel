from peel_gen.defined_type import DefinedType
from peel_gen.specializations import generate_get_type_specialization, generate_value_traits_specialization

class Boxed(DefinedType):
    def __init__(self, attrs, ns):
        attrs = dict(attrs)
        if 'name' not in attrs and 'glib:name' in attrs:
            attrs['name'] = attrs['glib:name']
        super().__init__(attrs, ns)
        self.get_type = attrs.get('glib:get-type', None)
        # TODO: support everything else by deriving from RecordLike

    def is_passed_by_ref(self):
        return True

    def generate_extra_include_members(self):
        self.resolve_stuff()
        s = set()
        if self.nested_in:
            s.add(self.nested_in)
        return s

    def generate_extra_forward_members(self):
        return set()

    def generate_extra_include_at_end_members(self):
        return set()

    def generate_forward_decl(self, for_nested=False):
        self.resolve_stuff()
        if self.nested_in and not for_nested:
            return False
        return 'class /* boxed */ {};'.format(self.own_name)

    def generate(self):
        l = [
            'class /* boxed */ {}'.format(self.emit_def_name),
            '{',
            'private:',
            '  {} () = delete;'.format(self.own_name),
            '  ~{} ();'.format(self.own_name),
            '};',
        ]
        return '\n'.join(l)

    def generate_specializations(self):
        self.resolve_stuff()
        full_name = self.emit_name_for_context(None)
        l = [
            generate_get_type_specialization(full_name, self.get_type + ' ()'),
            '',
            # TODO: this uses UniquePtr, but we should instead try to detect if the type is refcounted
            'template<>',
            'struct UniqueTraits<{}>'.format(full_name),
            '{',
            '  static void',
            '  free ({} *ptr)'.format(full_name),
            '  {',
            '    g_boxed_free ({} (), ptr);'.format(self.get_type),
            '  }',
            '',
            '  constexpr static',
            '  bool can_free_null = false;',
            '};',
            '',
            'template<>',
            'struct peel::internals::PspecTraits<{}>'.format(full_name),
            '{',
            '  constexpr PspecTraits ()',
            '  { }',
            '',
            '  ::GParamSpec *',
            '  create_pspec (PspecBasics basics)',
            '  {',
            '    return g_param_spec_boxed (basics.name, basics.nick, basics.blurb, {} (), basics.flags);'.format(self.get_type),
            '  }',
            '};',
            '',
            generate_value_traits_specialization(
                cpp_type=full_name,
                owned_type='UniquePtr<{}>'.format(full_name),
                unowned_type='const {} *'.format(full_name),
                arg_name='r',
                get_expr='reinterpret_cast<const {} *> (g_value_get_boxed (value))'.format(full_name),
                set_expr='g_value_set_boxed (value, r)'.format(full_name),
                dup_expr='UniquePtr<{}>::adopt_ref (reinterpret_cast<{} *> (g_value_dup_boxed (value)))'.format(full_name, full_name),
                take_expr='g_value_take_boxed (value, std::move (r).release_ref ())',
                support_set_marshal_return=False, # TODO
            ),
        ]
        return '\n'.join(l)
