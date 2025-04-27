from peel_gen.function_like import FunctionLike
from peel_gen.defined_type import DefinedType
from peel_gen import api_tweaks, c_function_wrapper

class Callback(FunctionLike, DefinedType):
    def __init__(self, attrs, ns):
        DefinedType.__init__(self, attrs, ns)
        FunctionLike.__init__(self, attrs, ns)
        self.tweak_ident = self.c_type
        self.force_cpp_wrapper = False

    def resolve_stuff(self):
        if self.has_resolved_stuff:
            return
        DefinedType.resolve_stuff(self)
        self.has_resolved_stuff = False
        FunctionLike.resolve_stuff(self)

        for tweak in api_tweaks.lookup(self.tweak_ident):
            if tweak[0] == 'cpp-wrapper':
                self.force_cpp_wrapper = True

        if self.params:
            for p in self.params.params:
                if p.closure is not None:
                    self.params.skip_params.append(p)

    def generate_forward_decl(self, for_nested=False):
        if for_nested or not self.nested_in:
            return 'struct /* callback */ {};'.format(self.own_name)

    def generate_specializations(self):
        return None

    def generate(self):
        l = [
            'struct /* callback */ {} final'.format(self.emit_def_name),
            '{',
            'private:',
            '  ::{} c_callback {{ nullptr }};'.format(self.c_type),
            '  gpointer user_data { nullptr }',
            '  ::GDestroyNotify destroy_notify { nullptr };'
            '',
            'public:',
            '  ~{} ()'.format(self.own_name),
            '  {',
            '    if (destroy_notify)',
            '      destroy_notify (user_data);',
            '  }',
            '',
            # TODO: this would be fine for forever and call callbacks
            '  {} (const {} &other) = delete;'.format(self.own_name, self.own_name),
            '',
            '  {} ({} &&other)'.format(self.own_name, self.own_name),
            '    : c_callback (other.c_callback)',
            '    , user_data (other.user_data)',
            '    , destroy_notify (other.destroy_notify)',
            '  {',
            '    other.c_callback = nullptr;',
            '    other.user_data = nullptr;',
            '    other.destroy_notify = nullptr;',
            '  }',
            '',
        ]
        l.append(c_function_wrapper.generate(
            name='operator ()',
            c_callee='c_callback',
            context=self,
            rv=self.rv,
            params=self.params,
            throws=self.throws,
            indent='  ',
        ))
        l.append('};')
        return '\n'.join(l)
