from peel_gen.function_like import FunctionLike
from peel_gen.utils import escape_cpp_name
from peel_gen import c_function_wrapper
from peel_gen import api_tweaks

class Method(FunctionLike):
    def __init__(self, attrs, containing_type):
        super().__init__(attrs, ns=containing_type.ns)
        self.containing_type = containing_type
        self.c_ident = attrs['c:identifier']
        self.tweak_ident = self.c_ident

    def __repr__(self):
        return 'Method({}.{}, c_ident={})'.format(self.containing_type, self.name, self.c_ident)

    def generate(self, indent):
        api_tweaks.skip_if_needed(self.c_ident, self.ns)
        l = []
        s = api_tweaks.ifdef_if_needed(self.c_ident)
        if s:
            l.append(s)
        l.append(c_function_wrapper.generate(
            name=escape_cpp_name(self.name),
            c_callee=self.c_ident,
            context=self.containing_type,
            rv=self.rv,
            params=self.params,
            throws=self.throws,
            indent=indent,
        ))
        s = api_tweaks.endif_if_needed(self.c_ident)
        if s:
            l.append(s)
        return '\n'.join(l)

