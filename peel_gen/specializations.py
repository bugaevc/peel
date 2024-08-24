def generate_get_type_specialization(cpp_type, gtype_expr):
    return '\n'.join([
        'template<>',
        'inline GObject::Type',
        'GObject::Type::of<{}> ()'.format(cpp_type),
        '{',
        '  return {};'.format(gtype_expr),
        '}',
    ])

def generate_ref_traits_specialization(cpp_type, c_type, ref_func, unref_func, ref_sink_func=None, sink_func=None, template_derived=True):
    l = []
    if template_derived:
        name = 'T'
        l.extend([
            'template<typename T>',
            'struct RefTraits<T, peel::enable_if_derived<{}, T, void>>'.format(cpp_type),
        ])
    else:
        name = cpp_type
        l.extend([
            'template<>',
            'struct RefTraits<{}, void>'.format(cpp_type),
        ])
    l.extend([
        '{',
        '  static void',
        '  ref ({} *ptr)'.format(name),
        '  {',
        '    {} (reinterpret_cast<::{} *> (ptr));'.format(ref_func, c_type),
        '  }',
        '',
        '  static void',
        '  unref ({} *ptr)'.format(name),
        '  {',
        '    {} (reinterpret_cast<::{} *> (ptr));'.format(unref_func, c_type),
        '  }',
    ])
    if ref_sink_func:
        l.extend([
            '',
            '  static void',
            '  ref_sink ({} *ptr)'.format(name),
            '  {',
            '    {} (reinterpret_cast<::{} *> (ptr));'.format(ref_sink_func, c_type),
            '  }',
        ])
    if sink_func:
        l.extend([
            '',
            '  static void',
            '  sink ({} *ptr)'.format(name),
            '  {',
            '    {} (reinterpret_cast<::{} *> (ptr));'.format(sink_func, c_type),
            '  }',
        ])
    elif ref_sink_func:
        l.extend([
            '',
            '  static void',
            '  sink ({} *ptr)'.format(name),
            '  {',
            '    {} (reinterpret_cast<::{} *> (ptr));'.format(ref_sink_func, c_type),
            '    {} (reinterpret_cast<::{} *> (ptr));'.format(unref_func, c_type),
            '  }',
        ])
    l.append('};')
    return '\n'.join(l)

def generate_unique_traits_specialization(cpp_type, c_type, free_func):
    return '\n'.join([
        'template<>',
        'struct UniqueTraits<{}>'.format(cpp_type),
        '{',
        '  static void',
        '  free ({} *ptr)'.format(cpp_type),
        '  {',
        '    {} (reinterpret_cast<::{} *> (ptr));'.format(free_func, c_type),
        '  }',
        '};',
    ])

def generate_value_traits_specialization(cpp_type, unowned_type, arg_name, get_expr, set_expr):
    return '\n'.join([
        'template<>',
        'struct GObject::Value::Traits<{}>'.format(cpp_type),
        '{',
        '  typedef {} UnownedType;'.format(unowned_type),
        '',
        '  static {}'.format(unowned_type),
        '  get (const ::GValue *value)',
        '  {',
        '    return {};'.format(get_expr),
        '  }',
        '',
        '  static void',
        '  set (::GValue *value, {} {})'.format(unowned_type, arg_name),
        '  {',
        '    {};'.format(set_expr),
        '  }',
        '};'
    ])

