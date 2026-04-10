# C++20 coroutines

peel implements some experimental integration with the C++20 [coroutines]
language feature.

[coroutines]: https://en.cppreference.com/w/cpp/language/coroutines.html

Using coroutines comes with the two main caveats:
* It requires C++20, whereas the rest of peel only requires C++11.
  Alternatively, it might possible to opt in to coroutines support even on
  older standard versions using compiler options such as `-fcoroutines`.
  Coroutine support is implemented [in GCC](gcc-coroutines) starting from
  GCC 10.
* Coroutines, as implemented in C++, have some inherent run-time overhead, in
  that they work via heap allocation and indirect function calls. That being
  said, the overhead is sufficiently small in practice.

[gcc-coroutines]: https://gcc.gnu.org/wiki/cxx-coroutines

However, if these limitations are acceptable for your use case, coroutines
provide a fairly robust and ergonomic way to write asynchronous logic.

The full design of C++ coroutines is complex, but the basics are as follows:
* Coroutines have to use a special return type;
* Inside of a coroutine body, use `co_await` to suspend while awaiting
  something, and `co_return` to return a value.

Coroutines cannot be used purely as a language feature. You need some library
types to be able to even define a coroutine, or to await a value. As of C++23,
the C++ standard library does not provide such types (except for
[`std::generator`], see also [P3552R3] and [P3801R0]).

[`std::generator`]: https://en.cppreference.com/w/cpp/coroutine/generator.html
[P3552R3]: https://wg21.link/P3552R3
[P3801R0]: https://wg21.link/P3801R0

peel includes an experimental coroutine support library, `peel::coro`. It has a
dual purpose:
* providing basic library types that enable usage of coroutines, as well as
  related utilities,
* integrating C++ coroutines with GLib-style async APIs and the
  [GLib main loop].

[GLib main loop]: https://developer.gnome.org/documentation/tutorials/main-contexts.html

`peel::coro` can be used on its own, or in combination with another C++
coroutines library, such as [CppCoro], [`folly::coro`], [boost.cobalt],
[concurrencpp].

[CppCoro]: https://github.com/lewissbaker/cppcoro
[`folly::coro`]: https://github.com/facebook/folly/tree/main/folly/coro
[boost.cobalt]: https://github.com/boostorg/cobalt
[concurrencpp]: https://github.com/David-Haim/concurrencpp

## SimpleTask

`SimpleTask` can be used as a return type to give a coroutine spawn-and-forget
semantics. It is designed to be promptly discarded by the coroutine's caller:

```cpp
$#include <peel/coro/SimpleTask.h>
$
$using namespace peel;
$
coro::SimpleTask
some_async_logic ()
{
  co_await foo ();
  co_await bar ();
}

void
on_button_clicked (Gtk::Button *button)
{
  /* Spawn the coroutine */
  some_async_logic ();
}
```

`SimpleTask` is extremely simple and minimal, and has no functionality beyond
enabling you to use coroutines at all. In particular, it does not track whether
the coroutine has completed, and there is no support for the coroutine
returning a value. The upside is that there is no additional overhead that
would have been required for tracking that state.

## Future

`Future<T>` can be used as a return type for a coroutine that eventually
returns a value of type `T` (which may be `void`). The caller can `co_await`
the future to obtain the return value of the coroutine.

```cpp
$#include <peel/coro/Future.h>
$
$using namespace peel;
$
coro::Future<int>
some_async_callee (int a)
{
  int b = co_await foo ();
  int c = co_await bar ();
  /* return an int */
  co_return a + b + c;
}

coro::Future<void>
some_async_caller ()
{
  int res = co_await some_async_callee (42);
  /* implicit co_return */
}
```

It is possible to start a coroutine, store the `Future`, and await it at a
later time. This enables running sub-coroutines concurrently:

```cpp
$#include <peel/coro/Future.h>
$
$using namespace peel;
$
$coro::Future<int>
$some_async_callee (int a)
${
$  int b = co_await foo ();
$  int c = co_await bar ();
$  /* return an int */
$  co_return a + b + c;
$}
$
coro::Future<long>
async_sum ()
{
  coro::Future<int> futures[100];
  /* Start the 100 coroutines, running concurrently */
  for (int i = 0; i < 100; i++)
    futures[i] = some_async_callee (i);
  /* Wait for them to complete */
  long sum = 0;
  for (coro::Future<int> &future : futures)
    sum += co_await std::move (future);
  co_return sum;
}
```

It is also valid to discard a future without awaiting it. The coroutine will
keep running, but its eventual result will also get discarded. If you're using
this, you should likely also tell the coroutine that it's cancelled, so it can
complete sooner; use [`Gio::Cancellable`] for this.

[`Gio::Cancellable`]: https://docs.gtk.org/gio/class.Cancellable.html

## AsyncResult

`AsyncResult` enables using GLib-style async APIs, which come in the form of
async/finish pairs and use [`Gio::AsyncResult`].

[`Gio::AsyncResult`]: https://docs.gtk.org/gio/iface.AsyncResult.html

To use it, make a value of type `coro::AsyncResult`, pass its `.callback ()` as
the `AsyncReadyCallback` to the "async" function. Awaiting the result then
evaluates to the `Gio::AsyncResult`, which you should pass to the "finish"
function:

```cpp
$#include <peel/coro/AsyncResult.h>
$#include <peel/coro/Future.h>
$
$using namespace peel;
$
coro::Future<String>
read_first_line (Gio::File *file, int priority,
                 RefPtr<Gio::Cancellable> cancellable,
                 UniquePtr<GLib::Error> *error)
{
  coro::AsyncResult async_result;

  /* Call the async function, passing .callback () */
  file->read_async (priority, cancellable, async_result.callback ());
  /* Await the async result, and pass it to the finish function */
  RefPtr<Gio::FileInputStream> stream = file->read_finish (co_await async_result, error);
  if (!stream)
    co_return nullptr; /* error was set */

  RefPtr<Gio::DataInputStream> data_stream = Gio::DataInputStream::create (stream);

  /* Perform another async call, re-using the same async result */
  data_stream->read_line_async (priority, cancellable, async_result.callback ());
  co_return data_stream->read_line_finish_utf8 (co_await async_result, nullptr, error);
}
```

> [!WARNING]
> Unlike with `Future`, it is invalid to discard an async result without
> awaiting it, after having passed its `.callback ()` to an async function.
