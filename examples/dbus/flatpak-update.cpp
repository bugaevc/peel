/*
 * Watch for Flatpak updates from inside a Flatpak application,
 * and initiate an update when one is available.
 *
 * https://blogs.gnome.org/mclasen/2019/12/19/9100/
 * https://docs.flatpak.org/en/latest/libflatpak-api-reference.html#gdbus-org.freedesktop.portal.Flatpak.UpdateMonitor
 */

#include "Flatpak.h"
#include <peel/Gio/BusType.h>
#include <peel/GLib/MainContext.h>
#include <peel/GLib/VariantType.h>

using namespace peel;

int
main ()
{
  UniquePtr<GLib::Error> error;

  /* For simplicity, the code below uses the sync (blocking) calls.
   * In a real GUI application, you would want to use async versions instead.
   */

  RefPtr<Flatpak> flatpak_portal = Flatpak::Proxy::create_sync (Gio::BusType::SESSION,
    "org.freedesktop.portal.Flatpak", "/org/freedesktop/portal/Flatpak", &error);
  if (error)
    {
      g_printerr ("Failed to connect to Flatpak portal: %s", error->message);
      return 1;
    }

  /* Check for FLATPAK_SPAWN_SUPPORT_FLAGS_EXPOSE_PIDS.
   * This is unrelated to update monitoring, and just showcases properties.
   */
  if (flatpak_portal->get_version () >= 3)
    {
      bool supports = flatpak_portal->get_supports () & 1;
      g_print ("Flatpak portal %s EXPOSE_PIDS\n", supports ? "supports" : "does not support");
    }

  /* Construct an empty a{sv} that we'll use for options */
  UniquePtr<GLib::Variant::Type> vardict_entry_type = GLib::Variant::Type::create ("{sv}");
  RefPtr<GLib::Variant> empty_vardict = GLib::Variant::create_array (vardict_entry_type, nullptr);

  String monitor_object_path;
  flatpak_portal->create_update_monitor_sync (empty_vardict /* options */, &monitor_object_path, &error);
  if (error)
    {
      g_printerr ("Failed to create Flatpak update monitor: %s", error->message);
      return 1;
    }
  RefPtr<UpdateMonitor> update_monitor = UpdateMonitor::Proxy::create_sync (
    /* connect directly to that name owner on that same connection, and not the well-known name */
    flatpak_portal->cast<Gio::DBusProxy> ()->get_connection (),
    flatpak_portal->cast<Gio::DBusProxy> ()->get_name_owner (),
    monitor_object_path, &error,
    Gio::DBusProxy::Flags::DO_NOT_LOAD_PROPERTIES /* no properties on the interface */);
  if (error)
    {
      g_printerr ("Failed to create Flatpak update monitor proxy: %s", error->message);
      return 1;
    }

  update_monitor->connect_update_available ([empty_vardict] (UpdateMonitor *update_monitor, GLib::Variant *update_info)
    {
      /* TODO: print update info */
      g_print ("Update is available, starting an update\n");
      update_monitor->update_async ("" /* parent window handle */, empty_vardict /* options */,
        [] (Object *source_object, Gio::AsyncResult *res)
        {
          UpdateMonitor *update_monitor = source_object->cast<UpdateMonitor> ();
          UniquePtr<GLib::Error> error;
          update_monitor->update_finish (res, &error);
          if (error)
            g_printerr ("Failed to start an update: %s", error->message);
          else
            g_print ("Update started\n");
        });
    });

  update_monitor->connect_progress ([] (UpdateMonitor *update_monitor, GLib::Variant *info)
    {
      /* TODO: print info */
      g_print ("Update progress\n");
    });

  /* Spin the loop, waiting for updates */
  GLib::MainContext *context = GLib::MainContext::default_ ();
  while (true)
    context->iteration (true);
}
