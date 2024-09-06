/*
 * Adapted from gtk4-rs:examples/rotation_bin.
 */

#include <peel/Gtk/Gtk.h>
#include <peel/Gsk/Transform.h>
#include <peel/Graphene/Point.h>
#include <peel/class.h>
#include <peel/enum.h>

using namespace peel;

namespace Demo
{

enum class Rotation
{
  NORMAL = 0,
  RIGHT = 90,
  FLIP = 180,
  LEFT = 270
};

} /* namespace Demo */

/* PEEL_ENUM () and PEEL_ENUM_IMPL () must appear in the root namespace,
 * since they expand to specializations of templates in the peel:: namespace.
 */
PEEL_ENUM (Demo::Rotation)

PEEL_ENUM_IMPL (Demo::Rotation, "DemoRotation",
  PEEL_ENUM_VALUE (Demo::Rotation::NORMAL, "normal"),
  PEEL_ENUM_VALUE (Demo::Rotation::RIGHT, "right"),
  PEEL_ENUM_VALUE (Demo::Rotation::FLIP, "flip"),
  PEEL_ENUM_VALUE (Demo::Rotation::LEFT, "left"))

namespace Demo
{

class RotationBin final : public Gtk::Widget
{
  PEEL_SIMPLE_CLASS (RotationBin, Gtk::Widget)
  friend class Gtk::Widget;

  template<typename F>
  static void
  define_properties (F &);

  Gtk::Widget *child;
  Rotation rotation;

  /* Overriden GObject::Object vfunc */
  void
  vfunc_dispose ();

  /* Overriden Gtk::Widget vfuncs */

  void
  vfunc_compute_expand (bool *hexpand, bool *vexpand);

  Gtk::SizeRequestMode
  vfunc_get_request_mode ();

  void
  vfunc_measure (Gtk::Orientation orientation, int for_size,
    int *min, int *nat, int *min_baseline, int *nat_baseline);

  void
  vfunc_size_allocate (int width, int height, int baseline);

public:
  static FloatPtr<RotationBin>
  create ()
  {
    return Object::create<RotationBin> ();
  }

  Gtk::Widget *
  get_child ()
  {
    return child;
  }

  void
  set_child (FloatPtr<Gtk::Widget>);

  PEEL_PROPERTY (Gtk::Widget, child, "child")

  Rotation
  get_rotation ()
  {
    return rotation;
  }

  void
  set_rotation (Rotation);

  PEEL_PROPERTY (Rotation, rotation, "rotation")

  void
  rotate_clockwise ();

