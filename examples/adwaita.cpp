#include <peel/Adw/Adw.h>
#include <peel/Gtk/Gtk.h>
#include <peel/Gio/Gio.h>
#include <peel/GLib/functions.h>
#include <peel/class.h>
#include <functional>

using namespace peel;

namespace Demo
{

class Window final : public Adw::ApplicationWindow
{
  PEEL_SIMPLE_CLASS (Window, Adw::ApplicationWindow)

  Adw::TabView *tab_view;

  inline void
  init (Class *);

  void
  open_new_tab ();

  void
  close_current_tab ();

  Adw::TabView *
  on_create_window (Adw::TabView *);

public:

  static Window *
  create (Gtk::Application *app)
  {
    Window *window = Object::create<Window> (prop_application (), app);
    window->open_new_tab ();
    return window;
  }
};

PEEL_CLASS_IMPL (Window, "DemoWindow", Adw::ApplicationWindow)

inline void
Window::Class::init ()
{
  install_action ("win.new-tab", nullptr, [] (Gtk::Widget *widget, const char *action_name, GLib::Variant *parameter)
  {
    (void) action_name;
    (void) parameter;
    widget->cast<Window> ()->open_new_tab ();
  });
  install_action ("win.close-tab", nullptr, [] (Gtk::Widget *widget, const char *action_name, GLib::Variant *parameter)
  {
    (void) action_name;
    (void) parameter;
    widget->cast<Window> ()->close_current_tab ();
  });

  add_binding_action (GDK_KEY_T, Gdk::ModifierType::CONTROL_MASK, "win.new-tab", nullptr);
  add_binding_action (GDK_KEY_W, Gdk::ModifierType::CONTROL_MASK, "win.close-tab", nullptr);
}

inline void
Window::init (Class *)
{
  set_title ("Demo Adwaita app, written with peel");
  set_default_size (700, 500);

  FloatPtr<Adw::HeaderBar> header_bar = Adw::HeaderBar::create ();
  FloatPtr<Adw::WindowTitle> window_title = Adw::WindowTitle::create (
    "Demo Adwaita app", "Written with peel");
  header_bar->set_title_widget (std::move (window_title));

  // New Tab button
  FloatPtr<Gtk::Button> new_tab_button = Gtk::Button::create_from_icon_name (
    "tab-new-symbolic");
  new_tab_button->set_action_name ("win.new-tab");
  new_tab_button->set_tooltip_text ("New Tab");
  new_tab_button->set_focus_on_click (false);
  header_bar->pack_start (std::move (new_tab_button));

  RefPtr<Gio::Menu> menu = Gio::Menu::create ();
  menu->append ("New Window", "app.new-window");
  menu->append ("About", "app.about");
  menu->append ("Quit", "app.quit");
  FloatPtr<Gtk::PopoverMenu> popover_menu = Gtk::PopoverMenu::create_from_model (menu);

  // Menu button
  FloatPtr<Gtk::MenuButton> menu_button = Gtk::MenuButton::create ();
  menu_button->set_popover (std::move (popover_menu));
  menu_button->set_icon_name ("open-menu-symbolic");
  menu_button->set_tooltip_text ("Menu");
  menu_button->set_focus_on_click (false);
  header_bar->pack_end (std::move (menu_button));

  // Tab Overview button
  FloatPtr<Gtk::Button> tab_overview_button = Gtk::Button::create_from_icon_name (
    "view-grid-symbolic");
  tab_overview_button->set_action_name ("overview.open");
  tab_overview_button->set_tooltip_text ("Tab Overview");
  tab_overview_button->set_focus_on_click (false);
  header_bar->pack_end (std::move (tab_overview_button));

  FloatPtr<Adw::TabView> float_tab_view = Adw::TabView::create ();
  tab_view = float_tab_view;
  tab_view->connect_create_window (this, &Window::on_create_window);

  FloatPtr<Adw::TabBar> tab_bar = Adw::TabBar::create ();
  tab_bar->set_view (tab_view);

  FloatPtr<Adw::ToolbarView> toolbar_view = Adw::ToolbarView::create ();
  toolbar_view->set_top_bar_style (Adw::ToolbarStyle::RAISED);
  toolbar_view->add_top_bar (std::move (header_bar));
  toolbar_view->add_top_bar (std::move (tab_bar));
  toolbar_view->set_content (std::move (float_tab_view));

  FloatPtr<Adw::TabOverview> tab_overview = Adw::TabOverview::create ();
  tab_overview->set_view (tab_view);
  tab_overview->set_child (std::move (toolbar_view));
  set_content (std::move (tab_overview));
}

void
Window::open_new_tab ()
{
  FloatPtr<Adw::StatusPage> status_page = Adw::StatusPage::create ();

  static unsigned tabs_open_this_far;
  String title = GLib::strdup_printf ("Tab #%d", ++tabs_open_this_far);
  status_page->set_title (title);

  Adw::TabPage *tab_page = tab_view->append (std::move (status_page));
  tab_page->set_title (title);
}

void
Window::close_current_tab ()
{
  Adw::TabPage *page = tab_view->get_selected_page ();
  tab_view->close_page (page);

  // If this was the last page, close the window.
  if (tab_view->get_n_pages () == 0)
    GLib::idle_add_once (std::bind (&Gtk::Window::close, this));
}

Adw::TabView *
Window::on_create_window (Adw::TabView *)
{
  Window *new_window = Object::create<Window> (prop_application (), get_application ());
  new_window->present ();
  return new_window->tab_view;
}

class Application final : public Adw::Application
{
  PEEL_SIMPLE_CLASS (Application, Adw::Application)
  friend class Gio::Application;

