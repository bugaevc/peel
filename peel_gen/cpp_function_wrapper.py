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
        '({}){} -> {}'.format(
            c_signature,
            ' ' + trailing_specs if trailing_specs else '',
            rv.generate_c_type()
        ),
        indent + '{'
    ]
    if extra_decls:
        l.append(extra_decls)
    args = []
    cpp_this_arg = None
    num_local_copies = 0
    if params is not None:
        for p in params.params:
            if p in params.skip_params:
                continue
            needs_local_copy = p.needs_local_copy()
            if needs_local_copy:
                num_local_copies += 1
            if not needs_local_copy or p.direction == 'inout':
                cast_from_c = p.generate_cast_from_c(
                    c_name=p.name if not needs_local_copy else '*' + p.name,
                    context=context,
                    for_local_copy=needs_local_copy,
                    skip_params_casted=False,
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
                    if needs_local_copy and p.optional:
                        l.extend([
                            indent + '  {};'.format(cpp_type),
                            indent + '  if ({})'.format(p.name),
                            indent + '    {} = {};'.format(casted_name, cast_from_c),
                        ])
                    else:
                        l.append(indent + '  {} = {};'.format(cpp_type, cast_from_c))
                    if p.direction == 'in' and p.ownership is not None and p.ownership != 'none':
                        arg = 'std::move ({})'.format(casted_name)
                    elif not needs_local_copy:
                        arg = casted_name
                    elif not p.optional:
                        arg = '&' + casted_name
                    else:
                        arg = '{} ? &{} : nullptr'.format(p.name, casted_name)
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
                if not p.optional:
                    args.append('&' + casted_name)
                else:
                    args.append('{} ? &{} : nullptr'.format(p.name, casted_name))
    if throws:
        l.append(indent + '  peel::UniquePtr<GLib::Error> _peel_error;')
        if num_local_copies != 0:
            # If we have local copies, we always need to know whether
            # an error has happened, even if our caller doesn't.
            args.append('&_peel_error')
        else:
            args.append('error ? &_peel_error : nullptr')
        num_local_copies += 1
    call = '{} ({})'.format(cpp_callee, ', '.join(args))
    if cpp_this_arg:
        call = '{}->{}'.format(cpp_this_arg, call)
    if rv.c_type != 'void':
        casted_name = rv.generate_casted_name()
        cast_to_c = rv.generate_cast_to_c(
            cpp_name=casted_name,
            context=context,
            for_local_copy=False,
            skip_params_casted=False,
        )
        if cast_to_c is None and num_local_copies == 0:
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

    tmp_indent = indent
    if throws:
        if num_local_copies == 1:
            l.extend([
                indent + '  if (error)',
                indent + '    *error = reinterpret_cast<::GError *> (std::move (_peel_error).release_ref ());',
            ])
        else:
            l.extend([
                indent + '  if (_peel_error)',
                indent + '    {',
                indent + '      if (error)',
                indent + '        *error = reinterpret_cast<::GError *> (std::move (_peel_error).release_ref ());',
                indent + '    }',
                indent + '  else',
                indent + '    {',
                # indent + '      if (error)',
                # indent + '        *error = nullptr;'
            ])
            tmp_indent = indent + '    '

    if params is not None:
        for p in params.params:
            if not p.needs_local_copy():
                continue
            casted_name = p.generate_casted_name()
            cast_from_c = p.generate_cast_to_c(
                cpp_name=casted_name,
                context=context,
                for_local_copy=True,
                skip_params_casted=False,
            )
            if cast_from_c is None:
                cast_from_c = p.name
            if p.optional:
                l.extend([
                    tmp_indent + '  if ({})'.format(p.name),
                    tmp_indent + '    *{} = {};'.format(p.name, cast_from_c),
                ])
            else:
                l.append(tmp_indent + '  *{} = {};'.format(p.name, cast_from_c))

    if tmp_indent != indent:
        assert(throws)
        assert(num_local_copies > 1)
        l.append(indent + '    }')

    if rv_expr is not None:
        l.append(indent + '  return {};'.format(rv_expr))
    l.append(indent + '}')

    return '\n'.join(l)

