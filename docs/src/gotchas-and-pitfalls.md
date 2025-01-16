# Gotchas and Pitfalls

## `gettext` return type

For internationalization, it is common to use [gettext with glib]. Here's a
simple example of using `gettext` to make the button's label translatable:

```cpp
auto button = Gtk::Button::create_with_label (_("Save Changes"));
```

This works great. Where things may go wrong is when you try to use this in
combination with properties (or values):

```cpp
auto button = Object::create<Gtk::Button> (
  Gtk::Button::prop_label (), _("Save Changes"),  /* <- broken! */
  Gtk::Widget::prop_halign (), Gtk::Align::CENTER,
  Gtk::Actionable::prop_action_name (), "win.save");
```

This is due to combination of:
* `gettext ()` [being declared][gettext-declaration] as returning `char *`, not
  `const char *`, for some reason,
* peel's `Value` machinery interpreting `char *` as an _owned_ string, which
  the value takes ownership of (by using [`g_value_take_string`]), and will
  eventually attempt to free with [`g_free`].

To avoid this, explicitly cast the string returned from `gettext ()` to
`const char *` when it's used in a property or value context:

```cpp
button->set_property (
  Gtk::Button::prop_label (),
  (const char *) _("Save Changes"));
```

[gettext with glib]: https://docs.gtk.org/glib/i18n.html
[gettext-declaration]: https://www.gnu.org/software/gettext/manual/html_node/Interface-to-gettext.html
[`g_value_take_string`]: https://docs.gtk.org/gobject/method.Value.take_string.html
[`g_free`]: https://docs.gtk.org/glib/func.free.html

## Capturing `WeakPtr`

blah blah

## Complex fields in GObject classes

GObject classes don't have a C++ constructor, instead, they have the GObject
initializer, spelled `MyType::init (Class *)` in peel. Before the initializer
is run, the body of the class is initialized to zero bytes, as if with
`memset`. This is enough to initialize many types to their default values (and
in fact similiar to how global variables located in `.bss` are initialized). In
particular:
* integers get set to 0,
* booleans get set to `false`,
* floating-point numbers get set to 0.0,
* raw pointers get set to `nullptr`,
* as an additional guarantee, peel smart pointers (such as [`RefPtr`],
  [`WeakPtr`], [`UniquePtr`]) get set to `nullptr`.

However, for more complex types (for example, [`std::string`]), the C++
language requires the actual constructor to be invoked. This happens implicitly
when you _do_ use a C++ constructor in your own class:

```cpp
class MyClass
{
public:
  MyClass ();
  int m_int;
  std::string m_str;
};

MyClass::MyClass ()
 : m_int (42)
 /* m_str implicitly constructed here */
{
  std::cout << "My C++ constructor" << std::endl;
}
```

In peel, as your class doesn't have a C++ constructor, you have to instead
explicitly use [placement new] in your initializer to construct the values of
complex types:

```cpp
class MyWidget : public Gtk::Widget
{
  /* ... */

  int m_int;
  std::string m_str;

  inline void
  init (Class *);
};

inline void
MyWidget::init (Class *)
{
  m_int = 42;
  new (&m_str) std::string;
}
```

On the other hand, peel classes _do_ have a C++ destructor, which is mapped to
the GObject [`finalize` vfunc]. For this reason, destructing the object will
properly destruct all of its fields, invoking their C++ destructors. You don't
have to do anything explicitly to make it happen.

[`RefPtr`]: ref-ptr.md
[`WeakPtr`]: weak-ptr.md
[`UniquePtr`]: unique-ptr.md
[`std::string`]: https://en.cppreference.com/w/cpp/string/basic_string
[placement new]: https://en.cppreference.com/w/cpp/language/new#Placement_new
[`finalize` vfunc]: https://docs.gtk.org/gobject/vfunc.Object.finalize.html
