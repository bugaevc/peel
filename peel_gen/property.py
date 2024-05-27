from peel_gen.node_handler import NodeHandler
from peel_gen.parameter import Parameter

class Property(NodeHandler):
    def __init__(self, attrs, cpp_class):
        self.cpp_class = cpp_class
        self.name = attrs['name']
        self.type_param = Parameter({ 'name': 'property-value' }, ns=self.cpp_class.ns)

    def __repr__(self):
        return 'Property({}.{})'.format(self.cpp_class, self.name)

    def start_child_element(self, name, attrs):
        if name in ('type', 'array'):
            return self.type_param.start_child_element(name, attrs)

    def generate_extra_include_members(self):
        return self.type_param.generate_extra_include_members()

    def generate_extra_forward_members(self):
        return self.type_param.generate_extra_forward_members()

    def generate_extra_include_at_end_members(self):
        return self.type_param.generate_extra_include_at_end_members()

    def generate(self):
        param_cpp_type = self.type_param.generate_cpp_type(name=None, context=self.cpp_class, strip_refs=1)
        l = [
            '  static peel::Property<{}>'.format(param_cpp_type),
            '  prop_{} ()'.format(self.name.replace('-', '_')),
            '  {',
            '    return peel::Property<{}> {{ "{}" }};'.format(param_cpp_type, self.name),
            '  }',
        ]
        return '\n'.join(l)