  void
  rotate_counter_clockwise ();
};

PEEL_CLASS_IMPL (RotationBin, "DemoRotationBin", Gtk::Widget)

void
RotationBin::Class::init ()
{
  override_vfunc_dispose<RotationBin> ();
  override_vfunc_compute_expand<RotationBin> ();
  override_vfunc_get_request_mode<RotationBin> ();
  override_vfunc_measure<RotationBin> ();
  override_vfunc_size_allocate<RotationBin> ();
}

template<typename F>
void
RotationBin::define_properties (F &f)
{
  f.prop (prop_child ())
    .get (&RotationBin::get_child)
    .set (&RotationBin::set_child);

  f.prop (prop_rotation (), Rotation::NORMAL)
    .get (&RotationBin::get_rotation)
    .set (&RotationBin::set_rotation);
}

void
RotationBin::set_child (FloatPtr<Gtk::Widget> new_child)
{
  if (child == new_child)
    return;

  if (child)
    child->unparent ();
  child = new_child;
  if (new_child)
    set_parent (std::move (new_child));

  notify (prop_child ());
}

void
RotationBin::set_rotation (Rotation new_rotation)
{
  if (rotation == new_rotation)
    return;

  rotation = new_rotation;
  queue_resize ();
  notify (prop_rotation ());
}

inline void
RotationBin::vfunc_dispose ()
{
  if (child)
    {
      child->unparent ();
      child = nullptr;
    }
  parent_vfunc_dispose<RotationBin> ();
}

static bool
should_swap_width_and_height (Rotation rotation)
{
  switch (rotation)
    {
    case Rotation::NORMAL:
    case Rotation::FLIP:
      return false;
    case Rotation::LEFT:
    case Rotation::RIGHT:
      return true;
    }

  g_assert_not_reached ();
}

inline void
RotationBin::vfunc_compute_expand (bool *hexpand, bool *vexpand)
{
  parent_vfunc_compute_expand<RotationBin> (hexpand, vexpand);
  if (should_swap_width_and_height (rotation))
    std::swap (*hexpand, *vexpand);
}

inline Gtk::SizeRequestMode
RotationBin::vfunc_get_request_mode ()
{
  if (!child)
    return Gtk::SizeRequestMode::CONSTANT_SIZE;

  Gtk::SizeRequestMode child_mode = child->get_request_mode ();
  if (!should_swap_width_and_height (rotation))
    return child_mode;

  switch (child_mode)
    {
    case Gtk::SizeRequestMode::CONSTANT_SIZE:
      return Gtk::SizeRequestMode::CONSTANT_SIZE;
    case Gtk::SizeRequestMode::WIDTH_FOR_HEIGHT:
      return Gtk::SizeRequestMode::HEIGHT_FOR_WIDTH;
    case Gtk::SizeRequestMode::HEIGHT_FOR_WIDTH:
      return Gtk::SizeRequestMode::WIDTH_FOR_HEIGHT;
    }

  g_assert_not_reached ();
}

inline void
RotationBin::vfunc_measure (Gtk::Orientation orientation, int for_size,
  int *min, int *nat, int *min_baseline, int *nat_baseline)
{
  if (!child)
    {
      *min = *nat = 0;
      *min_baseline = *nat_baseline = -1;
      return;
    }

  Gtk::Orientation child_orientation;
  if (!should_swap_width_and_height (rotation))
    child_orientation = orientation;
  else if (orientation == Gtk::Orientation::VERTICAL)
    child_orientation = Gtk::Orientation::HORIZONTAL;
  else
    child_orientation = Gtk::Orientation::VERTICAL;

  child->measure (child_orientation, for_size, min, nat, min_baseline, nat_baseline);

  if (should_swap_width_and_height (rotation))
    *min_baseline = *nat_baseline = -1;
}

inline void
RotationBin::vfunc_size_allocate (int width, int height, int baseline)
{
  if (!child)
    return;

  int child_width, child_height, child_baseline;
  if (!should_swap_width_and_height (rotation))
    {
      child_width = width;
      child_height = height;
    }
  else
    {
      child_width = height;
      child_height = width;
    }

  RefPtr<Gsk::Transform> transform = nullptr;
  if (rotation != Rotation::NORMAL)
    {
      child_baseline = -1;
      Graphene::Point center { width / 2.0f, height / 2.0f };
      Graphene::Point child_minus_center { -child_width / 2.0f, -child_height / 2.0f };
      transform = Gsk::Transform::translate (nullptr, &center);
      transform = Gsk::Transform::rotate (std::move (transform), static_cast<int> (rotation));
      transform = Gsk::Transform::translate (std::move (transform), &child_minus_center);
    }
  else
    child_baseline = baseline;

  child->allocate (child_width, child_height, child_baseline, std::move (transform));
}

void
RotationBin::rotate_clockwise ()
{
  Rotation new_rotation;

  switch (rotation)
    {
    case Rotation::NORMAL:
      new_rotation = Rotation::RIGHT;
      break;
    case Rotation::RIGHT:
      new_rotation = Rotation::FLIP;
      break;
    case Rotation::FLIP:
       new_rotation = Rotation::LEFT;
       break;
    case Rotation::LEFT:
      new_rotation = Rotation::NORMAL;
      break;
    }

  set_rotation (new_rotation);
}

void
RotationBin::rotate_counter_clockwise ()
{
  Rotation new_rotation;

  switch (rotation)
    {
    case Rotation::NORMAL:
      new_rotation = Rotation::LEFT;
      break;
    case Rotation::RIGHT:
      new_rotation = Rotation::NORMAL;
      break;
    case Rotation::FLIP:
       new_rotation = Rotation::RIGHT;
       break;
    case Rotation::LEFT:
      new_rotation = Rotation::FLIP;
      break;
    }

  set_rotation (new_rotation);
}

} /* namespace Demo */

