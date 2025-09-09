# Custom GObject Classes

While it's possible, to an extent, to use GTK by solely combining existing
classes, idiomatic GTK usage involves writing one's own GObject classes, such
as custom widgets (typically, [composite widget templates]). Implementing a
GObject class in peel is largely the same as implementing any other C++ class,
but there are some important specifics.

[composite widget templates]: gtk-widget-templates.md

You should include `<peel/class.h>`, derive your class from a
single[^single-inheritance] GObject base class (for example, `GObject::Object`
or `Gtk::Widget`). Unless you're actively planning for your class to be
derivable, you should declare it `final`, both as a general design principle,
and because it enables small optimizations inside GLib.

Then use the `PEEL_SIMPLE_CLASS` (or `PEEL_CLASS`) macro[^declare-type] as the
first thing inside the class body. The arguments to the macro are:
* the _unqualified_ name of class itself,
* the parent class (only for `PEEL_SIMPLE_CLASS`).

```cpp
#include <peel/GObject/Object.h>
#include <peel/class.h>

namespace Demo
{

class Gizmo final : public peel::GObject::Object
{
  PEEL_SIMPLE_CLASS (Gizmo, Object)
};

} /* namespace Demo */
```

In the implementation file (`.cpp`), use the corresponding `PEEL_CLASS_IMPL`
macro[^define-type]. The arguments to the macro are:
* the _qualified_ name of the class,
* GObject type name for the class, as a string[^naming],
* the parent class.

```cpp
~#include "Demo/Gizmo.h"
~
PEEL_CLASS_IMPL (Demo::Gizmo, "DemoGizmo", peel::GObject::Object)
```

Alternatively, it is possible to use `PEEL_CLASS_IMPL` inside a namespace,
like so:

```cpp
~#include "Demo/Gizmo.h"
~
namespace Demo
{

PEEL_CLASS_IMPL (Gizmo, "DemoGizmo", peel::GObject::Object)

} /* namespace Demo */
```

You should then implement, at a minimum, a _class initializer_, which in the
simplest case can be empty:

```cpp
inline void
Demo::Gizmo::Class::init ()
{ }
```

This is enough for the class to compile. You should now be able to create an
instance of the class using `Object::create`:

```cpp
~#include "Demo/Gizmo.h"
~
~using namespace peel;
~
int
main ()
{
  RefPtr<Demo::Gizmo> gizmo = Object::create<Demo::Gizmo> ();
  g_print ("%s\n", gizmo->get_type_name ());
}
```

[^single-inheritance]: similar to languages like Java, C#, and Objective-C,
and unlike C++, the GObject type system supports single inheritance, as well
as implementing multiple interfaces

[^declare-type]: `PEEL_SIMPLE_CLASS` is peel's counterpart of
[`G_DECLARE_FINAL_TYPE`](https://docs.gtk.org/gobject/func.DECLARE_FINAL_TYPE.html)
and [`Q_OBJECT`](https://doc.qt.io/qt-6/qobject.html#Q_OBJECT)

[^define-type]: `PEEL_CLASS_IMPL` is peel's counterpart of
[`G_DEFINE_TYPE`](https://docs.gtk.org/gobject/func.DEFINE_TYPE.html)

[^naming]: see [Naming conventions](naming-conventions.md)
