class AnyType:
    def resolve_stuff(self):
        pass

    def is_passed_by_ref(self):
        return False

    def can_be_allocated_by_value(self):
        return not self.is_passed_by_ref()

class VoidType(AnyType):
    def __init__(self):
        self.c_type = 'void'

    def __repr__(self):
        return 'VoidType()'

class PlainType(AnyType):
    def __init__(self, gname, stdname, corresponds_exactly=True):
        self.gname = gname
        self.stdname = stdname
        self.corresponds_exactly = corresponds_exactly

    def __repr__(self):
        return 'PlainType(gname={}, stdname={})'.format(self.gname, self.stdname)

    def clone(self):
        return PlainType(self.gname, self.stdname, self.corresponds_exactly)

class StrType(AnyType):
    def __repr__(self):
        return 'StrType()'

class VaListType(AnyType):
    def __repr__(self):
        return 'VaListType()'

# Ns.Name -> Python object deriving from AnyType
type_map = {
    'none': VoidType(),
    'gshort': PlainType('gshort', 'short'),
    'gushort': PlainType('gushort', 'unsigned short'),
    'gint': PlainType('gint', 'int'),
    'guint': PlainType('guint', 'unsigned'),
    'gint8': PlainType('gint8', 'int8_t'),
    'guint8': PlainType('guint8', 'uint8_t'),
    'gint16': PlainType('gint16', 'int16_t'),
    'guint16': PlainType('guint16', 'uint16_t'),
    'gint32': PlainType('gint32', 'int32_t'),
    'guint32': PlainType('guint32', 'uint32_t'),
    'gint64': PlainType('gint64', 'int64_t'),
    'guint64': PlainType('guint64', 'uint64_t'),
    'glong': PlainType('glong', 'long'),
    'gulong': PlainType('gulong', 'unsigned long'),
    'gsize': PlainType('gsize', 'size_t'),
    'gssize': PlainType('gssize', 'gssize'),
    'gfloat': PlainType('gfloat', 'float'),
    'gdouble': PlainType('gdouble', 'double'),
    'gboolean': PlainType('gboolean', 'bool', corresponds_exactly=False),
    'gpointer': PlainType('gpointer', 'void *'),
    'gconstpointer': PlainType('gconstpointer', 'const void *'),
    'goffset': PlainType('goffset', 'int64_t'),
    'gintptr': PlainType('gintptr', 'intptr_t'),
    'guintptr': PlainType('guintptr', 'uintptr_t'),
    'gchar': PlainType('gchar', 'char'),
    'gunichar': PlainType('gunichar', 'gunichar'),
    'gunichar32': PlainType('gunichar32', 'gunichar32'),
    'long double': PlainType('long double', 'long double'),
    'utf8': StrType(),
    'filename': StrType(),
    'va_list': VaListType(),

    'time_t': PlainType('time_t', 'time_t'),
    'off_t': PlainType('off_t', 'off_t'),
    'pid_t': PlainType('pid_t', 'pid_t'),
    'uid_t': PlainType('uid_t', 'uid_t'),
    'dev_t': PlainType('dev_t', 'dev_t'),
    'socklen_t': PlainType('socklen_t', 'socklen_t'),

    # This is really supposed to be GObject.Type, but some GIR files
    # contain this as its own plain type.
    'GType': PlainType('GType', 'GObject::Type'),
}

def lookup_type(gir_name, ns):
    if gir_name in type_map:
        return type_map[gir_name]
    from peel_gen.namespace import Namespace
    if isinstance(ns, Namespace):
        ns_name = ns.name
    else:
        ns_name = ns
    return type_map.get(ns_name + '.' + gir_name, None)

def pick_base_type(tp, must_inherit_type):
    from peel_gen.klass import Class
    from peel_gen.interface import Interface

    if isinstance(tp, Class) and tp.parent is None:
        return None
    candidates = list(tp.recursive_prereqs())
    prereq_numbers = []
    for c in candidates:
        assert(c is not tp)
        if isinstance(tp, Class) and isinstance(c, Interface) and c.no_autobase:
            prereq_numbers.append(-1)
            continue
        prereqs = c.recursive_prereqs()
        if must_inherit_type is not None and must_inherit_type is not c and must_inherit_type not in prereqs:
            prereq_numbers.append(-1)
            continue
        prereq_numbers.append(len(prereqs))

    # Apply prefer-autobase
    for i, c in enumerate(candidates):
        if not isinstance(c, Interface):
            continue
        if not c.prefer_autobase:
            continue
        if not c.prefer_autobase in candidates:
            continue
        pai = candidates.index(c.prefer_autobase)
        if prereq_numbers[pai] == -1:
            continue
        prereq_numbers[i] = -1

    most_prereqs = -1
    most_prereqs_c = None
    most_prereqs_nd = -1
    most_prereqs_nd_c = None

    for c, n in zip(candidates, prereq_numbers):
        if n == -1:
            continue
        if n > most_prereqs:
            most_prereqs = n
            most_prereqs_c = c
        if not c.deprecated and n > most_prereqs_nd:
            most_prereqs_nd = n
            most_prereqs_nd_c = c

    assert(most_prereqs != -1)
    if most_prereqs_nd_c is must_inherit_type or most_prereqs_nd == -1:
        return most_prereqs_c
    return most_prereqs_nd_c

