from peel_gen import api_tweaks
from peel_gen.defined_type import DefinedType
from peel_gen.type import lookup_type, VoidType, VoidAliasType
from peel_gen.utils import is_type_element

__all__ = ('Alias', 'chase_type_aliases')

class Alias(DefinedType):
    def __init__(self, attrs, ns):
        super().__init__(attrs, ns)
        self.aliased_name = None
        self.aliased_c_type = None
        self.aliased_type = None
        self.newtype = None

    def start_child_element(self, name, attrs):
        if is_type_element(name, attrs):
            self.aliased_name = attrs['name']
            self.aliased_c_type = attrs.get('c:type', None)
        # TODO: can we reference arrays this way?

    def resolve_stuff(self):
        if self.has_resolved_stuff:
            return
        super().resolve_stuff()
        if self.aliased_type is not None:
            return
        self.aliased_type = lookup_type(self.aliased_name, self.ns)
        if isinstance(self.aliased_type, VoidType):
            self.aliased_type = VoidAliasType(self)

        for tweak in api_tweaks.lookup(self.c_type):
            if tweak[0] == 'newtype':
                self.newtype = tweak[1]

    def generate_forward_decl(self, for_nested=False):
        if self.nested_in and not for_nested:
            return None
        if self.newtype is not None:
            return '{} {};'.format(self.newtype, self.own_name)

        # Aliases are not generated at all right now and resolved
        # to their final type
        return None

    def generate(self):
        assert(not self.nested_types)
        return '/* using {} = ::{}; */'.format(self.name, self.c_type)

def chase_type_aliases(tp):
    while isinstance(tp, Alias):
        for tweak in api_tweaks.lookup(tp.c_type, 'skip'):
            if len(tweak) > 1 and tweak[1] != tp.ns.name:
                continue
            if len(tweak) > 2 and tweak[2] == 'manual':
                return tp
        tp.resolve_stuff()
        tp = tp.aliased_type
    return tp
