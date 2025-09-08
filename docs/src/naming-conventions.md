# Naming Conventions

Your class has two names: the C++ name, and the GObject type name. The GObject
type name is the one you'd see in various debug output, in the GTK inspector,
and use in [`Gtk.Builder`] files (including [Blueprints]).

[`Gtk.Builder`]: https://docs.gtk.org/gtk4/class.Builder.html
[Blueprints]: https://gnome.pages.gitlab.gnome.org/blueprint-compiler/

In order for types defined by various libraries not to clash, the well
established convention is for each project to pick a namespace, and use it as
a type name prefix (among other things). Conventionally, this prefix is short,
and, should it consist of multiple words, in `UpperCamelCase`.

Examples of prefixes used by various projects are:
* [GTK] uses `Gtk`
* [libadwaita] uses `Adw`
* [Pango] uses `Pango`
* [GStreamer] uses `Gst`
* [WebKitGTK] uses `WebKit`
* [libdex] uses `Dex`
* [libsoup] uses `Soup`
* [libportal] uses `Xdp` (for "[XDG desktop portal]")
* [libdazzle] uses `Dzl`

[GTK]: https://gtk.org/
[libadwaita]: https://gnome.pages.gitlab.gnome.org/libadwaita/
[Pango]: https://www.gtk.org/docs/architecture/pango
[GStreamer]: https://gstreamer.freedesktop.org/
[WebKitGTK]: https://webkitgtk.org/
[libdex]: https://gitlab.gnome.org/GNOME/libdex
[libsoup]: https://libsoup.gnome.org/
[libportal]: https://libportal.org/
[XDG desktop portal]: https://flatpak.github.io/xdg-desktop-portal/
[libdazzle]: https://gitlab.gnome.org/Archive/libdazzle

Then, peel's convention is to map these namespaces to C++ namespaces; for
instance, the GObject type name of `GtkButton` corresponds to
`peel::Gtk::Button` in peel.

We recommend that you follow these same conventions. Pick a short namespace
for your project, and use it both as a prefix for GObject type names, and as a
C++ namespace (though _not_ under `peel::`). If you pick `Foo` for a namespace
and have a class named `Frobnicator`, its GObject type name should be
`FooFrobnicator`, a the C++ name should be `Foo::Frobnicator`.

In this documentation, the namespace used for examples is `Demo`.
