# Defining signals

You can define [signals](signals.md) on classes
using a `Signal<Instance, Ret (Args...)>`.
Here is a breakdown of what each template parameter means:

* `Instance` is the type of the object involved in the event.
* `Ret (Args...)` is the signature of any handler connected to the signal
  **without** the first `Instance *` parameter.
* `Ret` is the return type of a handler. Usually `void`.
* `Args...` are parameters passed to handlers that inform about an event.

Here are the steps to add a `Signal` to a class:

1. Declare a `Signal` as a static member of your class.
2. Use the `PEEL_SIGNAL_CONNECT_METHOD` macro in your class to declare the
   <code>.connect_<i>signal_name</i> ()</code> method.
3. In your [class initializer], initialize the `Signal` static member with
   a call to `Signal::create ()`.

[class initializer]: initialization.md#class-initializer

GObject signal names are in `kebab-case` except when appearing in a function
name. `Signal::create ()` takes in the signal name as a string in `kebab-case`.
The first `PEEL_SIGNAL_CONNECT_METHOD` argument is the signal name in
`snake_case`. The second argument is the `Signal` static member.

When an event occurs, you can call `.emit ()` on your
`Signal`, and pass the `Instance *` and any other parameters.

Here is an example of a custom `MySwitch` class that emits
a signal when switched on or off.

```cpp
$#include <peel/GObject/Object.h>
$#include <peel/GObject/ParamSpec.h>
$#include <peel/class.h>
$#include <peel/signal.h>
$
$using namespace peel;
$
class MySwitch final : public Object
{
  PEEL_SIMPLE_CLASS (MySwitch, Object)

  bool state;

  /* The signature of a handler is void (MySwitch *, bool) */
  static Signal<MySwitch, void (bool)> sig_switched;

public:

  /* Declares the connect_switched function. */
  PEEL_SIGNAL_CONNECT_METHOD (switched, sig_switched)

  void
  set_enabled (bool new_state);
};

/* C++ requires us to explicitly define storage for a static member. */
Signal<MySwitch, void (bool)> MySwitch::sig_switched;

PEEL_CLASS_IMPL (MySwitch, "MySwitch", Object)

void
MySwitch::Class::init ()
{
  sig_switched = Signal<MySwitch, void (bool)>::create ("switched");
}

void
MySwitch::set_enabled (bool new_state)
{
  /* Don't emit if the state doesn't change. */
  if (new_state == state)
    return;

  state = new_state;
  sig_switched.emit (this, new_state);
}

int
main ()
{
  RefPtr<MySwitch> my_switch = Object::create<MySwitch> ();
  my_switch->set_enabled (false);

  my_switch->connect_switched (
    [] (MySwitch *, bool is_on)
    {
      g_print ("The switch is %s.\n", is_on ? "on" : "off");
    });

  my_switch->set_enabled (true);
  my_switch->set_enabled (false);
}
```
