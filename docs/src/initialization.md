# Initialization

Unlike regular C++ classes, GObject classes **do not have C++ constructors**.
Instead, they have GObject intializers.

Generally, GObject construction/initialization flow is somewhat complex, and
explaining it is outside of the scope of this documentation. Please see
["A gentle introduction to GObject construction"] for one explanation of this
process, and ["Object construction"] section of the GObject Tutorial for
another one. This page documents the parts that are specific to using C++ and
peel.

["A gentle introduction to GObject construction"]: https://blogs.gnome.org/desrt/2012/02/26/a-gentle-introduction-to-gobject-construction/
["Object construction"]: https://docs.gtk.org/gobject/tutorial.html#object-construction

## Instance initializer

When an instance of a GObject class is created, memory for the instance is
allocated and zero-initialized. Then, the instance initializer is invoked.
The instance inializer has the signature of `void MyType::init (Class *)` (you
can ignore the class pointer most of the time). Note that unlike a C++
constructor, an instance initializer **cannot accept any parameters** (other
than the class pointer); you should use [properties] for that use case instead.

Things you might want to do in an instance initializer of a GTK widget include:
* Explicitly initializing any fields that should not be zero-initialized;
* Setting up any child widgets;
* Setting up event controllers.

A typical implementation of an instance initializer for a widget (in this case,
a window) looks like this:

```cpp
~#include <peel/Gtk/Gtk.h>
~
~using namespace peel;
~
~class MyWindow final : public Gtk::Window
~{
~  PEEL_SIMPLE_CLASS (MyWindow, Gtk::Widget)
~
~  inline void
~  init (Class *);
~
~  int last_index;
~};
~
~PEEL_CLASS_IMPL (MyWindow, "MyWindow", Gtk::Window)
~
inline void
MyWindow::init (Class *)
{
  /* fields that need initialization other than to 0 */
  last_index = -1;

  /* child widget */
  FloatPtr<Gtk::Button> button = Gtk::Button::create_with_label ("Click me");
  button->connect_clicked (this, &MyWindow::on_button_clicked);
  set_child (std::move (button));

  /* misc set-up */
  set_default_size (500, 300);
  action_set_enabled ("win.undo-close-tab", false);
}
```

## Class initializer

The first time an instance of a class is created (or the class otherwise
referenced), the initializer for the class itself is invoked. The class
initializer has signature of `void MyType::Class::init ()`, and is declared
automatically by the `PEEL_SIMPLE_CLASS` macro.

In the class initializer, you set up the _class structure_ (the
`MyType::Class`) that is shared among all instances of the class and stores
per-class data. Specifically:
* Override any relevant [virtual functions];
* Set up any [signals];
* Set up your base class, typically using its class methods.

  For instance, in a GTK widget class you might want to use [`set_css_name`]
  to set the CSS name for instances of the class, [`set_layout_manager_type`]
  to set the type the layout manager to be used with instances of the class,
  and install some actions using [`install_action`].
* Initialize your own [per-class data], if any.

[virtual functions]: vfuncs.md
[signals]: signals.md
[`set_css_name`]: https://docs.gtk.org/gtk4/class_method.Widget.set_css_name.html
[`set_layout_manager_type`]: https://docs.gtk.org/gtk4/class_method.Widget.set_layout_manager_type.html
[`install_action`]: https://docs.gtk.org/gtk4/class_method.Widget.install_action.html
[per-class data]: per-class-data.md

A typical implementation of a class initializer for a widget looks like this:

```cpp
~#include <peel/Gtk/Gtk.h>
~
~using namespace peel;
~
~namespace Demo
~{
~
~class Gizmo final : public Gtk::Widget
~{
~  PEEL_SIMPLE_CLASS (Gizmo, Gtk::Widget)
~  friend class Gtk::Widget::Class;
~
~  inline void
~  vfunc_dispose ();
~
~  inline void
~  vfunc_snapshot (Gtk::Snapshot *);
~};
~
~PEEL_CLASS_IMPL (Gizmo, "DemoGizmo", Gtk::Widget)
~
inline void
Gizmo::Class::init ()
{
  /* vfunc overrides */
  override_vfunc_dispose<Gizmo> ();
  override_vfunc_snapshot<Gizmo> ();

  /* widget class setup */
  set_css_name ("gizmo");
  set_layout_manager_type (Type::of<Gtk::BinLayout> ());
  install_action ("gizmo.frob", nullptr, /* ... */);
}
~
~} /* namespace Demo */
```

Note that the C++ `this` pointer inside the class initializer refers to an
instance of the class structure, which inherits from the base type's class
structure (here, `Gizmo::Class` inherits from `Gtk::Widget::Class`). This is
what enables usage of the concise syntax for invoking class methods, such as
`Gtk::Widget::Class::set_css_name`.

## Constructors

_Constructors_  (or _constructor methods_) in the GObject world are simply
thin convenience wrappers over creating an instance of a type using
`Object::create` (for `Object`-derived classes), while passing the values of
the most relevant [properties]. Conventionally, constructors are named
`create`, <code>create_<i>somehow</i></code>, or
<code>create_with_<i>something</i></code>.

[properties]: class-properties.md

For example, a constructor for a label widget might look like this:

```cpp
~#include <peel/Gtk/Gtk.h>
~
~using namespace peel;
~
~namespace Demo
~{
~
class Label final : public Gtk::Widget
{
  PEEL_SIMPLE_CLASS (Label, Gtk::Widget)

  /* ... */

public:
  PEEL_PROPERTY (String, text, "text")

  static FloatPtr<Label>
  create (const char *text);
};

FloatPtr<Label>
Label::create (const char *text)
{
  return Object::create<Label> (prop_text (), text);
}
~
~} /* namespace Demo */
```

Most importantly, please note that such a constructor is _not_ a single,
canonical way to create an instance of the type. `Object::create` is such a
way, and the constructor methods are merely convenient wrappers over calling
`Object::create`. Indeed, in many cases, such as when constructing an instance
using `Gtk::Builder`, the constructor will not be called. For this reason, you
should avoid putting any logic into constructors.
