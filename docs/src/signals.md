# Signals

GObject signals are a system[^similar-signals] for registering handlers for
specific events.

In peel, any type of callable can be a signal handler.
The first argument of a handler is a pointer to the object
emitting the signal. The return type and any other arguments of the handler
are defined by the signal.

> [!NOTE]
> The type of the handler's first argument is the type that defined the signal.
> For example, if the signal was defined in a base class and you connect to an
> instance of a derived class, the first argument of the handler
> is a pointer to a base class instance. This pointer can be `cast` to the
> derived class.
>
> For example, the [`clicked`] signal is defined on `Gtk::Button`. Even though
> `Gtk::LinkButton` inherits the signal, the first argument of any handler
> to `clicked` is a `Gtk::Button *`.
>
> [`clicked`]: https://docs.gtk.org/gtk4/signal.Button.clicked.html
>
> ```cpp
> $#include <peel/Gtk/Gtk.h>
> $#include <peel/signal.h>
> $
> $using namespace peel;
> $
> RefPtr<Gtk::LinkButton> link_button = Object::create<Gtk::LinkButton> ();
> link_button->set_uri ("https://bugaevc.pages.gitlab.gnome.org/peel/");
> link_button->set_label ("peel documentation");
>
> link_button->connect_clicked(
>   [] (Gtk::Button *button)
>   {
>     g_print ("The link was visited.\n");
>     button->cast<Gtk::LinkButton> ()->set_visited (true);
>   });
> ```

Handlers are connected to signals by calling
<code>.connect_<i>signal_name</i> ()</code> on the object emitting the signal.
There are two different ways to call <code>.connect_<i>signal_name</i> ()</code>:

1. Pass a GObject and member function.
2. Pass a C++ callable (like a lambda or function pointer).

Using a member function is the recommended way because your handler
will automatically be disconnected when it is finalized.

The result of <code>.connect_<i>signal_name</i> ()</code> can be assigned to a
`SignalConnection` which can be used to disconnect or block the handler.

You can define your own signals in a custom GObject. Read the page on
[defining signals](defining-signals.md)
for details.

## Connecting GObject member functions

If your signal is handled by a custom GObject class, this is the recommended
way to connect to the signal.

Define a member function on your class, and then call
<code>.connect_<i>signal_name</i> ()</code> passing in

* a pointer to the class instance,
* a [pointer to the member function] that handles the signal.

[pointer to the member function]: https://en.cppreference.com/w/cpp/language/pointer.html#Pointers_to_member_functions

This example of a window with a button and a label, shows how to use
a member function as a signal handler:

```cpp
$#include <peel/Gtk/Gtk.h>
$#include <peel/GLib/MainContext.h>
$#include <peel/class.h>
$#include <cstring>
$
$using namespace peel;
$
class MyWindow final : public Gtk::Window
{
  PEEL_SIMPLE_CLASS (MyWindow, Gtk::Window)

  RefPtr<Gtk::Button> m_button;
  RefPtr<Gtk::Label>  m_label;
  int m_times_clicked;

  void
  init (Class *);

  void
  vfunc_dispose ();

  void
  on_button_clicked (Gtk::Button *);

};

/* ... */

$PEEL_CLASS_IMPL (MyWindow, "MyWindow", Gtk::Window)
$
$void
$MyWindow::Class::init ()
${
$  override_vfunc_dispose<MyWindow> ();
$}
$
$void
$MyWindow::vfunc_dispose ()
${
$  m_button = nullptr;
$  m_label = nullptr;
$  parent_vfunc_dispose<MyWindow> ();
$}
$
void
MyWindow::on_button_clicked (Gtk::Button *)
{
  char label_text[64];
  snprintf (label_text, sizeof (label_text),
            "The button was clicked %d times", ++m_times_clicked);
  m_label->set_label (label_text);
}

void
MyWindow::init (Class *)
{
$  FloatPtr<Gtk::Box> box = Gtk::Box::create (Gtk::Orientation::VERTICAL, 6);
$
$  m_button = Gtk::Button::create_with_label ("Click me");
$  m_label = Gtk::Label::create ("The button was clicked 0 times.");
$  box->append (m_label);
$  box->append (m_button);
$
$  set_child (std::move (box));
$
  /* Allocate the button and label ... */

  m_button->connect_clicked (this, &MyWindow::on_button_clicked);
}
$
$int
$main ()
${
$  Gtk::init ();
$
$  bool should_stop = false;
$  MyWindow *window = Object::create<MyWindow> ();
$
$  window->connect_destroy (
$    [&should_stop] (Gtk::Widget *)
$    {
$      should_stop = true;
$    });
$  window->present ();
$
$  GLib::MainContext *main_context = GLib::MainContext::default_ ();
$  while (!should_stop)
$    main_context->iteration (true);
$}
```

