from pathlib import Path

from peel_gen.type import AnyType, lookup_type, type_map
from peel_gen.node_handler import NodeHandler
from peel_gen.utils import massage_c_type
from peel_gen import api_tweaks

class DefinedType(NodeHandler, AnyType):
    def __init__(self, attrs, ns):
        self.ns = ns
        self.gir_name = attrs['name']
        if 'peel-fake-defined-type' not in attrs:
            full_gir_name = ns.name + '.' + self.gir_name
            assert(full_gir_name not in type_map)
            type_map[full_gir_name] = self
        self.c_type = attrs.get('c:type', None)
        if self.c_type is None:
            self.c_type = attrs.get('glib:type-name', None)
        if self.c_type is not None:
            self.c_type = massage_c_type(self.c_type)
        self.deprecated = attrs.get('deprecated', None) == '1'

        self.emit_name = self.emit_def_name = self.own_name = self.gir_name
        self.nested_in_name = None
        self.nested_in_alias_name = None
        self.nested_in = None
        self.nested_in_alias = None
        self.nested_types = []
        self.nested_type_aliases = []
        if self.c_type is not None:
            for tweak in api_tweaks.lookup(self.c_type, 'nest'):
                self.emit_name = self.emit_def_name = tweak[1]
                self.nested_in_name, self.own_name = self.emit_name.rsplit('::', 1)
                self.nested_in_name = self.nested_in_name.replace('::', '')
            for tweak in api_tweaks.lookup(self.c_type, 'nest-alias'):
                self.emit_name = tweak[1]
                self.nested_in_alias_name, not_own_name = self.emit_name.rsplit('::', 1)
                self.nested_in_alias_name = self.nested_in_alias_name.replace('::', '')
        self.has_resolved_stuff = False

    def __repr__(self):
        return '{}({}.{}, c_type={})'.format(
            type(self).__name__,
            self.ns.name,
            self.gir_name,
            self.c_type,
        )

    def resolve_stuff(self):
        if self.has_resolved_stuff:
            return
        self.has_resolved_stuff = True
        # Look up the type we're nested in.
        if self.nested_in_name:
            # GEnumClass / GFlagsClass are special in the sense that the
            # instance types don't exist in .gir and resolution would fail
            # later, so we create dummy types here now
            if self.c_type == 'GEnumClass':
                attrs = dict()
                attrs['name'] = 'Enum'
                attrs['c:type'] = 'GEnum'
                attrs['peel-fake-defined-type'] = '1'
                self.nested_in = DefinedType(attrs, self.ns)
            elif self.c_type == 'GFlagsClass':
                attrs = dict()
                attrs['name'] = 'Flags'
                attrs['c:type'] = 'GFlags'
                attrs['peel-fake-defined-type'] = '1'
                self.nested_in = DefinedType(attrs, self.ns)
            else:
                self.nested_in = lookup_type(self.nested_in_name, self.ns)
            self.nested_in.nested_types.append(self)
        elif self.nested_in_alias_name:
            self.nested_in_alias = lookup_type(self.nested_in_alias_name, self.ns)
            self.nested_in_alias.nested_type_aliases.append(self)

    def emit_name_for_context(self, ns_or_tp):
        from peel_gen.namespace import Namespace
        from peel_gen.type_struct import TypeStruct

        if ns_or_tp is None:
            return self.ns.name + '::' + self.emit_def_name
        if isinstance(ns_or_tp, Namespace):
            if ns_or_tp is not self.ns:
                return self.ns.name + '::' + self.emit_name
            return self.emit_name
        tp = ns_or_tp
        if isinstance(tp, TypeStruct):
            tp = tp.type_struct_for
        assert(isinstance(tp, DefinedType))
        if tp.ns is not self.ns:
            return self.ns.name + '::' + self.emit_name
        while tp is not None:
            for nested_type in tp.nested_types:
                if nested_type.own_name == self.emit_name:
                    return self.ns.name + '::' + self.emit_name
            tp = tp.nested_in
        return self.emit_name

    def make_file_path(self):
        name = self.emit_name.replace('::', '')
        return (Path('peel') / self.ns.name / name).with_suffix('.h').as_posix()

    def generate_nested_type_defs(self):
        l = []
        for nested_type in self.nested_types:
            l.append('  ' + nested_type.generate_forward_decl(for_nested=True))
        for nested_type in self.nested_type_aliases:
            alias_name = nested_type.emit_name.split('::')[-1]
            l.append('  typedef {} {};'.format(nested_type.own_name, alias_name))
        if self.nested_types or self.nested_type_aliases:
            l.append('')
        return '\n'.join(l)
