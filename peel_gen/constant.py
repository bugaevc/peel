from peel_gen.node_handler import NodeHandler
from peel_gen.parameter import Parameter

class Constant(NodeHandler):
    def __init__(self, attrs, ns):
        self.name = attrs['name']
        self.ns = ns
        # Yes, the GIR attribute is c:type
        self.c_name = attrs.get('c:type', None)
        self.value = attrs.get('value', None)
        self.param = Parameter({ 'name': self.name }, ns=ns)

    def __repr__(self):
        return 'Constant({})'.format(self.param)

    def resolve_stuff(self):
        self.param.resolve_stuff()

    def start_child_element(self, name, attrs):
        if name == 'type':
            self.param.start_child_element(name, attrs)

    def generate_extra_include_members(self):
        return self.param.generate_extra_include_members()

    def generate_extra_forward_members(self):
        return self.param.generate_extra_forward_members()

    def generate_extra_include_at_end_members(self):
        return self.param.generate_extra_include_at_end_members()

    def generate(self):
        decl = self.param.generate_cpp_type(name=self.name, context=self.ns)
        val = self.param.generate_cast_from_c(
            c_name=self.c_name,
            context=self.ns,
            for_local_copy=False,
            skip_params_casted=False,
        )
        if val is None:
            val = self.c_name
        return 'constexpr {} = {};'.format(decl, val)
