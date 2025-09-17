from peel_gen.alias import chase_type_aliases
from peel_gen.array import Array
from peel_gen.callback import Callback
from peel_gen.node_handler import NodeHandler
from peel_gen.parameter import Parameter
from peel_gen.exceptions import UnsupportedForNowException
from peel_gen.utils import is_type_element, make_simple_decl

class Field(NodeHandler):
    def __init__(self, attrs, cpp_record, element_name):
        self.cpp_record = cpp_record
        self.name = attrs['name']
        self.c_name = self.name
        self.private = attrs.get('private', None) == '1'
        self.bits = attrs.get('bits', None)
        self.param = Parameter({ 'name': self.name }, ns=self.cpp_record.ns)
        self.param.is_record_field = True
        self.we_support_this = True
        self.element_name = element_name

        if element_name == 'field':
            return

        # Otherwise, this field represents an inline anonymous union or struct.
        assert(element_name in ('union', 'record'))
        self.we_support_this = False
        self.param.c_type = None
        self.param.type_name = None
        tp_attrs = dict(attrs)
        # name is the field's name, c:type is a lie.
        tp_attrs['name'] = None
        tp_attrs.pop('c:type', None)
        tp_attrs['peel-fake-defined-type'] = '1'
        if element_name == 'union':
            from peel_gen.union import Union
            self.param.type = Union(tp_attrs, ns=self.cpp_record.ns)
        elif element_name == 'record':
            from peel_gen.record import Record
            self.param.type = Record(tp_attrs, ns=self.cpp_record.ns)

    def __repr__(self):
        return 'Field({}.{})'.format(self.cpp_record, self.param)

    def resolve_stuff(self):
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

        tp = chase_type_aliases(self.param.type)
        if isinstance(tp, Array):
            # Dynamic-sized arrays are not generated publicly currently as they would
            # ideally be represented as an `ArrayRef` or `UniquePtr<T[]>`
            # instead of a pointer and size field.
            #
            # Similarly zero-terminated arrays would have to be represented by
            # some kind of array type instead of a plain pointer.
            if tp.length is not None or tp.zero_terminated:
                self.we_support_this = False
            # Apart from that only fixed-size arrays are supported right now
            # and generated as public fields
            elif tp.fixed_size is None:
                self.we_support_this = False
        elif isinstance(tp, Callback):
            # Callbacks operate on plain C types
            self.we_support_this = False

    def start_child_element(self, name, attrs):
        if self.element_name == 'field':
            # Forward nested elements to the parameter.
            return self.param.start_child_element(name, attrs)
        else:
            # Forward nested elements to the parameter's type.
            return self.param.type.start_child_element(name, attrs)

    def generate_extra_include_members(self):
        if not self.we_support_this or self.private:
            return set()
        return self.param.generate_extra_include_members()

    def generate_extra_forward_members(self):
        if not self.we_support_this or self.private:
            return set()
        return self.param.generate_extra_forward_members()

    def generate_extra_include_at_end_members(self):
        if not self.we_support_this or self.private:
            return set()
        return self.param.generate_extra_include_at_end_members()

    def generate(self):
        if self.we_support_this and not self.private:
            s = self.param.generate_cpp_type(name=self.name, context=self.cpp_record)
        else:
            try:
                c_type = self.param.generate_c_type()
            except UnsupportedForNowException:
                # Does our record at least have a C type?
                if self.cpp_record.c_type is None:
                    raise
                c_type = 'decltype (::{}::{})'.format(self.cpp_record.c_type, self.c_name)
            s = make_simple_decl(c_type, self.name)
        if self.bits is not None:
            s = '{} : {}'.format(s, self.bits)
        return '  {};'.format(s)

