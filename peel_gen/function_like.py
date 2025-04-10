from peel_gen.node_handler import NodeHandler
from peel_gen.array import Array
from peel_gen.type import lookup_type
from peel_gen.alias import chase_type_aliases
from peel_gen import api_tweaks

class FunctionLike(NodeHandler):
    def __init__(self, attrs, ns):
        self.ns = ns
        self.name = attrs['name']
        self.throws = attrs.get('throws', '0') != '0'
        self.params = None
        self.rv = None
        self.visibility = 'public'
        self.extra_includes = []
        self.has_resolved_stuff = False

    def start_child_element(self, name, attrs):
        if name == 'return-value':
            from peel_gen.parameter import Parameter
            self.rv = Parameter(attrs, ns=self.ns)
            self.rv.is_rv = True
            self.rv.is_instance = False
            return self.rv
        elif name == 'parameters':
            from peel_gen.parameters import Parameters
            self.params = Parameters(attrs, ns=self.ns)
            return self.params

    def is_cpp_static(self):
        if self.params is None:
            return True
        return not any(p.is_cpp_this() for p in self.params.params)

    def find_param_for_tweak(self, tweak1):
        if tweak1 == 'return':
            return self.rv
        assert(self.params is not None)
        for p in self.params.params:
            if p.name == tweak1:
                return p

    def resolve_stuff(self):
        from peel_gen.callback import Callback
        if self.has_resolved_stuff:
            return
        self.has_resolved_stuff = True
        for tweak in api_tweaks.lookup(self.tweak_ident):
            if tweak[0] == 'protected':
                self.visibility = 'protected'
            elif tweak[0] == 'include':
                self.extra_includes.append(tweak[1])
            elif tweak[0] in ('float', 'unowned', 'owned', 'in', 'out', 'inout', 'optional', 'nonnull', 'this', 'scope', 'type'):
                p = self.find_param_for_tweak(tweak[1])
                assert(p is not None)
                if tweak[0] == 'float':
                    p.ownership = 'floating'

                    p.resolve_stuff()
                    tp = chase_type_aliases(p.type)
                    from peel_gen.klass import Class
                    from peel_gen.record import Record
                    from peel_gen.interface import Interface
                    assert(isinstance(tp, (Class, Record, Interface)))
                    assert(tp.is_initially_floating)
                elif tweak[0] == 'unowned':
                    p.ownership = 'none'
                elif tweak[0] == 'owned':
                    p.ownership = 'full'
                elif tweak[0] == 'in':
                    p.direction = 'in'
                elif tweak[0] == 'out':
                    p.direction = 'out'
                elif tweak[0] == 'inout':
                    p.direction = 'inout'
                elif tweak[0] == 'optional':
                    p.optional = True
                elif tweak[0] == 'nonnull':
                    p.nullable = False
                elif tweak[0] == 'this':
                    p.force_cpp_this = True
                elif tweak[0] == 'scope':
                    p.scope = tweak[2]
                elif tweak[0] == 'type':
                    p.type_name = tweak[2]
            elif tweak[0] == 'vararg':
                p = self.params.params[-1]
                assert(p.name == '...')
                p.vararg_mode = tweak[1]

        self.rv.resolve_stuff()
        if self.params is not None:
            for p in self.params.params:
                p.resolve_stuff()

        # TODO: code duplication with Parameters.resovle_stuff()
        assert(self.rv.type is not None)
        offset = 1 if self.params and any(p.is_instance for p in self.params.params) else 0
        tp = chase_type_aliases(self.rv.type)
        if isinstance(tp, Array):
            if tp.length is not None:
                assert(self.params is not None)
                tp.length_param = self.params.params[int(tp.length) + offset]
        elif isinstance(tp, Callback):
            if self.rv.closure is not None:
                assert(self.params is not None)
                self.rv.closure_param = self.params.params[int(self.rv.closure) + offset]
                if self.rv.destroy is not None:
                    self.rv.destroy_param = self.params.params[int(self.rv.destroy) + offset]
        if self.params is not None:
            self.rv.append_skip_params_to(self.params.skip_params)

    def generate_extra_include_members(self):
        self.resolve_stuff()
        s = self.rv.generate_extra_include_members()
        if self.params:
            s.update(self.params.generate_extra_include_members())
        s.update(self.extra_includes)
        return s

    def generate_extra_forward_members(self):
        self.resolve_stuff()
        if not self.params:
            return self.rv.generate_extra_forward_members()
        s = self.params.generate_extra_forward_members()
        s.update(self.rv.generate_extra_forward_members())
        if self.throws:
            s.add(lookup_type('Error', 'GLib'))
        return s

    def generate_extra_include_at_end_members(self):
        self.resolve_stuff()
        s = self.rv.generate_extra_include_at_end_members()
        if self.params:
            s.update(self.params.generate_extra_include_at_end_members())
        if self.throws:
            s.add(lookup_type('Error', 'GLib'))
        return s
