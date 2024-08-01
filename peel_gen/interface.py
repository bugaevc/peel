from peel_gen.defined_type import DefinedType
from peel_gen.method import Method
from peel_gen.constructor import Constructor
from peel_gen.vfunc import Vfunc
from peel_gen.signal import Signal
from peel_gen.property import Property
from peel_gen.type import lookup_type, pick_base_type
from peel_gen.exceptions import UnsupportedForNowException
from peel_gen.specializations import generate_get_type_specialization
from peel_gen import api_tweaks

class Interface(DefinedType):
    def __init__(self, attrs, ns):
        super().__init__(attrs, ns)
        self.get_type = attrs.get('glib:get-type', None)
        self.constructors = []
        self.methods = []
        self.vfuncs = []
        self.signals = []
        self.props = []
        self.prereq_names = []
        self.is_refcounted = True  # Fine if it always requires GObject?
        self.type_struct_name = attrs.get('glib:type-struct', None)
        self.type_struct = None

        self.is_initially_floating = False
        self.is_initially_fully_unowned = False

        self.no_autobase = False
        self.prefer_autobase = None
        self.prefer_autobase_name = None

        for tweak in api_tweaks.lookup(self.c_type):
            if tweak[0] == 'no-autobase':
                self.no_autobase = True
            elif tweak[0] == 'prefer-autobase':
                self.prefer_autobase_name = tweak[1]

    def is_passed_by_ref(self):
        return True

    def start_child_element(self, name, attrs):
        if name == 'prerequisite':
            self.prereq_names.append(attrs['name'])
            return
        elif name in ('method', 'function'):
            m_name = attrs['name']
            if m_name == 'new' or m_name.startswith('new_'):
                c = Constructor(attrs, self)
                self.constructors.append(c)
                return c
            m = Method(attrs, self)
            self.methods.append(m)
            return m
        elif name == 'virtual-method':
            v = Vfunc(attrs, self)
            self.vfuncs.append(v)
            return v
        elif name == 'glib:signal':
            s = Signal(attrs, self)
            self.signals.append(s)
            return s
        elif name == 'property':
            p = Property(attrs, self)
            self.props.append(p)
            return p

    def resolve_stuff(self):
        if self.has_resolved_stuff:
            return
        super().resolve_stuff()

        self.prereqs = []
        for name in self.prereq_names:
            prereq = lookup_type(name, self.ns)
            assert(prereq is not None)
            self.prereqs.append(prereq)
        # FIXME: There could be non-GObject interfaces. But, there doesn't seem
        # to be a way to tell from peel_gen.IR whether an interface requires GObject or
        # not.
        if not self.prereqs:
            self.prereqs.append(lookup_type('GObject.Object', self.ns))

        if self.type_struct_name is not None:
            self.type_struct = lookup_type(self.type_struct_name, self.ns)
            assert(self.type_struct is not None)
            self.type_struct.resolve_stuff()
        if self.prefer_autobase_name is not None:
            self.prefer_autobase = lookup_type(self.prefer_autobase_name, self.ns)
            assert(self.prefer_autobase is not None)

        self.cpp_base_type = pick_base_type(self, must_inherit_type=None)
        self.is_initially_floating = any(pr.is_initially_floating for pr in self.prereqs)
        self.is_initially_fully_unowned = any(pr.is_initially_fully_unowned for pr in self.prereqs)

    def recursive_prereqs(self):
        self.resolve_stuff()
        s = set(self.prereqs)
        for p in self.prereqs:
            s.update(p.recursive_prereqs())
        return s

    def generate_extra_include_members(self):
        self.resolve_stuff()
        s = { lookup_type('TypeInterface', 'GObject') }
        if self.cpp_base_type:
            s.add(self.cpp_base_type)
        for member in self.constructors + self.methods + self.vfuncs + self.props + self.signals + [self.type_struct]:
            if member is None:
                continue
            try:
                s.update(member.generate_extra_include_members())
            except UnsupportedForNowException:
                pass
        s.discard(self)
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
        for nested_type in self.nested_types:
            s.discard(nested_type)
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
        s.discard(self)
        for nested_type in self.nested_types:
            s.discard(nested_type)
        return s

    def generate_forward_decl(self, for_nested=False):
        if for_nested or not self.nested_in:
            return 'class /* interface */ {};'.format(self.own_name)

    def generate(self):
        if self.cpp_base_type:
            cpp_base_type_emit_name = self.cpp_base_type.emit_name_for_context(self)
        else:
            cpp_base_type_emit_name = None
        l = [
            'class /* interface */ {}'.format(self.emit_def_name),
            '{',
            'private:',
            '  {} () = delete;'.format(self.own_name),
            '  {} (const {} &) = delete;'.format(self.own_name, self.own_name),
            '  {} ({} &&) = delete;'.format(self.own_name, self.own_name),
            '',
            '  {} &'.format(self.own_name),
            '  operator = (const {} &) = delete;'.format(self.own_name),
            '  {} &'.format(self.own_name),
            '  operator = ({} &&) = delete;'.format(self.own_name),
            '',
            'public:'
        ]
        if self.cpp_base_type:
            l[0] += ' : public {}'.format(cpp_base_type_emit_name)
        if self.prereqs:
            l.insert(1, '/* requires ' + ', '.join(
                prereq.emit_name_for_context(self) for prereq in self.prereqs
            ) + ' */')
        l.append(self.generate_nested_type_defs())
        for constructor in self.constructors:
            try:
                l.append(constructor.generate())
            except UnsupportedForNowException as e:
                l.append('  /* Unsupported for now: {}: {} */'.format(constructor.name, e.reason))
            l.append('')
        for method in self.methods:
            try:
                l.append(method.generate(indent='  '))
            except UnsupportedForNowException as e:
                l.append('  /* Unsupported for now: {}: {} */'.format(method.name, e.reason))
            l.append('')
        for signal in self.signals:
            try:
                signal.resolve_stuff()
                l.append(signal.generate())
            except UnsupportedForNowException as e:
                l.append('  /* Unsupported for now: {}: {} */'.format(signal.name, e.reason))
            l.append('')
        for prop in self.props:
            try:
                l.append(prop.generate())
            except UnsupportedForNowException as e:
                l.append('  /* Unsupported for now: {}: {} */'.format(prop.name, e.reason))
            l.append('')
        for vfunc in self.vfuncs:
            try:
                vfunc.resolve_stuff()
                l.append(vfunc.generate())
            except UnsupportedForNowException as e:
                l.append('  /* Unsupported for now: {}: {} */'.format(vfunc.name, e.reason))
            l.append('')
        if self.type_struct is not None:
            l.append(self.type_struct.generate())
        if not l[-1]:
            l.pop()
        l.append('};' + ' /* interface {} */'.format(self.emit_name))
        if self.cpp_base_type:
            l.extend([
                'static_assert (sizeof ({}) == sizeof ({}),'.format(self.emit_def_name, cpp_base_type_emit_name),
                '               "{} size mismatch");'.format(self.emit_def_name),
                'static_assert (alignof ({}) == alignof ({}),'.format(self.emit_def_name, cpp_base_type_emit_name),
                '               "{} align mismatch");'.format(self.emit_def_name),
            ])
        return '\n'.join(l)

    def generate_specializations(self):
        if self.get_type:
            return generate_get_type_specialization(self.emit_name_for_context(None), self.get_type + ' ()')

