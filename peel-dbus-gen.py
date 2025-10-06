#! /usr/bin/env python3

import sys
import xml.sax
from argparse import ArgumentParser
from pathlib import Path

# These will be set by Meson in the installed file.
peel_gen_module_path = '@module_path@'

if peel_gen_module_path == '@' + 'module_path' + '@':
    # Running uninstalled from the source tree.
    peel_gen_module_path = str(Path(__file__).parent)

sys.path.insert(0, peel_gen_module_path)

if __name__ == '__main__' and __package__ is None:
    __package__ = 'peel_dbus_gen'

from peel_dbus_gen.interface import Interface
from peel_dbus_gen.method import Method
from peel_dbus_gen.signal import Signal
from peel_dbus_gen.argument import Argument
from peel_dbus_gen.property import Property

class SaxHandler(xml.sax.handler.ContentHandler):
    def __init__(self):
        self.all_interfaces = []
        self.interface = None
        self.method = None
        self.signal = None
        self.arg = None
        self.property = None

    def startElement(self, name, attrs):
        if name == 'node':
            assert(self.interface is None)
            assert(self.method is None)
            assert(self.signal is None)
            assert(self.arg is None)
            assert(self.property is None)
        elif name == 'interface':
            assert(self.interface is None)
            assert(self.method is None)
            assert(self.signal is None)
            assert(self.arg is None)
            assert(self.property is None)
            self.interface = Interface(attrs)
            self.all_interfaces.append(self.interface)
        elif name == 'method':
            assert(self.interface is not None)
            assert(self.method is None)
            assert(self.signal is None)
            assert(self.arg is None)
            assert(self.property is None)
            self.method = Method(attrs, iface=self.interface)
            self.interface.methods.append(self.method)
        elif name == 'signal':
            assert(self.interface is not None)
            assert(self.method is None)
            assert(self.signal is None)
            assert(self.arg is None)
            assert(self.property is None)
            self.signal = Signal(attrs)
            self.interface.signals.append(self.signal)
        elif name == 'arg':
            assert(self.interface is not None)
            assert((self.method is not None) != (self.signal is not None))
            assert(self.arg is None)
            assert(self.property is None)
            if self.method is not None:
                self.arg = Argument(attrs, default_direction='in')
                self.method.arguments.append(self.arg)
            else:
                self.arg = Argument(attrs, default_direction='out')
                self.signal.arguments.append(self.arg)
        elif name == 'property':
            assert(self.interface is not None)
            assert(self.method is None)
            assert(self.signal is None)
            assert(self.arg is None)
            assert(self.property is None)
            self.property = Property(attrs, iface=self.interface)
            self.interface.properties.append(self.property)
        else:
            print('Unhandled tag', name, file=sys.stderr)

    def endElement(self, name):
        if name == 'node':
            assert(self.interface is None)
            assert(self.method is None)
            assert(self.signal is None)
            assert(self.arg is None)
            assert(self.property is None)
        elif name == 'interface':
            assert(self.interface is not None)
            assert(self.method is None)
            assert(self.signal is None)
            assert(self.arg is None)
            assert(self.property is None)
            self.interface = None
        elif name == 'method':
            assert(self.interface is not None)
            assert(self.method is not None)
            assert(self.signal is None)
            assert(self.arg is None)
            assert(self.property is None)
            self.method = None
        elif name == 'signal':
            assert(self.interface is not None)
            assert(self.method is None)
            assert(self.signal is not None)
            assert(self.arg is None)
            assert(self.property is None)
            self.signal = None
        elif name == 'arg':
            assert(self.interface is not None)
            assert((self.method is not None) != (self.signal is not None))
            assert(self.arg is not None)
            assert(self.property is None)
            self.arg = None
        elif name == 'property':
            assert(self.interface is not None)
            assert(self.method is None)
            assert(self.signal is None)
            assert(self.arg is None)
            assert(self.property is not None)
            self.property = None

def main():
    arg_parser = ArgumentParser(prog='peel-dbus-gen')
    arg_parser.add_argument('interface', metavar='interface.xml')
    arg_parser.add_argument('output_header', metavar='Interface.h')
    arg_parser.add_argument('output_cpp', metavar='Interface.cpp')

    args = arg_parser.parse_args()
    parser = xml.sax.make_parser()
    handler = SaxHandler()
    parser.setContentHandler(handler)
    parser.parse(args.interface)

    for interface in handler.all_interfaces:
        print(interface.interface_name, file=sys.stderr)
        s = interface.generate_header()
        with open(args.output_header, 'w') as f:
            f.write(s)
        s = interface.generate_cpp(generated_header_name=args.output_header)
        with open(args.output_cpp, 'w') as f:
            f.write(s)
        # FIXME
        break

if __name__ == '__main__':
    main()
