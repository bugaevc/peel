from argparse import ArgumentParser
from pathlib import Path

# Gotta love Python's module system.
import sys
sys.path.insert(0, str(Path(__file__).parent.parent))

if __name__ == '__main__' and __package__ is None:
    __package__ = 'peel_gen'

from peel_gen import api_tweaks
from peel_gen.type import AnyType
from peel_gen.function import Function
from peel_gen.exceptions import UnsupportedForNowException
from peel_gen.repository import find_and_parse_gir_repo, repository_map
import peel_gen.namespace

def write_required_forward_decls(target_file, forward_members):
    per_ns = dict()
    for member in forward_members:
        if member.ns not in per_ns:
            per_ns[member.ns] = [member]
        else:
            per_ns[member.ns].append(member)
    first = True
    for ns in per_ns:
        if not first:
            target_file.write('\n')
        first = False
        target_file.write('namespace {}\n{{\n'.format(ns.name))
        for member in per_ns[ns]:
            try:
                s = member.generate_forward_decl()
            except UnsupportedForNowException:
                continue
            if s:
                target_file.write(s + '\n')
        target_file.write('}} /* namespace {} */\n'.format(ns.name))

def emit_umbrella_header(repo, file_path, includes):
    with file_path.open(mode='w') as target_file:
        header = repo.generate_basic_header()
        target_file.write(header + '\n')
        for include in includes:
            target_file.write('#include <{}>\n'.format(include))

def emit_file(repo, file_path, members):
    extra_include_members = set()
    extra_forward_members = set()
    extra_include_at_end_members = set()
    for member in members:
        try:
            extra_include_members.update(member.generate_extra_include_members())
            extra_forward_members.update(member.generate_extra_forward_members())
            extra_include_at_end_members.update(member.generate_extra_include_at_end_members())
        except UnsupportedForNowException:
            continue

    extra_includes = []
    for extra_include_member in extra_include_members:
        if not extra_include_member.ns.should_emit_file(extra_include_member):
            continue
        extra_include_at_end_members.discard(extra_include_member)
        # extra_forward_members.discard(extra_include_member)
        if not extra_include_member.ns.emit_raw:
            extra_includes.append(extra_include_member.make_file_path())
        else:
            extra_includes.extend(extra_include_member.ns.c_includes)

    extra_forward_members.update([member for member in members if isinstance(member, AnyType)])
    for member in members:
        extra_include_at_end_members.discard(member)

    extra_includes_at_end = []
    for extra_include_member in extra_include_at_end_members:
        if not extra_include_member.ns.should_emit_file(extra_include_member):
            continue
        if not extra_include_member.ns.emit_raw:
            extra_includes_at_end.append(extra_include_member.make_file_path())
        else:
            # FIXME: does this make sense?
            extra_includes_at_end.extend(extra_include_member.ns.c_includes)

    extra_includes = sorted(set(map(str, extra_includes)))
    extra_forward_members = sorted(extra_forward_members, key=lambda m: m.emit_name_for_context(None))
    extra_includes_at_end = sorted(set(map(str, extra_includes_at_end)))

    per_ns = dict()
    for member in members:
        if member.ns not in per_ns:
            per_ns[member.ns] = [member]
        else:
            per_ns[member.ns].append(member)

    # Avoid creating the file if we're binding a single member and we're skipping it.
    if len(members) == 1:
        try:
            member_generated = member.generate()
        except UnsupportedForNowException:
            return False
    else:
        member_generated = None

    with file_path.open(mode='w') as target_file:
        header = repo.generate_header(extra_includes=extra_includes)
        target_file.write(header)
        write_required_forward_decls(target_file, extra_forward_members)
        for member in members:
            if isinstance(member, AnyType):
                try:
                    s = member.generate_specializations()
                except UnsupportedForNowException:
                    continue
                else:
                    if s:
                        target_file.write('\n' + s + '\n')
        for ns in per_ns:
            target_file.write('\n\nnamespace {}\n{{\n'.format(member.ns.name))
            for member in per_ns[ns]:
                if member_generated is not None:
                    target_file.write(member_generated)
                    break
                else:
                    try:
                        s = member.generate()
                    except UnsupportedForNowException as e:
                        s = '/* Unsupported for now: {}: {} */'.format(member.name, e.reason)
                    target_file.write(s + '\n\n')
            target_file.write('\n\n}} /* namespace {} */\n'.format(member.ns.name))
        target_file.write(repo.generate_footer())
        if extra_includes_at_end:
            target_file.write('\n')
        for include in extra_includes_at_end:
            target_file.write('#include <{}>\n'.format(include))
    return True

def emit_repo(repo):
    for ns in repo.namespaces:
        if ns.name in peel_gen.namespace.raw_namespace_names:
            continue
        base_path = Path('peel') / ns.name
        base_path.mkdir(parents=True, exist_ok=True)
        functions = []
        emitted_files = []
        for member in ns.members:
            if isinstance(member, Function):
                functions.append(member)
                continue
            elif not ns.should_emit_file(member):
                continue
            file_path = member.make_file_path()
            if emit_file(repo, file_path, [member]):
                emitted_files.append(file_path)

        if functions:
            file_path = base_path / 'functions.h'
            if emit_file(repo, file_path, functions):
                emitted_files.append(file_path)

        file_path = base_path / '{}.h'.format(ns.name)
        emit_umbrella_header(repo, file_path, emitted_files)

def main():
    arg_parser = ArgumentParser(prog='gen.py')
    # TODO: Make it possible to specify multiple repos
    arg_parser.add_argument('name', metavar='repo-name')
    # TODO: Don't require version
    arg_parser.add_argument('version', metavar='repo-version')

    arg_parser.add_argument('--raw', action='append', default=[], metavar='namespace')
    arg_parser.add_argument('--api-tweaks', action='append', default=[], metavar='path')
    arg_parser.add_argument('-r', '--recursive', action='store_true')

    args = arg_parser.parse_args()
    peel_gen.namespace.raw_namespace_names = args.raw

    for path in args.api_tweaks:
        api_tweaks.load_from_file(path)

    repo = find_and_parse_gir_repo(args.name, args.version)
    if args.recursive:
        for repo in repository_map.values():
            emit_repo(repo)
    else:
        emit_repo(repo)

if __name__ == '__main__':
    main()
