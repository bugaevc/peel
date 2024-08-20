#include <peel/Gtk/Gtk.h>
#include <peel/Gio/File.h>
#include <peel/Gio/FileInfo.h>
#include <peel/Gio/Gio.h>
#include <peel/GLib/MainContext.h>
#include <peel/class.h>

using namespace peel;

class MainColumnCell final : public Gtk::Widget
{
  PEEL_SIMPLE_CLASS (MainColumnCell, Gtk::Widget)

  Gtk::TreeExpander *tree_expander;
  Gtk::Label *label;
  Gtk::Image *icon;

  inline void
  vfunc_dispose ();

  inline void
  init (Class *);

public:
    static FloatPtr<MainColumnCell>
    create ()
    {
        return Object::create<MainColumnCell> ();
    }

    void
    set_tree_list_row (Gtk::TreeListRow *row);
};

PEEL_CLASS_IMPL (MainColumnCell, "MainColumnCell", Gtk::Widget)

inline void
MainColumnCell::init (Class *)
{
  // Our main colun cell contains the following widgetry:
  //
  // Gtk::TreeExpander {
  //   Gtk::Box {
  //     Gtk::Image (file icon)
  //     Gtk::Label (file name)
  //   }
  // }
  FloatPtr<Gtk::Box> box = Gtk::Box::create (Gtk::Orientation::HORIZONTAL, 6);

  FloatPtr<Gtk::Image> icon = Gtk::Image::create ();
  this->icon = icon;
  box->append (std::move (icon));

  FloatPtr<Gtk::Label> label = Gtk::Label::create (nullptr);
  label->set_xalign (0);
  this->label = label;
  box->append (std::move (label));

  FloatPtr<Gtk::TreeExpander> tree_expander = Gtk::TreeExpander::create ();
  tree_expander->set_child (std::move (box));
  this->tree_expander = tree_expander;
  set_parent (std::move (tree_expander));
}

inline void
MainColumnCell::Class::init ()
{
  set_layout_manager_type (Type::of<Gtk::BinLayout> ());
  override_vfunc_dispose<MainColumnCell> ();
}

inline void
MainColumnCell::vfunc_dispose ()
{
  if (tree_expander)
    {
      tree_expander->unparent ();
      tree_expander = nullptr;
    }
  parent_vfunc_dispose<MainColumnCell> ();
}

void
MainColumnCell::set_tree_list_row (Gtk::TreeListRow *row)
{
  tree_expander->set_list_row (row);
  RefPtr<Gio::FileInfo> file_info = row->get_item ().cast<Gio::FileInfo> ();

  label->set_label (file_info->get_display_name ());
  icon->set_from_gicon (file_info->get_icon ());
}

// We're going to ask Gtk::DirectoryList to load these file attributes.
#define QUERY_ATTRS "standard::*,time::modified"

