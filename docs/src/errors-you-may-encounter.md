# Errors You May Encounter

These page lists the various error messages you could get when using peel (from
`peel-gen`, from your C++ compiler, or at runtime), and describes what they
mean and what to do about them.

* **RuntimeError: GIR file for `Something-42.0` not found**

  This happens when you try to [run `peel-gen`] to generate the bindings based
  on a GIR file, but it cannot find that GIR file. You should check that you do
  have the relevant GIR file installed (typically in `/usr/share/gir-1.0/`). If
  it's installed in an unusual location, you may have to set `GI_GIR_PATH` to
  let `peel-gen` (and other tools) know that it should look for GIR files
  there.

  In particular, on Debian 13 (trixie), they have decided to move
  `GLib-2.0.gir` (provided by the [`gir1.2-glib-2.0-dev`] package) from its
  classic location at `/usr/share/gir-1.0/` into an architecture-specific
  directory, such as `/usr/lib/x86_64-linux-gnu/gir-1.0/`. peel doesn't
  currently have any built-in logic to handle that. So if you're running Debian
  13 or a distribution derived from it, you should set `GI_GIR_PATH`
  appropriately.

[run `peel-gen`]: peel-gen.md
[`gir1.2-glib-2.0-dev`]: https://packages.debian.org/en/sid/gir1.2-glib-2.0-dev

* **undefined reference to `Something::_peel_get_type()`** or
  **undefined reference to `peel::GObject::Type::of<Something>()`**

  These methods are implemented by the `PEEL_CLASS_IMPL`, `PEEL_ENUM_IMPL`, and
  `PEEL_FLAGS_IMPL` macros. So either you are forgetting to compile and link in
  the `.cpp` file containing the use of one of these macros, or you forgot to
  use the macro in the first place.

* **no declaration matches `Something::_peel_get_type()`**

  This internal method is declared by the `PEEL_CLASS`/`PEEL_SIMPLE_CLASS`
  macros, perhaps you have forgotten to use them.

* **specialization of `template<class T> static peel::GObject::Type
  peel::GObject::Type::of()` in different namespace**

  C++ [disallows] specializing a template outside of its namespace, which means
  that `PEEL_ENUM_IMPL` and `PEEL_FLAGS_IMPL` macros cannot be used inside of
  your namespace. You really need to use them at the root scope. Note that
  `PEEL_CLASS_IMPL` is implemented differently, and doesn't have this
  limitation.

[disallows]: https://quuxplusone.github.io/blog/2024/07/15/specialization-in-another-namespace/

* **reference to `GObject` is ambiguous**

  This happens when you put [`using namespace peel`] before including some C
  header.

[`using namespace peel`]: gotchas-and-pitfalls.md#using-namespace-peel

* **`g_object_something`: object class `Something` has no property named
  `some-property`**

  This happens when you _declare_ that your class has a property using
  `PEEL_PROPERTY`, but forget to _define_ it using `define_properties`.

* **`SOME_VARIANT` is not a member of `peel::Some::Enum`**

  This happens when the compiler only sees a forward declaration of the enum,
  and not the full one. Make sure you're including the relevant
  `<peel/Some/Enum.h>` header.

* **use of deleted function `peel::FloatPtr<T>::FloatPtr(const peel::FloatPtr<U>&)`**

  [`FloatPtr`] intentionally has a deleted copy constructor. Most likely, you
  should move the floating reference instead, using `std::move ()`.

[`FloatPtr`]: float-ptr.md

* **use of deleted function `T* peel::RefPtr<T>::operator->() &&`** or
  **use of deleted function `peel::RefPtr<T>::operator T*() &&`**

  These happen when you try to use a temporary `RefPtr` (e.g. when a `RefPtr`
  is returned from some method, perhaps a constructor of some type) as a plain
  pointer. Unfortunately, this is not supported.

  There is a simple workaround: just store the `RefPtr` into a variable, so
  it's no longer temporary. If you're trying to call `->cast ()`, call
  `.cast ()` instead, so you cast the `RefPtr` itself, and not the plain
  pointer it decays to.

* **no matching function for call to `peel::internals::PspecTraits<Something>::PspecTraits`**

  This happens when you pass a wrong set of arguments to the `.prop ()` call
  inside your `define_properties`. The correct set of arguments to pass depends
  on the type of the property you're declaring; it commonly is either:
  * Nothing (such as for object and boxed record properties),
  * Default value (such as for string and boolean properties),
  * Minimum, maximum, and default values (such as for numeric properties).

* **Invalid object type `Something`**

  You may see this at runtime when trying to instantiate an object using
  [`GtkBuilder`]. This may mean one of two things:

  * You spelled the _GObject type name_ of the class differently in the `.ui`
  (or [Blueprint]) file and in the `PEEL_CLASS_IMPL` macro. See the page on
  [Naming Conventions].

  * You forgot to ensure the type is at all registered with GObject type
  system.

    _Before_ you instantiate your interface from `.ui` or Blueprint (so, either
    before calling `Gtk::Builder` APIs, or [`Gtk::Widget::init_template`] if
    using [widget templates]), call `GObject::Type::ensure ()` for the relevant
    types that the template uses. For example, if `Demo::Window`'s template
    refers to a type named `Demo::Page` (which would likely be spelled as
    `DemoPage` inside the template itself), its initializer might look like
    this:

    ```cpp
    inline void
    Demo::Window::init (Class *)
    {
      Type::of<Demo::Page> ().ensure ();
      init_template ();
    }
    ```

[`GtkBuilder`]: https://docs.gtk.org/gtk4/class.Builder.html
[Blueprint]: https://gnome.pages.gitlab.gnome.org/blueprint-compiler/
[Naming Conventions]: naming-conventions.md
[`Gtk::Widget::init_template`]: https://docs.gtk.org/gtk4/method.Widget.init_template.html
[widget templates]: gtk-widget-templates.md