  inline void
  init (Class *);

  inline void
  vfunc_activate ();

  void
  action_quit (Gio::SimpleAction *, GLib::Variant *);

  void
  action_new_window (Gio::SimpleAction *, GLib::Variant *);

  void
  action_about (Gio::SimpleAction *, GLib::Variant *);

public:
  static RefPtr<Application>
  create ()
  {
    return Object::create<Application> (
      prop_application_id (), "org.example.Demo",
      prop_flags (), Gio::Application::Flags::DEFAULT_FLAGS);
  }
};

PEEL_CLASS_IMPL (Application, "DemoApplication", Adw::Application)

inline void
Application::init (Class *)
{
  RefPtr<Gio::SimpleAction> action = Gio::SimpleAction::create ("quit", nullptr);
  action->connect_activate (this, &Application::action_quit);
  cast<Gio::ActionMap> ()->add_action (action);
  set_accels_for_action ("app.quit", (const char *[]) { "<Ctrl>Q", nullptr });

  action = Gio::SimpleAction::create ("new-window", nullptr);
  action->connect_activate (this, &Application::action_new_window);
  cast<Gio::ActionMap> ()->add_action (action);
  set_accels_for_action ("app.new-window", (const char *[]) { "<Ctrl>N", nullptr });

  action = Gio::SimpleAction::create ("about", nullptr);
  action->connect_activate (this, &Application::action_about);
  cast<Gio::ActionMap> ()->add_action (action);
}

inline void
Application::Class::init ()
{
  override_vfunc_activate<Application> ();
}

inline void
Application::vfunc_activate ()
{
  parent_vfunc_activate<Application> ();

  Window *window = Window::create (this);
  window->present ();
}

void
Application::action_quit (Gio::SimpleAction *, GLib::Variant *)
{
  quit ();
}

void
Application::action_new_window (Gio::SimpleAction *, GLib::Variant *)
{
  Window *window = Window::create (this);
  window->present ();
}

void Application::action_about (Gio::SimpleAction *, GLib::Variant *)
{
  Gtk::Window *parent_window = get_active_window ();
  if (parent_window && !parent_window->check_type<Window> ())
    parent_window = nullptr;

  Adw::show_about_dialog (parent_window,
    Adw::AboutDialog::prop_application_name (), "peel Adwaita demo",
    Adw::AboutDialog::prop_application_icon (), "application-x-executable",
    Adw::AboutDialog::prop_version (), "0.1",
    Adw::AboutDialog::prop_license_type (), Gtk::License::MIT_X11,
    Adw::AboutDialog::prop_comments (), "A simple application demonstrating how Adwaita can be used from C++ with peel.",
    Adw::AboutDialog::prop_support_url (), "https://matrix.to/#/#peel:matrix.org",
    Adw::AboutDialog::prop_issue_url (), "https://gitlab.gnome.org/bugaevc/peel/-/issues",
    Adw::AboutDialog::prop_developers (), (const char *[])
      {
        "Sergey Bugaev <bugaevc@gmail.com>",
        "Sebastian Dröge <sebastian@centricular.com>",
        "amyspark <amy@amyspark.me>",
        "peel project contributors",
        nullptr
      });
}

} /* namespace Demo */

int
main (int argc, char *argv[])
{
  RefPtr<Demo::Application> app = Demo::Application::create ();
  return app->run (argc, argv);
}
