from peel_gen.defined_type import DefinedType

class Union(DefinedType):
    def __init__(self, attrs, ns):
        super().__init__(attrs, ns)

    def generate_forward_decl(self):
        return 'union {};'.format(self.emit_name)
