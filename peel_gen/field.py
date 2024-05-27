from peel_gen.node_handler import NodeHandler
from peel_gen.parameter import Parameter

class Field(NodeHandler):
    def __init__(self, attrs, cpp_record):
        self.cpp_record = cpp_record
        self.name = attrs['name']
        self.private = attrs.get('private', None) == '1'
        self.param = Parameter({ 'name': self.name }, ns=self.cpp_record.ns)
        self.param.is_record_field = True
        self.we_support_this = True

    def __repr__(self):
        return 'Field({}.{})'.format(self.cpp_record, self.param)

    def resolve_stuff(self):
        if not we_support_this:
            return
        self.param.resolve_stuff()

    def start_child_element(self, name, attrs):
        if name in ('callback', 'array'):
            self.we_support_this = False
            return
        elif name == 'type':
            return self.param.start_child_element(name, attrs)

    def generate_extra_include_members(self):
        if not self.we_support_this:
            return set()
        return self.param.generate_extra_include_members()

    def generate_extra_forward_members(self):
        if not self.we_support_this:
            return set()
        return self.param.generate_extra_forward_members()

    def generate_extra_include_at_end_members(self):
        if not self.we_support_this:
            return set()
        return self.param.generate_extra_include_at_end_members()

    def generate(self):
        assert(self.we_support_this)
        s = self.param.generate_cpp_type(name=self.name, context=self.cpp_record)
        return '  {};'.format(s)

