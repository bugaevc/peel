# GTK Widget Templates

A common way to implement composite GTK widgets is by using [widget templates].

To start using widget templates, you should:

1. Write the template in a `.ui` file (perhaps using [Blueprint]),
2. Call [`Gtk::Widget::Class::set_template_from_resource`] in your widget's
   [class initializer],
3. Call [`Gtk::Widget::init_template`] in your widget's `init`,
4. Call [`Gtk::Widget::dispose_template`] with your widget's type in your
   widget's `vfunc_dispose`.

Here's how it might look:

```cpp
inline void
MyWidget::Class::init ()
{
  override_vfunc_dispose<MyWidget> ();

  set_template_from_resource ("/org/example/my-widget.ui");
}

inline void
MyWidget::init (Class *)
{
  init_template ();
}

inline void
MyWidget::vfunc_dispose ()
{
  dispose_template (Type::of<MyWidget> ());
  parent_vfunc_dispose<MyWidget> ();
}
```

Referencing objects defined in the template, as well as providing callbacks
referenced in the template _in plain C_ is done by using the
[`gtk_widget_class_bind_template_child`] and
[`gtk_widget_class_bind_template_callback`] macros. peel doesn't provide direct
wrappers for these macros (so it's not possible to just write e.g.
`bind_template_child ()` inside your `Class::init`). Instead, peel provides its
own analogs to them in the form of `PEEL_WIDGET_TEMPLATE_BIND_CHILD` and
`PEEL_WIDGET_TEMPLATE_BIND_CALLBACK` macros, defined in
`<peel/widget-template.h>`:

```cpp
#include <peel/widget-template.h>

class MyWidget : public Gtk::Widget
{
  /* ... */

  /* template child */
  Gtk::Button *button;

  /* template callback */
  void
  button_clicked_cb (Gtk::Button *);
};

inline void
MyWidget::Class::init ()
{
  override_vfunc_dispose<MyWidget> ();

  set_template_from_resource ("/org/example/my-widget.ui");

  PEEL_WIDGET_TEMPLATE_BIND_CHILD (MyWidget, button);
  PEEL_WIDGET_TEMPLATE_BIND_CALLBACK (MyWidget, button_clicked_cb);
}
```

The first macro argument must be the widget class (commonly just a name, but it
could be namespaced, or referenced through a typedef). The second argument must
be a name of an instance data member (for `BIND_CHILD`) or a method (for
`BIND_CALLBACK`) with a matching type.

When using the two-argument versions of the macros, the name of the C++ member
must match the ID/name used in the template. There are also three-argument
versions that let you pass a name explicitly. This can be useful when your C++
code follows one naming convention (such as prefixing all data member names
with `m_`) and your `.ui` files follow a different one:

```cpp
#include <peel/widget-template.h>

class MyWidget : public Gtk::Widget
{
  /* ... */

  /* template children */
  Gtk::Button *m_button;
  Gtk::GestureClick *m_gesture_click;
};

inline void
MyWidget::Class::init ()
{
  override_vfunc_dispose<MyWidget> ();

  set_template_from_resource ("/org/example/my-widget.ui");

  PEEL_WIDGET_TEMPLATE_BIND_CHILD (MyWidget, m_button, "button");
  PEEL_WIDGET_TEMPLATE_BIND_CHILD (MyWidget, m_gesture_click, "gesture-click");
}
```

Note that binding a template child only works when the data member is a
**plain pointer**, not a `RefPtr` or another smart pointer type! This is fine,
however, since each widget already keeps internal references to all child
objects bound this way.

[widget templates]: https://developer.gnome.org/documentation/tutorials/widget-templates.html
[Blueprint]: https://jwestman.pages.gitlab.gnome.org/blueprint-compiler/
[`Gtk::Widget::Class::set_template_from_resource`]: https://docs.gtk.org/gtk4/class_method.Widget.set_template_from_resource.html
[class initializer]: initialization.md#class-initializer
[`Gtk::Widget::init_template`]: https://docs.gtk.org/gtk4/method.Widget.init_template.html
[`Gtk::Widget::dispose_template`]: https://docs.gtk.org/gtk4/method.Widget.dispose_template.html
[`gtk_widget_class_bind_template_child`]: https://docs.gtk.org/gtk4/func.widget_class_bind_template_child.html
[`gtk_widget_class_bind_template_callback`]: https://docs.gtk.org/gtk4/func.widget_class_bind_template_callback.html