This method only works when the class is derived from [`GObject::Object`].
Ordinary C++ classes and classes derived from other types in the GObject type
system such as [`Dex::Object`] won't work.

[`GObject::Object`]: https://docs.gtk.org/gobject/class.Object.html
[`Dex::Object`]: https://gnome.pages.gitlab.gnome.org/libdex/libdex-1/class.Object.html

Connecting using member functions is recommended for two lifetime
safety reasons:

* The signal runtime will keep the object alive while the handler is running by
  adding an additional reference before the handler is called.
* The signal handler is automatically disconnected when the
  object is destructed, preventing use-after-frees.

## Connecting C++ callables

A signal handler can be any C++ callable (any object with an
`operator ()`). This includes lambdas, plain functions, [`std::function`], etc.

[`std::function`]: https://en.cppreference.com/w/cpp/utility/functional/function.html

Although handlers in peel do not include `gpointer user_data` as their last
argument, a lambda can capture any variables you want the handler to access.
The lambda can capture a variable of any size and can capture multiple variables.

Here is the same example from previous section using a
lambda instead:

```cpp
$#include <peel/Gtk/Gtk.h>
$#include <peel/GLib/MainContext.h>
$#include <peel/class.h>
$#include <cstring>
$
$using namespace peel;
$
$int
$main ()
${
$  Gtk::init ();
$
$  FloatPtr<Gtk::Box> box = Gtk::Box::create (Gtk::Orientation::VERTICAL, 6);
$
RefPtr<Gtk::Button> button = Gtk::Button::create_with_label ("Click me");
RefPtr<Gtk::Label> label = Gtk::Label::create ("The button was clicked 0 times.");
$  box->append (label);
$  box->append (button);
$
$  bool should_stop = false;
$  Gtk::Window *window = Gtk::Window::create ();
$
$  window->set_child (std::move (box));

button->connect_clicked (
  [label, count = 0] (Gtk::Button *) mutable
  {
    char label_text[64];
    snprintf (label_text, sizeof(label_text),
              "The button was clicked %d times", ++count);
    label->set_label (label_text);
  });
$
$  window->connect_destroy (
$    [&should_stop] (Gtk::Widget *)
$    {
$      should_stop = true;
$    });
$  window->present ();
$
$  GLib::MainContext *main_context = GLib::MainContext::default_ ();
$  while (!should_stop)
$    main_context->iteration (true);
$}
```

> [!WARNING]
> Signal handlers connected this way **do not** automatically disconnect when
> a capture variable is destructed. This can cause use-after-free
> vulnerabilities if the object emitting the signal outlasts the lambda capture
> [^use-after-free-blog].
>
> To avoid the possibility of a use-after-free you can either:
>
> * Use a member function as the handler instead.
> * Manually disconnect the signal handler using a `SignalConnection`.

## SignalConnection

A `SignalConnection` is used to disconnect a signal handler
either manually or automatically when it is destructed.

Before a `SignalConnection` can be used, it must be assigned
to the result of a <code>.connect_<i>signal_name</i> ()</code> call.

```cpp
SignalConnection connection = button->connect_clicked (some_function);
```

### Disconnecting a handler

The `SignalConnection` will automatically disconnect the signal handler
when the `SignalConnection` is destructed. This makes it very convenient to
have a `SignalConnection` as a member variable of a custom GObject. When
the object is finalized, the handler is disconnected.

You can also manually disconnect the handler by calling `.disconnect ()`
on the `SignalConnection`.

> [!WARNING]
>
> If you both:
> 1. connect to a signal using a GObject and member function,
> 2. save that connection to a `SignalConnection` member variable,
>
> then, there are two mechanisms to disconnect the handler when the object
> destructs. One will disconnect the handler, and the other will try to disconnect
> an already disconnected handler. This is an error, so
> a critical warning is emitted reading:
>
> ```
> g_signal_handler_disconnect: assertion 'G_TYPE_CHECK_INSTANCE (instance)' failed
> ```
>
> To work around this issue, manually disconnect the handler in the object's
> `dispose` function.
>
> ```cpp
> class MyObject final : public Object
> {
>   PEEL_SIMPLE_CLASS (MyObject, Object)
>
>   SignalConnection m_connection;
>   RefPtr<Gtk::Button> m_button;
>
>   inline void
>   on_clicked (Gtk::Button *);
>
>   inline void
>   init (Class *);
> }
>
> inline void
> MyObject::init (Class *)
> {
>   m_button = Object::create<Gtk::Button> ();
>   m_connection = m_button->connect_clicked (this, &MyObject::on_clicked);
> }
>
> inline void
> MyObject::vfunc_dispose ()
> {
>   /* Disconnecting manually avoids a double disconnect. */
>   m_connection.disconnect ();
>   parent_vfunc_dispose<MyObject> ();
> }
> ```

