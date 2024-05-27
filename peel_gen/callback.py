from peel_gen.function_like import FunctionLike
from peel_gen.defined_type import DefinedType

class Callback(FunctionLike, DefinedType):
    def __init__(self, attrs, ns):
        DefinedType.__init__(self, attrs, ns)
        FunctionLike.__init__(self, attrs, ns)
        self.tweak_ident = self.c_type
