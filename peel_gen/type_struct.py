from peel_gen.defined_type import DefinedType
from peel_gen.method import Method
from peel_gen.klass import Class
from peel_gen.interface import Interface
from peel_gen.field import Field
from peel_gen.type import lookup_type
from peel_gen.exceptions import UnsupportedForNowException
from peel_gen import api_tweaks

class TypeStruct(DefinedType):
    def __init__(self, attrs, ns, type_struct_for=None):
        super().__init__(attrs, ns)
        assert('glib:get-type' not in attrs)
        self.methods = []
        self.incomplete = True # Will be unset if we see any fields
        self.fields = []
        self.type_struct_for = None
        if type_struct_for:
            self.type_struct_for_name = type_struct_for
        else:
            self.type_struct_for_name = attrs['glib:is-gtype-struct-for']
        self.is_refcounted = True

    def __repr__(self):
        return 'TypeStruct(c_type={})'.format(self.c_type)

    def is_passed_by_ref(self):
        return True

    def start_child_element(self, name, attrs):
        assert(name != 'constructor')
        if name in ('method', 'function'):
            m = Method(attrs, self)
            self.methods.append(m)
            return m
        elif name == 'field':
            self.incomplete = False
            f = Field(attrs, self)
            self.fields.append(f)
            return f
        elif name == 'union':
            self.incomplete = False
            self.fields.append('union')
            return

    def resolve_stuff(self):
        if self.has_resolved_stuff:
            return
        super().resolve_stuff()
        self.type_struct_for = lookup_type(self.type_struct_for_name, self.ns)
        assert(self.type_struct_for is not None)
        self.type_struct_for.resolve_stuff()

        if isinstance(self.type_struct_for, Class):
            self.own_name = 'Class'
            self.emit_name = self.type_struct_for.emit_name + '::Class'
        elif isinstance(self.type_struct_for, Interface):
            self.own_name = 'Iface'
            self.emit_name = self.type_struct_for.emit_name + '::Iface'
        else:
            assert(self.c_type in ('GTypeClass', 'GTypeInterface'))
            if self.c_type == 'GTypeClass':
                self.emit_name = self.own_name = 'TypeClass'
            else:
                self.emit_name = self.own_name = 'TypeInterface'

    def emit_name_for_context(self, context):
        if self.c_type == 'GTypeClass':
            return 'GObject::TypeClass'
        elif self.c_type == 'GTypeInterface':
            return 'GObject::TypeInterface'
        return self.type_struct_for.emit_name_for_context(context) + '::' + self.own_name

    def generate_extra_include_members(self):
        self.resolve_stuff()
        s = set()
        for method in self.methods:
            try:
                s.update(method.generate_extra_include_members())
            except UnsupportedForNowException:
                pass
        return s

    def generate_extra_forward_members(self):
        self.resolve_stuff()
        s = set()
        for method in self.methods:
            try:
                s.update(method.generate_extra_forward_members())
            except UnsupportedForNowException:
                pass
        return s

    def generate_extra_include_at_end_members(self):
        self.resolve_stuff()
        s = set()
        for method in self.methods:
            try:
                s.update(method.generate_extra_include_at_end_members())
            except UnsupportedForNowException:
                pass
        return s

    def generate_forward_decl(self):
        return '  class {};'.format(self.own_name)

    def should_emit_placeholder_member(self):
        # GInitiallyUnowned is typedefed from struct _GObject, not
        # struct _GInitiallyUnowned containing a GObject.
        if self.c_type == 'GInitiallyUnownedClass':
            return False
        return len(self.fields) > 1

    def generate(self):
        api_tweaks.skip_if_needed(self.c_type, self.ns)

        if isinstance(self.type_struct_for, Class):
            parent_instance_type = self.type_struct_for.parent
            if parent_instance_type is not None:
                parent_class_type_name = parent_instance_type.type_struct.emit_name_for_context(self)
            else:
                parent_class_type_name = 'GObject::TypeClass'
        elif isinstance(self.type_struct_for, Interface):
            parent_class_type_name = 'GObject::TypeInterface'
        else:
            assert(False)

        l = [
            '  class {} : public {}'.format(self.own_name, parent_class_type_name),
            '  {',
            '  private:',
            '    {} () = delete;'.format(self.own_name),
            '    {} (const {} &) = delete;'.format(self.own_name, self.own_name),
            '    {} ({} &&) = delete;'.format(self.own_name, self.own_name),
            '',
        ]
        if self.should_emit_placeholder_member():
            s = api_tweaks.ifdef_for_non_opaque(self.c_type)
            if s:
                l.append(s)
            if not self.incomplete or s:
                l.append(
                    '    unsigned char _placeholder[sizeof (::{}) - sizeof ({})] peel_no_warn_unused;'.format(
                        self.c_type,
                        parent_class_type_name,
                    )
                )
            s = api_tweaks.endif_for_non_opaque(self.c_type)
            if s:
                l.append(s)
        l.append('  public:');
        for method in self.methods:
            try:
                method.resolve_stuff()
                l.append(method.generate(indent='    '))
            except UnsupportedForNowException as e:
                l.append('    /* Unsupported for now: {}: {} */'.format(method.name, e.reason))
            l.append('')
        if self.type_struct_for.vfuncs and isinstance(self.type_struct_for, Class):
            l.append('  protected:')
        for vfunc in self.type_struct_for.vfuncs:
            try:
                vfunc.resolve_stuff()
                l.append(vfunc.generate_override())
            except UnsupportedForNowException as e:
                l.append('    /* Unsupported for now: {}: {} */'.format(vfunc.name, e.reason))
            l.append('')
        if not l[-1]:
            l.pop()
        l.append('  };')
        l.append('')
        s = api_tweaks.ifdef_for_non_opaque(self.c_type)
        if s:
            l.append(s)
        if not self.incomplete or s:
            l.extend([
                '  static_assert (sizeof ({}) == sizeof (::{}),'.format(self.own_name, self.c_type),
                '                 "{} size mismatch");'.format(self.emit_name),
                '  static_assert (alignof ({}) == alignof (::{}),'.format(self.own_name, self.c_type),
                '                 "{} align mismatch");'.format(self.emit_name),
            ])
        s = api_tweaks.endif_for_non_opaque(self.c_type)
        if s:
            l.append(s)
        return '\n'.join(l)

