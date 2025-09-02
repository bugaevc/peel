# Running peel-gen

> TL;DR: `peel-gen --recursive Gtk 4.0`

Most of the C++ code which makes up the peel bindings is not manually written,
but gets automatically generated from [GIR], the machine-readable XML-based
API description format.

[GIR]: https://gi.readthedocs.io/

The generated bindings are not shipped along with peel. Rather, you should
generate bindings yourself. You could generate the bindings once, and then
check them into a source control repository. However, we recommend you to
instead generate the bindings right at your project's build time, as a part of
your project's build system if you use one. This way, you get bindings
tailored for your specfic environment you're building in, such as the specific
version of GTK that you have, and the system-dependent definitions such as
`GPid` and `goffset`.

To generate the bindings, you should, first of all, obtain the relevant GIR
files for the libraries you're going to use; for example, the file describing
GTK 4 is named `Gtk-4.0.gir`. Typically, on a Linux distribution, these files
are part of the relevant development packages, along with the C headers. For
instance, the GIR file for GTK is contained in the `libgtk-4-dev` package on
Debian, and in the `gtk4-devel` package on Fedora. Typically, system-wide GIR
files are installed into `/usr/share/gir-1.0/`.

Secondly, you need the `peel-gen` executable, which is the peel bindings
generator. It is implemented in [Python 3] with no external dependencies (same
as [Meson]). It can be run directly from the peel source repository
([`peel-gen.py`], this is known as running `peel-gen` "uninstalled"), or as
`peel-gen` command if peel is installed.

[Python 3]: https://www.python.org/
[Meson]: https://mesonbuild.com/
[`peel-gen.py`]: https://gitlab.gnome.org/bugaevc/peel/-/blob/tmp/peel-gen.py?ref_type=heads