int
main ()
{
  Gtk::init ();

  RefPtr<Gio::File> root_file = Gio::File::create_for_path ("/");
  RefPtr<Gtk::DirectoryList> root_directory_model = Gtk::DirectoryList::create (QUERY_ATTRS, root_file);

  RefPtr<Gtk::TreeListModel> tree_model = Gtk::TreeListModel::create (
    std::move(root_directory_model), false, false,
    [] (Object *item_obj) -> RefPtr<Gio::ListModel>
    {
      Gio::FileInfo *file_info = item_obj->cast<Gio::FileInfo> ();
      Gio::FileType file_type = file_info->get_file_type ();
      if (file_type != Gio::FileType::DIRECTORY && file_type != Gio::FileType::UNKNOWN)
        return nullptr;
      Gio::File *file = file_info->get_attribute_object ("standard::file")->cast<Gio::File> ();
      return Gtk::DirectoryList::create (QUERY_ATTRS, file);
    });

  FloatPtr<Gtk::ColumnView> column_view = Gtk::ColumnView::create (nullptr);
  column_view->add_css_class ("data-table");

  RefPtr<Gtk::SignalListItemFactory> main_column_factory = Gtk::SignalListItemFactory::create ();
  main_column_factory->connect_setup (
    [] (Gtk::SignalListItemFactory *, Object *obj)
    {
      Gtk::ListItem *item = obj->cast<Gtk::ListItem> ();
      item->set_focusable (false);
      item->set_child (MainColumnCell::create ());
    });
  main_column_factory->connect_bind (
    [] (Gtk::SignalListItemFactory *, Object *obj)
    {
      Gtk::ListItem *item = obj->cast<Gtk::ListItem> ();
      MainColumnCell *cell = item->get_child ()->cast<MainColumnCell> ();
      Gtk::TreeListRow *tree_list_row = item->get_item ()->cast<Gtk::TreeListRow> ();
      cell->set_tree_list_row (tree_list_row);
    });
  RefPtr<Gtk::ColumnView::Column> main_column = Gtk::ColumnView::Column::create (
    "Name", std::move (main_column_factory));
  main_column->set_expand (true);
  main_column->set_resizable (true);
  column_view->append_column (main_column);

  RefPtr<Gtk::SignalListItemFactory> size_column_factory = Gtk::SignalListItemFactory::create ();
  size_column_factory->connect_setup (
    [] (Gtk::SignalListItemFactory *, Object *obj)
    {
      Gtk::ListItem *item = obj->cast<Gtk::ListItem> ();
      item->set_focusable (false);
      item->set_child (Gtk::Inscription::create (nullptr));
    });
  size_column_factory->connect_bind (
    [](Gtk::SignalListItemFactory *, Object *obj)
    {
      Gtk::ListItem *item = obj->cast<Gtk::ListItem> ();
      Gtk::Inscription *inscription = item->get_child ()->cast<Gtk::Inscription> ();
      Gtk::TreeListRow *tree_list_row = item->get_item ()->cast<Gtk::TreeListRow> ();
      RefPtr<Gio::FileInfo> file_info = tree_list_row->get_item ().cast<Gio::FileInfo> ();
      char *size = g_format_size (file_info->get_size ());
      inscription->set_text (size);
      g_free (size);
    });
  RefPtr<Gtk::ColumnView::Column> size_column = Gtk::ColumnView::Column::create (
    "Size", std::move (size_column_factory));
  size_column->set_resizable (true);
  column_view->append_column (size_column);

  RefPtr<Gtk::SignalListItemFactory> mtime_column_factory = Gtk::SignalListItemFactory::create ();
  mtime_column_factory->connect_setup (
    [] (Gtk::SignalListItemFactory *, Object *obj)
    {
      Gtk::ListItem *item = obj->cast<Gtk::ListItem> ();
      item->set_focusable (false);
      item->set_child (Gtk::Inscription::create (nullptr));
    });
  mtime_column_factory->connect_bind (
    [] (Gtk::SignalListItemFactory *, Object *obj)
    {
      Gtk::ListItem *item = obj->cast<Gtk::ListItem> ();
      Gtk::Inscription *inscription = item->get_child ()->cast<Gtk::Inscription> ();
      Gtk::TreeListRow *tree_list_row = item->get_item ()->cast<Gtk::TreeListRow> ();
      RefPtr<Gio::FileInfo> file_info = tree_list_row->get_item ().cast<Gio::FileInfo> ();
      RefPtr<GLib::DateTime> mtime = file_info->get_modification_date_time ();
      char *text = mtime->format ("%c");
      inscription->set_text (text);
      g_free (text);
    });
  RefPtr<Gtk::ColumnView::Column> mtime_column = Gtk::ColumnView::Column::create (
    "Modification time", std::move (mtime_column_factory));
  mtime_column->set_resizable (true);
  column_view->append_column (mtime_column);

  RefPtr<Gtk::SelectionModel> selection_model = Gtk::SingleSelection::create (std::move (tree_model));
  column_view->set_model (selection_model);

  // When an item gets activated (e.g. with a double mouse click), open
  // the file it represents in the default handler for the file type.
  column_view->connect_activate (
    [] (Gtk::ColumnView *column_view, unsigned position)
    {
      // Extract the Gio::File.
      Gio::ListModel *model = column_view->get_model ();
      RefPtr<Gtk::TreeListRow> tree_list_row = model->get_object (position).cast<Gtk::TreeListRow> ();
      RefPtr<Gio::FileInfo> file_info = tree_list_row->get_item ().cast<Gio::FileInfo> ();
      Gio::File *file = file_info->get_attribute_object ("standard::file")->cast<Gio::File> ();
      // Call Gio::AppInfo::launch_default_for_uri (), passing the file URI.
      char *uri = file->get_uri ();
      RefPtr<Gio::AppLaunchContext> launch_context = column_view->get_display ()->get_app_launch_context ();
      Gio::AppInfo::launch_default_for_uri (uri, launch_context, nullptr);
      g_free (uri);
    });

  // Wrap the column view into a scrolled window, to enable scrolling.
  FloatPtr<Gtk::ScrolledWindow> scrolled_window = Gtk::ScrolledWindow::create ();
  scrolled_window->set_child (std::move (column_view));
  scrolled_window->set_propagate_natural_width (true);

  bool should_stop = false;
  Gtk::Window *window = Gtk::Window::create ();
  window->set_title ("File browser");
  window->set_default_size (700, 500);
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
