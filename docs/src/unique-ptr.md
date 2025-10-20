# UniquePtr

`UniquePtr<T>` represents a unique owned reference to an instance of type `T`.
When the `UniquePtr` goes out of scope, the instance will be deallocated.

`UniquePtr` is used for types that need memory management, but are not
reference-counted (for types that that are reference-counted, [`RefPtr`] is
used instead). The most common type by far it is used with is [`GLib::Error`],
where `UniquePtr` will automatically call [`g_error_free`] when the pointer
goes out of scope.

[`RefPtr`]: ref-ptr.md
[`GLib::Error`]: https://docs.gtk.org/glib/struct.Error.html
[`g_error_free`]: https://docs.gtk.org/glib/method.Error.free.html

`UniquePtr` is similar to [`std::unique_ptr`] with a custom deleter that calls
the appropriate function to properly free the referenced GObject type.

[`std::unique_ptr`]: https://en.cppreference.com/w/cpp/memory/unique_ptr

Here's a typical example of using `UniquePtr<GLib::Error>` to call a function
that can potentially fail:

```cpp
$#include <peel/Gtk/Gtk.h>
$
$using namespace peel;
$
Gtk::ColorDialog *dialog = /* ... */;
Gio::AsyncResult *res = /* ... */;

UniquePtr<GLib::Error> error;
UniquePtr<Gdk::RGBA> color = dialog->choose_rgba_finish (res, &error);

if (error)
  g_print ("Failed to pick color: %s\n", error->message);
```

Here, [`Gtk::ColorDialog::choose_rgba_finish`] both returns an instance of
[`Gdk::RGBA`] structure allocated on the heap, and on error, sets an
out-parameter describing the error.

[`Gtk::ColorDialog::choose_rgba_finish`]: https://docs.gtk.org/gtk4/method.ColorDialog.choose_rgba_finish.html
[`Gdk::RGBA`]: https://docs.gtk.org/gdk4/struct.RGBA.html

There's also an array variant of `UniquePtr`, which is described on
[its own page](unique-ptr-arr.md).
