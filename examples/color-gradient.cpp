#include <peel/Gtk/Gtk.h>
#include <peel/Gdk/RGBA.h>
#include <peel/Graphene/Rect.h>
#include <peel/Gsk/ColorStop.h>
#include <peel/GLib/MainContext.h>
#include <peel/GObject/Binding.h>
#include <peel/class.h>

using namespace peel;

namespace Demo
{

class Gradient : public Gtk::Widget
{
  PEEL_SIMPLE_CLASS (Gradient, Gtk::Widget)
  friend class Gtk::Widget;

  void
  init (Class *)
  {
  }

  template<typename F>
  static void
  define_properties (F &f)
  {
    f.prop (prop_start_color ())
      .get (&Gradient::get_start_color)
      .set (&Gradient::set_start_color);
    f.prop (prop_end_color ())
      .get (&Gradient::get_end_color)
      .set (&Gradient::set_end_color);
  }

  inline void
  vfunc_snapshot (Gtk::Snapshot *);

  Gdk::RGBA start_color;
  Gdk::RGBA end_color;

public:
  Gdk::RGBA *
  get_start_color ()
  {
    return &start_color;
  }

  void
  set_start_color (Gdk::RGBA *);

  PEEL_PROPERTY (Gdk::RGBA, start_color, "start-color")

  Gdk::RGBA *
  get_end_color ()
  {
    return &end_color;
  }

  void
  set_end_color (Gdk::RGBA *);

  PEEL_PROPERTY (Gdk::RGBA, end_color, "end-color")
};

PEEL_CLASS_IMPL (Gradient, "DemoGradient", Gtk::Widget)

inline void
Gradient::Class::init ()
{
  override_vfunc_snapshot<Gradient> ();
  set_css_name ("gradient");
}

inline void
Gradient::vfunc_snapshot (Gtk::Snapshot *snapshot)
{
  Graphene::Rect bounds;
  bool ok = compute_bounds (this, &bounds);
  g_assert (ok);
  Graphene::Point top_left, top_right;
  bounds.get_top_left (&top_left);
  bounds.get_top_right (&top_right);

  Gsk::ColorStop color_stops[]
  {
    { 0.0, start_color },
    { 1.0, end_color }
  };

  snapshot->append_linear_gradient (
    &bounds,
    &top_left,
    &top_right,
    color_stops);
}

void
Gradient::set_start_color (Gdk::RGBA *color)
{
  g_return_if_fail (color != nullptr);
  if (start_color.equal (color))
    return;
  start_color = *color;
  queue_draw ();
  notify (prop_start_color ());
}

void
Gradient::set_end_color (Gdk::RGBA *color)
{
  g_return_if_fail (color != nullptr);
  if (end_color.equal (color))
    return;
  end_color = *color;
  queue_draw ();
  notify (prop_end_color ());
}

}

int
main ()
{
  using BindingFlags = peel::GObject::Binding::Flags;

  Gtk::init ();

  FloatPtr<Demo::Gradient> gradient = Object::create<Demo::Gradient> ();
  RefPtr<Gtk::ColorDialog> dialog = Gtk::ColorDialog::create ();
  Gdk::RGBA color;

  FloatPtr<Gtk::ColorDialogButton> start_button = Gtk::ColorDialogButton::create (dialog);
  color.parse ("red");
  start_button->set_rgba (&color);
  Object::bind_property (
    start_button, Gtk::ColorDialogButton::prop_rgba (),
    gradient, Demo::Gradient::prop_start_color (),
    BindingFlags::SYNC_CREATE | BindingFlags::BIDIRECTIONAL);

  FloatPtr<Gtk::ColorDialogButton> end_button = Gtk::ColorDialogButton::create (dialog);
  color.parse ("blue");
  end_button->set_rgba (&color);
  Object::bind_property (
    end_button, Gtk::ColorDialogButton::prop_rgba (),
    gradient, Demo::Gradient::prop_end_color (),
    BindingFlags::SYNC_CREATE | BindingFlags::BIDIRECTIONAL);

  FloatPtr<Gtk::HeaderBar> header_bar = Gtk::HeaderBar::create ();
  header_bar->pack_start (std::move (start_button));
  header_bar->pack_end (std::move (end_button));

  bool should_stop = false;
  Gtk::Window *window = Gtk::Window::create ();
  window->connect_destroy (
    [&should_stop] (Gtk::Widget *)
    {
      should_stop = true;
    });
  window->set_child (std::move (gradient));
  window->set_titlebar (std::move (header_bar));
  window->present ();

  GLib::MainContext *main_context = GLib::MainContext::default_ ();
  while (!should_stop)
    main_context->iteration (true);
}
