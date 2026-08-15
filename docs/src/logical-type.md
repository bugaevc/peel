# Logical type

The concept of _logical type_ shows up in various places throughout peel. It is
the way peel models `GType`-related concepts in the C++ type system. In
particular, logical types show up when working with [properties] and [values].

[properties]: properties.md
[values]: value.md

Logical type is usually just a "plain" version of the corresponding C++ type:
* For basic types like `int`, `unsigned int`, and `bool`, the logical type is
  same as the C++ type.
* For object types (as well as similar types like boxed records), the logical
  type is just the class itself, stripped of any pointers and memory management
  concerns.
* For strings, whether owned or unowned, [`String`] is the logical
  type[^str-type]. For string arrays, whether owned or unowned `Strv`, is the
  logical type.

[`String`]: string.md

Here are a few examples:

| Logical type    | C++ type(s)              | Meaning        | GType            |
| --------------- | -------------------------| -------------- | ---------------- |
| `int`           | `int`                    | integer        | `G_TYPE_INT`     |
| `bool`          | `bool`                   | boolean        | `G_TYPE_BOOLEAN` |
| `void`          | (none)                   | no value       | `G_TYPE_NONE`    |
| `void *`        | `void *`                 | opaque pointer | `G_TYPE_POINTER` |
| `String`        | `String` and `const char *` | string      | `G_TYPE_STRING`  |
| `Object`        | `Object *` and `RefPtr<Object>` | object  | `G_TYPE_OBJECT`  |
| `Gtk::Widget`   | `Gtk::Widget *` and `RefPtr<Gtk::Widget>` | GTK widget | `GTK_TYPE_WIDGET` |
| `Strv`          | `Strv`, `StrvRef`        | array of strings | `G_TYPE_STRV`  |
| [`GLib::Error`] | `UniquePtr<GLib::Error>` | error          | `G_TYPE_ERROR`   |

[`GLib::Error`]: errors.md

A logical type can correspond to one or several C++ types (typically differing
in ownership).

[^str-type]: For historical reasons, it's also possible to use `const char *`
  as a logical type to mean a string.
