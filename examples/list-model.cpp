#include <peel/Gtk/Gtk.h>
#include <peel/Gio/ListModel.h>
#include <peel/GLib/MainContext.h>
#include <peel/class.h>

using namespace peel;

namespace Demo
{

class Model final : public Gio::ListModel
{
  PEEL_SIMPLE_CLASS (Model, Object)
  friend class Gio::ListModel;

  static void
  init_type (Type tp)
  {
    PEEL_IMPLEMENT_INTERFACE (tp, Gio::ListModel);
  }

  static void
  init_interface (Gio::ListModel::Iface *iface)
  {
    iface->override_vfunc_get_item<Model> ();
    iface->override_vfunc_get_item_type<Model> ();
    iface->override_vfunc_get_n_items<Model> ();
  }

  Type
  vfunc_get_item_type ()
  {
    return Type::of<Gtk::StringObject> ();
  }

  RefPtr<Gtk::StringObject> items[5];

  void
  init (Class *)
  {
    items[0] = Gtk::StringObject::create ("One");
    items[1] = Gtk::StringObject::create ("Two");
    items[2] = Gtk::StringObject::create ("Three");
    items[3] = Gtk::StringObject::create ("Four");
    items[4] = Gtk::StringObject::create ("Five");
  }

  unsigned
  vfunc_get_n_items ()
  {
    return sizeof (items) / sizeof(items[0]);
  }

  RefPtr<Object>
  vfunc_get_item (unsigned position)
  {
    g_return_val_if_fail (position < vfunc_get_n_items (), nullptr);
    return items[position];
  }

  void
  vfunc_dispose ()
  {
    for (RefPtr<Gtk::StringObject> &item : items)
      item = nullptr;
    parent_vfunc_dispose<Model> ();
  }
};

PEEL_CLASS_IMPL (Model, "DemoModel", Object)

inline void
Model::Class::init ()
{
  override_vfunc_dispose<Model> ();
}

}

int
main ()
{
  Gtk::init ();

  RefPtr<Demo::Model> model = Object::create<Demo::Model> ();
  RefPtr<Gtk::SelectionModel> selection = Gtk::SingleSelection::create (
    std::move (model));

  RefPtr<Gtk::SignalListItemFactory> factory = Gtk::SignalListItemFactory::create ();
  factory->connect_setup (
    [] (Gtk::SignalListItemFactory *factory, Object *obj)
    {
      Gtk::ListItem *item = obj->cast<Gtk::ListItem> ();
      item->set_child (Gtk::Inscription::create (nullptr));
    });
  factory->connect_bind (
    [] (Gtk::SignalListItemFactory *factory, Object *obj)
    {
      Gtk::ListItem *item = obj->cast<Gtk::ListItem> ();
      Gtk::Inscription *inscription = item->get_child ()->cast<Gtk::Inscription> ();
      Gtk::StringObject *str = item->get_item ()->cast<Gtk::StringObject> ();
      inscription->set_text (str->get_string ());
    });

  FloatPtr<Gtk::ListView> list_view = Gtk::ListView::create (
    std::move (selection), std::move (factory));
  FloatPtr<Gtk::ScrolledWindow> scrolled_window = Gtk::ScrolledWindow::create ();
  scrolled_window->set_propagate_natural_width (true);
  scrolled_window->set_propagate_natural_height (true);
  scrolled_window->set_child (std::move (list_view));

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
