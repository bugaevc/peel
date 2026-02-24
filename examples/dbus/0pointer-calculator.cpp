#include "Calculator.h"
#include <peel/Gio/BusType.h>
#include <peel/Gio/DBusConnection.h>
#include <peel/Gio/DBusError.h>
#include <peel/Gio/functions.h>
#include <peel/GLib/MainContext.h>
#include <peel/GLib/functions.h>
#include <peel/class.h>

using namespace peel;

namespace Demo
{

enum CalculatorError
{
  DIVISION_BY_ZERO
};

GLib::Quark
calculator_error_quark ()
{
  const char *q = "calculator-error-quark";
  Gio::dbus_error_register_error (q, DIVISION_BY_ZERO, "net.poettering.DivisionByZero");
  return q;
}

#define CALCULATOR_ERROR (calculator_error_quark ())

class CalculatorImpl final : public Calculator::Skeleton
{
  PEEL_SIMPLE_CLASS (CalculatorImpl, Calculator::Skeleton)
  friend class Calculator::Iface;

  static void
  init_type (Type tp)
  {
    PEEL_IMPLEMENT_INTERFACE (tp, Calculator);
  }

  static void
  init_interface (Calculator::Iface *iface)
  {
    iface->override_vfunc_multiply_sync<CalculatorImpl> ();
    iface->override_vfunc_divide_sync<CalculatorImpl> ();
  }

  bool
  vfunc_multiply_sync (int64_t x, int64_t y, int64_t *res, UniquePtr<GLib::Error> *error, Gio::Cancellable *, Gio::DBusCallFlags, int timeout)
  {
    *res = x * y;
    return true;
  }

  bool
  vfunc_divide_sync (int64_t x, int64_t y, int64_t *res, UniquePtr<GLib::Error> *error, Gio::Cancellable *, Gio::DBusCallFlags, int timeout)
  {
    if (y == 0)
      {
        GLib::set_error_literal (error, CALCULATOR_ERROR, DIVISION_BY_ZERO, "Sorry, can't allow division by zero.");
        return false;
      }

    *res = x / y;
    return true;
  }
};

PEEL_CLASS_IMPL (CalculatorImpl, "DemoCalculatorImpl", Calculator::Skeleton)

inline void
CalculatorImpl::Class::init ()
{ }

} /* namespace Demo */

int
main ()
{
  UniquePtr<GLib::Error> error;
  RefPtr<Gio::DBusConnection> connection = Gio::bus_get_sync (Gio::BusType::SESSION, nullptr, &error);
  RefPtr<Calculator> calculator = Object::create<Demo::CalculatorImpl> ().cast<Calculator> ();
  calculator->export_on_bus (connection, "/net/poettering/Calculator", &error);
  if (G_UNLIKELY (error))
    {
      g_printerr ("Failed to export object on bus: %s\n", error->message);
    }

  /* Spin the loop */
  GLib::MainContext *context = GLib::MainContext::default_ ();
  while (true)
    context->iteration (true);
}
