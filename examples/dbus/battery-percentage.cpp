/*
 * Display battery device state and charge percentage via UPower,
 * using its D-Bus API directly (and not through libupower-glib).
 *
 * https://upower.freedesktop.org/docs/ref-dbus.html
 *
 * This example primarily showcases binding D-Bus object properties
 * directly to GTK UI elements, using GObject property binding.
 */

#include "UPower.h"
#include <peel/Gio/BusType.h>
#include <peel/Gtk/Gtk.h>
#include <peel/GObject/Binding.h>
#include <peel/GLib/MainContext.h>

using namespace peel;

enum class DeviceState : unsigned
{
  UNKNOWN,
  CHARGING,
  DISCHARGING,
  EMPTY,
  FULLY_CHARGED,
  PENDING_CHARGE,
  PENDING_DISCHARGE
};

static const char *
device_state_to_string (DeviceState state)
{
  switch (state)
    {
    case DeviceState::UNKNOWN:
      return "unknown";
    case DeviceState::CHARGING:
      return "charging";
    case DeviceState::DISCHARGING:
      return "discharging";
    case DeviceState::EMPTY:
      return "empty";
    case DeviceState::FULLY_CHARGED:
      return "fully charged";
    case DeviceState::PENDING_CHARGE:
      return "pending charge";
    case DeviceState::PENDING_DISCHARGE:
      return "pending discharge";
    default:
      return "???";
    }
}

int
main ()
{
  UniquePtr<GLib::Error> error;

  Gtk::init ();

  /* Connect to UPower's "display device" */
  RefPtr<Device> battery = Device::Proxy::create_sync (Gio::BusType::SYSTEM,
    "org.freedesktop.UPower", "/org/freedesktop/UPower/devices/DisplayDevice",
    &error);
  if (error)
    {
      g_printerr ("Failed to connect to UPower: %s", error->message);
      return 1;
    }

  Gtk::Window *window = Gtk::Window::create ();
  window->set_title ("Battery");

  FloatPtr<Gtk::Grid> grid = Gtk::Grid::create ();
  grid->set_column_spacing (12);
  grid->set_row_spacing (12);
  FloatPtr<Gtk::Label> label;

  label = Gtk::Label::create (nullptr);
  label->set_markup ("<b>State</b>:");
  label->set_xalign (1.0);
  grid->attach (std::move (label), 0, 0, 1, 1);
  label = Gtk::Label::create (nullptr);
  label->set_markup ("<b>Percentage</b>:");
  label->set_xalign (1.0);
  grid->attach (std::move (label), 0, 1, 1, 1);

  /* Bind the UPower device's "state" property to a label's text,
   * converting it to a string representation.
   */
  label = Gtk::Label::create (nullptr);
  label->set_xalign (0.0);
  Object::bind_property (
    battery, Device::prop_state (),
    label, Gtk::Label::prop_label (),
    peel::GObject::Binding::Flags::SYNC_CREATE,
    [] (unsigned state) -> const char *
    {
      return device_state_to_string ((DeviceState) state);
    });
  grid->attach (std::move (label), 1, 0, 1, 1);

  /* Bind the UPower device's "percentage" property to a label's text.
   * Note that property binding can automatically convert the double
   * value to a string.
   */
  label = Gtk::Label::create (nullptr);
  label->set_xalign (0.0);
  Object::bind_property (
    battery, Device::prop_percentage (),
    label, Gtk::Label::prop_label (),
    peel::GObject::Binding::Flags::SYNC_CREATE);
  grid->attach (std::move (label), 1, 1, 1, 1);

  window->set_child (std::move (grid));
  window->present ();

  GLib::MainContext *context = GLib::MainContext::default_ ();
  while (true)
    context->iteration (true);
}
