/*
 * Query and watch the dark style preference via the settings portal.
 *
 * https://flatpak.github.io/xdg-desktop-portal/docs/doc-org.freedesktop.portal.Settings.html
 */

#include "Settings.h"
#include <peel/Gio/BusType.h>
#include <peel/GLib/MainContext.h>

using namespace peel;

static const char *
decode_preference (GLib::Variant *value)
{
  switch (value->get<uint32_t> ())
    {
    case 0:
      return "default";
    case 1:
      return "prefer-dark";
    case 2:
      return "prefer-light";
    default:
      return "???";
    }
}

int
main ()
{
  UniquePtr<GLib::Error> error;

  RefPtr<Settings> settings_portal = Settings::Proxy::create_sync (Gio::BusType::SESSION,
    "org.freedesktop.portal.Desktop", "/org/freedesktop/portal/desktop", &error);
  if (error)
    {
      g_printerr ("Failed to create portal proxy: %s", error->message);
      return 1;
    }

  g_print ("Portal version: %u\n", settings_portal->get_version ());

  /* When the setting changes, print the new value */
  settings_portal->connect_setting_changed ([] (Settings *, const char *ns, const char *setting, GLib::Variant *value)
    {
      g_print ("Setting changed: %s %s\n", ns, setting);
      if (!strcmp (ns, "org.freedesktop.appearance") && !strcmp (setting, "color-scheme"))
        g_print ("Color scheme now: %s\n", decode_preference (value));
    });

  /* Fetch the initial value straight away */
  RefPtr<GLib::Variant> value;
  settings_portal->read_one_sync ("org.freedesktop.appearance", "color-scheme", &value, &error);
  if (error)
    {
      g_printerr ("Failed to read setting: %s", error->message);
      return 1;
    }
  g_print ("Color scheme at startup: %s\n", decode_preference (value));

  /* Loop, waiting for setting changes */
  GLib::MainContext *context = GLib::MainContext::default_ ();
  while (true)
    context->iteration (true);
}
