import sys
import os
import xml.sax
from pathlib import Path

from peel_gen.node_handler import NodeHandler
from peel_gen.namespace import Namespace
from peel_gen import api_tweaks

# (Name, version) (e.g. ('Gtk', '4.0')) to Repository
repository_map = dict()

class Repository(NodeHandler):
    def __init__(self, name, attrs):
        self.name = name
        self.c_includes = []
        self.package_names = []
        self.namespaces = []

    def __repr__(self):
        return 'Repository({})'.format(self.name)

    def start_child_element(self, name, attrs):
        if name == 'include':
            gir_name = attrs['name']
            gir_version = attrs['version']
            if (gir_name, gir_version) not in repository_map:
                find_and_parse_gir_repo(gir_name, gir_version)
            return
        elif name == 'c:include':
            name = attrs['name']
            for tweak in api_tweaks.lookup(name, 'repo-skip'):
                if tweak[1] == self.name:
                    return
            self.c_includes.append(name)
            return
        elif name == 'package':
            name = attrs['name']
            for tweak in api_tweaks.lookup(name, 'repo-skip'):
                if tweak[1] == self.name:
                    return
            self.package_names.append(name)
            return
        elif name == 'namespace':
            ns = Namespace(attrs)
            self.namespaces.append(ns)
            ns.c_includes = self.c_includes
            return ns

    def generate_basic_header(self):
        l = [
            '#pragma once',
            '',
            '/* Auto-generated, do not modify */',
        ]
        for package_name in self.package_names:
            l.append('/* Package {} */'.format(package_name))
        return '\n'.join(l)

    def generate_header(self, extra_includes):
        l = [self.generate_basic_header()]
        l.extend([
            '',
            '#include <peel/GObject/Type.h>',
            '#include <peel/RefPtr.h>',
            '#include <peel/FloatPtr.h>',
            '#include <peel/UniquePtr.h>',
            '#include <peel/ArrayRef.h>',
            '#include <peel/signal.h>',
            '#include <peel/callback.h>',
            '#include <peel/property.h>',
            '#include <peel/lang.h>',
            '#include <cstdint>',
            '#include <utility>',
        ])
        for include in self.c_includes + extra_includes:
            l.append('#include <{}>'.format(include))
        l.extend([
            '',
            'peel_begin_header',
            '',
            'namespace peel',
            '{',
            '',
        ])
        return '\n'.join(filter(lambda line: line is not None, l))

    def generate_footer(self):
        return (
            '} /* namespace peel */\n'
            '\n'
            'peel_end_header\n'
        )

def work_out_gir_paths():
    # See gi-docgen:gidocgen/utils.py:default_search_paths()
    paths = [Path.cwd()]
    gi_gir_path = os.getenv('GI_GIR_PATH')
    if gi_gir_path:
        paths.extend(map(Path, gi_gir_path.split(os.pathsep)))
    if sys.platform != 'win32':
        data_home = os.getenv('XDG_DATA_HOME')
        if data_home is not None:
            data_home = Path(data_home)
        else:
            try:
                home = Path.home()
                data_home = home / '.local/share'
            except RuntimeError:
                pass
        if data_home is not None:
            paths.append(data_home / 'gir-1.0')
        data_dirs = os.getenv('XDG_DATA_DIRS', '/usr/share:/usr/local/share').split(os.pathsep)
        for data_dir in data_dirs:
            paths.append(Path(data_dir) / 'gir-1.0')
    return paths

gir_paths = work_out_gir_paths()

def find_gir_file(gir_name, gir_version):
    for gir_dir in gir_paths:
        p = gir_dir / (gir_name + '-' + gir_version + '.gir')
        if p.exists():
            return p
    return None

def find_and_parse_gir_repo(name, version):
    gir_path = find_gir_file(name, version)
    if gir_path is None:
        raise RuntimeError('GIR file for {}-{} not found'.format(name, version))
    parser = xml.sax.make_parser()
    import peel_gen.sax_handler
    sax_handler = peel_gen.sax_handler.SaxHandler(name, version)
    parser.setContentHandler(sax_handler)
    parser.parse(str(gir_path))
    return sax_handler.repo
