# RefPtr

Among peel smart pointer types, `RefPtr<T>` is the one that you'll encounter
early on and most often.

A `RefPtr<T>` owns a single reference to an instance of a reference-counted
type (most commonly a subclass of `GObject::Object`). A `RefPtr`, while it
exists, will keep the object alive, and once all `RefPtr`s that refer to an
object (and any other references to the same object) go out of scope, the
object will be destroyed.

A `RefPtr` can be made from a `nullptr`, or a plain pointer to an object:

```cpp
~#include <peel/Gtk/Gtk.h>
~
~using namespace peel;
~
Gtk::Button *my_button = /* ... */;
RefPtr<Gtk::Widget> widget = nullptr;
/* Keep an additional reference on my_button */
widget = my_button;
```

A `RefPtr` can be dereferenced with `->` just like a plain pointer:

```cpp
~#include <peel/Gtk/Gtk.h>
~
~using namespace peel;
~
RefPtr<Gtk::Button> button = /* ... */;
button->set_label ("My Button");
```

## RefPtr compared to std::shared_ptr

`RefPtr` is in many ways similar to [`std::shared_ptr`]. There is however an
important difference:
* `std::shared_ptr` adds *its own layer of reference counting* around a type
  that itself doesn't implement reference-counting (so it is possible to use
  `std::shared_ptr<int>` for example);
* `RefPtr` expects the object type to implement *its own reference counting
  mechanism* (for `GObject::Object` subclasses, these are [`g_object_ref`]
  and [`g_object_unref`]), and wraps that mechanism into a C++ smart pointer
  type.

[`std::shared_ptr`]: https://en.cppreference.com/w/cpp/memory/shared_ptr
[`g_object_ref`]: https://docs.gtk.org/gobject/method.Object.ref.html
[`g_object_unref`]: https://docs.gtk.org/gobject/method.Object.unref.html

This also enables `RefPtr` to be constructed directly from a plain pointer,
without needing something like [`std::enable_shared_from_this`].

[`std::enable_shared_from_this`]: https://en.cppreference.com/w/cpp/memory/enable_shared_from_this

## Using RefPtr

The `RefPtr` class has reasonable implementations of various members,
including copy and move constructors and assignment operators, that manage the
reference count of the referenced object as expected. This means that various
operations involving plain pointers and `RefPtr`s (and other peel pointer
types like `FloatPtr` and `WeakPtr`) "just work" and manage the reference
count of the object automatically and correctly.

For example, assume there are two functions, one of which accepts an object by
a plain pointer, and another by `RefPtr`. You can just call both of them
whether you have a plain pointer or a `RefPtr`:

```cpp
~#include <peel/RefPtr.h>
~
~using namespace peel;
~
void takes_plain_ptr (Object *object);
void takes_ref_ptr (RefPtr<Object> object);

Object *my_plain_ptr = /* ... */;
RefPtr<Object> my_ref_ptr = /* ... */;

/* All of these work: */
takes_plain_ptr (my_plain_ptr);
takes_plain_ptr (my_ref_ptr);
takes_ref_ptr (my_plain_ptr);
takes_ref_ptr (my_ref_ptr);
takes_ref_ptr (std::move (my_ref_ptr));
```

When a `RefPtr` is used on an API boundary, meaning it is passed as an argument
to a function or returned from one, it signifies transfer of ownership, also
known as `trasnfer full` in GObject world. For instance,
[`Gio::ListModel::get_object`] returns an owned reference to an object, and has
the following signature in peel:

```cpp
RefPtr<GObject::Object>
Gio::ListModel::get_object (unsigned position) noexcept;
```

[`Gio::ListModel::get_object`]: https://docs.gtk.org/gio/method.ListModel.get_object.html

Sometimes, it makes sense to create temporary `RefPtr`s in order to make sure
an object is kept alive over some manipulation. For example, here's how you
would move a button between two boxes:

```cpp
~#include <peel/Gtk/Gtk.h>
~
~using namespace peel;
~
Gtk::Box *box1 = /* ... */;
Gtk::Box *box2 = /* ... */;
Gtk::Button *button = /* ... */;
/* button is a child of box1, to be moved into box2 */

RefPtr<Gtk::Button> button_ref = button;
box1->remove(button);
/* box1 has dropped its reference on button here! */
box2->append(button);
/* box2 has added a reference on button,
 * so our RefPtr can be safely dropped.
 */
```

