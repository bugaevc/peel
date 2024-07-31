from peel_gen.exceptions import UnsupportedForNowException

def generate(cpp_callee, context, rv, params, throws, indent, extra_decls=None, trailing_specs=None):
    if params is not None:
        c_signature = params.generate_c_signature()
    else:
        c_signature = ''

    if throws:
        error_param = '::GError **error'
        if c_signature:
            c_signature += ', ' + error_param
        else:
            c_signature = error_param

    l = [
        '({}){} -> {}'.format(c_signature, ' ' + trailing_specs if trailing_specs else '', rv.generate_c_type()),
        indent + '{'
    ]
    if extra_decls:
        l.append(extra_decls)
    args = []
    cpp_this_arg = None
    have_local_copies = False
    if params is not None:
        for p in params.params:
            needs_local_copy = p.needs_local_copy()
            if needs_local_copy:
                have_local_copies = True
            if p in params.skip_params:
                assert(not needs_local_copy)
                continue
            if not needs_local_copy or p.direction == 'inout':
                cast_from_c = p.generate_cast_from_c(
                    c_name=p.name,
                    context=context,
                    for_local_copy=needs_local_copy,
                )
                if cast_from_c is None:
                    assert(not p.is_instance)
                    args.append(p.name)
                else:
                    casted_name = p.generate_casted_name()
                    cpp_type = p.generate_cpp_type(
                        name=casted_name,
                        context=context,
                        for_local_copy=needs_local_copy,
                    )
                    l.append(indent + '  {} = {};'.format(cpp_type, cast_from_c))
                    if p.direction == 'in' and p.ownership is not None and p.ownership != 'none':
                        arg = 'std::move ({})'.format(casted_name)
                    elif needs_local_copy:
                        arg = '&' + casted_name
                    else:
                        arg = casted_name
                    if p.is_cpp_this():
                        cpp_this_arg = arg
                    else:
                        args.append(arg)
            else:
                casted_name = p.generate_casted_name()
                cpp_type = p.generate_cpp_type(
                    name=casted_name,
                    context=context,
                    for_local_copy=True,
                )
                l.append(indent + '  {};'.format(cpp_type))
                args.append('&' + casted_name)
    if throws:
        l.append(indent + '  peel::UniquePtr<GLib::Error> _peel_error;')
        args.append('error ? &_peel_error : nullptr')
        have_local_copies = True
    call = '{} ({})'.format(cpp_callee, ', '.join(args))
    if cpp_this_arg:
        call = '{}->{}'.format(cpp_this_arg, call)
    if rv.c_type != 'void':
        casted_name = rv.generate_casted_name()
        cast_to_c = rv.generate_cast_to_c(cpp_name=casted_name, context=context)
        if cast_to_c is None and not have_local_copies:
            rv_expr = call
        else:
            l.append(indent + '  {} = {};'.format(rv.generate_cpp_type(name=casted_name, context=context), call))
            if cast_to_c is None:
                rv_expr = casted_name
            else:
                rv_expr = cast_to_c
    else:
        l.append(indent + '  {};'.format(call))
        rv_expr = None

    if params is not None:
        for p in params.params:
            if not p.needs_local_copy():
                continue
            casted_name = p.generate_casted_name()
            cast_from_c = p.generate_cast_to_c(cpp_name=casted_name, context=context, for_local_copy=True)
            if cast_from_c is None:
                cast_from_c = p.name
            # TODO: surely it can't be that easy?
            l.append(indent + '  *{} = {};'.format(p.name, cast_from_c))
    if throws:
        l.append(indent + '  if (error)')
        l.append(indent + '    *error = reinterpret_cast<::GError *> (std::move (_peel_error).release_ref ());')

    if rv_expr is not None:
        l.append(indent + '  return {};'.format(rv_expr))
    l.append(indent + '}')

    return '\n'.join(l)

