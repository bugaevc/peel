/*
 * A small clone of timedatectl(1), using the D-Bus API of systemd-timedated.
 *
 * https://www.freedesktop.org/software/systemd/man/latest/org.freedesktop.timedate1.html
 *
 * $ peel-dbus-gen \
 *     --interface-prefix org.freedesktop \
 *     --namespace Demo \
 *     /usr/share/dbus-1/interfaces/org.freedesktop.timedate1.xml \
 *     TimeDate.h TimeDate.cpp
 */

#include "TimeDate.h"
#include <peel/Gio/BusType.h>
#include <peel/Gio/DBusError.h>
#include <peel/GLib/DateTime.h>
#include <peel/GLib/TimeZone.h>
#include <peel/GLib/functions.h>

using namespace peel;

static void
do_status (Demo::timedate1 *time_date)
{
  uint64_t usec = time_date->get_time_usec ();
  RefPtr<GLib::DateTime> local_date_time = GLib::DateTime::create_from_unix_local_usec (usec);
  RefPtr<GLib::DateTime> utc_date_time = local_date_time->to_utc ();
  uint64_t rtc_usec = time_date->get_rtc_time_usec ();
  bool rtc_local = time_date->get_local_rtc ();
  RefPtr<GLib::DateTime> rtc_date_time = rtc_local ?
    GLib::DateTime::create_from_unix_local_usec (rtc_usec) :
    GLib::DateTime::create_from_unix_utc_usec (rtc_usec);

  const char *ntp_status;
  if (time_date->get_can_ntp ())
    ntp_status = time_date->get_ntp () ? "active" : "inactive";
  else
    ntp_status = "n/a";

  g_print ("               Local time: %s\n", (const char *) local_date_time->format ("%a %F %H:%M:%S %Z"));
  g_print ("           Universal time: %s\n", (const char *) utc_date_time->format ("%a %F %H:%M:%S %Z"));
  g_print ("                 RTC time: %s\n", (const char *) utc_date_time->format ("%a %F %H:%M:%S"));
  g_print ("                Time zone: %s\n", (const char *) time_date->get_timezone ());
  g_print ("System clock synchronized: %s\n", time_date->get_ntp_synchronized () ? "yes" : "no");
  g_print ("              NTP service: %s\n", ntp_status);
  g_print ("          RTC in local TZ: %s\n", rtc_local ? "yes" : "no");
}

static bool
do_set_time (Demo::timedate1 *time_date, const char *arg, UniquePtr<GLib::Error> *error)
{
  if (!arg)
    {
      GLib::set_error_literal (error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE,
        "No time specified");
      return false;
    }

  RefPtr<GLib::TimeZone> local_tz = GLib::TimeZone::create_local ();
  RefPtr<GLib::DateTime> date_time = GLib::DateTime::create_from_iso8601 (arg, local_tz);
  if (!date_time)
    {
      GLib::set_error (error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE,
        "Failed to parse time '%s'", arg);
      return false;
    }

  return time_date->set_time_sync (date_time->to_unix_usec (),
    false /* relative */, true /* interactive */, error,
    nullptr, Gio::DBusCallFlags::ALLOW_INTERACTIVE_AUTHORIZATION);
}

static bool
do_set_timezone (Demo::timedate1 *time_date, const char *arg, UniquePtr<GLib::Error> *error)
{
  if (!arg)
    {
      GLib::set_error_literal (error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE,
        "No timezone specified");
      return false;
    }

  return time_date->set_timezone_sync (arg, true /* interactive */, error,
    nullptr, Gio::DBusCallFlags::ALLOW_INTERACTIVE_AUTHORIZATION);
}

static bool
do_list_timezones (Demo::timedate1 *time_date, UniquePtr<GLib::Error> *error)
{
  Strv timezones;
  bool ok = time_date->list_timezones_sync (&timezones, error);
  if (!ok)
    return false;

  for (String &timezone : timezones)
    g_print ("%s\n", (const char *) timezone);
  return true;
}

static bool
parse_boolean_argument (const char *arg, bool *value, UniquePtr<GLib::Error> *error)
{
  if (!g_strcmp0 (arg, "yes") || !g_strcmp0 (arg, "1") || !g_strcmp0 (arg, "true"))
    {
      *value = true;
      return true;
    }
  else if (!g_strcmp0 (arg, "no") || !g_strcmp0 (arg, "0") || !g_strcmp0 (arg, "false"))
    {
      *value = false;
      return true;
    }
  else
    {
      GLib::set_error (error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE,
        "Failed to parse boolean argument '%s'", arg);
      return false;
    }
}

static bool
do_set_local_rtc (Demo::timedate1 *time_date, const char *arg, UniquePtr<GLib::Error> *error)
{
  bool local;
  if (!parse_boolean_argument (arg, &local, error))
    return false;
  return time_date->set_local_rtc_sync (local,
    false /* fix_system */, true /* interactive */, error,
    nullptr, Gio::DBusCallFlags::ALLOW_INTERACTIVE_AUTHORIZATION);
}

static bool
do_set_ntp (Demo::timedate1 *time_date, const char *arg, UniquePtr<GLib::Error> *error)
{
  bool use_ntp;
  if (!parse_boolean_argument (arg, &use_ntp, error))
    return false;
  return time_date->set_ntp_sync (use_ntp, true /* interactive */, error,
    nullptr, Gio::DBusCallFlags::ALLOW_INTERACTIVE_AUTHORIZATION);
}

int
main (int argc, char *argv[])
{
  UniquePtr<GLib::Error> error;
  const char *subcommand;

  if (argc == 1)
    subcommand = "status";
  else
    subcommand = argv[1];

  RefPtr<Demo::timedate1> time_date = Demo::timedate1::Proxy::create_sync (Gio::BusType::SYSTEM,
    "org.freedesktop.timedate1", "/org/freedesktop/timedate1", &error);
  if (error)
    {
      g_printerr ("Failed to connect to timedate daemon: %s\n", error->message);
      return 1;
    }

  if (!strcmp (subcommand, "status"))
    do_status (time_date);
  else if (!strcmp (subcommand, "set-time"))
    do_set_time (time_date, argv[2], &error);
  else if (!strcmp (subcommand, "set-timezone"))
    do_set_timezone (time_date, argv[2], &error);
  else if (!strcmp (subcommand, "list-timezones"))
    do_list_timezones (time_date, &error);
  else if (!strcmp (subcommand, "set-local-rtc"))
    do_set_local_rtc (time_date, argv[2], &error);
  else if (!strcmp (subcommand, "set-ntp"))
    do_set_ntp (time_date, argv[2], &error);
  else
    {
      g_printerr ("Unimplemented subcommand\n");
      return 1;
    }

  if (error)
    {
      if (Gio::dbus_error_is_remote_error (error))
        {
          g_printerr ("D-Bus error: %s\n", (const char *) Gio::dbus_error_get_remote_error (error));
          Gio::dbus_error_strip_remote_error (error);
        }
      g_printerr ("%s\n", error->message);
      return 1;
    }
}
