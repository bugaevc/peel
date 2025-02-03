# Value

The [`GObject::Value`] (aliased as just `peel::Value` for short) shows up a lot
less in C++ code using peel compared to the frequency of `GValue` in plain C
code (this is in no small part because of peel implementing the
[`get_property`] and [`set_property`] vfuncs automatically). Still, it plays an
important role in various APIs.

[`GObject::Value`]: https://docs.gtk.org/gobject/struct.Value.html
[`get_property`]: https://docs.gtk.org/gobject/vfunc.Object.get_property.html
[`set_property`]: https://docs.gtk.org/gobject/vfunc.Object.set_property.html

To create a new value, define a variable of type `Value` (on the stack) and
initialize it by passing the type that it's going to hold:

```cpp
~#include <peel/GObject/Value.h>
~
~using namespace peel;
~
Value value { Type::of<int> () };
```

To get and set the value, use the method templates `get` and `set`:

```cpp
~#include <peel/GObject/Value.h>
~
~using namespace peel;
~
~Value value { Type::of<int> () };
value->set<int> (42);
int fourty_two = value->get<int> ();
```

The template type argument used here
