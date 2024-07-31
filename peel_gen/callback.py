from peel_gen.function_like import FunctionLike
from peel_gen.defined_type import DefinedType
from peel_gen import api_tweaks

class Callback(FunctionLike, DefinedType):
    def __init__(self, attrs, ns):
        DefinedType.__init__(self, attrs, ns)
        FunctionLike.__init__(self, attrs, ns)
        self.tweak_ident = self.c_type
        self.force_cpp_wrapper = False

    def resolve_stuff(self):
        if self.has_resolved_stuff:
            return
        DefinedType.resolve_stuff(self)
        self.has_resolved_stuff = False
        FunctionLike.resolve_stuff(self)

        for tweak in api_tweaks.lookup(self.tweak_ident):
            if tweak[0] == 'cpp-wrapper':
                self.force_cpp_wrapper = True
