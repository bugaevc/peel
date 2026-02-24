

#include "AvahiServer.h"
#include "AvahiServiceBrowser.h"
#include <peel/Gio/BusType.h>
#include <peel/Gio/ListModel.h>
#include <peel/GObject/ParamFlags.h>
#include <peel/class.h>

using namespace peel;

namespace Demo
{

class Service final : public Object
{
  PEEL_SIMPLE_CLASS (Service, Object)

  int interface;
  int protocol;
  String name;
  String type;
  String domain;
  unsigned flags;
};

class Browser final : public Gio::ListModel
{
  PEEL_SIMPLE_CLASS (Browser, Object)
  friend class Gio::ListModel::Iface;

  static void
  init_type (Type tp)
  {
    PEEL_IMPLEMENT_INTERFACE (tp, Gio::ListModel);
  }

  static void
  init_interface (Gio::ListModel::Iface *iface)
  {
    iface->override_vfunc_get_item_type<Browser> ();
  }

  RefPtr<Avahi::ServiceBrowser> avahi_browser;

  /* construct-only setter */
  void
  set_avahi_browser (Avahi::ServiceBrowser *avahi_browser)
  {
    g_return_if_fail (avahi_browser->check_type<Avahi::ServiceBrowser> ());
    g_assert (this->avahi_browser == nullptr);

    this->avahi_browser = avahi_browser;
    avahi_browser->connect_item_new (this, &Browser::on_item_new);
  }

  template<typename F>
  static void
  define_properties (F &f)
  {
    f.prop (prop_avahi_browser ())
      .get (&Browser::get_avahi_browser)
      .set (&Browser::set_avahi_browser)
      .flags (peel::GObject::ParamFlags::CONSTRUCT_ONLY);
  }

  void
  on_item_new (Avahi::ServiceBrowser *avahi_browser, int interface, int protocol, const char *name, const char *type, const char *domain, unsigned int flags)
  {
    g_print ("On item new\n");
  }

  Type
  vfunc_get_item_type ()
  {
    return Type::of<Service> ();
  }

  void
  vfunc_dispose ()
  {
    avahi_browser = nullptr;
    parent_vfunc_dispose<Browser> ();
  }

public:
  Avahi::ServiceBrowser *
  get_avahi_browser ()
  {
    return avahi_browser;
  }

  PEEL_PROPERTY (Avahi::ServiceBrowser, avahi_browser, "avahi-browser")

  static RefPtr<Browser>
  create (RefPtr<Avahi::ServiceBrowser> avahi_browser)
  {
    g_return_val_if_fail (avahi_browser, nullptr);
    g_return_val_if_fail (avahi_browser->check_type<Avahi::ServiceBrowser> (), nullptr);

    return Object::create<Browser> (prop_avahi_browser (), std::move (avahi_browser));
  }
};

PEEL_CLASS_IMPL (Browser, "DemoBrowser", Object)

inline void
Browser::Class::init ()
{
  override_vfunc_dispose<Browser> ();
}

PEEL_CLASS_IMPL (Service, "DemoService", Object)

inline void
Service::Class::init ()
{ }

} /* namespace Demo */

int
main ()
{
  UniquePtr<GLib::Error> error;

  RefPtr<Demo::Avahi::Server2> avahi_server = Demo::Avahi::Server2::Proxy::create_sync (
    Gio::BusType::SYSTEM, "org.freedesktop.Avahi", "/", &error);
  if (error)
    {
      g_printerr ("Failed to connect to Avahi: %s", error->message);
      return 1;
    }

  String version;
  bool ok = avahi_server->get_version_string_sync (&version, &error);
  if (!ok)
    {
      g_printerr ("Failed to get server version: %s", error->message);
      return 1;
    }
  g_print ("Avahi server version: %s\n", (const char *) version);

  String service_browser_path;
  ok = avahi_server->service_browser_prepare_sync (-1, -1, "", "", 0, &service_browser_path, &error);
  if (!ok)
    {
      g_printerr ("Failed to prepare a service browser: %s", error->message);
      return 1;
    }
  g_debug ("Service browser: %s", (const char *) service_browser_path);
  RefPtr<Demo::Avahi::ServiceBrowser> service_browser = Demo::Avahi::ServiceBrowser::Proxy::create_sync (
    Gio::BusType::SYSTEM, "org.freedesktop.Avahi", service_browser_path, &error);
  if (error)
    {
      g_printerr ("Failed to create service browser proxy: %s", error->message);
      return 1;
    }

  RefPtr<Demo::Browser> browser = Demo::Browser::create (std::move (service_browser));
}
