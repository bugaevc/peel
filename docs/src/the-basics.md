# The Basics

Most GObject APIs are mapped straightforwardly into C++. GObject classes are
mapped to C++ classes, and their methods are mapped to C++ methods.

The following is an example of invoking the [`Gtk::Button::set_label`] and
[`Gtk::Widget::set_valign`] methods on a button:

```cpp
$#include <peel/Gtk/Gtk.h>
$
$using namespace peel;
$
Gtk::Button *button = /* ... */;
button->set_label ("My Button");
button->set_valign (Gtk::Align::CENTER);
```

[`Gtk::Button::set_label`]: https://docs.gtk.org/gtk4/method.Button.set_label.html
[`Gtk::Widget::set_valign`]: https://docs.gtk.org/gtk4/method.Widget.set_valign.html

GObject constructors are mapped to C++ static factory methods, with the "`new`"
part of a constructor name replaced with "`create`" (because a bare `new` is a
C++ keyword):

```cpp
$#include <peel/Gtk/Gtk.h>
$
$using namespace peel;
$
auto window = Gtk::Window::create ();
window->set_title ("My Window");

auto button = Gtk::Button::create_with_label ("My Button");
```

To connect to a GObject [signal], use the corresponding `connect_*` method,
which you can pass a C++ lambda to:

```cpp
$#include <peel/Gtk/Gtk.h>
$
$using namespace peel;
$
$auto button = Gtk::Button::create_with_label ("Click me");
int times_clicked = 0;
/* Connect a handler to the "clicked" signal of the button */
button->connect_clicked ([&times_clicked] (Gtk::Button *button)
  {
    g_print ("You clicked the button %d times!\n", ++times_clicked);
  });
```

[signal]: https://docs.gtk.org/gobject/signals.html

The following is a complete working example of a tiny application:

```cpp
#include <peel/Gtk/Gtk.h>
#include <peel/GLib/GLib.h>

using namespace peel;

int
main ()
{
  Gtk::init ();

  auto window = Gtk::Window::create ();
  window->set_title ("Example");

  auto button = Gtk::Button::create_with_label ("Click me");
  int times_clicked = 0;
  button->connect_clicked ([&times_clicked] (Gtk::Button *button)
    {
      g_print ("You clicked the button %d times!\n", ++times_clicked);
    });

  window->set_child (std::move (button));
  window->present ();

  auto main_loop = GLib::MainLoop::create (nullptr, false);
  main_loop->run ();

  return 0;
}
```