### Blocking a handler

Blocking temporarily stops a handler from running. This is commonly useful
when you programmatically want to set a property on an object, but you don't
want your handler to run because of the change.

To block a handler, call `.block ()` on your `SignalConnection`.
This returns a `SignalBlockGuard`.

When the `SignalBlockGuard` destructs, or when `.unblock ()` is called on it,
the handler is unblocked.
It is recommended to block the handler in a syntatic block (inside a `{ }`)
so that the handler is automatically unblocked at the end of the block.

The `SignalBlockGuard` returned from `.block ()` must be stored in a variable.
Otherwise, it will immediately destruct and unblock.

This example shows how to change the value of a spin button without invoking
a value-changed handler:

```cpp
$#include <peel/Gtk/Gtk.h>
$#include <peel/signal.h>
$
$using namespace peel;
$
$inline void
$some_function (Gtk::SpinButton *spin_button)
${
$  g_print ("The value of the spin button is %d.\n",
$            spin_button->get_value_as_int ());
$}
$
RefPtr<Gtk::SpinButton> spin_button = /* ... */;

SignalConnection connection = spin_button->connect_value_changed (some_function);

{
  SignalBlockGuard guard = connection.block ();
  spin_button->set_value (42.0); /* some_function is not called. */
} /* The handler is unblocked when guard is destructed. */
```

## Detailed signals

Detailed signals allow handlers to selectively respond to some events but not
others. When connecting to a detailed signal, a handler can optionally be
associated with a `detail`. When the signal is emitted, a `detail`
is also emitted, and the handler will only run if the two details match.

In peel, detailed signals support passing in the `detail` as the first argument
to <code>.connect_<i>signal_name</i> ()</code>. The type of the `detail`
depends on the signal.

The most common detailed signal is [`GObject::notify`]. It is emitted when a
property changes on a GObject. The `detail` argument is a `Property<T>`
representing the property which changed.

This example shows how to track when the selected entry of a [`Gtk::DropDown`] changes:

```cpp
$#include <peel/Gtk/Gtk.h>
$#include <peel/signal.h>
$#include <peel/GObject/Object.h>
$#include <peel/GObject/ParamSpec.h>
$
$using namespace peel;
$
RefPtr<Gtk::DropDown> dropdown = /* ... */;

dropdown->connect_notify (
  Gtk::DropDown::prop_selected (), /* Only run the callback when the     */
                                   /* property named "selected" changes. */
  [] (Object *obj, peel::GObject::ParamSpec *pspec)
  {
    int selected = obj->cast<Gtk::DropDown> ()->get_selected ();
    g_print ("The index of the selected item is %d.\n", selected);
  });
```

[`GObject::notify`]: https://docs.gtk.org/gobject/signal.Object.notify.html
[`Gtk::DropDown`]: https://docs.gtk.org/gtk4/class.DropDown.html

## Dynamic signal connection

Dynamic signal connection allows you to connect to a signal by its name.

You call `.connect_signal ()` passing in the name
of the signal as a string in `kebab-case` and the handler.

```cpp
$#include <peel/Gtk/Gtk.h>
$#include <peel/GLib/MainContext.h>
$#include <peel/class.h>
$
RefPtr<Gtk::Button> button = /* ... */;

button->connect_signal ("clicked",
  [] (Gtk::Button *)
  {
    g_print ("The button was clicked.\n");
  });
```

Passing a misspelled signal name or a handler with an incorrect signature
will cause runtime errors rather than compile time errors. Unless you have a
specific reason to use `.connect_signal ()`, you should use the static
 <code>.connect_<i>signal_name</i> ()</code>functions instead.

[^similar-signals]: GObject signals are similar to [Qt signals and slots](https://doc.qt.io/qt-6/signalsandslots.html).
[^use-after-free-blog]: Michael Catanzaro's blog post titled
["Common GLib Programming Errors"](https://blogs.gnome.org/mcatanzaro/2022/07/27/common-glib-programming-errors/)
explains more about this problem.
