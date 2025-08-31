#include <peel/Gtk/Gtk.h>
#include <peel/GObject/Binding.h>
#include <peel/GLib/MainContext.h>

using namespace peel;

int
main ()
{
  Gtk::init ();

  FloatPtr<Gtk::Box> box = Gtk::Box::create (Gtk::Orientation::VERTICAL, 6);
  box->set_margin_start (6);
  box->set_margin_end (6);
  box->set_margin_top (6);
  box->set_margin_bottom (6);

  FloatPtr<Gtk::Entry> entry1 = Gtk::Entry::create ();
  FloatPtr<Gtk::Entry> entry2 = Gtk::Entry::create ();

  Object::bind_property (
    entry1, Gtk::Editable::prop_text (),
    entry2, Gtk::Editable::prop_text (),
    peel::GObject::Binding::Flags::BIDIRECTIONAL,
    /* transform-to function: we add 1 to each character */
    [] (const char *s) -> String
    {
      char *r = g_strdup (s);
      for (char *c = r; *c; c++)
        ++*c;
      return String::adopt_string (r);
    },
    /* transform-from function: we subtract 1 from each character */
    [] (const char *s) -> String
    {
      char *r = g_strdup (s);
      for (char *c = r; *c; c++)
        --*c;
      return String::adopt_string (r);
    });

  box->append (std::move (entry1));
  box->append (std::move (entry2));

  Gtk::Window *window = Gtk::Window::create ();
  window->set_title ("Property binding");
  bool should_stop = false;
  window->connect_destroy (
    [&should_stop] (Gtk::Widget *)
    {
      should_stop = true;
    });
  window->set_child (std::move (box));
  window->present ();

  GLib::MainContext *main_context = GLib::MainContext::default_ ();
  while (!should_stop)
    main_context->iteration (true);
}
