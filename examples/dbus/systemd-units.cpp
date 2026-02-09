/*
 * Display the list of systemd units, along with their state, updated
 * live, in a GTK list view.
 *
 * We use the Unit D-Bus object directly as an element of the list model.
 * The Unit object has properties that we're interested in, such as
 * active-state. This is a perfect fit for the GTK list view architecture.
 * We use Gtk::BuilderListItemFactory and Gtk::PropertyExpression to bind
 * the view cells to the properties of the units.
 *
 * https://www.freedesktop.org/wiki/Software/systemd/dbus/
 */

#include "SystemdManager.h"
#include "SystemdUnit.h"
#include <peel/Gio/BusType.h>
#include <peel/Gio/ListModel.h>
#include <peel/Gtk/Gtk.h>
#include <peel/GLib/functions.h>
#include <peel/class.h>
#include <peel/WeakPtr.h>
#include <vector>

using namespace peel;

namespace Demo
{

class UnitList final : public Gio::ListModel
{
  PEEL_SIMPLE_CLASS (UnitList, Object)
  friend class Gio::ListModel::Iface;

  std::vector<RefPtr<Unit>> m_units;
  struct LoadingUnit
  {
    String id;
    RefPtr<Gio::Cancellable> cancellable;

    LoadingUnit (String id, RefPtr<Gio::Cancellable> cancellable)
      : id (std::move (id))
      , cancellable (std::move (cancellable))
    { }
  };
  std::vector<LoadingUnit> m_loading_units;

  RefPtr<Gio::Cancellable>
  remove_loading_unit (const char *id)
  {
    std::vector<LoadingUnit>::iterator it = m_loading_units.begin ();

    while (it != m_loading_units.end ())
      {
        if (it->id == id)
          {
            RefPtr<Gio::Cancellable> cancellable = std::move (it->cancellable);
            m_loading_units.erase (it);
            return cancellable;
          }
        ++it;
      }

    return nullptr;
  }

  RefPtr<Manager> m_manager;

  static void
  init_type (Type tp)
  {
    PEEL_IMPLEMENT_INTERFACE (tp, Gio::ListModel);
  }

  static void
  init_interface (Gio::ListModel::Iface *iface)
  {
    iface->override_vfunc_get_item_type<UnitList> ();
    iface->override_vfunc_get_n_items<UnitList> ();
    iface->override_vfunc_get_item<UnitList> ();
  }

  void
  init (Class *)
  {
    UniquePtr<GLib::Error> error;

    m_manager = Manager::Proxy::create_sync (Gio::BusType::SYSTEM,
      "org.freedesktop.systemd1", "/org/freedesktop/systemd1", &error);
    if (error)
      g_printerr ("Failed to connect to systemd: %s", error->message);

    m_manager->connect_unit_new (this, &UnitList::on_unit_new);
    m_manager->connect_unit_removed (this, &UnitList::on_unit_removed);
    m_manager->subscribe_sync (&error);

    m_manager->list_units_async ([self = WeakPtr (this), workaround = true] (Object *source_object, Gio::AsyncResult *res)
      {
        if (!self)
          return;
        UniquePtr<GLib::Error> error;
        RefPtr<GLib::Variant> units;
        Manager *manager = source_object->cast<Manager> ();
        bool ok = manager->list_units_finish (res, &units, &error);
        if (!ok)
          {
            g_warning ("Failed to list units: %s", error->message);
            return;
          }
        // Ugh.
        GVariantIter iter;
        g_variant_iter_init (&iter, reinterpret_cast<GVariant *> ((GLib::Variant *) units));
        const char *id;
        const char *unit_path;
        unsigned added = 0;
        while (g_variant_iter_loop (&iter, "(&sssss&souso)", &id, nullptr, nullptr, nullptr, nullptr, nullptr, &unit_path, nullptr, nullptr, nullptr))
          {
            // FIXME, should not be constructing these all sync
            RefPtr<Unit> unit = Unit::Proxy::create_sync (
              manager->cast<Gio::DBusProxy> ()->get_connection (),
              manager->cast<Gio::DBusProxy> ()->get_name_owner (),
              unit_path, &error);
            if (error)
              g_warning ("Failed to create a proxy for %s: %s", id, error->message);
            else
              {
                self->m_units.push_back (std::move (unit));
                added++;
              }
          }
        self->items_changed (self->m_units.size () - added, 0, added);
      });
  }

