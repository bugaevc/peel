# ArrayRef

`ArrayRef<T>` references an array of a dynamic size. It is a simple non-owning
pointer that contains a base pointer of type `T *` and size. `ArrayRef` is
analogous to C++20 [`std::span`], or a [Rust slice].

[`std::span`]: https://en.cppreference.com/w/cpp/container/span
[Rust slice]: https://doc.rust-lang.org/std/primitive.slice.html

`ArrayRef` doesn't do much by itself, and is mostly used on API boundaries to
make it ergonomic to pass "an array" as an argument in various ways:

```cpp
~#include <peel/ArrayRef.h>
~
~using namespace peel;
~
void takes_int_array (ArrayRef<int> arr);

/* a C array (size deduced) */
int my_array[5] = { 1, 2, 3, 4, 5 };
takes_int_array (my_array);

/* base pointer + count */
int *my_ptr = /* ... */;
size_t my_count = /* ... */;
takes_int_array ({ my_ptr, my_count });

/* nullptr */
takes_int_array (nullptr);

/* ArrayRef instance */
ArrayRef<int> another_array = /* ... */;
takes_int_array (another_array);
```

To take apart an `ArrayRef`, getting a base pointer and a size, you can use
the `data` and the `size` methods respectively. This is also compatible with
C++17 [`std::data`] and [`std::size`].

[`std::data`]: https://en.cppreference.com/w/cpp/iterator/data
[`std::size`]: https://en.cppreference.com/w/cpp/iterator/size

`ArrayRef` supports common C++ idioms, for example it can be indexed and
iterated over. This also means large parts of the [C++ algorithm library]
should work with it.

[C++ algorithm library]: https://en.cppreference.com/w/cpp/algorithm

```cpp
ArrayRef<Gtk::Widget *> widgets = /* ... */;

Gtk::Widget *first_widget = widgets[0];

for (Gtk::Widget *widget : widgets)
  g_print ("%s\n", widget->get_name ());
```

`ArrayRef` can also be sliced, which produces a new `ArrayRef` referring to a
part (slice) of the original array. This is useful when working with data, for
instance here's what the body of a read-write echo loop might look like:

```cpp
Gio::InputStream *input_stream = /* ... */;
Gio::OutputStream *output_stream = /* ... */;

/* read some data */

unsigned char buffer[1024];
auto n_read = input_stream->read (buffer, nullptr, nullptr);
/* deal with errors... */

/* write back what we have read */

ArrayRef<unsigned char> to_write { buffer, n_read };
while (to_write)
  {
    auto n_written = output_stream->write (to_write, nullptr, nullptr);
    /* deal with errors... */
    to_write = to_write.slice (n_written, to_write.size () - n_written);
  }
```

## Mutability

`ArrayRef<T>` is a mutable version, in that it's possible to mutate the values
of type `T` that it references through the `ArrayRef`. `ArrayRef<const T>` is
the constant version which does not allow mutation. This is very similar to the
difference between `T *` and `const T *`.

The following two functions from [libdex] demonstrate the difference:

[libdex]: https://blogs.gnome.org/chergert/2022/11/24/concurrency-parallelism-i-o-scheduling-thread-pooling-and-work-stealing/

```cpp
RefPtr<Dex::Future>
Dex::input_stream_read (Gio::InputStream *self, ArrayRef<uint8_t> buffer, int io_priority) noexcept;

RefPtr<Dex::Future>
Dex::output_stream_write (Gio::OutputStream *self, ArrayRef<const uint8_t> buffer, int io_priority) noexcept;
```

[`Dex::input_stream_read`]'s `buffer` argument is a mutable array of bytes that
the call will fill with read data, whereas [`Dex::output_stream_write`]'s
`buffer` argument is a read-only array of bytes.

[`Dex::input_stream_read`]: https://gnome.pages.gitlab.gnome.org/libdex/libdex-1/func.input_stream_read.html
[`Dex::output_stream_write`]: https://gnome.pages.gitlab.gnome.org/libdex/libdex-1/func.output_stream_write.html

See [`examples/libdex.cpp`] in the peel repository for an example of using
`ArrayRef` and libdex APIs.

[`examples/libdex.cpp`]: https://gitlab.gnome.org/bugaevc/peel/-/blob/tmp/examples/libdex.cpp?ref_type=heads

## Fixed size arrays

`ArrayRef` is used for arrays whose size is only known dynamically. For fixed-
size arrays, peel instead uses C++ references to C arrays[^note]. For example,
[`Gsk::BorderNode::create`] accepts two arrays, both of them having fixed size
of four, and the [`Gsk::BorderNode::get_widths`] getter returns the widths:

[`Gsk::BorderNode::create`]: https://docs.gtk.org/gsk4/ctor.BorderNode.new.html
[`Gsk::BorderNode::get_widths`]: https://docs.gtk.org/gsk4/method.BorderNode.get_widths.html

```cpp
RefPtr<Gsk::BorderNode>
Gsk::BorderNode::create (const Gsk::RoundedRect *outline, const float (&border_width)[4], const Gdk::RGBA (&border_color)[4]) noexcept;

const float
(&Gsk::BorderNode::get_widths () const noexcept)[4];
```

While the declaration syntax may look somewhat intimidating[^peel-gen], this
enables you to pass a C array of a matching size in a very natural way:

```cpp
Gsk::RoundedRect outline = /* ... */;
float border_width[4] = /* ... */;
Gdk::RGBA border_color[4] = /* ... */;

auto node = Gsk::BorderNode::create (&outline, border_width, border_color);
```

On the other hand, attempting to pass an array of a wrong size will result in a
type error. (You can use an explicit cast to silence the error if you think you
know what you're doing.)

Using a returned fixed-size array is similarly straightforward:

```cpp
Gsk::BorderNode *node = /* ... */;
auto &widths = node->get_widths ();

g_print ("%f\n", widths[3]);
```

Note the `&` on `widths` declaration; if you omit it, a local array will be
declared instead of a reference to one, and the returned array will be
immediately copied out into the local array[^small].

Thanks to the type being a reference to an array and not a plain pointer, the
compiler should be able to emit a warning if you use an out-of-bounds index:

```
warning: array index 42 is past the end of the array (that has type 'const float[4]') [-Warray-bounds]
```

[^note]: this is a somewhat unusual combination, leading to a farily weird
syntax, and also about the only place where C++ references show up in peel
APIs (other than in special members like copy constructors)

[^peel-gen]: to humans and machines alike: the peel bindings generator required
significant enhancements to be able to produce this syntax

[^small]: considering that fixed-size arrays typically have a fairly small
size (in this case, 4), this may be just fine too
