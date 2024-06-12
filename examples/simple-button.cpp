#include <peel/Gtk/Gtk.h>
#include <peel/GLib/MainContext.h>
#include <peel/class.h>

using namespace peel;

namespace Demo
{

class Button : public Gtk::Widget
{
  PEEL_SIMPLE_CLASS (Button, Gtk::Widget)
  friend class Gtk::Widget;

  Gtk::Widget *child;

  inline void
  init (Class *);

  template<typename F>
  static void
  define_properties (F &f)
  {
    f.prop (prop_child ())
      .get (&Button::get_child)
      .set (&Button::set_child);
  }

  void
  on_pressed (Gtk::GestureClick *,
              int n_press,
              double x, double y);
  void
  on_released (Gtk::GestureClick *,
               int n_press,
               double x, double y);

  static Signal<Button, void (void)> sig_clicked;

  inline void
  vfunc_dispose ();

public:
  static FloatPtr<Button>
  create ()
  {
    return Object::create<Button> ();
  }

  static FloatPtr<Button>
  create_with_label (const char *label)
  {
    FloatPtr<Gtk::Label> child = Gtk::Label::create (label);
    return Object::create<Button> (prop_child (), std::move (child));
  }

  Gtk::Widget *
  get_child ()
  {
    return child;
  }

  void
  set_child (Gtk::Widget *new_child);

  PEEL_PROPERTY (Gtk::Widget, child, "child")

  PEEL_SIGNAL_CONNECT_METHOD (clicked, sig_clicked)
};

Signal<Button, void (void)> Button::sig_clicked;

PEEL_CLASS_IMPL (Button, "DemoButton", Gtk::Widget)

inline void
Button::init (Class *)
{
  set_focusable (true);
  set_receives_default (true);

  RefPtr<Gtk::GestureClick> gesture_click = Gtk::GestureClick::create ();
  gesture_click->connect_pressed (this, &Button::on_pressed);
  gesture_click->connect_released (this, &Button::on_released);
  add_controller (std::move (gesture_click));
}

inline void
Button::Class::init ()
{
  set_css_name ("button");
  set_accessible_role (Gtk::Accessible::Role::BUTTON);
  set_layout_manager_type (Type::of<Gtk::BinLayout> ());

  sig_clicked.register_ ("clicked", G_SIGNAL_RUN_LAST);

  override_vfunc_dispose<Button> ();
}

void
Button::set_child (Gtk::Widget *new_child)
{
  if (new_child == child)
    return;

  if (child)
    child->unparent ();
  child = new_child;
  if (new_child)
    new_child->set_parent (this);

  notify (prop_child ());
}

inline void
Button::vfunc_dispose ()
{
  if (child)
    {
      child->unparent ();
      child = nullptr;
    }

  parent_vfunc_dispose<Button> ();
}

void
Button::on_pressed (Gtk::GestureClick *,
                    int n_press,
                    double x, double y)
{
  (void) n_press;
  (void) x;
  (void) y;

  if (get_focus_on_click () && !has_focus ())
    grab_focus ();
}

void
Button::on_released (Gtk::GestureClick *gesture_click,
                     int n_press,
                     double x, double y)
{
  (void) n_press;

  gesture_click->set_state (Gtk::EventSequenceState::CLAIMED);
  if (is_sensitive () && contains (x, y))
    sig_clicked.emit (this);
}

}

int
main ()
{
  Gtk::init ();

  FloatPtr<Demo::Button> button = Demo::Button::create_with_label ("Click me");
  button->connect_clicked (
    [] (Demo::Button *button)
    {
      g_print ("You clicked %s!\n", button->get_name ());
    });

  bool should_stop = false;
  Gtk::Window *window = Gtk::Window::create ();
  window->connect_destroy (
    [&should_stop] (Gtk::Widget *)
    {
      should_stop = true;
    });
  window->set_child (std::move (button));
  window->present ();

  GLib::MainContext *main_context = GLib::MainContext::default_ ();
  while (!should_stop)
    main_context->iteration (true);
}
