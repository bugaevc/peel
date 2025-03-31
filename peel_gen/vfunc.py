from peel_gen.function_like import FunctionLike
from peel_gen.alias import chase_type_aliases
from peel_gen import c_function_wrapper
from peel_gen import cpp_function_wrapper
from peel_gen import api_tweaks

class Vfunc(FunctionLike):
    def __init__(self, attrs, cpp_class):
        from peel_gen.klass import Class
        super().__init__(attrs, cpp_class.ns)
        self.cpp_class = cpp_class
        self.tweak_ident = cpp_class.c_type + '::vfunc_' + self.name
        # Set visibility to protected for classes, but leave it public for interfaces.
        if isinstance(cpp_class, Class):
            self.visibility = 'protected'

    def __repr__(self):
        return 'Vfunc({!r}.{})'.format(self.cpp_class, self.name)

    def generate(self):
        from peel_gen.klass import Class
        from peel_gen.klass import Interface

        api_tweaks.skip_if_needed(self.tweak_ident, self.ns)
        if isinstance(self.cpp_class, Class):
            extra_decls = '    ::{} *_peel_class = reinterpret_cast<::{} *> (Class::peek<DerivedClass> ()->peek_parent ());'.format(
                self.cpp_class.type_struct.c_type,
                self.cpp_class.type_struct.c_type,
            )
            c_callee = '_peel_class->{}'.format(self.name)
        elif isinstance(self.cpp_class, Interface):
            extra_decls = '    ::{} *_peel_iface = reinterpret_cast<::{} *> (Class::peek<DerivedClass> ()->peek_interface (Type::of<{}> ())->peek_parent ());'.format(
                self.cpp_class.type_struct.c_type,
                self.cpp_class.type_struct.c_type,
                self.cpp_class.emit_name,
            )
            c_callee = '_peel_iface->{}'.format(self.name)
        else:
            assert(False)
        return c_function_wrapper.generate(
            name='parent_vfunc_' + self.name,
            c_callee=c_callee,
            context=self.cpp_class,
            rv=self.rv,
            params=self.params,
            throws=self.throws,
            indent='  ',
            extra_decls=extra_decls,
            templates=['typename DerivedClass'],
        )

    def generate_override(self):
        from peel_gen.parameter import Parameter
        from peel_gen.klass import Class

        api_tweaks.skip_if_needed(self.tweak_ident, self.ns)
        if self.params is None:
            params = None
        else:
            params = self.params.clone()
            for i in range(len(params.params)):
                p = params.params[i]
                # Note: this should check for is_instance, not is_cpp_this().
                if p.is_instance:
                    p.resolve_stuff()
                    new_p = Parameter({
                        'name': p.name,
                    }, ns=self.cpp_class.ns)
                    new_p.ownership = p.ownership
                    new_p.is_instance = True
                    new_p.is_rv = False
                    new_p.c_type = p.c_type
                    fake_class = Class({
                        'name': 'DerivedClass',
                        'c:type': p.type.c_type,
                        'peel-fake-defined-type': '1',
                    }, ns=self.cpp_class.ns)
                    fake_class.is_refcounted = p.type.is_refcounted
                    fake_class.is_initially_floating = p.type.is_initially_floating
                    fake_class.is_initially_fully_unowned = p.type.is_initially_fully_unowned
                    fake_class.has_resolved_stuff = True
                    new_p.type = fake_class
                    params.params[i] = new_p

        cpp_wrapper = cpp_function_wrapper.generate(
            cpp_callee='DerivedClass::vfunc_' + self.name,
            context=self.cpp_class,
            rv=self.rv,
            params=params,
            throws=self.throws,
            indent='      ',
        )
        return '\n'.join([
            '    template<typename DerivedClass>',
            '    void',
            '    override_vfunc_{} ()'.format(self.name),
            '    {',
            '      ::{} *klass = reinterpret_cast<::{} *> (this);'.format(
                self.cpp_class.type_struct.c_type,
                self.cpp_class.type_struct.c_type,
            ),
            '      klass->{} = +[] {};'.format(self.name, cpp_wrapper),
            '    }',
        ])

