from peel_gen.node_handler import NodeHandler
from peel_gen.type import AnyType, lookup_type
from peel_gen.array import Array
from peel_gen.utils import is_type_element

# Not a defined type! There is a GLib.List record, but this class represents
# a specific mention / instantiation of it, and not the original one.
class List(NodeHandler, AnyType):
    def __init__(self, type_name, ns):
        self.ns = ns
        self.type_name = type_name
        self.item_c_type = None
        self.item_type_name = None
        self.item_type = None
        self.has_resolved_stuff = False

    def is_list_type(ns, type_name):
        # TODO: add HashTable
        if ns == 'GLib':
            return type_name in ('List', 'SList')
        return type_name in ('GLib.List', 'GLib.SList')

    def underlying_type(self):
        return lookup_type(self.type_name, 'GLib')

    def emit_name_for_context(self, context, ownership, item_cpp_type=None):
        name = self.underlying_type().emit_name_for_context(context)
        if ownership in ('none', None):
            name = name + 'Ref'
            if '*' in item_cpp_type:
                item_cpp_type = item_cpp_type + ' const'
            else:
                item_cpp_type = 'const ' + item_cpp_type
        if item_cpp_type:
            return name + '<' + item_cpp_type + '>'
        else:
            return name

    def __repr__(self):
        return 'List(type_name={}, item_type_name={})'.format(self.type_name, self.item_type_name)

    def start_child_element(self, name, attrs):
        if is_type_element(name, attrs):
            self.item_type_name = attrs['name']
            self.item_c_type = attrs.get('c:type', None)
            if List.is_list_type(self.ns, self.item_type_name):
                # List of lists?
                self.item_type = List(self.item_type_name, self.ns)
                return self.item_type
        elif name == 'array':
            self.item_type = Array(attrs, ns=self.ns)
            self.item_c_type = attrs.get('c:type', None)
            return self.item_type

    def resolve_stuff(self):
        if self.has_resolved_stuff:
            return
        self.has_resolved_stuff = True
        if self.item_type is None:
            self.item_type = lookup_type(self.item_type_name, self.ns)
            assert(self.item_type is not None)
        self.item_type.resolve_stuff()
