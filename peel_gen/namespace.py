from peel_gen.node_handler import NodeHandler
from peel_gen.alias import Alias
from peel_gen.constant import Constant
from peel_gen.bitfield import Bitfield
from peel_gen.enumeration import Enumeration
from peel_gen.type_struct import TypeStruct
from peel_gen.record import Record
from peel_gen.union import Union
from peel_gen.callback import Callback
from peel_gen.klass import Class
from peel_gen.interface import Interface
from peel_gen.function import Function
from peel_gen import api_tweaks

__all__ = ('Namespace', 'raw_namespace_names')

raw_namespace_names = []

class Namespace(NodeHandler):
    def __init__(self, attrs):
        self.name = attrs['name']
        self.emit_raw = self.name in raw_namespace_names
        for tweak in api_tweaks.lookup(self.name, 'raw'):
            self.emit_raw = True
            raw_namespace_names.append(self.name)
        self.shared_library = attrs.get('shared-library', None)
        self.members = []
        self.c_includes = []

    def __repr__(self):
        return 'Namespace({})'.format(self.name)

    def start_child_element(self, name, attrs):
        if name == 'alias':
            a = Alias(attrs, ns=self)
            self.members.append(a)
            return a
        elif name == 'constant':
            c = Constant(attrs)
            self.members.append(c)
            return c
        elif name == 'bitfield':
            b = Bitfield(attrs, ns=self)
            self.members.append(b)
            return b
        elif name == 'enumeration':
            e = Enumeration(attrs, ns=self)
            self.members.append(e)
            return e
        elif name == 'record':
            if 'glib:is-gtype-struct-for' in attrs:
                return TypeStruct(attrs, ns=self)
            for tweak in api_tweaks.lookup(attrs['c:type'], 'type-struct-for'):
                return TypeStruct(attrs, ns=self, type_struct_for=tweak[1])
            r = Record(attrs, ns=self)
            self.members.append(r)
            return r
        elif name == 'union':
            u = Union(attrs, ns=self)
            self.members.append(u)
            return u
        elif name == 'callback':
            c = Callback(attrs, ns=self)
            self.members.append(c)
            return c
        elif name == 'class':
            c = Class(attrs, ns=self)
            self.members.append(c)
            return c
        elif name == 'interface':
            i = Interface(attrs, ns=self)
            self.members.append(i)
            return i
        elif name == 'function':
            f = Function(attrs, ns=self)
            self.members.append(f)
            return f

    def end_element(self):
        from peel_gen.type import AnyType
        for member in self.members:
            if isinstance(member, AnyType):
                member.resolve_stuff()

    def should_emit_file(self, member):
        if not isinstance(member, (Class, Interface, Record, Enumeration, Bitfield, Union)):
            return False
        if isinstance(member, Record) and member.is_private:
            return False
        if api_tweaks.should_skip(member.c_type, ns=self, keep_manual=True):
            return False
        return True
