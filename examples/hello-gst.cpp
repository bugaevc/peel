#include <peel/Gst/Gst.h>
#include <peel/GLib/MainLoop.h>

using namespace peel;

int
main (int argc, char *argv[])
{
  Gst::init (nullptr);

  RefPtr<GLib::MainLoop> loop = GLib::MainLoop::create (nullptr, false);

  RefPtr<Gst::Element> pipeline = Gst::parse_launch ("audiotestsrc num-buffers=100 ! autoaudiosink", nullptr);
  RefPtr<Gst::Bus> bus = pipeline->get_bus ();

  GLib::MainLoop *loop_ptr = loop;
  bus->add_watch_full (G_PRIORITY_DEFAULT,
    [loop_ptr] (Gst::Bus *bus, Gst::Message *msg)
    {
      switch (msg->type)
        {
        case Gst::Message::Type::ERROR_:
          g_printerr ("ERROR\n");
          loop_ptr->quit ();
          break;
        case Gst::Message::Type::EOS:
          g_printerr ("EOS\n");
          loop_ptr->quit ();
          break;
        default:
          break;
        }

      return true;
    });

  pipeline->set_state (Gst::State::PLAYING);

  loop->run ();

  pipeline->set_state (Gst::State::NULL_);

  return 0;
}
