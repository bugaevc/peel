# ArrayRef

`ArrayRef<T>` is a simple non-owning pointer that references an array of values
of type `T`, along with their count. It is analogous to C++20 [`std::span`], or
a [Rust slice].

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

/* a C array: */
int my_array[5] = { 1, 2, 3, 4, 5 };
takes_int_array (my_array);

/* base pointer + count: */
int *my_ptr = /* ... */;
size_t my_count = /* ... */;
takes_int_array ({ my_ptr, my_count });

/* nullptr: */
takes_int_array (nullptr);
```

To take apart an `ArrayRef`, you can use the `data` method to get the base
pointer and the `size` method to get the number of items.

`ArrayRef` supports common idioms. It can be indexed, iterated over, and
sliced:

```cpp
ArrayRef<Gtk::Widget> widgets = /* ... */;

Gtk::Widget *first_widget = widgets[0];

for (Gtk::Widget *widget : widgets)
  g_print ("%s\n", widget->get_name ());

ArrayRef<Gtk::Widget> widgets_slice = widgets.slice (2, 3);
```

`ArrayRef` is also compatible with C++17 [`std::data`] and [`std::size`].

[`std::data`]: https://en.cppreference.com/w/cpp/iterator/data
[`std::size`]: https://en.cppreference.com/w/cpp/iterator/size

## Mutability

`ArrayRef<T>` is a mutable version, in that it's possible to mutate the values
of type `T` that it references through the `ArrayRef`. `ArrayRef<const T>` is
the constant version which does not allow mutation. This is very similar to the
difference between `T *` and `const T *`.

The following two functions from [libdex] demonstrate the difference:

[libdex]: https://blogs.gnome.org/chergert/2022/11/24/concurrency-parallelism-i-o-scheduling-thread-pooling-and-work-stealing/

```cpp
RefPtr<Future>
Dex::input_stream_read (Gio::InputStream *self, ArrayRef<uint8_t> buffer, int io_priority) noexcept;

RefPtr<Future>
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
