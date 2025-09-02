#include <peel/Gtk/Gtk.h>
#include <peel/Gdk/RGBA.h>
#include <peel/Graphene/Rect.h>
#include <peel/GLib/MainContext.h>
#include <peel/class.h>

using namespace peel;

namespace Demo
{

#define CELL_SIZE 50

class Table final : public Gtk::Widget
{
  PEEL_SIMPLE_CLASS (Table, Gtk::Widget)
  friend class Gtk::Widget;
  friend class Gtk::Scrollable;

  static void
  init_type (Type tp)
  {
    PEEL_IMPLEMENT_INTERFACE (tp, Gtk::Scrollable);
  }

  static void
  init_interface (Gtk::Scrollable::Iface *iface)
  {
    (void) iface;
  }
  RefPtr<Gtk::Adjustment> hadjustment;
  RefPtr<Gtk::Adjustment> vadjustment;
  SignalConnection hadjustment_connection;
  SignalConnection vadjustment_connection;
  Gtk::Scrollable::Policy hscroll_policy;
  Gtk::Scrollable::Policy vscroll_policy;

  Gtk::Adjustment *
  get_hadjustment ()
  {
    return hadjustment;
  }

  inline void
  set_hadjustment (Gtk::Adjustment *);

  Gtk::Adjustment *
  get_vadjustment ()
  {
    return vadjustment;
  }

  inline void
  set_vadjustment (Gtk::Adjustment *);

  Gtk::Scrollable::Policy
  get_hscroll_policy ()
  {
    return hscroll_policy;
  }

  inline void
  set_hscroll_policy (Gtk::Scrollable::Policy);

  Gtk::Scrollable::Policy
  get_vscroll_policy ()
  {
    return vscroll_policy;
  }

  inline void
  set_vscroll_policy (Gtk::Scrollable::Policy);

  template<typename F>
  static void
  define_properties (F &f)
  {
    f.override_prop (Gtk::Scrollable::prop_hadjustment ())
      .get (&Table::get_hadjustment)
      .set (&Table::set_hadjustment);
    f.override_prop (Gtk::Scrollable::prop_vadjustment ())
      .get (&Table::get_vadjustment)
      .set (&Table::set_vadjustment);
    f.override_prop (Gtk::Scrollable::prop_hscroll_policy ())
      .get (&Table::get_hscroll_policy)
      .set (&Table::set_hscroll_policy);
    f.override_prop (Gtk::Scrollable::prop_vscroll_policy ())
      .get (&Table::get_vscroll_policy)
      .set (&Table::set_vscroll_policy);
    f.prop (prop_hcells (), 1, G_MAXINT, 1)
      .get (&Table::get_hcells)
      .set (&Table::set_hcells);
    f.prop (prop_vcells (), 1, G_MAXINT, 1)
      .get (&Table::get_vcells)
      .set (&Table::set_vcells);
  }

  void
  init (Class *)
  {
    set_overflow (Gtk::Overflow::HIDDEN);
    hcells = 20;
    vcells = 20;
  }

  void
  vfunc_dispose ()
  {
    hadjustment_connection.disconnect ();
    vadjustment_connection.disconnect ();
    hadjustment = nullptr;
    vadjustment = nullptr;
    parent_vfunc_dispose<Table> ();
  }

  void
  on_adjustment_value_changed (Gtk::Adjustment *)
  {
    queue_allocate ();
  }

  inline void
  vfunc_measure (Gtk::Orientation orienatation, int for_size,
                 int *minimum, int *natural,
                 int *minimum_baseline, int *natural_baseline);

  inline void
  vfunc_size_allocate (int width, int height, int baseline);

  inline void
  vfunc_snapshot (Gtk::Snapshot *);

  unsigned hcells;
  unsigned vcells;

public:
  unsigned
  get_hcells ()
  {
    return hcells;
  }

  void
  set_hcells (unsigned);

  PEEL_PROPERTY (unsigned, hcells, "hcells")

  unsigned
  get_vcells ()
  {
    return vcells;
  }

  void
  set_vcells (unsigned);

