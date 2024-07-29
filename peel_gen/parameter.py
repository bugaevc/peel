from peel_gen.node_handler import NodeHandler
from peel_gen.array import Array
from peel_gen.alias import chase_type_aliases
from peel_gen.utils import massage_c_type, extract_constness_from_c_type, add_asterisk
from peel_gen.type import lookup_type, PlainType, VoidType, StrType, VaListType
from peel_gen.callback import Callback
from peel_gen.enumeration import Enumeration
from peel_gen.bitfield import Bitfield
from peel_gen.defined_type import DefinedType
from peel_gen.exceptions import UnsupportedForNowException
from peel_gen import cpp_function_wrapper

class Parameter(NodeHandler):
    def __init__(self, attrs, ns):
        self.ns = ns
        self.name = attrs.get('name', None)
        self.type_name = None
        self.type = None
        # self.is_rv = None
        # self.is_instance = None
        self.is_record_field = False
        self.ownership = attrs.get('transfer-ownership', None)
        self.direction = attrs.get('direction', 'in')
        self.caller_allocates = attrs.get('caller-allocates', None) == '1'
        self.nullable = attrs.get('nullable', None) == '1'
        self.optional = attrs.get('optional', None) == '1'
        self.scope = attrs.get('scope', None)
        self.closure = attrs.get('closure', None)
        self.destroy = attrs.get('destroy', None)
        self.has_resolved_stuff = False

    def __repr__(self):
        return 'Parameter({} {})'.format(self.c_type, self.name)

    def start_child_element(self, name, attrs):
        if name == 'type':
            self.type_name = attrs.get('name', None)
            self.c_type = attrs.get('c:type', None)
            if self.c_type:
                self.c_type = massage_c_type(self.c_type)
        elif name == 'array':
            # self.type_name = self.c_type = attrs['c:type']
            self.c_type = attrs.get('c:type', None)
            if self.c_type:
                self.c_type = massage_c_type(self.c_type)
            self.type = Array(attrs, self.ns)
            return self.type

    def resolve_stuff(self):
        if self.has_resolved_stuff:
            return
        self.has_resolved_stuff = True
        if self.type is None:
            if self.type_name is None:
                raise UnsupportedForNowException('no type name for {}'.format(self.name))
            self.type = lookup_type(self.type_name, self.ns)
            assert(self.type is not None)
        self.type.resolve_stuff()
        # Beautify the type when a misc type pointer is smuggled
        # through GIR as a gpointer. See g_date_to_struct_tm ()
        # for an example. Note we already detect the constness
        # that gconstpointer implies, and add our own 'const'.
        if self.type_name == 'gpointer':
            assert(isinstance(self.type, PlainType))
            if self.direction == 'in' and self.c_type not in ('gpointer', 'gconstpointer'):
                self.type = self.type.clone()
                self.type.stdname = self.c_type
                self.type.corresponds_exactly = True
            elif self.direction != 'in' and self.c_type not in ('gpointer*', 'gconstpointer*'):
                self.type = self.type.clone()
                assert(self.c_type.endswith('*'))
                self.type.stdname = self.c_type[:-1].strip()
                self.type.corresponds_exactly = True

    def generate_extra_include_members(self):
        self.resolve_stuff()
        tp = chase_type_aliases(self.type)
        while isinstance(tp, Array):
            tp = chase_type_aliases(tp.item_type)
        if isinstance(tp, Callback):
            return tp.generate_extra_include_members()
        elif not isinstance(tp, DefinedType):
            return set()
        elif tp.ns.emit_raw:
            return set()
        elif not tp.is_passed_by_ref() or tp.nested_in:
            return { tp }
        elif self.is_record_field and tp.can_be_allocated_by_value():
            return { tp }
        else:
            return set()

    def generate_extra_forward_members(self):
        self.resolve_stuff()
        tp = chase_type_aliases(self.type)
        while isinstance(tp, Array):
            tp = chase_type_aliases(tp.item_type)
        if isinstance(tp, Callback):
            return tp.generate_extra_forward_members()
        elif not isinstance(tp, DefinedType):
            return set()
        elif tp.ns.emit_raw:
            return set()
        else:
            return { tp }

    def generate_extra_include_at_end_members(self):
        self.resolve_stuff()
        tp = chase_type_aliases(self.type)
        while isinstance(tp, Array):
            tp = chase_type_aliases(tp.item_type)
        if isinstance(tp, Callback):
            return tp.generate_extra_include_at_end_members()
        elif not isinstance(tp, DefinedType):
            return set()
        elif tp.ns.emit_raw:
            return set()
        elif not tp.is_passed_by_ref():
            return set()
        elif self.ownership is not None and self.ownership != 'none':
            # return { tp, self.type }
            return { tp }
        else:
            return set()

    def needs_local_copy(self):
        if self.direction == 'in':
            return False
        tp = chase_type_aliases(self.type)
        if isinstance(tp, Array):
            return not self.caller_allocates
        if tp.is_passed_by_ref():
            return self.ownership not in (None, 'none')
        if isinstance(tp, PlainType) and tp.corresponds_exactly:
            return False
        if isinstance(tp, (StrType, Enumeration, Bitfield)):
            return False
        return True

    def generate_c_type(self, for_local_copy=False):
        self.resolve_stuff()
        tp = chase_type_aliases(self.type)
        # Handle the case where we don't have a C type and need to make one up
        # completely. This happens in signals.
        if self.c_type is None:
            if isinstance(tp, DefinedType):
                type_name = '::' + tp.c_type
            elif isinstance(tp, PlainType):
                type_name = tp.gname
            else:
                raise UnsupportedForNowException('no C type, unclear what to do')
            if tp.is_passed_by_ref() and not self.caller_allocates:
                type_name = add_asterisk(type_name)
            if self.direction != 'in' and not for_local_copy:
                type_name += add_asterisk(type_name)
            return type_name

        if isinstance(tp, Array):
            itp = chase_type_aliases(tp.item_type)
        else:
            itp = tp

        if not isinstance(itp, DefinedType) or itp.ns.emit_raw:
            type_name = self.c_type
        elif self.c_type.startswith('const '):
            type_name = 'const ::' + self.c_type[6:]
        else:
            type_name = '::' + self.c_type

        if self.direction != 'in' and for_local_copy:
            assert(type_name.endswith('*'))
            type_name = type_name[:-1].strip()

        return type_name

    def generate_cpp_type(self, name, context, strip_refs=0, for_local_copy=False):
        self.resolve_stuff()
        tp = chase_type_aliases(self.type)

        if self.direction == 'in' or for_local_copy:
            out_asterisk = ''
        elif self.caller_allocates and tp.is_passed_by_ref():
            if not tp.can_be_allocated_by_value():
                raise UnsupportedForNowException('caller-allocates non-allocatable type (perhaps {} should be marked onstack?)'.format(tp))
            # Don't add an additional asterisk to the one that will be added
            # by the usual "this type gets passed by ref" logic anyway.
            out_asterisk = ''
        elif strip_refs:
            strip_refs -= 1
            out_asterisk = ''
        else:
            out_asterisk = '*'

        if isinstance(tp, Callback):
            if self.closure is None:
                # Plain C callback with no C++ callable wrapping
                if name is None:
                    return '::' + tp.c_type
                else:
                    return '::' + tp.c_type + ' ' + name
            if strip_refs:
                assert(name is None)
                return tp.name
            if name is None:
                return tp.name + ' &&'
            else:
                return '{} &&{}'.format(tp.name, name)
        # Other params marked with scope/closure/destroy should not reach here.
        assert(self.scope is None and self.closure is None and self.destroy is None)

        if isinstance(tp, Array):
            itp = chase_type_aliases(tp.item_type)
            if isinstance(itp, Array):
                raise UnsupportedForNowException('array of arrays')
        else:
            itp = tp
        assert(not isinstance(itp, Array))

        constness = extract_constness_from_c_type(self.c_type) if self.c_type else []
        constness0 = 'const ' if constness and constness[0] else ''
        constness1 = ' const' if len(constness) >= 2 and constness[1] else ''

        def make_type(s):
            if not isinstance(tp, Array):
                if name is None:
                    return s
                if not s.endswith('*'):
                    s = s + ' '
                return s + out_asterisk + name
            elif tp.fixed_size is not None:
                if name is None:
                    return '{} (&)[{}]'.format(s, tp.fixed_size)
                if out_asterisk:
                    raise UnsupportedForNowException('out fixed-size array')
                return '{} (&{})[{}]'.format(s, name, tp.fixed_size)
            elif tp.length is not None:
                if self.ownership == 'none' or self.ownership is None:
                    array_type = 'peel::ArrayRef<{}>'.format(s)
                elif self.ownership in ('container', 'full'):
                    array_type = 'peel::UniquePtr<{}[]>'.format(s)
                if strip_refs:
                    assert(name is None)
                    return s
                if name is None:
                    return array_type
                return '{} {}{}'.format(array_type, out_asterisk, name)
            else:
                # TODO support null-terminated arrays
                raise UnsupportedForNowException('Complex array')

        if isinstance(itp, DefinedType) and itp.ns.emit_raw:
            #if self.c_type is not None:
            #    c_type = self.c_type
            #else:
            c_type = itp.c_type
            if itp.is_passed_by_ref() and (itp is tp or not itp.can_be_allocated_by_value()):
                c_type = add_asterisk(c_type)
            return make_type(constness0 + c_type)

        if isinstance(itp, DefinedType):
            type_name = itp.emit_name_for_context(context)
        else:
            type_name = None

        if not itp.is_passed_by_ref():
            if isinstance(itp, VoidType):
                assert(self.is_rv)
                if name is None:
                    return 'void'
                assert(name == 'fake-return-name')
                return 'void fake-return-name'
            elif isinstance(itp, VaListType):
                assert(self.direction == 'in')
                return make_type(self.c_type)
            elif isinstance(itp, PlainType):
                return make_type(constness0 + itp.stdname)
            elif isinstance(itp, StrType):
                if self.ownership == 'full':
                    return make_type('/* owned */ char *' + constness1)
                else:
                    return make_type('const char *' + constness1)
            elif isinstance(itp, (Enumeration, Bitfield)):
                return make_type(constness0 + type_name)
            else:
                raise UnsupportedForNowException('unsupported type {}'.format(itp))

        if strip_refs and itp is tp:
            strip_refs -= 1
            return make_type(constness0 + type_name)
        if self.ownership == 'none' or self.ownership is None:
            if (itp is not tp or self.is_record_field) and itp.can_be_allocated_by_value():
                return make_type(constness0 + type_name)
            return make_type(constness0 + add_asterisk(type_name) + constness1)
        elif self.ownership == 'floating':
            assert(itp is tp)
            return make_type('peel::FloatPtr<{}{}>'.format(constness0, type_name))
        elif self.ownership == 'container':
            if itp is not tp:
                make_type(constness0 + add_asterisk(type_name) + constness1)
            raise UnsupportedForNowException('non-array transfer container')
        else:
            if itp.is_refcounted:
                return make_type('peel::RefPtr<{}{}>'.format(constness0, type_name))
            elif itp.free_func:
                return make_type('peel::UniquePtr<{}{}>'.format(constness0, type_name))
            else:
                raise UnsupportedForNowException('no idea about ownership semantics')

    def generate_casted_name(self):
        self.resolve_stuff()
        if self.is_instance:
            name = 'this'
        elif self.is_rv:
            name = 'return'
        else:
            name = self.name
        return '_peel_' + name

    def generate_cast_to_c(self, cpp_name, context, for_local_copy=False):
        self.resolve_stuff()
        tp = chase_type_aliases(self.type)
        c_type = self.generate_c_type(for_local_copy=for_local_copy)

        if isinstance(tp, Callback):
            if self.direction != 'in':
                raise UnsupportedForNowException('out callback parameter')
            if self.closure is None:
                return
            if tp.c_type == 'GCallback':
                raise UnsupportedForNowException('GCallback')
            assert(tp.params is not None)
            assert(len(tp.params.params) >= 1)
            user_data_param = tp.params.params[-1]
            assert(user_data_param.type_name == 'gpointer')
            tp.params.resolve_stuff(has_typed_tweak=False)
            # FIXME: This is a gross place to do this.
            if user_data_param not in tp.params.skip_params:
                 tp.params.skip_params.append(user_data_param)
            plain_closure_type = self.generate_cpp_type(
                name=None,
                context=context,
                strip_refs=1,
            )
            extra_decls = '        {} &{} = reinterpret_cast<{} &> (*reinterpret_cast<unsigned char *> ({}));'.format(
                plain_closure_type,
                cpp_name,
                plain_closure_type,
                user_data_param.name,
            )
            if self.scope != 'async':
                cpp_callee_expr = cpp_name
            else:
                cpp_callee_expr = 'static_cast<{} &&> ({})'.format(
                    plain_closure_type,
                    cpp_name,
                )
            lambda_expr = cpp_function_wrapper.generate(
                cpp_callee_expr,
                context,
                tp.rv,
                tp.params,
                throws=False,
                indent='      ',
                extra_decls=extra_decls,
            )
            closure_param_name = self.closure_param.generate_casted_name()
            callback_helper_type = 'peel::internals::CallbackHelper<{}>'.format(', '.join(
                p.generate_c_type() for p in [tp.rv] + tp.params.params if p is not user_data_param
            ))
            if self.scope == 'notified':
                wrap_method_name = 'wrap_notified_callback'
                misc_args = ', &{}'.format(self.destroy_param.generate_casted_name())
            elif self.scope == 'forever':
                wrap_method_name = 'wrap_notified_callback'
                misc_args = ', nullptr'
            elif self.scope == 'async':
                wrap_method_name = 'wrap_async_callback'
                misc_args = ''
            elif self.scope in ('call', None):
                return '({} = reinterpret_cast<gpointer> (&{}), +[] {})'.format(
                    self.closure_param.generate_casted_name(),
                    cpp_name,
                    lambda_expr,
                )
            if wrap_method_name:
                return '\n'.join([
                    '{}::{} ('.format(callback_helper_type, wrap_method_name),
                    '      static_cast<{} &&> ({}),'.format(plain_closure_type, cpp_name),
                    '      [] {},'.format(lambda_expr),
                    '      &{}{})'.format(closure_param_name, misc_args),
                ])

        if isinstance(tp, Array):
            itp = chase_type_aliases(tp.item_type)
            if isinstance(itp, PlainType) and itp.stdname == 'bool':
                raise UnsupportedForNowException('array of bool')
            if tp.fixed_size is not None:
                return 'reinterpret_cast<{}> ({})'.format(c_type, cpp_name)
            elif tp.length is not None:
                if self.ownership == 'none' or self.ownership is None:
                    ptr_expr = '{}.ptr ()'.format(cpp_name)
                else:
                    ptr_expr = 'std::move ({}).release_ref ()'.format(cpp_name)
                if tp.length_param.direction == 'in':
                    length_param_place = tp.length_param.generate_casted_name()
                else:
                    length_param_place = '*' + tp.length_param.name
                if c_type == 'char **' and self.ownership in (None, 'none'):
                    return '({} = {}.count (), const_cast<char **> ({}.ptr ()))'.format(
                        length_param_place,
                        cpp_name,
                        cpp_name,
                    )
                return '({} = {}.count (), reinterpret_cast<{}> ({}))'.format(
                    length_param_place,
                    cpp_name,
                    c_type,
                    ptr_expr,
                )
            else:
                raise UnsupportedForNowException('Complex array')

        if self.direction == 'in' and for_local_copy:
            cpp_name = '*' + cpp_name

        if self.direction != 'in' and not for_local_copy:
            if isinstance(tp, StrType):
                return None
            param_cpp_type = self.generate_cpp_type(
                name='',
                context=context,
                strip_refs=0,
                for_local_copy=False,
            )
            if param_cpp_type == c_type:
                return None
            return 'reinterpret_cast<{}> ({})'.format(c_type, cpp_name)

        if not tp.is_passed_by_ref():
            if isinstance(tp, (Enumeration, Bitfield)) or (isinstance(tp, PlainType) and not tp.corresponds_exactly and tp.stdname != self.c_type):
                return 'static_cast<{}> ({})'.format(c_type, cpp_name)
            return None

        if tp.ns.emit_raw:
            return None

        if self.ownership == 'none' or self.ownership is None:
            return 'reinterpret_cast<{}> ({})'.format(c_type, cpp_name)
        elif self.ownership == 'container':
            raise UnsupportedForNowException('non-array transfer container')
        elif self.ownership == 'floating':
            assert(not self.is_instance)
            return 'reinterpret_cast<{}> (std::move ({}).release_floating_ptr ())'.format(c_type, cpp_name)
        elif not self.is_instance:
            if tp.is_refcounted or tp.free_func:
                return 'reinterpret_cast<{}> (std::move ({}).release_ref ())'.format(c_type, cpp_name)
            else:
                raise UnsupportedForNowException('no idea about ownership semantics')
        else:
            # We'll have to take an extra ref here sadly :(
            plain_cpp_type = self.generate_cpp_type(name=None, context=context, strip_refs=1)
            return 'reinterpret_cast<{}> ((RefTraits<{}>::ref (this), this))'.format(c_type, plain_cpp_type)

    def generate_cast_from_c(self, c_name, context, for_local_copy=False):
        self.resolve_stuff()
        plain_cpp_type = self.generate_cpp_type(
            name=None,
            context=context,
            strip_refs=1,
            for_local_copy=for_local_copy,
        )
        assert(plain_cpp_type is not None)
        tp = chase_type_aliases(self.type)

        if isinstance(tp, Array):
            itp = chase_type_aliases(tp.item_type)
            if isinstance(itp, PlainType) and itp.stdname == 'bool':
                raise UnsupportedForNowException('array of bool')
            if tp.fixed_size is not None:
                return 'reinterpret_cast<{}> (*{})'.format(plain_cpp_type, c_name)
            elif tp.length is not None:
                if tp.length_param.direction == 'in':
                    length_param_name = tp.length_param.name
                else:
                    length_param_name = tp.length_param.generate_casted_name()
                if self.c_type == add_asterisk(plain_cpp_type):
                    ptr_expr = c_name
                else:
                    ptr_expr = 'reinterpret_cast<{}> ({})'.format(add_asterisk(plain_cpp_type), c_name)
                if self.ownership == 'none' or self.ownership is None:
                    return 'peel::ArrayRef<{}> ({}, {})'.format(
                        plain_cpp_type,
                        ptr_expr,
                        length_param_name,
                    )
                else:
                    return 'peel::UniquePtr<{}[]>::adopt_ref ({}, {})'.format(
                        plain_cpp_type,
                        ptr_expr,
                        length_param_name,
                    )
            else:
                raise UnsupportedForNowException('Complex array')

        if isinstance(tp, Callback):
            raise UnsupportedForNowException('casting callback from C to C++')
            # return '/* callback cast of {} goes here */'.format(c_name)

        if for_local_copy and self.direction == 'in':
            c_name = '*' + c_name

        if self.direction != 'in' and not for_local_copy:
            # Assuming we're not dealing with a local copy, we just need
            # to reinterpret_cast<> the pointer, unless the types match up
            # exactly already.
            if isinstance(tp, PlainType) and tp.corresponds_exactly:
                return None
            return 'reinterpret_cast<{}> ({})'.format(add_asterisk(plain_cpp_type), c_name)

        if not tp.is_passed_by_ref():
            if isinstance(tp, PlainType) and not tp.corresponds_exactly and tp.stdname != self.c_type:
                if tp.gname == 'GType':
                    return None
                #if self.c_type.endswith('*'):
                #    raise UnsupportedForNowException('don\'t know how to handle pointers')
                if tp.stdname == 'bool':
                    return '!!{}'.format(c_name)
                return 'static_cast<{}> ({})'.format(tp.stdname, c_name)
            if isinstance(tp, (Enumeration, Bitfield)):
                return 'static_cast<{}> ({})'.format(plain_cpp_type, c_name)
            return None

        if tp.ns.emit_raw:
            return None

        if self.ownership == 'full':
            if tp.is_refcounted:
                return 'peel::RefPtr<{}>::adopt_ref (reinterpret_cast<{}> ({}))'.format(
                    plain_cpp_type,
                    add_asterisk(plain_cpp_type),
                    c_name,
                )
            elif tp.free_func is not None:
                return 'peel::UniquePtr<{}>::adopt_ref (reinterpret_cast<{}> ({}))'.format(
                    plain_cpp_type,
                    add_asterisk(plain_cpp_type),
                    c_name,
                )
            else:
                raise UnsupportedForNowException('no idea about ownership semantics')
        elif self.ownership == 'container':
            raise UnsupportedForNowException('non-array transfer container')
        elif self.ownership == 'floating':
            return 'peel::FloatPtr<{}> (reinterpret_cast<{}> ({}))'.format(
                plain_cpp_type,
                add_asterisk(plain_cpp_type),
                c_name,
            )
        else:
            return 'reinterpret_cast<{}> ({})'.format(add_asterisk(plain_cpp_type), c_name)

    def generate_rv_function_attributes(self):
        assert(self.is_rv)
        # TODO: duplicated logic with Parameters.should_add_nonnull()
        tp = chase_type_aliases(self.type)
        if isinstance(tp, (Callback, Array)):
            return []
        if self.ownership is not None and self.ownership != 'none':
            return []
        if not tp.is_passed_by_ref():
            return []
        if self.nullable:
            return []
        return ['peel_returns_nonnull']

    def generate_post_call_assumes(self):
        assert(self.is_rv)
        # TODO: same for out/inout params
        tp = chase_type_aliases(self.type)
        if not tp.is_passed_by_ref():
            return None
        if self.nullable:
            return None
        return ['peel_assume ({});'.format(self.generate_casted_name())]
