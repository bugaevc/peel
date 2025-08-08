from peel_gen.node_handler import NodeHandler
from peel_gen.parameter import Parameter
from peel_gen.exceptions import UnsupportedForNowException
from peel_gen.utils import is_type_element

class Field(NodeHandler):
    def __init__(self, attrs, cpp_record):
        self.cpp_record = cpp_record
        self.name = attrs['name']
        self.private = attrs.get('private', None) == '1'
        self.bits = attrs.get('bits', None)
        self.param = Parameter({ 'name': self.name }, ns=self.cpp_record.ns)
        self.param.is_record_field = True
        self.we_support_this = True

    def __repr__(self):
        return 'Field({}.{})'.format(self.cpp_record, self.param)

    def resolve_stuff(self):
        from peel_gen.union import Union
        if not self.we_support_this:
            return
        if self.param.type is self.param.type_name is None:
            self.we_support_this = False
            return
        try:
            self.param.resolve_stuff()
        except UnsupportedForNowException:
            self.we_support_this = False
            return
        if self.param.type is None:
            self.we_support_this = False
        elif isinstance(self.param.type, Union):
            self.we_support_this = False

    def start_child_element(self, name, attrs):
        if is_type_element(name, attrs):
            return self.param.start_child_element(name, attrs)
        elif name in ('callback', 'array'):
            self.we_support_this = False
            return

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
        if self.bits is not None:
            s = '{} : {}'.format(s, self.bits)
        return '  {};'.format(s)

