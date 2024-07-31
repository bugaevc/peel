from peel_gen.defined_type import DefinedType
from peel_gen.method import Method
from peel_gen.constructor import Constructor
from peel_gen.vfunc import Vfunc
from peel_gen.interface import Interface
from peel_gen.signal import Signal
from peel_gen.property import Property
from peel_gen.field import Field
from peel_gen.type import lookup_type, pick_base_type
from peel_gen.exceptions import UnsupportedForNowException
from peel_gen.specializations import generate_get_type_specialization, generate_ref_traits_specialization
from peel_gen.utils import intern_get_type_map, escape_cpp_name, VisibilityTracker
from peel_gen import api_tweaks

class Class(DefinedType):
    def __init__(self, attrs, ns):
        super().__init__(attrs, ns)
        self.get_type = attrs.get('glib:get-type', None)
        self.get_value_func = attrs.get('glib:get-value-func', None)
        self.set_value_func = attrs.get('glib:set-value-func', None)
        self.take_value_func = None
        self.ref_func = attrs.get('glib:ref-func', None)
        self.unref_func = attrs.get('glib:unref-func', None)
        self.free_func = None  # Always None
        self.ref_sink_func = None
        self.methods = []
        self.vfuncs = []
        self.constructors = []
        self.impl_iface_names = []
        self.signals = []
        self.props = []
        self.hidden_members = []
        self.parent_gir_name = attrs.get('parent', None)
        self.sealed = True  # Will be set to false if we see any fields
        self.fields = []
        self.type_struct_name = attrs.get('glib:type-struct', None)
        self.type_struct = None

    def is_passed_by_ref(self):
        return True

    def resolve_stuff(self):
        if self.has_resolved_stuff:
            return
        super().resolve_stuff()
        if self.parent_gir_name is not None:
            self.parent = lookup_type(self.parent_gir_name, self.ns)
            assert(self.parent is not None)
        else:
            self.parent = None
        if self.parent is not None:
            self.parent.resolve_stuff()
            # sealed doesn't imply final
            # assert(not self.parent.sealed)
            self.is_initially_floating = self.parent.is_initially_floating
            self.is_initially_fully_unowned = self.parent.is_initially_fully_unowned
            self.is_gobject_derived = self.parent.is_gobject_derived
        else:
            self.is_initially_floating = False
            self.is_initially_fully_unowned = False
            self.is_gobject_derived = self.c_type == 'GObject'

        for tweak in api_tweaks.lookup(self.c_type):
            if tweak == ('unowned', 'construct'):
               self.is_initially_fully_unowned = True
            elif tweak[0] == 'float':
                self.is_initially_floating = True
            elif tweak[0] == 'ref':
                self.ref_func = tweak[1]
            elif tweak[0] == 'ref-sink':
                self.ref_sink_func = tweak[1]
            elif tweak[0] == 'take-value':
                self.take_value_func = tweak[1]
            elif tweak[0] == 'hide':
                self.hidden_members.append(tweak[1])

        if self.parent is None:
            self.is_refcounted = bool(self.ref_func)
        else:
            self.is_refcounted = self.parent.is_refcounted

        self.impl_ifaces = []
        for iface_name in self.impl_iface_names:
            iface = lookup_type(iface_name, self.ns)
            assert(iface is not None)
            self.impl_ifaces.append(iface)

        if self.type_struct_name is not None:
            self.type_struct = lookup_type(self.type_struct_name, self.ns)
            assert(self.type_struct is not None)
            self.type_struct.resolve_stuff()

        self.cpp_base_type = pick_base_type(self, must_inherit_type=self.parent)

    def recursive_prereqs(self):
        self.resolve_stuff()
        s = set(self.impl_ifaces)
        if self.parent:
            s.add(self.parent)
            s.update(self.parent.recursive_prereqs())
        for p in self.impl_ifaces:
            s.update(p.recursive_prereqs())
        return s

    def start_child_element(self, name, attrs):
        if name == 'implements':
            self.impl_iface_names.append(attrs['name'])
            return
        elif name == 'constructor':
            c = Constructor(attrs, self)
            self.constructors.append(c)
            return c
        elif name in ('method', 'function'):
            m = Method(attrs, self)
            self.methods.append(m)
            return m
        elif name == 'virtual-method':
            v = Vfunc(attrs, self)
            self.vfuncs.append(v)
            return v
        elif name == 'field':
            self.sealed = False
            f = Field(attrs, self)
            self.fields.append(f)
            return f
        elif name == 'union':
            self.sealed = False
            self.fields.append('union')
            return
        elif name == 'glib:signal':
            s = Signal(attrs, self)
            self.signals.append(s)
            return s
        elif name == 'property':
            p = Property(attrs, self)
            self.props.append(p)
            return p

    def generate_forward_decl(self, for_nested=False):
        if for_nested or not self.nested_in:
            return 'class {};'.format(self.own_name)

    def generate_extra_include_members(self):
        self.resolve_stuff()
        s = set()
        if self.cpp_base_type is not None:
            s.add(self.cpp_base_type)
        else:
            s.add(lookup_type('TypeInstance', 'GObject'))
        for member in self.constructors + self.methods + self.vfuncs + self.props + self.signals + [self.type_struct]:
            if member is None:
                continue
            try:
                s.update(member.generate_extra_include_members())
            except UnsupportedForNowException:
                pass
        # If self got into the set (e.g. due to being passed inside RefPtr<>),
        # get rid of it, since there's no point in including ourselves.
        s.discard(self)
        if self.type_struct is not None:
            s.discard(self.type_struct)
        for nested_type in self.nested_types:
            s.discard(nested_type)
        return s

    def generate_extra_forward_members(self):
        self.resolve_stuff()
        s = set()
        for member in self.constructors + self.methods + self.vfuncs + self.props + self.signals + [self.type_struct]:
            if member is None:
                continue
            try:
                s.update(member.generate_extra_forward_members())
            except UnsupportedForNowException:
                pass
        s.discard(self)
        if self.type_struct is not None:
            s.discard(self.type_struct)
        for nested_type in self.nested_types:
            s.discard(nested_type)
        for nested_type in self.nested_type_aliases:
            s.add(nested_type)
        return s

    def generate_extra_include_at_end_members(self):
        self.resolve_stuff()
        s = set()
        for member in self.constructors + self.methods + self.vfuncs + self.props + self.signals + [self.type_struct]:
            if member is None:
                continue
            try:
                s.update(member.generate_extra_include_at_end_members())
            except UnsupportedForNowException:
                pass
        # If self got into the set (e.g. due to being passed inside RefPtr<>),
        # get rid of it, since there's no point in including ourselves.
        s.discard(self)
        if self.type_struct is not None:
            s.discard(self.type_struct)
        for nested_type in self.nested_types:
            s.discard(nested_type)
        return s

    def should_hide(self, parent_member):
        # Only explicitly hide if we don't declare a member with the same name.
        if isinstance(parent_member, Constructor):
            return not any(c.name == parent_member.name for c in self.constructors)
        elif isinstance(parent_member, Method):
            return not any(c.name == parent_member.name for c in self.methods)

    def should_emit_placeholder_member(self):
        # GInitiallyUnowned is typedefed from struct _GObject, not
        # struct _GInitiallyUnowned containing a GObject.
        if self.c_type == 'GInitiallyUnowned':
            return False
        return len(self.fields) > 1

    def generate(self):
        api_tweaks.skip_if_needed(self.c_type, self.ns)

        l = []

        s = api_tweaks.ifdef_if_needed(self.c_type)
        if s:
            l.append(s)

        if self.cpp_base_type:
            cpp_base_type_emit_name = self.cpp_base_type.emit_name_for_context(self)
        else:
            cpp_base_type_emit_name = 'GObject::TypeInstance'
        l += [
            'class ' + self.emit_def_name + ' : public ' + cpp_base_type_emit_name,
            '{',
            'private:',
        ]
        if self.impl_ifaces:
            l.insert(1, '/* implements ' + ', '.join(
                iface.emit_name_for_context(self) for iface in self.impl_ifaces
            ) + ' */')
        if self.cpp_base_type is not self.parent and self.parent is not None:
            l.insert(1, '/* extends {} */'.format(self.parent.emit_name_for_context(self)))
        if self.sealed:
            l.insert(1, '/* non-derivable */')
        if self.should_emit_placeholder_member():
            s = api_tweaks.ifdef_for_non_opaque(self.c_type)
            if s:
                l.append(s)
            if not self.sealed or s:
                # We cannot know the instance size this way for sealed classes
                l.append(
                    '  unsigned char _placeholder[sizeof (::{}) - sizeof ({})] peel_no_warn_unused;'.format(
                        self.c_type,
                        cpp_base_type_emit_name,
                    ),
                )
            s = api_tweaks.endif_for_non_opaque(self.c_type)
            if s:
                l.append(s)
        for member in self.hidden_members:
            l.append('  using {}::{};'.format(cpp_base_type_emit_name, member))
        if self.parent is not None:
            if self.parent.c_type == 'GObject':
                if not any(c.name == 'new' for c in self.constructors):
                    l.append('  using GObject::Object::create;')
                l.append('  using GObject::Object::bind_property;')
            else:
                for constructor in self.parent.constructors:
                    if not self.should_hide(constructor):
                        continue
                    l.append('  using {}::{};'.format(cpp_base_type_emit_name, constructor.emit_name()))
                for method in self.parent.methods:
                    if not method.is_cpp_static():
                        continue
                    if not self.should_hide(method):
                        continue
                    l.append('  using {}::{};'.format(cpp_base_type_emit_name, escape_cpp_name(method.name)))
        l.extend([
            '  friend class GObject::Type;',
            '  friend class GObject::TypeInstance;',
            '  friend class GObject::TypeClass;',
            '',
            '  {} () = delete;'.format(self.own_name),
            '  {} (const {} &) = delete;'.format(self.own_name, self.own_name),
            '  {} ({} &&) = delete;'.format(self.own_name, self.own_name),
            '  {} &'.format(self.own_name),
            '  operator = (const {} &) = delete;'.format(self.own_name),
            '  {} &'.format(self.own_name),
            '  operator = ({} &&) = delete;'.format(self.own_name),
            '',
            'protected:',
            '  ~{} () = default;'.format(self.own_name),
        ])
        l.append('')
        l.append('public:')
        l.append(self.generate_nested_type_defs())

        visibility = VisibilityTracker(l, 'public')

        for constructor in self.constructors:
            try:
                constructor.resolve_stuff()
                visibility.switch(constructor.visibility)
                l.append(constructor.generate())
            except UnsupportedForNowException as e:
                l.append('  /* Unsupported for now: {}: {} */'.format(constructor.name, e.reason))
                l.extend([
                    '  static void',
                    '  {} (UnsupportedForNowToken);'.format(constructor.emit_name()),
                ])
            l.append('')
        for method in self.methods:
            try:
                method.resolve_stuff()
                if method.c_ident in (self.ref_func, self.unref_func, self.ref_sink_func):
                    l.append('  /* {} bound as RefTraits */'.format(method.name))
                    l.append('')
                    continue
                visibility.switch(method.visibility)
                l.append(method.generate(indent='  '))
            except UnsupportedForNowException as e:
                l.append('  /* Unsupported for now: {}: {} */'.format(method.name, e.reason))
                l.extend([
                    '  static void',
                    '  {} (UnsupportedForNowToken);'.format(escape_cpp_name(method.name)),
                ])
            l.append('')
        for signal in self.signals:
            try:
                signal.resolve_stuff()
                visibility.switch(signal.visibility)
                l.append(signal.generate())
            except UnsupportedForNowException as e:
                l.append('  /* Unsupported for now: {}: {} */'.format(signal.name, e.reason))
            l.append('')
        visibility.switch('public')
        for prop in self.props:
            try:
                l.append(prop.generate())
            except UnsupportedForNowException as e:
                l.append('  /* Unsupported for now: {}: {} */'.format(prop.name, e.reason))
            l.append('')
        s = api_tweaks.ifdef_for_non_opaque(self.c_type)
        if s:
            l.append(s)
        if self.is_gobject_derived and not self.sealed:
            visibility.switch('protected')
            l.extend([
                '  peel_nothrow',
                '  static void',
                '  _peel_chain_finalize (::GObject *obj)',
                '  {',
                '    ::GObjectClass *_peel_class = reinterpret_cast<::GObjectClass *> (GObject::TypeClass::peek_static<{}> ());'.format(self.own_name),
                '    _peel_class->finalize (obj);',
                '  }',
                '',
            ])
        if self.vfuncs:
            for vfunc in self.vfuncs:
                if self.is_gobject_derived and vfunc.name == 'finalize':
                    continue
                try:
                    vfunc.resolve_stuff()
                    visibility.switch(vfunc.visibility)
                    l.append(vfunc.generate())
                except UnsupportedForNowException as e:
                    l.append('  /* Unsupported for now: {}: {} */'.format(vfunc.name, e.reason))
                l.append('')
        if self.type_struct is not None:
            visibility.switch('public')
            try:
                l.append(self.type_struct.generate())
            except UnsupportedForNowException as e:
                l.append('  /* Type struct unsupported for now: {} */'.format(e.reason))
        s = api_tweaks.endif_for_non_opaque(self.c_type)
        if s:
            l.append(s)
        if not l[-1]:
            l.pop()
        l.append('};' + ' /* class {} */'.format(self.emit_name))
        s = api_tweaks.ifdef_for_non_opaque(self.c_type)
        if not self.sealed or s:
            l.append('')
        if s:
            l.append(s)
        if not self.sealed or s:
            l.extend([
                'static_assert (sizeof ({}) == sizeof (::{}),'.format(self.emit_def_name, self.c_type),
                '               "{} size mismatch");'.format(self.emit_def_name),
                'static_assert (alignof ({}) == alignof (::{}),'.format(self.emit_def_name, self.c_type),
                '               "{} align mismatch");'.format(self.emit_def_name),
            ])
        s = api_tweaks.endif_for_non_opaque(self.c_type)
        if s:
            l.append(s)
        s = api_tweaks.endif_if_needed(self.c_type)
        if s:
            l.append(s)

        return '\n'.join(l)

    def generate_specializations(self):
        api_tweaks.skip_if_needed(self.c_type, self.ns)
        full_name = self.emit_name_for_context(None)
        s = ''
        if self.get_type == 'intern':
            get_type = generate_get_type_specialization(full_name, intern_get_type_map[self.gir_name])
        elif self.get_type:
            get_type = generate_get_type_specialization(full_name, self.get_type + ' ()')
        else:
            get_type = ''
        s = get_type
        if self.is_initially_fully_unowned and self.parent is not None and not self.parent.is_initially_fully_unowned:
            s += '\n'.join([
                '',
                'template<typename T>',
                'struct GObject::Object::Traits<T, peel::enable_if_derived<{}, T, void>>'.format(full_name),
                '{',
                '  typedef T *CreateType;',
                '',
                '  static CreateType',
                '  created (::GObject *obj)',
                '  {',
                '    return reinterpret_cast<T *> (obj);',
                '  }',
                '};'
            ])
        if self.get_value_func is not None or self.set_value_func is not None:
            l = [
                '',
                'template<typename T>',
                'struct GObject::Value::Traits<T, peel::enable_if_derived<{}, T, void>>'.format(full_name),
                '{',
                '  typedef T *UnownedType;',
            ]
            if self.take_value_func is not None:
                l.append('  typedef RefPtr<T> OwnedType;')
            l.extend([
                '',
                '  static T *',
                '  get (const ::GValue *value)',
                '  {',
                '    void *obj = {} (value);'.format(self.get_value_func),
                '    if (std::is_same<T, {}>::value)'.format(full_name),
                '      return reinterpret_cast<{} *> (obj);'.format(full_name),
                '#if defined (G_DISABLE_CAST_CHECKS) || defined (__OPTIMIZE__)',
                '    return reinterpret_cast<T *> (obj);',
                '#else',
                '    ::GType tp = static_cast<::GType> (Type::of<T> ());',
                '    return G_TYPE_CHECK_INSTANCE_CAST (obj, tp, T);',
                '#endif',
                '  }',
                '',
                '  static void',
                '  set (::GValue *value, T *object)',
                '  {',
                '    ::{} *_peel_object = reinterpret_cast<::{} *> (object);'.format(self.c_type, self.c_type),
                '    {} (value, _peel_object);'.format(self.set_value_func),
                '  }',
            ])
            if self.take_value_func is not None:
                l.extend([
                    '',
                    '  static void',
                    '  take (::GValue *value, RefPtr<T> &&object)',
                    '  {',
                    '    ::{} *_peel_object = reinterpret_cast<::{} *> (std::move (object).release_ref ());'.format(self.c_type, self.c_type),
                    '    {} (value, _peel_object);'.format(self.take_value_func),
                    '  }',
                ])
            l.append('};')
            s += '\n'.join(l)
        if self.ref_func or self.unref_func:
            s += '\n\n' + generate_ref_traits_specialization(
                full_name,
                self.c_type,
                self.ref_func,
                self.unref_func,
                self.ref_sink_func,
                template_derived=True,
            )
        si = api_tweaks.ifdef_if_needed(self.c_type)
        if not si:
            return s
        return si + '\n' + s + '\n' + api_tweaks.endif_if_needed(self.c_type)

