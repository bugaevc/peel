from peel_gen.function_like import FunctionLike
from peel_gen.utils import escape_cpp_name
from peel_gen import c_function_wrapper
from peel_gen import api_tweaks

class Function(FunctionLike):
    def __init__(self, attrs, ns):
        super().__init__(attrs, ns)
        self.c_ident = attrs['c:identifier']
        self.tweak_ident = self.c_ident
        self.moved_to = attrs.get('moved-to', None)

    def __repr__(self):
        return 'Function({}.{}, c_ident={})'.format(self.ns.name, self.name, self.c_ident)

    def generate_extra_include_members(self):
        if self.moved_to is not None:
            return set()
        return super().generate_extra_include_members()

    def generate_extra_forward_members(self):
        if self.moved_to is not None:
            return set()
        return super().generate_extra_forward_members()

    def generate_extra_include_at_end_members(self):
        if self.moved_to is not None:
            return set()
        return super().generate_extra_forward_members()

    def generate(self):
        if self.moved_to is not None:
            return '/* {} moved to {} */'.format(self.name, self.moved_to)
        api_tweaks.skip_if_needed(self.tweak_ident)

        return c_function_wrapper.generate(
            name=escape_cpp_name(self.name),
            c_callee=self.c_ident,
            context=self.ns,
            rv=self.rv,
            params=self.params,
            throws=self.throws,
            indent='',
            attributes=['peel_no_warn_unused'],
        )

