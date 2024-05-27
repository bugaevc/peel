#include <peel/Gtk/Gtk.h>
#include <peel/GLib/MainContext.h>

using namespace peel;

int
main ()
{
  Gtk::init ();

  Gtk::Window *window = Gtk::Window::create ();
  window->set_title ("Hello, world!");
  window->present ();

  // Spin the event loop.
  GLib::MainContext *context = GLib::MainContext::default_ ();
  while (true)
    context->iteration (true);
}
