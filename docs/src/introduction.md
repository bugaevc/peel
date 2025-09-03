# Introduction

peel is the project providing modern C++ bindings[^modern] for GObject-based
libraries, including [GTK] and [GStreamer].

[GTK]: https://www.gtk.org/
[GStreamer]: https://gstreamer.freedesktop.org/

peel is designed to:

* Integrate deeply with the **GObject type system**, supporting and faithfully
  exposing GObject features such as per-type instance and class initializers,
  properties, dynamic instantiation, and non-Object-derived types.

  This notably enables modern GTK patterns such as [composite templates],
  [list models], and [expressions].

* Have **no runtime overhead** compared to the equivalent code in plain C.
  peel follows the [zero-overhead principle], which states:

  > What you don’t use, you don’t pay for. And further: What you do use, you
  > couldn’t hand code any better.

  peel's C++ wrappers are completely optimized away by the compiler, and the
  produced machine code is equivalent[^better-than-c] to what you'd get if you
  implemented the same logic in plain C.

* Be **easy to set up** and use on a wide range of systems, including Windows.
  peel only requires C++11 (though there is some support for C++20 coroutines).
  peel's [bindings generator] is written in Python with no external
  dependencies. There is some integration with [CMake] and [Meson] build
  systems, and peel can be easily pulled into your project as a Meson
  subproject.

* Have very **complete and up-to-date API coverage**.

  The majority of the wrapper code is generated automatically based on
  [GObject introspection] data (GIR), and peel has rather good support for
  various GIR features, including some less used ones. This means that it's
  easy to get peel working with pretty much any GObject-based
  library[^internal-library], and that new features (like new [libadwaita]
  widgets) can be used with peel immediately when they appear in the upstream
  library---without having to wait for updated bindings to be uploaded to some
  sort of a central binding repository.

[composite templates]: https://developer.gnome.org/documentation/tutorials/widget-templates.html
[list models]: https://blogs.gnome.org/gtk/2020/09/08/on-list-models/
[expressions]: https://docs.gtk.org/gtk4/class.Expression.html
[zero-overhead principle]: https://en.cppreference.com/w/cpp/language/Zero-overhead_principle.html
[bindings generator]: peel-gen.md
[CMake]: using-peel-with-cmake.html
[Meson]: using-peel-with-meson.html
[GObject introspection]: https://gi.readthedocs.io/
[libadwaita]: https://gnome.pages.gitlab.gnome.org/libadwaita/

[^modern]: "modern" here relates to "GObject bindings", not "C++"
[^better-than-c]: or at times better, when we're able to take advantage of C++
features such as the strong type system to do things more efficiently than
plain C
[^internal-library]: if a part of your project is implemented as an internal
library, you should be able to use that with peel, too
