from peel_gen import api_tweaks
from peel_gen.utils import extract_constness_from_c_type
from peel_gen.exceptions import UnsupportedForNowException

def generate(name, c_callee, context, rv, params, throws, indent, extra_decls=None, templates=None, attributes=None):
    """
    Common helper for function-likes that need to wrap a C callee into a C++ function.
    """
    if attributes is None:
        attributes = []
    typed_tweak_callee = None
    for tweak in api_tweaks.lookup(c_callee, 'typed'):
        typed_tweak_callee = tweak[1]
        assert(params.params[-1].name == '...')
        # assert(params.params[-2].type.c_type == 'GType')
        break

    if params is not None:
        templates = templates or []
        has_typed_tweak = typed_tweak_callee is not None
        cpp_signature = params.generate_cpp_signature(context=context, typed_tweak='T' if has_typed_tweak else None)
        more_templates = params.generate_cpp_signature_templates(has_typed_tweak)
        if more_templates:
            templates.extend(more_templates)
        attributes.extend(params.generate_function_attributes(has_typed_tweak))
        if templates:
            # TODO: handle both at the same time
            assert(typed_tweak_callee is None)
        elif typed_tweak_callee:
            templates.append('typename T')
    else:
        cpp_signature = ''
        templates = None

    attributes.extend(rv.generate_rv_function_attributes())
    post_call_assumes = rv.generate_post_call_assumes()

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
    l.append(indent + 'peel_nothrow' + ''.join(' ' + attr for attr in attributes))
    fake_return_name = 'fake-return-name'
    rv_cpp_signature = rv.generate_cpp_type(name=fake_return_name, context=context)
    params_signature = '{} ({})'.format(name, cpp_signature) + (' const' if is_const else '')
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
    have_local_copies = False
    if params is not None:
        # First, declare the skip params.
        for p in params.params:
            if p in params.skip_params:
                needs_local_copy = p.direction != 'in'
                l.append(indent + '  {} {};'.format(p.generate_c_type(for_local_copy=needs_local_copy), p.generate_casted_name()))
        for p in params.params:
            if p in params.skip_params:
                casted_name = p.generate_casted_name()
                if p.direction == 'in':
                    args.append(casted_name)
                else:
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
            if p.name == '...':
                assert(p is params.params[-1])
                cast_to_c = p.generate_cast_to_c(cpp_name='args', context=context, for_local_copy=False)
                args.append(cast_to_c)
                break
            needs_local_copy = p.needs_local_copy()
            if needs_local_copy:
                have_local_copies = True
            if not needs_local_copy or p.direction == 'inout':
                if p.is_cpp_this():
                    cpp_name = 'this'
                else:
                    cpp_name = p.name
                cast_to_c = p.generate_cast_to_c(cpp_name=cpp_name, context=context, for_local_copy=needs_local_copy)
                if cast_to_c is None:
                    assert(not p.is_cpp_this())
                    args.append(p.name)
                else:
                    casted_name = p.generate_casted_name()
                    l.append(indent + '  {} {} = {};'.format(p.generate_c_type(for_local_copy=needs_local_copy), casted_name, cast_to_c))
                    if not needs_local_copy:
                        args.append(casted_name)
                    else:
                        args.append('&' + casted_name)
            else:
                casted_name = p.generate_casted_name()
                l.append(indent + '  {} {};'.format(p.generate_c_type(for_local_copy=True), casted_name))
                if p.optional:
                    args.append('{} ? &{} : nullptr'.format(p.name, casted_name))
                else:
                    args.append('&' + casted_name)

    if throws:
        l.append(indent + '  ::GError *_peel_error = nullptr;')
        args.append('error ? &_peel_error : nullptr')
        have_local_copies = True
    call = '{} ({})'.format(c_callee if typed_tweak_callee is None else typed_tweak_callee, ', '.join(args))
    if rv.c_type != 'void':
        casted_name = rv.generate_casted_name()
        cast_from_c = rv.generate_cast_from_c(c_name=casted_name, context=context)
        if cast_from_c is None and not have_local_copies and not post_call_assumes:
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

    if params is not None:
        for p in params.params:
            if p in params.skip_params:
                continue
            if not p.needs_local_copy():
                continue
            casted_name = p.generate_casted_name()
            cast_from_c = p.generate_cast_from_c(c_name=casted_name, context=context, for_local_copy=True)
            # TODO: surely it can't be that easy?
            if p.optional:
                l.extend([
                    indent + '  if ({})'.format(p.name),
                    indent + '    *{} = {};'.format(p.name, cast_from_c),
                ])
            else:
                l.append(indent + '  *{} = {};'.format(p.name, cast_from_c))
    if throws:
        l.append(indent + '  if (error)')
        l.append(indent + '    *error = peel::UniquePtr<GLib::Error>::adopt_ref (reinterpret_cast<GLib::Error *> (_peel_error));')

    if post_call_assumes:
        for pc_assume in post_call_assumes:
            l.append(indent + '  ' + pc_assume)

    if rv_expr is not None:
        l.append(indent + '  return {};'.format(rv_expr))
    l.append(indent + '}')
    return '\n'.join(l)

