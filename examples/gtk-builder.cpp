#include <peel/widget-template.h>
#include <peel/Gtk/Gtk.h>
#include <peel/GLib/MainContext.h>
#include <peel/GLib/Bytes.h>
#include <peel/class.h>

using namespace peel;

static const char template_string[] = R"(
<?xml version="1.0" encoding="UTF-8"?>
<interface>
  <template class="DemoWindow" parent="GtkWindow">
    <property name="title" translatable="yes">Demo Window</property>
    <child>
      <object class="GtkBox" id="box">
        <property name="orientation">vertical</property>
        <property name="spacing">6</property>
        <child>
          <object class="GtkButton">
            <property name="label" translatable="yes">Button</property>
            <signal name="clicked" handler="button_clicked_cb"/>
          </object>
        </child>
        <child>
          <object class="GtkLabel" id="label">
            <property name="label" translatable="yes">Click the button</property>
          </object>
        </child>
      </object>
    </child>
  </template>
</interface>
)";

namespace Demo {

class Window : public Gtk::Window
{
  PEEL_SIMPLE_CLASS (Window, Gtk::Window);

  unsigned click_count;
  Gtk::Label *label;

  void
  button_clicked_cb (Gtk::Button *);

  inline void
  init (Class *);

  inline void
  vfunc_dispose ();
};

PEEL_CLASS_IMPL (Window, "DemoWindow", Gtk::Window)

inline void
Window::Class::init ()
{
  override_vfunc_dispose<Window> ();

  // Normally you'd use set_template_from_resource () instead of this.
  auto template_bytes = GLib::Bytes::create_static ({ reinterpret_cast<const uint8_t *> (template_string), strlen (template_string) });
  set_template (template_bytes);

  PEEL_WIDGET_TEMPLATE_BIND_CHILD (Window, label);

  auto scope = BuilderScope::create ();
  scope->add_callback ("button_clicked_cb", &Window::button_clicked_cb);
  set_template_scope (scope);
}

inline void
Window::init (Class *)
{
  init_template ();
}

inline void
Window::vfunc_dispose ()
{
  dispose_template (Type::of<Window> ());
  parent_vfunc_dispose<Window> ();
}

void
Window::button_clicked_cb (Gtk::Button *)
{
  char buffer[100];
  snprintf (buffer, sizeof (buffer), "You clicked the button %d times", ++click_count);
  label->set_label (buffer);
}

}

int
main ()
{
  Gtk::init ();

  Demo::Window *window = Object::create<Demo::Window> ();
  window->present ();

  GLib::MainContext *context = GLib::MainContext::default_ ();
  while (true)
    context->iteration (true);
}
