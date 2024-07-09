#include <peel/Gtk/Gtk.h>
#include <peel/GLib/MainLoop.h>

using namespace peel;

int
main ()
{
  Gtk::init ();

  Gtk::Window *window = Gtk::Window::create ();
  window->present ();

  RefPtr<GLib::MainLoop> loop = GLib::MainLoop::create (nullptr, false);
  loop->run ();
}
