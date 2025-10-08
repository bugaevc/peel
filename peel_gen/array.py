from peel_gen.node_handler import NodeHandler
from peel_gen.type import AnyType, lookup_type
from peel_gen.utils import is_type_element

# Not a defined type!
class Array(NodeHandler, AnyType):
    def __init__(self, attrs, ns):
        self.ns = ns
        self.fixed_size = attrs.get('fixed-size', None)
        self.length = attrs.get('length', None)
        zero_terminated = attrs.get('zero-terminated', None)
        if zero_terminated is not None:
            self.zero_terminated = zero_terminated == '1'
        elif self.length is not None or self.fixed_size is not None:
            self.zero_terminated = False
        else:
            # Defaults to true for arrays, see giscanner/girwriter.py:GIRWriter._write_type
            self.zero_terminated = True
        self.item_type_name = None
        self.item_c_type = None
        self.item_type = None
        self.has_resolved_stuff = False

    def __repr__(self):
        return 'Array(item_type_name={})'.format(self.item_type_name)

    def start_child_element(self, name, attrs):
        if is_type_element(name, attrs):
            self.item_type_name = attrs['name']
            self.item_c_type = attrs.get('c:type', None)
        elif name == 'array':
            self.item_type = Array(attrs, ns=self.ns)
            self.item_c_type = attrs.get('c:type', None)
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