  void
  vfunc_dispose ()
  {
    if (m_manager)
      {
        m_manager->unsubscribe_async (nullptr);
        m_manager = nullptr;
      }
    for (LoadingUnit &loading_unit : m_loading_units)
      loading_unit.cancellable->cancel ();
    m_loading_units.clear ();
    m_units.clear ();
    parent_vfunc_dispose<UnitList> ();
  }

  Type
  vfunc_get_item_type ()
  {
    return Type::of<Unit> ();
  }

  unsigned
  vfunc_get_n_items ()
  {
    return m_units.size ();
  }

  RefPtr<Object>
  vfunc_get_item (unsigned position)
  {
    g_return_val_if_fail (position < m_units.size (), nullptr);

    return m_units[position];
  }

  void
  on_unit_new (Manager *manager, const char *id, const char *unit_path)
  {
    /* A new unit has been added. Delay loading it by a short timeout,
     * and if the unit is removed again during that time, don't even
     * start loading it. If we don't do this, systemd somehow enters
     * an infinite loop adding and removing the same units.
     */
    RefPtr<Gio::Cancellable> cancellable = Gio::Cancellable::create ();
    GLib::timeout_add_once (500,
      [cancellable, self = WeakPtr (this), id = String (id), unit_path = String (unit_path)] () mutable
      {
        if (!self || cancellable->is_cancelled ())
          return;
        /* Start loading the actual unit proxy */
        Unit::Proxy::create (
          self->m_manager->cast<Gio::DBusProxy> ()->get_connection (),
          self->m_manager->cast<Gio::DBusProxy> ()->get_name_owner (),
          unit_path, [id = std::move (id), self = std::move (self)] (Object *, Gio::AsyncResult *res)
          {
            UniquePtr<GLib::Error> error;
            if (!self)
              return;
            /* Remove ourselves from the loading list, if not yet */
            self->remove_loading_unit (id);
            RefPtr<Unit> unit = Unit::Proxy::create_finish (res, &error);
            if (error)
              {
                if (!error->matches (G_IO_ERROR, G_IO_ERROR_CANCELLED))
                  g_printerr ("Failed to create a proxy for %s: %s", (const char *) id, error->message);
                return;
              }
            /* Push the new unit onto the list, and emit items-changed */
            self->m_units.push_back (std::move (unit));
            self->items_changed (self->m_units.size () - 1, 0, 1);
          }, Gio::DBusProxy::Flags::NONE, cancellable);
      });
    /* Remember that we're about to load this unit */
    m_loading_units.emplace_back (id, std::move (cancellable));
  }

  void
  on_unit_removed (Manager *manager, const char *id, const char *object_path)
  {
    RefPtr<Gio::Cancellable> cancellable = remove_loading_unit (id);
    if (cancellable)
      {
        /* It was still being loaded in */
        cancellable->cancel ();
        return;
      }
    for (unsigned i = 0; i < m_units.size (); i++)
      {
        if (m_units[i]->get_id () == id)
          {
            m_units.erase (m_units.begin () + i);
            items_changed (i, 1, 0);
            return;
          }
      }
  }
};

PEEL_CLASS_IMPL (UnitList, "DemoUnitList", Object)

inline void
UnitList::Class::init ()
{
  override_vfunc_dispose<UnitList> ();
}

}