A much more common pattern is keeping a `RefPtr` as a member variable in a
[class], in order to make sure that the referenced object stays alive for as
long as an instance of this class does:

```cpp
~#include <peel/Gtk/Gtk.h>
~#include <peel/class.h>
~
~using namespace peel;
~
class MyWidget : public Gtk::Widget
{
  /* ... */

private:
  RefPtr<Gio::ListModel> model;
  RefPtr<Gtk::Adjustment> h_adjustment, v_adjustment;
};
```

[class]: custom-gobject-classes.md

## Copy and move semantics

Note that copy and move semantics apply to `RefPtr` as usual in C++. Passing a
`RefPtr` into a function by copy, like here:


```cpp
~void takes_ref_ptr (RefPtr<Object> object);
~
RefPtr<Object> my_ref_ptr = /* ... */;

takes_ref_ptr (my_ref_ptr);
```

will result in:

* A fresh new instance of `RefPtr` being created for the function argument,
  initialized by copying the existing `my_ref_ptr` instance. This increments
  the reference count of the object by calling `g_object_ref` on it.
* The function gets called, and consumes the temporary `RefPtr`.
* Later, the original `RefPtr` is deallocated, and its destructor decrements
  the reference count of the object by calling `g_object_unref` on it.

This may be what you wanted if you plan to keep using the object (and the
`RefPtr`) after the function returns. On the other hand, if you pass the
`RefPtr` "by move" (typically using [`std::move`]), like this:

```cpp
~void takes_ref_ptr (RefPtr<Object> object);
~
RefPtr<Object> my_ref_ptr = /* ... */;

takes_ref_ptr (std::move (my_ref_ptr));
```

the temporary `RefPtr` for the argument will "steal" the reference from
`my_ref_ptr`. As the result, `my_ref_ptr` will be set to `nullptr` after the
call, and there will be no extra `g_object_ref`/`g_object_unref` calls at
runtime.

[`std::move`]: https://en.cppreference.com/w/cpp/utility/move

## Casting

**Upcasting** means converting a pointer to a derived type into a pointer to
its base type.

Upcasting should for the most part "just work" with `RefPtr`. In particular, it
should be possible to pass an instance of `RefPtr<DerivedType>` (or a plain
`DerivedType *` pointer) in places where an instance of `RefPtr<BaseType>` is
expected.

**Downcasting** means converting a pointer to a base type into a pointer to a
derived type, when we know that the dynamic type of the object te pointer
points to actually matches the derived type.

The usual way to perform downcasting in peel is with the
`GObject::TypeInstance::cast` method, like this:

```cpp
~#include <peel/Gtk/Gtk.h>
~
~using namespace peel;
~
Gtk::Widget *widget = /* ... */;
/* We know that it's actually a button */
Gtk::Button *button = widget->cast<Gtk::Button> ();
```

When used with a `RefPtr`, this will dereference the `RefPtr` and call the
usual `cast` method, which will return a plain pointer. To downcast the
`RefPtr` itself (moving it), use the `RefPtr::cast` method. This method is
defined on `RefPtr` itself rather than on the type it references, so in order
to call it, make sure to use a dot (`.`) and not an arrow (`->`):

```cpp
~#include <peel/Gtk/Gtk.h>
~
~using namespace peel;
~
RefPtr<Gtk::Widget> widget = /* ... */;
/* We know that it's actually a button */
RefPtr<Gtk::Button> button = std::move (widget).cast<Gtk::Button> ();
```

## Non object-derived types

Even though `RefPtr` is most commonly used with types derived from
`GObject::Object`, it also works with other types that support GObject-style
reference counting semantics. For example:

* `RefPtr<Gtk::Expression>` manages an instance of [`Gtk::Expression`], using
  [`gtk_expression_ref`] and [`gtk_expression_unref`];
* `RefPtr<Gsk::RenderNode>` manages an instance of [`Gsk::RenderNode`], using
  [`gsk_render_node_ref`] and [`gsk_render_node_unref`];
