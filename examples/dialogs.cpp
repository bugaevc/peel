#include <peel/Gtk/Gtk.h>
#include <peel/Gdk/RGBA.h>
#include <peel/GLib/MainContext.h>

using namespace peel;

int
main ()
{
  Gtk::init ();

  FloatPtr<Gtk::Box> vbox = Gtk::Box::create (Gtk::Orientation::VERTICAL, 6);
  FloatPtr<Gtk::Box> hbox = Gtk::Box::create (Gtk::Orientation::HORIZONTAL, 6);
  FloatPtr<Gtk::Label> label = Gtk::Label::create ("Click one of the buttons");

  FloatPtr<Gtk::Button> pick_color_button = Gtk::Button::create_with_label ("Pick color");
  pick_color_button->connect_clicked (
    // Capture a raw pointer to the label, as opposed to making a copy
    // of the FloatPtr. This specific capture syntax requires C++ 14.
    [label = (Gtk::Label *) label] (Gtk::Button *button)
    {
      RefPtr<Gtk::ColorDialog> dialog = Gtk::ColorDialog::create ();
      Gtk::Window *window = button->get_root ()->cast<Gtk::Window> ();
      // https://docs.gtk.org/gtk4/method.ColorDialog.choose_rgba.html
      dialog->choose_rgba (window, nullptr, nullptr,
        [label] (Object *obj, Gio::AsyncResult *res)
        {
          Gtk::ColorDialog *dialog = obj->cast<Gtk::ColorDialog> ();
          // https://docs.gtk.org/gtk4/method.ColorDialog.choose_rgba_finish.html
          UniquePtr<GLib::Error> error;
          UniquePtr<Gdk::RGBA> color = dialog->choose_rgba_finish (res, &error);

          char *text;
          if (error)
            text = g_strdup_printf ("Failed to pick a color: %s", error->message);
          else
            text = g_strdup_printf ("Picked rgba (%f, %f, %f, %f)",
                                    color->red, color->green,
                                    color->blue, color->alpha);
          label->set_label (text);
          g_free (text);
        });
    }
  );
  hbox->append (std::move (pick_color_button));

  FloatPtr<Gtk::Button> pick_file_button = Gtk::Button::create_with_label ("Pick file");
  pick_file_button->connect_clicked (
    [label = (Gtk::Label *) label] (Gtk::Button *button)
    {
      RefPtr<Gtk::FileDialog> dialog = Gtk::FileDialog::create ();
      Gtk::Window *window = button->get_root ()->cast<Gtk::Window> ();
      // https://docs.gtk.org/gtk4/method.FileDialog.open.html
      dialog->open (window, nullptr,
        [label] (Object *obj, Gio::AsyncResult *res)
        {
          Gtk::FileDialog *dialog = obj->cast<Gtk::FileDialog> ();
          // https://docs.gtk.org/gtk4/method.FileDialog.open_finish.html
          UniquePtr<GLib::Error> error;
          RefPtr<Gio::File> file = dialog->open_finish (res, &error);
          char *text;
          if (error)
            text = g_strdup_printf ("Failed to pick a file: %s", error->message);
          else
            text = g_strdup_printf ("Picked %s", file->get_path ());
          label->set_label (text);
          g_free (text);
        });
    });
  hbox->append (std::move (pick_file_button));

  FloatPtr<Gtk::Button> pick_font_button = Gtk::Button::create_with_label ("Pick font");
  pick_font_button->connect_clicked (
    [label = (Gtk::Label *) label] (Gtk::Button *button)
    {
      RefPtr<Gtk::FontDialog> dialog = Gtk::FontDialog::create ();
      Gtk::Window *window = button->get_root ()->cast<Gtk::Window> ();
      // https://docs.gtk.org/gtk4/method.FontDialog.choose_face.html
      dialog->choose_face (window, nullptr, nullptr,
        [label] (Object *obj, Gio::AsyncResult *res)
        {
          Gtk::FontDialog *dialog = obj->cast<Gtk::FontDialog> ();
          // https://docs.gtk.org/gtk4/method.FontDialog.choose_face_finish.html
          UniquePtr<GLib::Error> error;
          RefPtr<Pango::FontFace> font_face = dialog->choose_face_finish (res, &error);

          char *text;
          if (error)
            text = g_strdup_printf ("Failed to pick a font: %s", error->message);
          else
            text = font_face->describe ()->to_string ();
          label->set_label (text);
          g_free (text);
        });
    });
  hbox->append (std::move (pick_font_button));

  vbox->append (std::move (hbox));
  vbox->append (std::move (label));

  bool should_stop = false;
  Gtk::Window *window = Gtk::Window::create ();
  window->set_title ("Dialogs");
  window->connect_destroy (
    [&should_stop] (Gtk::Widget *)
    {
      should_stop = true;
    });
  window->set_child (std::move (vbox));
  window->present ();

  // Spin the even loop until the window gets destroyed.
  GLib::MainContext *main_context = GLib::MainContext::default_ ();
  while (!should_stop)
    main_context->iteration (true);

  window->close ();
}