static const uint8_t id_template[] = R"(
<?xml version="1.0" encoding="UTF-8"?>
<interface>
  <template class="GtkListItem">
    <property name="child">
      <object class="GtkInscription">
        <property name="xalign">0</property>
        <property name="nat-chars">30</property>
        <binding name="text">
          <lookup name="id" type="Unit">
            <lookup name="item">GtkListItem</lookup>
          </lookup>
        </binding>
      </object>
    </property>
  </template>
</interface>
)";

static const uint8_t active_state_template[] = R"(
<?xml version="1.0" encoding="UTF-8"?>
<interface>
  <template class="GtkListItem">
    <property name="child">
      <object class="GtkInscription">
        <property name="xalign">0</property>
        <property name="nat-chars">7</property>
        <binding name="text">
          <lookup name="active-state" type="Unit">
            <lookup name="item">GtkListItem</lookup>
          </lookup>
        </binding>
      </object>
    </property>
  </template>
</interface>
)";

static const uint8_t sub_state_template[] = R"(
<?xml version="1.0" encoding="UTF-8"?>
<interface>
  <template class="GtkListItem">
    <property name="child">
      <object class="GtkInscription">
        <property name="xalign">0</property>
        <property name="nat-chars">7</property>
        <binding name="text">
          <lookup name="sub-state" type="Unit">
            <lookup name="item">GtkListItem</lookup>
          </lookup>
        </binding>
      </object>
    </property>
  </template>
</interface>
)";

template<size_t size>
static RefPtr<GLib::Bytes>
make_template_bytes (const uint8_t (&templ)[size])
{
  return GLib::Bytes::create_static ({ templ, size - 1 });
}

int
main ()
{
  Gtk::init ();

  RefPtr<Gio::ListModel> unit_list = Object::create<Demo::UnitList> ();

  FloatPtr<Gtk::ColumnView> column_view = Gtk::ColumnView::create (Gtk::NoSelection::create (std::move (unit_list)));
  column_view->add_css_class ("data-table");

  RefPtr<GLib::Bytes> id_template_bytes = make_template_bytes (id_template);
  RefPtr<Gtk::BuilderListItemFactory> id_factory = Gtk::BuilderListItemFactory::create_from_bytes (nullptr, id_template_bytes);
  RefPtr<Gtk::ColumnView::Column> id_column = Gtk::ColumnView::Column::create (
    "ID", std::move (id_factory));
  id_column->set_expand (true);
  column_view->append_column (id_column);

  RefPtr<GLib::Bytes> active_state_template_bytes = make_template_bytes (active_state_template);
  RefPtr<Gtk::BuilderListItemFactory> active_state_factory = Gtk::BuilderListItemFactory::create_from_bytes (nullptr, active_state_template_bytes);
  RefPtr<Gtk::ColumnView::Column> active_state_column = Gtk::ColumnView::Column::create (
    "Active state", std::move (active_state_factory));
  column_view->append_column (active_state_column);

  RefPtr<GLib::Bytes> sub_state_template_bytes = make_template_bytes (sub_state_template);
  RefPtr<Gtk::BuilderListItemFactory> sub_state_factory = Gtk::BuilderListItemFactory::create_from_bytes (nullptr, sub_state_template_bytes);
  RefPtr<Gtk::ColumnView::Column> sub_state_column = Gtk::ColumnView::Column::create (
    "Sub-state", std::move (sub_state_factory));
  column_view->append_column (sub_state_column);

  FloatPtr<Gtk::ScrolledWindow> scrolled_window = Gtk::ScrolledWindow::create ();
  scrolled_window->set_propagate_natural_width (true);
  scrolled_window->set_child (std::move (column_view));
  Gtk::Window *window = Gtk::Window::create ();
  window->set_title ("systemd units");
  window->set_child (std::move (scrolled_window));
  window->set_default_size (-1, 300);
  window->present ();

  GLib::MainContext *context = GLib::MainContext::default_ ();
  while (true)
    context->iteration (true);
}
