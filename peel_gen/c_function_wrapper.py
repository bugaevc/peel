from peel_gen import api_tweaks
from peel_gen.utils import extract_constness_from_c_type, make_simple_decl
from peel_gen.exceptions import UnsupportedForNowException

def generate(name, c_callee, context, rv, params, throws, indent, extra_decls=None, templates=None, attributes=None, raw_param_names=()):
    """
    Common helper for function-likes that need to wrap a C callee into a C++ function.
    """
    if attributes is None:
        attributes = []
    object_new = False
    for tweak in api_tweaks.lookup(c_callee, 'vararg'):
       if tweak[1] == 'object-new':
           object_new = True
    typed_tweak_callee = None
    for tweak in api_tweaks.lookup(c_callee, 'typed'):
        typed_tweak_callee = tweak[1]
        assert(params.params[-1].name == '...')
        # assert(params.params[-2].type.c_type == 'GType')
        break

    if params is not None:
        templates = templates or []
        has_typed_tweak = typed_tweak_callee is not None
        cpp_signature = params.generate_cpp_signature(
            context=context,
            typed_tweak='T' if has_typed_tweak else None,
            raw_param_names=raw_param_names,
        )
        more_templates = params.generate_cpp_signature_templates(has_typed_tweak)
        if more_templates:
            templates.extend(more_templates)
        # FIXME attribute indexes for raw params
        attributes.extend(params.generate_function_attributes(has_typed_tweak))
        if templates:
            # TODO: handle both at the same time
            assert(typed_tweak_callee is None)
        elif typed_tweak_callee:
            templates.append('typename T')
    else:
        cpp_signature = ''
        templates = None

    attributes.extend(rv.generate_rv_function_attributes(throws))

    post_call_assumes_non_thrown = rv.generate_post_call_assumes(thrown=False)
    #if throws:
    #    post_call_assumes_thrown = rv.generate_post_call_assumes(thrown=True)
    #else:
    #    post_call_assumes_thrown = None
    have_post_call_assumes = post_call_assumes_non_thrown #or post_call_assumes_thrown

    if throws:
        error_param = 'peel::UniquePtr<GLib::Error> *error'
        if not cpp_signature:
            cpp_signature = error_param
        else:
            cpp_signature += ', ' + error_param

    is_static = True
    is_const = False
    if params is not None:
        for p in params.params:
            if not p.is_cpp_this():
                continue
            is_static = False
            constness = extract_constness_from_c_type(p.c_type)
            if len(constness) != 1:
                raise UnsupportedForNowException('weird number of indirections in instance parameter')
            is_const = constness[0]
            break
    if is_static:
        assert(not is_const)

    l = []
    if templates:
        l.append(indent + 'template<{}>'.format(', '.join(templates)))
    if attributes:
        l.append(indent + ' '.join(attr for attr in attributes))
    fake_return_name = 'fake-return-name'
    rv_cpp_signature = rv.generate_cpp_type(name=fake_return_name, context=context)
    params_signature = '{} ({})'.format(name, cpp_signature) + (' const' if is_const else '') + ' noexcept'
    if rv_cpp_signature.endswith(fake_return_name):
        rv_cpp_type = rv_cpp_signature[:-len(fake_return_name)].strip()
        l.extend([
            indent + '{}{}'.format('static ' if is_static else '', rv_cpp_type),
            indent + params_signature,
        ])
    else:
        fake_return_name_pos = rv_cpp_signature.find(fake_return_name)
        space_pos = rv_cpp_signature.rfind(' ', 0, fake_return_name_pos)
        assert(space_pos != -1)
        l.extend([
            indent + '{}{}'.format('static ' if is_static else '', rv_cpp_signature[:space_pos]),
            indent + '{}{}{}'.format(
                rv_cpp_signature[space_pos+1:fake_return_name_pos],
                params_signature,
                rv_cpp_signature[fake_return_name_pos+len(fake_return_name):],
            )
        ])
    l.append(indent + '{')
    if extra_decls:
        l.append(extra_decls)
    args = []
    num_local_copies = 0
    if params is not None:
        # First, declare the skip params.
        for p in params.params:
            if p in params.skip_params and not p.name in raw_param_names:
                needs_local_copy = p.direction != 'in'
                l.append(indent + '  {} {};'.format(p.generate_c_type(for_local_copy=needs_local_copy), p.generate_casted_name()))
        for p in params.params:
            if p.name in raw_param_names:
                assert(not p.is_cpp_this())
                args.append(p.name)
                continue
            if p in params.skip_params:
                casted_name = p.generate_casted_name()
                if p.direction == 'in':
                    args.append(casted_name)
                else:
                    # TODO: Should pass nullptr when the owning param is nullptr.
                    args.append('&' + casted_name)
                continue
            if typed_tweak_callee and p is params.params[-2]:
                l.extend([
                    indent + '  GObject::Value _peel_value { GObject::Type::of<T> () };',
                    indent + '  _peel_value.set<T> (value);',
                    indent + '  ::GValue *_peel_value_ref = reinterpret_cast<::GValue *> (&_peel_value);',
                ])
                args.append('_peel_value_ref')
                break
            needs_local_copy = p.needs_local_copy()
            if needs_local_copy:
                num_local_copies += 1

            if p.name == '...':
                assert(p is params.params[-1])
                assert(not needs_local_copy)
                cast_to_c = p.generate_cast_to_c(
                    cpp_name='args',
                    context=context,
                    for_local_copy=needs_local_copy,
                    skip_params_casted=True,
                    vararg_reference_param=params.params[-2],
                )
                args.append(cast_to_c)
                break

            if not needs_local_copy or p.direction == 'inout':
                if p.is_cpp_this():
                    cpp_name = 'this'
                else:
                    cpp_name = p.name
                cast_to_c = p.generate_cast_to_c(
                    cpp_name=cpp_name if not needs_local_copy else '*' + cpp_name,
                    context=context,
                    for_local_copy=needs_local_copy,
                    skip_params_casted=True,
                )
                if cast_to_c is None:
                    assert(not p.is_cpp_this())
                    args.append(p.name)
                else:
                    casted_name = p.generate_casted_name()
                    c_type = p.generate_c_type(for_local_copy=needs_local_copy)
                    if needs_local_copy and p.optional:
                        l.extend([
                            indent + '  {} {};'.format(c_type, casted_name),
                            indent + '  if ({})'.format(cpp_name),
                            indent + '    {} = {};'.format(casted_name, cast_to_c),
                        ])
                    else:
                        l.append(indent + '  {} = {};'.format(make_simple_decl(c_type, casted_name), cast_to_c))
                    if not needs_local_copy:
                        args.append(casted_name)
                    elif not p.optional:
                        args.append('&' + casted_name)
                    else:
                        args.append('{} ? &{} : nullptr'.format(cpp_name, casted_name))
            else:
                casted_name = p.generate_casted_name()
                c_type = p.generate_c_type(for_local_copy=True)
                l.append(indent + '  {};'.format(make_simple_decl(c_type, casted_name)))
                if p.optional:
                    args.append('{} ? &{} : nullptr'.format(p.name, casted_name))
                else:
                    args.append('&' + casted_name)

    if throws:
        l.append(indent + '  ::GError *_peel_error = nullptr;')
        if num_local_copies != 0:
            # If we have local copies, we always need to know whether
            # an error has happened, even if our caller doesn't.
            args.append('&_peel_error')
        else:
            args.append('error ? &_peel_error : nullptr')
        num_local_copies += 1

    callee_expr = c_callee
    if typed_tweak_callee:
        callee_expr = typed_tweak_callee
    elif object_new:
        # TODO: unhardcode the name Args...
        assert('typename... Args' in templates)
        invoke_template_params = [
            rv.generate_c_type(for_local_copy=False),
            'decltype ({}) *'.format(c_callee),
        ] + [p.generate_c_type(for_local_copy=False) for p in params.params[:-2]]
        callee_expr = 'peel::internals::ObjectCreateHelper<Args...>::template invoke<{}>'.format(', '.join(invoke_template_params))
        args = [c_callee] + args
        args.pop(-2)
    call = '{} ({})'.format(callee_expr, ', '.join(args))
    if rv.c_type != 'void':
        casted_name = rv.generate_casted_name()
        cast_from_c = rv.generate_cast_from_c(
            c_name=casted_name,
            context=context,
            for_local_copy=False,
            skip_params_casted=True,
        )
        if cast_from_c is None and num_local_copies == 0 and not have_post_call_assumes:
            rv_expr = call
        else:
            l.append(indent + '  {} {} = {};'.format(rv.generate_c_type(for_local_copy=False), casted_name, call))
            if cast_from_c is None:
                rv_expr = casted_name
            else:
                rv_expr = cast_from_c
    else:
        l.append(indent + '  {};'.format(call))
        rv_expr = None

    tmp_indent = indent
    if throws:
        if num_local_copies == 1:
            l.extend([
                indent + '  if (error)',
                indent + '    *error = peel::UniquePtr<GLib::Error>::adopt_ref (reinterpret_cast<GLib::Error *> (_peel_error));',
            ])
        else:
            l.extend([
                indent + '  if (_peel_error)',
                indent + '    {',
                indent + '      if (error)',
                indent + '        *error = peel::UniquePtr<GLib::Error>::adopt_ref (reinterpret_cast<GLib::Error *> (_peel_error));',
                indent + '      else',
                indent + '        g_error_free (_peel_error);',
                indent + '    }',
                indent + '  else',
                indent + '    {',
                indent + '      if (error)',
                indent + '        *error = nullptr;',
            ])
            tmp_indent = indent + '    '

    if params is not None:
        for p in params.params:
            if p in params.skip_params or p.name in raw_param_names:
                continue
            if not p.needs_local_copy():
                continue
            casted_name = p.generate_casted_name()
            cast_from_c = p.generate_cast_from_c(
                c_name=casted_name,
                context=context,
                for_local_copy=True,
                skip_params_casted=True,
            )
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
    elif not throws and post_call_assumes_non_thrown:
        for pc_assume in post_call_assumes_non_thrown:
            l.append(indent + '  ' + pc_assume)

    if rv_expr is not None:
        l.append(indent + '  return {};'.format(rv_expr))
    l.append(indent + '}')
    return '\n'.join(l)

