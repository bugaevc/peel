def massage_c_type(c_type):
    c_type = c_type.strip()
    # Look for trailing asterisks
    ast_count = 0
    for i in range(len(c_type) -1, 0, -1):
        if c_type[i] == '*':
            ast_count += 1
        elif not c_type[i].isspace():
            break
    if not ast_count:
        return c_type
    return c_type[:i+1] + ' ' + '*' * ast_count

def add_root_namespace(c_type):
    if c_type.startswith('const '):
        return 'const ::' + c_type[6:]
    else:
        return '::' + c_type

def add_asterisk(tp):
    if tp.endswith('*'):
        return tp + '*'
    return tp + ' *'

def escape_cpp_name(name):
    if name in (
        'delete',
        'new',
        'register',
        'import',
        'export',
        'final',
        'auto',
        'union',
        'default',
        'double',
        'int',
        'continue',
        'break',
        'try',
        'catch',
        'finally',
        'FALSE',
        'TRUE',
        'NULL',
        'EOF',
        'BIG_ENDIAN',
        'LITTLE_ENDIAN',
        'UNICODE',
        'EXPORT',
        'CALLBACK',
        'ERROR',
        'min',
        'max',
        'IGNORE',
        'IN',
        'OUT',
        'OVERFLOW',
        'OPTIONAL',
        'UNDERFLOW',
        'DIFFERENCE',
        'FAR',
        'far',
        'NEAR',
        'near',
        'CONST',
        'HOST_NOT_FOUND',
        'TRY_AGAIN',
        'ALTERNATE',
        'WINDING',
        'DOUBLE_CLICK',
        'interface',
    ):
        return name + '_'
    assert(name)
    if not name[0].isidentifier():
        return '_' + name
    return name

def extract_constness_from_c_type(c_type):
    assert('[' not in c_type)
    c_type = (c_type
        .replace('gconstpointer', 'const void *')
        .replace('gpointer', 'void *')
        .replace('gchararray', 'char *')
    )
    res = []
    for part in c_type.split('*')[:-1]:
        part = part.strip()
        # TODO make this smarter
        is_const = part == 'const' or part.startswith('const') and not part[5].isidentifier()
        res.append(is_const)
    return res

intern_get_type_map = {
    'ParamSpec':           'G_TYPE_PARAM',
    'ParamSpecChar':       'G_TYPE_PARAM_CHAR',
    'ParamSpecUChar':      'G_TYPE_PARAM_UCHAR',
    'ParamSpecBoolean':    'G_TYPE_PARAM_BOOLEAN',
    'ParamSpecInt':        'G_TYPE_PARAM_INT',
    'ParamSpecUInt':       'G_TYPE_PARAM_UINT',
    'ParamSpecLong':       'G_TYPE_PARAM_LONG',
    'ParamSpecULong':      'G_TYPE_PARAM_ULONG',
    'ParamSpecInt64':      'G_TYPE_PARAM_INT64',
    'ParamSpecUInt64':     'G_TYPE_PARAM_UINT64',
    'ParamSpecUnichar':    'G_TYPE_PARAM_UNICHAR',
    'ParamSpecEnum':       'G_TYPE_PARAM_ENUM',
    'ParamSpecFlags':      'G_TYPE_PARAM_FLAGS',
    'ParamSpecFloat':      'G_TYPE_PARAM_FLOAT',
    'ParamSpecDouble':     'G_TYPE_PARAM_DOUBLE',
    'ParamSpecString':     'G_TYPE_PARAM_STRING',
    'ParamSpecParam':      'G_TYPE_PARAM_PARAM',
    'ParamSpecBoxed':      'G_TYPE_PARAM_BOXED',
    'ParamSpecPointer':    'G_TYPE_PARAM_POINTER',
    'ParamSpecValueArray': 'G_TYPE_PARAM_VALUE_ARRAY',
    'ParamSpecObject':     'G_TYPE_PARAM_OBJECT',
    'ParamSpecOverride':   'G_TYPE_PARAM_OVERRIDE',
    'ParamSpecGType':      'G_TYPE_PARAM_GTYPE',
    'ParamSpecVariant':    'G_TYPE_PARAM_VARIANT',
    'Variant':             'G_TYPE_VARIANT',
}

class VisibilityTracker:
    def __init__(self, l, initial_visibility, indent=''):
        self.l = l
        self.current_visibility = initial_visibility
        self.indent = indent

    def switch(self, visibility):
        if self.current_visibility == visibility:
            return
        self.l.append(self.indent + visibility + ':')
        self.current_visibility = visibility

def is_type_element(name, attrs):
    if name == 'type':
        return True
    if name == 'array':
        # e.g. <array name="GLib.ByteArray" c:type="GByteArray*">
        return 'name' in attrs
    return False
