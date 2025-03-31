#include <peel/Gtk/Gtk.h>
#include <peel/Gdk/Gdk.h>
#include <peel/Gsk/Gsk.h>
#include <peel/Gio/Gio.h>
#include <peel/GLib/String.h>

using namespace peel;

static void
print_type (Type type, GLib::String *prefix, bool last_child)
{
  if (!type.is_fundamental ())
    g_print ("%s%s", prefix->str, last_child ? "└── " : "├── ");
  else
    g_assert (prefix->len == 0);

  g_print ("%s", type.name ());
  if (type.is_abstract ())
    g_print (" (abstract)");
  auto ifaces = type.interfaces ();
  if (ifaces)
    {
      bool first = true;
      g_print (" implements ");
      for (Type iface : ifaces)
        {
          g_print ("%s%s", first ? "" : ", ", iface.name ());
          first = false;
        }
    }
  g_print ("\n");

  auto prefix_len = prefix->len;
  prefix->append (last_child ? "    " : "│   ");

  auto children = type.children ();
  for (size_t i = 0; i < children.size (); i++)
    {
      last_child = i == children.size () - 1;
      print_type (children[i], prefix, last_child);
    }
  prefix->erase (prefix_len, -1);
}

int
main ()
{
  /* Initialize some types */
  Gtk::init ();
  Type::of<Gdk::Toplevel::Size> ().ensure ();
  Type::of<Gdk::DragSurface::Size> ().ensure ();
  Type::of<Value> ().ensure ();
  (void) Gtk::HeaderBar::create ();
  {
    auto file = Gio::File::create_for_path ("/");
    file->monitor_directory (Gio::File::MonitorFlags::NONE, nullptr, nullptr);
  }

  Type root_types[]
  {
    Type::of<Gsk::RenderNode> (),
    Type::of<Gdk::Event> (),
    Type::of<peel::GObject::ParamSpec> (),
    Type::of<Object> (),
    Type::boxed (),
    Type::interface_ (),
    Type::of<void *> ()
  };

  UniquePtr<GLib::String> prefix = GLib::String::create (nullptr);
  for (Type type : root_types)
    print_type (type, prefix, true);
}
