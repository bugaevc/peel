# Destruction

[GObject destruction] happens in two phases: _dispose_, then _finalize_. In
plain C, these are both virtual functions, but peel bridges _finalize_ to the
C++ destructor. Therefore, while GObject classes in peel [do not have C++
constructors](initialization.md), they **do have C++ destructors**.

[GObject destruction]: https://docs.gtk.org/gobject/tutorial.html#object-destruction

## Disposing

_Dispose_ is a virtual function defined in the base `GObject::Object` class
([`GObject::Object::dispose`]), and you can override it much like any other vfunc:

[`GObject::Object::dispose`]: https://docs.gtk.org/gobject/vfunc.Object.dispose.html

```cpp
inline void
Gizmo::Class::init ()
{
  override_vfunc_dispose<Gizmo> ();
}

inline void
Gizmo::vfunc_dispose ()
{
  /* ...tear things down... */

  parent_vfunc_dispose<Gizmo> ();
}
```

Generally, in _dispose_ you should release (let go of, disconnect yourself
from) any other objects that your object references, so your object becomes
inert. But note that `vfunc_dispose` may, potentially, be invoked multiple
times for the same object, so you should not crash or misbehave if it is
invoked repeatedly.

If you're keeping a reference to another object using [`RefPtr`], you can just
assign `nullptr` to it, which will release the object if the pointer wasn't
already null[^g_clear_object]:

[`RefPtr`]: ref-ptr.md

[^g_clear_object]: assigning `nullptr` to a `RefPtr` is peel’s counterpart of
[`g_clear_object`](https://docs.gtk.org/gobject/func.clear_object.html)

```cpp
class Gizmo final : public peel::GObject::Object
{
  PEEL_SIMPLE_CLASS (Gizmo, Object)

  inline void
  vfunc_dispose ();

  RefPtr<Gio::InputStream> input_stream;
};

inline void
Gizmo::vfunc_dispose ()
{
  /* release the input stream, if any */
  input_stream = nullptr;

  parent_vfunc_dispose<Gizmo> ();
}
```

If your class is a GTK widget that holds another widget as a direct child (and
not as a [template] child), you should remember to [`unparent`] the child in
your _dispose_, but only do so the first time `vfunc_dispose` is invoked, like
this:

[template]: gtk-widget-templates.md
[`unparent`]: https://docs.gtk.org/gtk4/method.Widget.unparent.html

```cpp
inline void
Gizmo::vfunc_dispose ()
{
  if (child)
    {
      child->unparent ();
      child = nullptr;
    }

  parent_vfunc_dispose<Gizmo> ();
}
```

## Finalization

Eventually (typically, immediately following disposal), your object instance
will be _finalized_. peel bridges finalization to the C++ destructor of your
class, which generally makes things "just work", as the destructor
automatically tears down any remaining data that your object owns, such as any
strings or vectors (or any `RefPtr` you forget to nullify in _dispose_).

Note that even though you don't (and can't) declare the C++ destructor
`virtual`, peel will properly invoke the correct destructor of the actual
dynamic type of the object, much like a [`virtual` destructor] behaves in
regular C++.

[`virtual` destructor]: https://en.cppreference.com/w/cpp/language/destructor.html#Virtual_destructors

Although it's rarely needed, you can still provide an explicit destructor using
the regular C++ syntax for destructors:

```cpp
class Gizmo final : public peel::GObject::Object
{
  PEEL_SIMPLE_CLASS (Gizmo, Object)

protected:
  ~Gizmo ();
};

inline
Gizmo::~Gizmo ()
{
  g_print ("Finalizing a Gizmo object\n");

  /* implicitly chains up to the parent destructor here */
}
```