  PEEL_PROPERTY (unsigned, vcells, "vcells")
};

PEEL_CLASS_IMPL (Table, "DemoTable", Gtk::Widget)

inline void
Table::Class::init ()
{
  set_css_name ("demo-table");
  override_vfunc_dispose<Table> ();
  override_vfunc_measure<Table> ();
  override_vfunc_size_allocate<Table> ();
  override_vfunc_snapshot<Table> ();
}

inline void
Table::set_hadjustment (Gtk::Adjustment *adj)
{
  if (adj == hadjustment)
    return;

  hadjustment = adj;
  if (adj)
    hadjustment_connection = adj->connect_value_changed (this, &Table::on_adjustment_value_changed);
  else
    hadjustment_connection.disconnect ();

  queue_allocate ();
  notify (Gtk::Scrollable::prop_hadjustment ());
}

inline void
Table::set_vadjustment (Gtk::Adjustment *adj)
{
  if (adj == vadjustment)
    return;

  vadjustment = adj;
  if (adj)
    vadjustment_connection = adj->connect_value_changed (this, &Table::on_adjustment_value_changed);
  else
    vadjustment_connection.disconnect ();

  queue_allocate ();
  notify (Gtk::Scrollable::prop_vadjustment ());
}

inline void
Table::set_hscroll_policy (Gtk::Scrollable::Policy policy)
{
  if (hscroll_policy == policy)
    return;
  hscroll_policy = policy;
  notify (Gtk::Scrollable::prop_hscroll_policy ());
}

inline void
Table::set_vscroll_policy (Gtk::Scrollable::Policy policy)
{
  if (vscroll_policy == policy)
    return;
  vscroll_policy = policy;
  notify (Gtk::Scrollable::prop_vscroll_policy ());
}

void
Table::set_hcells (unsigned cells)
{
  if (hcells == cells)
    return;
  hcells = cells;
  queue_resize ();
  notify (prop_hcells ());
}

void
Table::set_vcells (unsigned cells)
{
  if (vcells == cells)
    return;
  vcells = cells;
  queue_resize ();
  notify (prop_vcells ());
}


inline void
Table::vfunc_measure (Gtk::Orientation orienatation, int for_size,
                      int *minimum, int *natural,
                      int *minimum_baseline, int *natural_baseline)
{
  (void) for_size;
  unsigned cells = (orienatation == Gtk::Orientation::HORIZONTAL) ? hcells : vcells;
  *minimum = *natural = cells * CELL_SIZE;
  *minimum_baseline = *natural_baseline = -1;
}

inline void
Table::vfunc_size_allocate (int width, int height, int baseline)
{
  (void) baseline;
  g_assert (hadjustment);
  g_assert (vadjustment);

  double hadj = hadjustment->get_value ();
  double vadj = vadjustment->get_value ();
  double full_width = hcells * CELL_SIZE;
  double full_height = vcells * CELL_SIZE;
  hadjustment->configure (hadj, 0, full_width, width * 0.1, width * 0.9, width);
  vadjustment->configure (vadj, 0, full_height, height * 0.1, height * 0.9, height);
}

inline void
Table::vfunc_snapshot (Gtk::Snapshot *snapshot)
{
  g_assert (hadjustment);
  g_assert (vadjustment);

  double hadj = hadjustment->get_value ();
  double vadj = vadjustment->get_value ();

  unsigned first_visible_row = vadj / CELL_SIZE;
  unsigned first_visible_column = hadj / CELL_SIZE;
  unsigned last_visible_row = (vadj + get_height () + CELL_SIZE - 1) / CELL_SIZE;
  unsigned last_visible_column = (hadj + get_width () + CELL_SIZE - 1) / CELL_SIZE;

  for (unsigned row = first_visible_row;
       row < MIN (last_visible_row, vcells);
       row++)
    for (unsigned column = first_visible_column;
         column < MIN (last_visible_column, hcells);
         column++)
      {
        Gdk::RGBA rgba
        {
          ((row + (column * 2)) % 10) / 10.0f,
          (((row * 2) + column) % 10) / 10.0f,
          ((row + column) % 10) / 10.0f,
          1.0f
        };
        Graphene::Rect bounds
        {
          { float (column * CELL_SIZE - hadj), float (row * CELL_SIZE - vadj) },
          { CELL_SIZE, CELL_SIZE }
        };
        snapshot->append_color (&rgba, &bounds);
      }
}

}

int
main ()
{
  Gtk::init ();

  FloatPtr<Demo::Table> table = Object::create<Demo::Table> ();
  FloatPtr<Gtk::ScrolledWindow> scrolled_window = Gtk::ScrolledWindow::create ();
  scrolled_window->set_child (std::move (table));

  bool should_stop = false;
  Gtk::Window *window = Gtk::Window::create ();
  window->connect_destroy (
    [&should_stop] (Gtk::Widget *)
    {
      should_stop = true;
    });
  window->set_child (std::move (scrolled_window));
  window->present ();

  GLib::MainContext *main_context = GLib::MainContext::default_ ();
  while (!should_stop)
    main_context->iteration (true);
}
