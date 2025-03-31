from peel_gen.node_handler import NodeHandler
from peel_gen.type import AnyType, lookup_type

# Not a defined type!
class Array(NodeHandler, AnyType):
    def __init__(self, attrs, ns):
        self.ns = ns
        self.fixed_size = attrs.get('fixed-size', None)
        self.length = attrs.get('length', None)
        self.zero_terminated = attrs.get('zero-terminated', '0') == '1'
        self.item_type_name = None
        self.item_type = None
        self.has_resolved_stuff = False

    def __repr__(self):
        return 'Array(item_type_name={})'.format(self.item_type_name)

    def start_child_element(self, name, attrs):
        if name == 'type':
            self.item_type_name = attrs['name']
        elif name == 'array':
            self.item_type = Array(attrs, ns=self.ns)
            return self.item_type

    def is_passed_by_ref(self):
        return True

    def can_be_allocated_by_value(self):
        # return self.fixed_size is not None
        return True

    def resolve_stuff(self):
        if self.has_resolved_stuff:
            return
        self.has_resolved_stuff = True
        if self.item_type is None:
            self.item_type = lookup_type(self.item_type_name, self.ns)
            assert(self.item_type is not None)
        self.item_type.resolve_stuff()

