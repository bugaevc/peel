# String

A `String` represents an owned UTF-8 string, allocated using GLib heap API.
When a `String` goes out of scope, the owned string on the heap is deallocated.
A `String` is nullable and therefore might not reference any string.
The underlying `const char *` can be accessed by casting the string to
`const char *`, or calling the `.c_str ()` method.

A `String` can be directly created from a `const char *`, a `nullptr`, or
another `String`. They are also returned from functions which return an owned
string, like [`GLib::DateTime::format`] and [`Gtk::Editable::get_chars`].

[`GLib::DateTime::format`]: https://docs.gtk.org/glib/method.DateTime.format.html
[`Gtk::Editable::get_chars`]: https://docs.gtk.org/gtk4/method.Editable.get_chars.html

Most functions that take a string as a parameter accept unowned strings, which
in peel are represented simply with `const char *` (same as in C). You can
pass a string literal or a `String`, among other things. For example, a
[`Gtk::Label`]'s label can be set using a `const char *`, a `String`, or even
an [`std::string`]:

[`Gtk::Label`]: https://docs.gtk.org/gtk4/class.Label.html
[`std::string`]: https://en.cppreference.com/w/cpp/string/basic_string.html

```cpp
$#include <string>
$#include <peel/String.h>
$#include <peel/Gtk/Label.h>
$
$using namespace peel;
$
RefPtr<Gtk::Label> label = /* ... */;

label->set_label ("string literal");

String peel_string = "peel::String";
label->set_label (peel_string); // A String will implicitly convert to const char *.

std::string std_string = "std::string";
label->set_label (std_string.c_str ());
```

## `peel::String` compared to `std::string`

`String` is not meant to be the canonical ultimate string type. Instead, it
should be thought of as a smart pointer to an owned `char[]` string on the
heap, comparable to [`std::unique_ptr<char[]>`](https://en.cppreference.com/w/cpp/memory/unique_ptr.html).

For this reason, `String` only provides a rather minimal API compared to
`std::string`. A lot more of string utilities are available in GLib, and work
on both owned and unowned strings. Examples of such utilities are
[`GLib::strrstr`], [`GLib::str_has_suffix`], [`GLib::strjoin`].

[`GLib::strrstr`]: https://docs.gtk.org/glib/func.strrstr.html
[`GLib::str_has_suffix`]: https://docs.gtk.org/glib/func.str_has_suffix.html
[`GLib::strjoin`]: https://docs.gtk.org/glib/func.strjoin.html

Here are a few differences between `String` and `std::string`:

* `String` is nullable, `std::string` is not;
* A default constructed `String` is null, but a default constructed
  `std::string` is empty;
* `String` does not track its length and capacity unlike `std::string`;
* `String` uses `g_malloc`/`g_free` for allocation/deallocation
   whereas `std::string` uses `operator new`/`operator delete`;
* `String` is in UTF-8 but `std::string` might not be.

While neither peel nor STL depend on each other, so there's no direct
conversion between the two string types, you can easily convert between them by
going through `const char *`:

```cpp
$#include <string>
$#include <peel/String.h>
$
$using namespace peel;
$
String peel_string_1 = "peel::String";
std::string std_string = peel_string_1.c_str ();
String peel_string_2 = std_string.c_str ();
```
