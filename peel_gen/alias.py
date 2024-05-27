from peel_gen.defined_type import DefinedType
from peel_gen.type import lookup_type

__all__ = ('Alias', 'chase_type_aliases')

class Alias(DefinedType):
    def __init__(self, attrs, ns):
        super().__init__(attrs, ns)
        self.aliased_name = None
        self.aliased_c_type = None
        self.aliased_type = None

    def start_child_element(self, name, attrs):
        if name == 'type':
            self.aliased_name = attrs['name']
            self.aliased_c_type = attrs.get('c:type', None)

    def resolve_stuff(self):
        if self.has_resolved_stuff:
            return
        super().resolve_stuff()
        if self.aliased_type is not None:
            return
        self.aliased_type = lookup_type(self.aliased_name, self.ns)

    def generate(self):
        assert(not self.nested_types)
        return '/* using {} = ::{}; */'.format(self.name, self.c_type)

def chase_type_aliases(tp):
    while isinstance(tp, Alias):
        tp.resolve_stuff()
        tp = tp.aliased_type
    return tp