static void
build_ui (Gio::Application *app)
{
  FloatPtr<Gtk::Image> image = Gtk::Image::create_from_icon_name ("audio-x-generic");
  image->set_pixel_size (128);

  FloatPtr<Gtk::Box> interactive_box = Gtk::Box::create (Gtk::Orientation::VERTICAL, 6);
  interactive_box->set_halign (Gtk::Align::CENTER);
  interactive_box->set_valign (Gtk::Align::CENTER);
  interactive_box->append (std::move (image));

  FloatPtr<Demo::RotationBin> rotation_bin = Demo::RotationBin::create ();
  /* Capture the raw pointer, not the FloatPtr.  */
  Demo::RotationBin *rotation_bin_ptr = rotation_bin;

  FloatPtr<Gtk::Button> button_clockwise = Gtk::Button::create_with_label ("Rotate clockwise");
  FloatPtr<Gtk::Button> button_counter_clockwise = Gtk::Button::create_with_label ("Rotate counter clockwise");
  button_clockwise->connect_clicked (
    [rotation_bin_ptr] (Gtk::Button *)
    {
      rotation_bin_ptr->rotate_clockwise ();
    });
  button_counter_clockwise->connect_clicked (
    [rotation_bin_ptr] (Gtk::Button *)
    {
      rotation_bin_ptr->rotate_counter_clockwise ();
    });
  interactive_box->append (std::move (button_clockwise));
  interactive_box->append (std::move (button_counter_clockwise));
  rotation_bin->set_child (std::move (interactive_box));

  FloatPtr<Gtk::Grid> grid = Gtk::Grid::create ();
  grid->set_margin_bottom (6);
  grid->set_margin_top (6);
  grid->set_margin_start (6);
  grid->set_margin_end (6);
  grid->set_halign (Gtk::Align::CENTER);
  grid->set_valign (Gtk::Align::CENTER);
  grid->set_row_spacing (6);
  grid->set_column_spacing (6);

  FloatPtr<Gtk::Button> button_normal = Gtk::Button::create_with_label ("Set rotation to 0 degrees");
  FloatPtr<Gtk::Button> button_right = Gtk::Button::create_with_label ("Set rotation to 90 degrees");
  FloatPtr<Gtk::Button> button_flip = Gtk::Button::create_with_label ("Set rotation to 180 degrees");
  FloatPtr<Gtk::Button> button_left = Gtk::Button::create_with_label ("Set rotation to 270 degrees");
  button_normal->connect_clicked (
    [rotation_bin_ptr] (Gtk::Button *)
    {
      rotation_bin_ptr->set_rotation (Demo::Rotation::NORMAL);
    });
  button_right->connect_clicked (
    [rotation_bin_ptr] (Gtk::Button *)
    {
      rotation_bin_ptr->set_rotation (Demo::Rotation::RIGHT);
    });
  button_flip->connect_clicked (
    [rotation_bin_ptr] (Gtk::Button *)
    {
      rotation_bin_ptr->set_rotation (Demo::Rotation::FLIP);
    });
  button_left->connect_clicked (
    [rotation_bin_ptr] (Gtk::Button *)
    {
      rotation_bin_ptr->set_rotation (Demo::Rotation::LEFT);
    });
  grid->attach (std::move (button_normal), 0, 1, 1, 1);
  grid->attach (std::move (button_right), 1, 1, 1, 1);
  grid->attach (std::move (button_flip), 0, 2, 1, 1);
  grid->attach (std::move (button_left), 1, 2, 1, 1);

  FloatPtr<Gtk::Box> vbox = Gtk::Box::create (Gtk::Orientation::VERTICAL, 6);
  vbox->set_margin_bottom (6);
  vbox->set_margin_top (6);
  vbox->set_margin_start (6);
  vbox->set_margin_end (6);
  vbox->set_halign (Gtk::Align::CENTER);
  vbox->set_valign (Gtk::Align::CENTER);

  vbox->append (std::move (rotation_bin));
  vbox->append (std::move (grid));

  Gtk::ApplicationWindow *window = Gtk::ApplicationWindow::create (app->cast<Gtk::Application> ());
  window->set_child (std::move (vbox));
  window->present ();
}

int
main (int argc, char **argv)
{
  RefPtr<Gtk::Application> app = Gtk::Application::create ("org.example.rotation_bin", Gio::Application::Flags::NON_UNIQUE);
  app->connect_activate (build_ui);
  ArrayRef<const char *> args { const_cast<const char **> (argv), static_cast<unsigned> (argc) };
  return app->run (args);
}
