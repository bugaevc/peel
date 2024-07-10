from peel_gen.function_like import FunctionLike
from peel_gen.utils import escape_cpp_name
from peel_gen import c_function_wrapper
from peel_gen import api_tweaks

class Constructor(FunctionLike):
    def __init__(self, attrs, containing_type):
        super().__init__(attrs, ns=containing_type.ns)
        self.containing_type = containing_type
        self.c_ident = attrs['c:identifier']
        self.tweak_ident = self.c_ident

    def __repr__(self):
        return 'Constructor({}.{}.{}, c_ident={})'.format(
            self.containing_type.ns.name,
            self.containing_type.gir_name,
            self.name,
            self.c_ident,
        )

    def resolve_stuff(self):
        if self.has_resolved_stuff:
            return
        super().resolve_stuff()
        self.rv.resolve_stuff()

        from peel_gen.klass import Class
        from peel_gen.record import Record
        from peel_gen.interface import Interface
        floating_types = (Class, Record, Interface)

        if isinstance(self.rv.type, floating_types) and isinstance(self.containing_type, floating_types):
            # Check if the return type (as declared in C) is an ancestor
            # of the type the constructor is attached to. In that case,
            # make the constructor actually return the derived type in C++.
            tp = self.containing_type
            while tp:
                if tp is self.rv.type:
                    is_ancestor = True
                    break
                if isinstance(tp, Class):
                    tp = tp.parent
                elif isinstance(tp, Interface):
                    class_prereqs = [pr for pr in tp.prereqs if isinstance(tp, Class)]
                    if class_prereqs:
                        assert(len(class_prereqs) == 1)
                        tp = class_prereqs[0]
                        continue
                    tp = tp.prereqs[0]
                else:
                    break
            else:
                is_ancestor = False
            if is_ancestor:
                self.rv.type = self.containing_type
                self.rv.type_name = self.containing_type.gir_name

            if isinstance(self.containing_type, (Class, Interface)) and self.containing_type.is_initially_fully_unowned:
                self.rv.ownership = 'none'
            elif self.containing_type.is_initially_floating and self.rv.ownership != 'full':
                self.rv.ownership = 'floating'

    def emit_name(self):
        if self.name == 'new' or self.name.startswith('new_'):
            name = 'create' + self.name[3:]
        else:
            name = self.name

        return escape_cpp_name(name)

    def generate(self):
        api_tweaks.skip_if_needed(self.tweak_ident, self.ns)

        return c_function_wrapper.generate(
            name=self.emit_name(),
            c_callee=self.c_ident,
            context=self.containing_type,
            rv=self.rv,
            params=self.params,
            throws=self.throws,
            indent='  ',
        )
