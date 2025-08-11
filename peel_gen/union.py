from peel_gen.defined_type import DefinedType
from peel_gen.utils import VisibilityTracker
from peel_gen.exceptions import UnsupportedForNowException
from peel_gen import api_tweaks

class Union(DefinedType):
    def __init__(self, attrs, ns):
        super().__init__(attrs, ns)

    def is_passed_by_ref(self):
        return True

    def generate_extra_include_members(self):
        self.resolve_stuff()
        s = set()
        if self.nested_in:
            s.add(self.nested_in)
        return s

    def generate_extra_forward_members(self):
        self.resolve_stuff()
        return set(self.nested_type_aliases)

    def generate_extra_include_at_end_members(self):
        return set()

    def generate_forward_decl(self, for_nested=False):
        self.resolve_stuff()
        return 'union {};'.format(self.emit_name)

    def generate(self):
        api_tweaks.skip_if_needed(self.c_type, self.ns)
        if not self.c_type:
            raise UnsupportedForNowException('No C type')
        l = [
            'union {}'.format(self.emit_def_name),
            '{',
            'private:',
            '  ::{} inner peel_no_warn_unused;'.format(self.c_type),
            '  {} () = delete;'.format(self.own_name),
            '  {} (const {} &) = delete;'.format(self.own_name, self.own_name),
            '  {} ({} &&) = delete;'.format(self.own_name, self.own_name),
            '  ~{} () = delete;'.format(self.own_name),
        ]
        visibility = VisibilityTracker(l, 'private')
        for nested_type in self.nested_types:
            visibility.switch('public')
            l.append('  ' + nested_type.generate_forward_decl(for_nested=True))
        if self.nested_types:
            l.append('')

        # TODO: Constructors, methods...

        l.extend([
            '};' + ' /* union {} */'.format(self.emit_name),
            '',
            'static_assert (sizeof ({}) == sizeof (::{})),'.format(self.emit_def_name, self.c_type),
            '               "{} size mismatch");'.format(self.emit_def_name),
            'static_assert (alignof ({}) == alignof (::{})),'.format(self.emit_def_name, self.c_type),
            '               "{} align mismatch");'.format(self.emit_def_name),
        ])
        return '\n'.join(l)

    def generate_specializations(self):
        return None
