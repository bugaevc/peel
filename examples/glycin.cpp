#include <peel/Gly/Loader.h>
#include <peel/GlyGtk4/functions.h>
#include <peel/Gtk/Window.h>
#include <peel/Gtk/Picture.h>
#include <peel/Gtk/functions.h>
#include <peel/Gdk/Texture.h>
#include <peel/Gdk/Paintable.h>
#include <peel/Gio/File.h>
#include <peel/GLib/MainContext.h>

using namespace peel;

int
main (int argc, char *argv[])
{
  UniquePtr<GLib::Error> error;

  if (argc != 2)
    {
      g_printerr ("Usage: %s image-file\n", argv[0]);
      return 1;
    }

  Gtk::init ();

  RefPtr<Gio::File> file = Gio::File::create_for_commandline_arg (argv[1]);
  RefPtr<Gly::Loader> loader = Gly::Loader::create (file);
  RefPtr<Gly::Image> image = loader->load (&error);
  if (error)
    {
      g_printerr ("Failed to load image: %s\n", error->message);
      return 1;
    }
  g_print ("MIME type: %s\n", image->get_mime_type ());
  RefPtr<Gly::Frame> frame = image->next_frame (&error);
  if (error)
    {
      g_printerr ("Failed to load frame: %s\n", error->message);
      return 1;
    }
  RefPtr<Gdk::Texture> texture = GlyGtk4::frame_get_texture (frame);

  Gtk::Window *window = Gtk::Window::create ();
  window->set_child (Gtk::Picture::create_for_paintable (texture->cast<Gdk::Paintable> ()));
  window->present ();

  GLib::MainContext *context = GLib::MainContext::default_ ();
  while (true)
    context->iteration (true);
}
