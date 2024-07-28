from peel_gen.function_like import FunctionLike
from peel_gen import c_function_wrapper
from peel_gen import cpp_function_wrapper
from peel_gen import api_tweaks

class Vfunc(FunctionLike):
    def __init__(self, attrs, cpp_class):
        super().__init__(attrs, cpp_class.ns)
        self.cpp_class = cpp_class
        self.tweak_ident = cpp_class.c_type + '::vfunc_' + self.name

    def __repr__(self):
        return 'Vfunc({!r}.{})'.format(self.cpp_class, self.name)

    def generate(self):
        api_tweaks.skip_if_needed(self.tweak_ident, self.ns)
        extra_decls = '    ::{} *_peel_class = reinterpret_cast<::{} *> (Class::peek<DerivedClass> ()->peek_parent ());'.format(
            self.cpp_class.type_struct.c_type,
            self.cpp_class.type_struct.c_type,
        )
        return c_function_wrapper.generate(
            name='parent_vfunc_' + self.name,
            c_callee='_peel_class->{}'.format(self.name),
            context=self.cpp_class,
            rv=self.rv,
            params=self.params,
            throws=self.throws,
            indent='  ',
            extra_decls=extra_decls,
            templates=['DerivedClass'],
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
                if p.is_instance:
                    p.resolve_stuff()
                    new_p = Parameter({
                        'name': p.name,
                        'ownership': p.ownership,
                    }, ns=self.cpp_class.ns)
                    new_p.is_instance = True
                    new_p.is_rv = False
                    new_p.c_type = p.c_type
                    fake_class = Class({
                        'name': 'DerivedClass',
                        'c:type': p.type.c_type,
                        'peel-fake-defined-type': '1',
                    }, ns=self.cpp_class.ns)
                    new_p.type = fake_class
                    params.params[i] = new_p

            params.resolve_stuff(has_typed_tweak=False)

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

