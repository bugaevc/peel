#include <peel/GObject/Object.h>
#include <peel/GObject/ParamSpec.h>
#include <peel/class.h>
#include <peel/signal.h>

using namespace peel;

// The Counter class has a single read-only integer property "count", which can
// be manipulated by adding values to it. The first time the counter reaches 10,
// the "reached-10" signal is emitted.
class Counter : public Object
{
  PEEL_SIMPLE_CLASS (Counter, Object)

  // We store the count as a field in our class.
  // This is common, but not required for properties in any way.
  unsigned count;

public:
  // A simple public getter for the count.
  unsigned
  get_count ()
  {
    return count;
  }

  // A method that causes count to change.
  void
  add (unsigned addend);

  // This declares the Property<unsigned> prop_count () method.
  PEEL_PROPERTY (unsigned, count, "count")

  // This declares the connect_reached_10 () method.
  PEEL_SIGNAL_CONNECT_METHOD (reached_10, sig_reached_10)

private:
  // In define_properties (), we define all of our properties.
  template<typename F>
  static void
  define_properties (F &f)
  {
    // A read-only property (with a getter, but no setter).
    f.prop (prop_count (), 0, 100, 0)
      .get (&Counter::get_count);
  }

  // Our "reached-10" signal, with no arguments or return value.
  static Signal<Counter, void (void)> sig_reached_10;
};

// C++ requires us to explicitly define storage for a static member.
Signal<Counter, void (void)> Counter::sig_reached_10;

PEEL_CLASS_IMPL (Counter, "Counter", Object)

void
Counter::Class::init ()
{
  // At class initialization time, we register our signal.
  sig_reached_10 = Signal<Counter, void (void)>::create ("reached-10");
}

void
Counter::add (unsigned addend)
{
  // If nothing is being changed, don't emit "notify" or "reached-10".
  if (addend == 0)
    return;

  bool was_below_10 = count < 10;

  count += addend;
  notify (prop_count ());

  if (count >= 10 && was_below_10)
    sig_reached_10.emit (this);
}

static void
on_reached_10 (Counter *c)
{
  g_print ("Count reached 10! (value is %d)\n", c->get_count ());
}

int
main ()
{
  RefPtr<Counter> counter = Object::create<Counter> ();

  // You can use C++ lambdas with arbitrary captures as signal handlers.
  // (Just be careful about object lifetimes and reference cycles.)
  counter->connect_notify (Counter::prop_count (),
    [] (Object *c, peel::GObject::ParamSpec *pspec)
    {
      unsigned count = c->cast<Counter> ()->get_count ();
      g_print ("Count changed to %d\n", count);
    });
  // Since plain C functions are callable, they too can serve as signal handlers.
  counter->connect_reached_10 (on_reached_10);

  counter->add (4);
  counter->add (0);
  counter->add (7);

  // In addition to our explicit getter, we can get the value of the property
  // via the generic property API. This is slower, so prefer explicit accessors
  // when possible. It will end up calling out getter under the hood.
  unsigned count = counter->get_property (Counter::prop_count ());
  g_print ("Count via get_property (): %d\n", count);
}
