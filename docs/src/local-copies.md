# Local copies

For _out_ and _inout_ parameters, we can frequently pass through the
user-provided pointer directly to the C function, perhaps casting the pointer
type:

```c++
peel_arg_out (1) peel_arg_out (2)
static void
foo (int *out_integer, GObject::Object **out_object) noexcept
{
  ::GObject **_peel_out_object = reinterpret_cast<::GObject **> (out_object);
  c_foo (out_integer, _peel_out_object);
}
```

In some more complex cases, it is not possible to pass through the same
pointer, and we have to create a *local copy* variable, into which we copy the
"in" value of the argument (for _inout_ params), pass the pointer to this
local copy into the C function, and copy out the "out" value to user-provided
pointer:

```c++
peel_arg_inout (1) peel_nonnull_args (1)
static void
bar (bool *inout_bool) noexcept
{
  gboolean _peel_inout_bool = static_cast<gboolean> (*inout_bool);
  c_bar (&_peel_inout_bool);
  *inout_bool = !!_peel_inout_bool;
}
```

In the example above, we need the local copy because `bool` and `gboolean`
differ in ABI/layout.

Note that this is still zero-overhead in practice: an optimizing compiler
would not actually materialize two distinct variables of types `bool` and
`gboolean`. Instead, the `inout_bool` "pointer" will be a synthetic one, and
subsequent code in the caller code will branch on the `gboolean` value
directly.

We also use local copies for owned reference transfer:

```c++
peel_arg_out (1) peel_nonnull_args (1)
static void
baz (peel::RefPtr<GObject::Object> *out_object) noexcept
{
  ::GObject *_peel_out_object;
  c_baz (&_peel_out_object);
  *out_object = peel::RefPtr<GObject::Object>::adopt_ref (reinterpret_cast<GObject::Object *> (_peel_out_object));
}
```

This way, this way, setting the value to `*out_object` goes through the C++
`operator =`, which properly unsets the previous value, and is "less UB"
compared to what would happen if we simply casted the pointer to `::GObject **`
and let the C code overwrite it, without using a local copy. Again, the hope
here is that the compiler will see through this and merge the two variables,
especially when the `*out_object` points to a fresh, null-initialized
`RefPtr` variable.

Another reason for needing to make a local copy is when the parameter is
optional, but we need to pass non-null into the C function even when null is
passed into the C++ wrapper. Notably, this happens with the `GError **`
argument, because peel needs to know for sure whether or not there had been an
error, because of other local copies which we only copy out when the C
function succeeds:

```c++
peel_arg_out (1) peel_arg_out (2) peel_nonnull_args (1)
static void
read_some_string (peel::String *out_string, peel::UniquePtr<GLib::Error> *error) noexcept
{
  gchar *_peel_out_string;
  ::GError *_peel_error = nullptr;
  c_read_some_data (&_peel_out_string, &_peel_error);
  if (_peel_error)
    {
      /* An error happened, _peel_out_string contains garbage */
      if (error) /* The caller wants to see the error */
        *error = peel::UniquePtr<GLib::Error>::adopt_ref (reinterpret_cast<GLib::Error *> (_peel_error));
      else
        g_error_free (_peel_error);
    }
  else
    {
      /* No error happened, copy out _peel_out_string */
      if (error)
        *error = nullptr;
      *out_string = peel::String::adopt_string (_peel_out_string);
    }
}
```
