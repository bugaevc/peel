#include <peel/Gtk/Gtk.h>

#include <gtkmm.h>
#include <gtkmm/init.h>
#include <glibmm.h>

template<typename peel_type, typename gtkmm_type>
static peel_type *
cast_to_peel (gtkmm_type &gtkmm_obj)
{
  // Cast from gtkmm wrapper to peel:
  // 1. Call .gobj () to get the C GObject pointer.
  // 2. Cast it to peel type using reinterpret_cast<>.
  return reinterpret_cast<peel_type *> (gtkmm_obj.gobj ());
}

template<typename gtkmm_type, typename peel_type>
static gtkmm_type *
cast_to_gtkmm (peel_type *peel_obj)
{
  // Cast from peel to gtkmm:
  // 1. Cast to a C GObject pointer, using G_OBJECT () macro or reinterpret_cast<>.
  // 2. Call Glib::wrap_auto ().
  // 3. Cast to the correct gtkmm subtype using dynamic_cast<>.
  Glib::ObjectBase *gtkmm_obj = Glib::wrap_auto (G_OBJECT (peel_obj));
  return dynamic_cast<gtkmm_type *> (gtkmm_obj);
}

int
main ()
{
  // Initialize GTK, using peel.
  peel::Gtk::init ();
  // Initialize gtkmm.
  Gtk::init_gtkmm_internals ();

  // Make a gtkmm window.
  Gtk::Window gtkmm_window;
  gtkmm_window.set_title ("A gtkmm window");

  peel::Gtk::Window *peel_window = cast_to_peel<peel::Gtk::Window> (gtkmm_window);

  // Connect a signal handler to the gtkmm-created window
  // using peel API.
  bool should_stop = false;
  peel_window->connect_close_request (
    [&should_stop] (peel::Gtk::Window *) -> bool
    {
      should_stop = true;
      return false;
    });

  // Make a peel button.
  peel::FloatPtr<peel::Gtk::Button> peel_button = peel::Gtk::Button::create_with_label ("A peel button");

  Gtk::Button *gtkmm_button = cast_to_gtkmm<Gtk::Button> ((peel::Gtk::Button *) peel_button);
  // Connect a signal handler to the peel-created button
  // using gtkmm API; also set it as the window's child.
  gtkmm_button->signal_clicked ().connect (
    [] ()
    {
      g_print ("Clicked\n");
    });
  gtkmm_window.set_child (*gtkmm_button);

  peel_window->present ();

  // Spin the main context using gtkmm.
  Glib::RefPtr<Glib::MainContext> context = Glib::MainContext::get_default ();
  while (!should_stop)
    context->iteration (true);
}
