#include <peel/Gtk/Gtk.h>
#include <peel/Gdk/RGBA.h>
#include <peel/Pango/Layout.h>
#include <peel/GLib/MainContext.h>
#include <peel/String.h>
#include <peel/class.h>

using namespace peel;

namespace Demo
{

class Label final : public Gtk::Widget
{
  PEEL_SIMPLE_CLASS (Label, Gtk::Widget)
  friend class Gtk::Widget;

  String text;
  RefPtr<Pango::Layout> layout;

  template<typename F>
  static void
  define_properties (F &f)
  {
    f.prop (prop_text (), nullptr)
      .get (&Label::get_text)
      .set (&Label::set_text);
  }

  inline void
  vfunc_measure (Gtk::Orientation orienatation, int for_size,
                 int *minimum, int *natural,
                 int *minimum_baseline, int *natural_baseline);

  inline void
  vfunc_snapshot (Gtk::Snapshot *);

  inline void
  vfunc_unroot ();

public:
  static FloatPtr<Label>
  create (const char *text)
  {
    return Object::create<Label> (prop_text (), text);
  }

  const char *
  get_text ()
  {
    return text;
  }

  void
  set_text (const char *);

  PEEL_PROPERTY (const char *, text, "text")
};

PEEL_CLASS_IMPL (Label, "DemoLabel", Gtk::Widget)

inline void
Label::Class::init ()
{
  set_css_name ("label");
  set_accessible_role (Gtk::Accessible::Role::LABEL);

  override_vfunc_measure<Label> ();
  override_vfunc_snapshot<Label> ();
  override_vfunc_unroot<Label> ();
}

void
Label::set_text (const char *new_text)
{
  if (!text.set (new_text))
    return;

  if (layout)
    layout->set_text (new_text, -1);

  notify (prop_text ());
  queue_resize ();
}

inline void
Label::vfunc_measure (Gtk::Orientation orienatation, int for_size,
                      int *minimum, int *natural,
                      int *minimum_baseline, int *natural_baseline)
{
  if (!layout)
    layout = create_pango_layout (text);

  int width, height;
  layout->get_pixel_size (&width, &height);
  int baseline = PANGO_PIXELS_CEIL (layout->get_baseline ());

  if (orienatation == Gtk::Orientation::HORIZONTAL)
    {
      *minimum = *natural = width;
      *minimum_baseline = *natural_baseline = -1;
    }
  else
    {
      *minimum = *natural = height;
      *minimum_baseline = *natural_baseline = baseline;
    }
}

inline void
Label::vfunc_snapshot (Gtk::Snapshot *snapshot)
{
  Gdk::RGBA text_color;
#if GTK_CHECK_VERSION (4, 10, 0)
  get_color (&text_color);
#else
  get_style_context ()->get_color (&text_color);
#endif
  snapshot->append_layout (layout, &text_color);
}

inline void
Label::vfunc_unroot ()
{
  parent_vfunc_unroot<Label> ();
  layout = nullptr;
}

}

int
main ()
{
  Gtk::init ();

  FloatPtr<Demo::Label> label = Demo::Label::create (
    "This is a simple label widget,\n"
    "implemented in C++ using peel :)\n"
    "You can alter this text using the GTK Inspector");

  bool should_stop = false;
  Gtk::Window *window = Gtk::Window::create ();
  window->connect_destroy (
    [&should_stop] (Gtk::Widget *)
    {
      should_stop = true;
    });
  window->set_child (std::move (label));
  window->present ();

  GLib::MainContext *main_context = GLib::MainContext::default_ ();
  while (!should_stop)
    main_context->iteration (true);
}
