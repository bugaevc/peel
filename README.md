# peel

peel provides modern C++ bindings for GObject-based libraries, including the [GTK toolkit](https://gtk.org/).
It notably makes some different design choices from the existing bindings.

**Design goals** include:
* zero-cost, no runtime overhead: the C++ wrappers are completely optimized away,
  the generated machine code is equivalent (or at times better) to what you'd get
  if you were writing your code in plain C;
* easy to set up, including on Windows;
* only C++11 is required, but some coroutine integration is available if you have C++20;
* full, seamless, first-class support for implementing custom GObject-derived
  classes, including with deep inheritance;
* faithfully expose GObject concepts (such as initializers and properties);
* most of the wrapper code is automatically generated from GObject introspection
  data, meaning the API coverage is mostly complete and up-to-date.

peel is a header-only library providing wrappers that are intended to be completely
inlined into your code. The peel project consists of two parts: manually written bindings
to the few fundamental types (such as `GObject::Object`), and the code generator, a
Python script with no external dependencies, that generates bindings to the majority
of the APIs.

## Code example

Here's a very simple GTK application:

```c++
#include <peel/Gtk/Gtk.h>
#include <peel/Gio/ApplicationFlags.h>

using namespace peel;

static void
build_ui (Gio::Application *app)
{
  auto window = Gtk::ApplicationWindow::create (app->cast<Gtk::Application> ());
  auto child = Gtk::Label::create ("Hello world!");
  window->set_child (std::move (child));
  window->present ();
}

int
main (int argc, const char **argv)
{
  auto app = Gtk::Application::create ("org.example.App", Gio::Application::Flags::DEFAULT_FLAGS);
  app->connect_activate (build_ui);
  app->run (argc, argv);
}
```

## Usage

You need to run the bindings generator, `peel-gen`, and add its output directory
to your include path. When using peel uninstalled, you also need to add the `include/` subdirectory from this repository to your include path.
bindings.

Use the following template to consume peel in Meson project:

```meson
project('example', 'cpp')

gtk = dependency('gtk4')
peel = dependency('peel')

peel_gen = find_program('peel-gen')
peel_gtk = custom_target('peel-codegen',
  command: [
    peel_gen,
    '--recursive',
    '--out-dir', '@OUTDIR@',
    'Gtk', '4.0',
  ],
  output: 'peel',
)

# Add the peel-generated headers as sources to your binary target,
# and specify peel itself, as well as the orignal libraries
# as dependencies.
executable('example',
  'example.cpp',
  peel_gtk,
  dependencies: [gtk, peel],
)
```

This should work both with a system-wide installation of peel and with peel
pulled as a Meson subproject.

You can either use peel uninstalled, which is most useful for consuming it as a Meson
subproject, or install it system-wide.

See [examples](examples/).

## Getting in Touch

Chat on Matrix: [#peel:matrix.org](https://matrix.to/#/#peel:matrix.org)

## Documentation

The documentation is hosted in [GitLab pages](https://bugaevc.pages.gitlab.gnome.org/peel/)
and generated from the [docs](docs/) directory in this repo using
[mbBook](https://rust-lang.github.io/mdBook/).

The documentation is currently very incomplete, so contributions are welcome!

## Copying

peel is licensed under the permissive MIT license. Note however that the libraries
you use through peel have their own licenses, which you have to follow.
