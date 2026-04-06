# Errors

GLib implements an error reporting and handling mechanism based on the
[`GLib::Error`] type. See [Error Reporting] in GLib documentation.

[`GLib::Error`]: https://docs.gtk.org/glib/struct.Error.html
[Error Reporting]: https://docs.gtk.org/glib/error-reporting.html

In peel, errors typically take form of
<code><a href="unique-ptr.md">UniquePtr</a>&lt;GLib::Error&gt;</code> that is
returned from a called function by an out-argument:

```cpp
$#include <peel/GLib/KeyFile.h>
$
$using namespace peel;
$
UniquePtr<GLib::Error> error;

RefPtr<GLib::KeyFile> key_file = GLib::KeyFile::load_from_file (
  "/path/to/key-file.ini", GLib::KeyFile::Flags::NONE, &error);

if (error)
  g_printerr ("Could not load the key file: %s\n", error->message);
```

Note that the `error` pointer must be initialized to `nullptr` before calling a
function that could report an error. `UniquePtr`'s default constructor does
that implicitly, but you may have to reset the error pointer to `nullptr` when
invoking another error-reporting function after having received an error.

The error's textual message is appropriate to be displayed to the user. In
addition, errors carry a _domain_ and a _code_ inside that domain. The domain
identifies a specific enumeration of possible error codes. You can use this to
handle errors differently depending on their kind.

## Reporting errors

To report an error in your function, use [`GLib::set_error`] (or its variant
[`GLib::set_error_literal`]):

```cpp
$#include <peel/GLib/GLib.h>
$
$using namespace peel;
$
bool
do_something (const char *address, UniquePtr<GLib::Error> *error)
{
  bool ok = do_something_inner (address);

  if (!ok)
    GLib::set_error (error, G_IO_ERROR, G_IO_ERROR_ADDRESS_IN_USE,
                     _("Address '%s' is already in use"), address);
  return ok;
}
```

[`GLib::set_error`]: https://docs.gtk.org/glib/func.set_error.html
[`GLib::set_error_literal`]: https://docs.gtk.org/glib/func.set_error_literal.html

Use [`GLib::propagate_error`] and [`GLib::propagate_prefixed_error`] to
propagate an error that you have received from another function out of your
function:

```cpp
$#include <peel/GLib/GLib.h>
$
$using namespace peel;
$
bool
do_something (const char *address, UniquePtr<GLib::Error> *error)
{
  UniquePtr<GLib::Error> local_error;

  do_something_inner (address, &local_error);

  if (local_error)
    {
      GLib::propagate_error (error, std::move (local_error));
      return false;
    }

  return true;
}
```

[`GLib::propagate_error`]: https://docs.gtk.org/glib/func.propagate_error.html
[`GLib::propagate_prefixed_error`]: https://docs.gtk.org/glib/func.propagate_prefixed_error.html

## Custom error domains

At its core, an error domain is an enumeration of error codes. Start by making
such an enumeration, using `int` for its underlying type:

```cpp
namespace Demo
{

enum class ThumbnailError : int
{
  /* We don't support loading images in this format */
  UNSUPPORTED_FORMAT,

  /* Image data was invalid */
  MALFORMED_IMAGE,

  /* Policy in use says we shouldn't load this image */
  DENIED_BY_POLICY
};

} /* namespace Demo */
```

Unlike in some other languages and error handling frameworks, you don't need to
have special values to wrap possible errors from other domains (such as a
`ThumbnailError::IO_ERROR` code wrapping an error in the `G_IO_ERROR` domain).
This is because you're still able to use errors from those other domains
directly; in other words, a `GLib::Error` already represents a union of all
possible error domains.

When deciding how fine- or coarse-grained your error codes should be, consider
whether your caller would likely want to react to the situations they describe
differently. In this example involving thumbnails, the caller is unlikely to
care exactly how the image data was malformed (e.g. which specific header field
contained an invalid value), therefore all of those cases are represented with
a single error code. The textual error message may contain more details about
what went wrong.

Next, define a _quark_ representing your error domain. By convention, you
should also provide a macro to reference the quark:

```cpp
$#include <peel/GLib/Quark.h>
$
$using namespace peel;
$
namespace Demo
{

GLib::Quark
thumbnail_error_quark ();

#define DEMO_THUMBNAIL_ERROR (Demo::thumbnail_error_quark ())

} /* namespace Demo */
```

To implement the quark function, simply make a quark from a string that
uniquely identifies your error domain. The convention is to use the name of the
quark function, including its [namespace], in kebab-case:

```cpp
$#include <peel/GLib/Quark.h>
$
$using namespace peel;
$
GLib::Quark
Demo::thumbnail_error_quark ()
{
  return "demo-thumbnail-error-quark";
}
```

[namespace]: naming-conventions.md

With that in place, you should be now able to use the new error domain:

```cpp
GLib::set_error (DEMO_THUMBNAIL_ERROR, (int) Demo::ThumbnailError::UNSUPPORTED_FORMAT,
                 _("Creating thumbnails for images in '%s' format is not supported"), image_format);
```

Similarly, you should be able to match on returned error to see if they belong
to your domain:

```cpp
UniquePtr<GLib::Error> error;

load_thumbnail (url, &error);

if (!error)
  /* Success! Show the loaded thumbnail */
else if (error->matches (G_IO_ERROR, G_IO_ERROR_NETWORK_UNREACHABLE))
  /* We're offline, stop trying and let the user know */
else if (error->matches (G_IO_ERROR, G_IO_ERROR_TIMED_OUT)
  /* Fetching timed out, maybe try again */
else if (error->domain == G_IO_ERROR)
  /* Some other I/O error, let the user know */
else if (error->matches (DEMO_THUMBNAIL_ERROR, (int) Demo::ThumbnailError::DENIED_BY_POLICY))
  /* Display a placeholder, suggest changing policy settings */
else
  /* ... */
```