* `RefPtr<GLib::Variant>` manages an instance of [`GLib::Variant`], using
  [`g_variant_ref`] and [`g_variant_unref`]. This also demonstrates that a type
  doesn't even necessarily have to be a "class" (a `GObject::TypeInstance`) to
  be usable with `RefPtr`.

[`Gtk::Expression`]: https://docs.gtk.org/gtk4/class.Expression.html
[`gtk_expression_ref`]: https://docs.gtk.org/gtk4/method.Expression.ref.html
[`gtk_expression_unref`]: https://docs.gtk.org/gtk4/method.Expression.unref.html
[`Gsk::RenderNode`]: https://docs.gtk.org/gsk4/class.RenderNode.html
[`gsk_render_node_ref`]: https://docs.gtk.org/gsk4/method.RenderNode.ref.html
[`gsk_render_node_unref`]: https://docs.gtk.org/gsk4/method.RenderNode.unref.html
[`GLib::Variant`]: https://docs.gtk.org/glib/struct.Variant.html
[`g_variant_ref`]: https://docs.gtk.org/glib/method.Variant.ref.html
[`g_variant_unref`]: https://docs.gtk.org/glib/method.Variant.unref.html

Under the hood, reference counting for various types is implemented with an
extensible mechanism using the "traits" design pattern (specializations of a
`RefTraits<T>` template). If you're getting compile errors mentioning
"incomplete type `RefTraits<Something>`", it's likely the case that you're
trying to use `RefPtr` with some type that isn't reference-counted. For
example, here's what happens if you try to use `RefPtr<Gdk::RGBA>`:

```
peel/RefPtr.h: In instantiation of ‘peel::RefPtr<T>::~RefPtr() [with T = peel::Gdk::RGBA]’:
test.cpp:8:28:   required from here
    8 |   RefPtr<Gdk::RGBA> rgba = nullptr;
      |                            ^~~~~~~
peel/RefPtr.h:115:27: error: incomplete type ‘peel::RefTraits<peel::Gdk::RGBA, void>’ used in nested name specifier
  115 |       RefTraits<T>::unref (ptr);
      |       ~~~~~~~~~~~~~~~~~~~~^~~~~
```

## FFI

When using plain C APIs that have no peel wrappers, or when wrapping your C++
API into a plain C interface, you may need to "bridge" between `RefPtr` and
plain pointers, whether `transfer full` or `transfer none`.

For `transfer none` pointers, just extract or assign a plain pointer using
normal means such as `static_cast` (or C-style cast) and `operator =`:

```cpp
/**
 * frob_button:
 * @button: (transfer none): a button to frob
 */
void
frob_button (GtkButton *button);

/**
 * find_button:
 *
 * Returns: (nullable) (transfer none) a button, or NULL if not found
 */
GtkButton *
find_button (void);

RefPtr<Gtk::Button> button = /* ... */;
GtkButton *c_button = GTK_BUTTON ((Gtk::Button *) button);
frob_button (c_button);

c_button = find_button ();
button = reinterpret_cast<Gtk::Button *> (c_button);
```

For `trasnfer full` pointers, use the `RefPtr::adopt_ref` static method and the
`release_ref` method. (Note that `release_ref` must be called on an rvalue,
for example the result of `std::move`).

```cpp
/**
 * consume_button:
 * @button: (transfer full): a button to consume
 */
void
consume_button (GtkButton *button);

/**
 * produce_button:
 *
 * Returns: (transfer full) a produced button
 */
GtkButton *
produce_button (void);

RefPtr<Gtk::Button> button = /* ... */;
Gtk::Button *owned_plain_button = std::move (button).release_ref ();
GtkButton *owned_c_button = GTK_BUTTON (owned_plain_button);
consume_button (owned_c_button);

owned_c_button = produce_button ();
owned_plain_button = reinterpret_cast<Gtk::Button *> (owned_c_button);
button = RefPtr<Gtk::Button>::adopt_ref (owned_plain_button);
```

You can even use `adopt_ref` and `release_ref` to intentionally leak a
reference owned by a `RefPtr`, and to intentionally make up a reference when
there is none (perhaps to recreate a reference you've leaked earlier).

Note that these APIs are less "safe" then typical `RefPtr` usage. You take
responsibility for maintaining the reference count and not messing it up.
