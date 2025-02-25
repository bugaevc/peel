from peel_gen.node_handler import NodeHandler
from peel_gen.parameter import Parameter
from peel_gen.exceptions import UnsupportedForNowException
from peel_gen.array import Array
from peel_gen.alias import chase_type_aliases

class Parameters(NodeHandler):
    def __init__(self, attrs, ns):
        self.ns = ns
        self.params = []
        self.skip_params = []
        self.has_resolved_stuff = False

    def clone(self):
        ps = Parameters(None, self.ns)
        ps.params = self.params[:]
        return ps

    def start_child_element(self, name, attrs):
        if name == 'parameter':
           p = Parameter(attrs, ns=self.ns)
           p.is_instance = False
           p.is_rv = False
           self.params.append(p)
           return p
        elif name == 'instance-parameter':
            p = Parameter(attrs, ns=self.ns)
            p.is_instance = True
            p.is_rv = False
            self.params.append(p)
            return p

    def generate_extra_include_members(self):
        s = set()
        for param in self.params:
            s.update(param.generate_extra_include_members())
        return s

    def generate_extra_forward_members(self):
        s = set()
        for param in self.params:
            s.update(param.generate_extra_forward_members())
        return s

    def generate_extra_include_at_end_members(self):
        s = set()
        for param in self.params:
            s.update(param.generate_extra_include_at_end_members())
        return s

    def generate_c_signature(self):
        l = []
        for p in self.params:
            if p.name == '...':
                raise UnsupportedForNowException('varargs')
            c_type = p.generate_c_type()
            if not c_type.endswith('*'):
                l.append(c_type + ' ' + p.name)
            else:
                l.append(c_type + p.name)
        return ', '.join(l)

    def resolve_stuff(self, has_typed_tweak):
        from peel_gen.callback import Callback
        if self.has_resolved_stuff:
            return
        self.has_resolved_stuff = True
        offset = 0
        for p in self.params:
            if p.is_instance:
                offset = 1
            p.resolve_stuff()
            if p.name == '...':
                assert(p is self.params[-1])
                break
            assert(p.type is not None)
            tp = chase_type_aliases(p.type)
            if isinstance(tp, Array):
                if tp.length is not None:
                    tp.length_param = self.params[int(tp.length) + offset]
                    self.skip_params.append(tp.length_param)
                continue
            if not isinstance(tp, Callback):
                continue
            if p in self.skip_params:
                continue
            if p.closure is None:
                continue
            p.closure_param = self.params[int(p.closure) + offset]
            self.skip_params.append(p.closure_param)
            if p.destroy is not None:
                p.destroy_param = self.params[int(p.destroy) + offset]
                self.skip_params.append(p.destroy_param)

    def generate_cpp_signature_templates(self, has_typed_tweak):
        from peel_gen.callback import Callback
        self.resolve_stuff(has_typed_tweak)
        l = []
        for p in self.params:
            if p in self.skip_params:
                continue
            if p.name == '...':
                assert(p is self.params[-1])
                if has_typed_tweak:
                    break
                if p.vararg_mode is None:
                    raise UnsupportedForNowException('varargs')
                l.append('typename... Args')
                break
            tp = chase_type_aliases(p.type)
            if not isinstance(tp, Callback):
                continue
            if p.closure is None and not tp.force_cpp_wrapper:
                continue
            # We pass context=self.ns to get unqualified callback type name,
            # to be used as template name.
            s = p.generate_cpp_type(name=None, context=self.ns, strip_refs=1000)
            assert('::' not in s)
            assert('*' not in s)
            assert(' ' not in s)
            l.append('typename ' + s)
        if not l:
            return None
        return l

    def should_add_nonnull(self, p, tp):
        if p.is_cpp_this():
            return False
        if p.ownership is not None and p.ownership != 'none':
            # Cannot use peel_nonnull_args () on smart pointers
            return False
        if p.direction == 'in':
            if not tp.is_passed_by_ref():
                return False
            return not p.nullable
        else:
            return not p.optional

    def should_add_arg_in(self, p, tp):
        if p.is_instance:
            return False
        if p.direction != 'in':
            return False
        if p.ownership is not None and p.ownership != 'none':
            # Cannot use peel_in_arg () on smart pointers
            return False
        from peel_gen.record import Record
        return isinstance(tp, Record)

    def generate_function_attributes(self, has_typed_tweak):
        from peel_gen.callback import Callback
        self.resolve_stuff(has_typed_tweak)
        l = []
        nonnull_args = []
        is_static = all(not p.is_cpp_this() for p in self.params)
        index = 1 if is_static else 2
        for p in self.params:
            if p in self.skip_params:
                continue
            elif p.name == '...':
                continue
            elif p.is_cpp_this():
                # Do not increment index.
                continue
            tp = chase_type_aliases(p.type)
            if isinstance(tp, (Callback, Array)) or p.closure is not None:
                # Do not even bother
                index += 1
                continue
            if self.should_add_nonnull(p, tp):
                nonnull_args.append(index)
            if self.should_add_arg_in(p, tp):
                l.append('peel_arg_in ({})'.format(index))
            elif p.direction == 'out':
                l.append('peel_arg_out ({})'.format(index))
            elif p.direction == 'inout':
                l.append('peel_arg_inout ({})'.format(index))
            index += 1

        if nonnull_args:
            l.append('peel_nonnull_args ({})'.format(', '.join(str(ind) for ind in nonnull_args)))
        return l

    def generate_cpp_signature(self, context, typed_tweak=None):
        from peel_gen.type import StrType
        self.resolve_stuff(has_typed_tweak=typed_tweak is not None)
        l = []
        for p in self.params:
            if p.is_cpp_this():
                continue
            if p in self.skip_params:
                continue
            if typed_tweak and p is self.params[-2]:
                l.append('typename GObject::Value::Traits<{}>::UnownedType value'.format(typed_tweak))
                break
            name = p.name
            if name == '...':
                assert(p is self.params[-1])
                name = 'args'
                if p.vararg_mode == 'object-new':
                    assert(len(self.params) > 1)
                    assert(isinstance(self.params[-2].type, StrType))
                    l.pop()
            l.append(p.generate_cpp_type(name, context))
        return ', '.join(l)

