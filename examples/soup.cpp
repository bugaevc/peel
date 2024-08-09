#include <peel/Soup/Session.h>
#include <peel/Soup/Message.h>
#include <peel/Soup/MessageHeaders.h>
#include <peel/GLib/Error.h>
#include <peel/Gio/File.h>
#include <peel/Gio/InputStream.h>

using namespace peel;

int
main (int argc, char **argv)
{
  const char *url = (argc > 1) ? argv[1] : "https://httpcats.com/200.jpg";
  UniquePtr<GLib::Error> error;

  RefPtr<Soup::Session> session = Soup::Session::create ();
  RefPtr<Soup::Message> message = Soup::Message::create (SOUP_METHOD_GET, url);

  RefPtr<Gio::InputStream> input_stream = session->send (message, nullptr, &error);
  if (G_UNLIKELY (error))
    {
      g_printerr ("Failed to request %s: %s\n", url, error->message);
      return 1;
    }

  RefPtr<Gio::FileIOStream> file_io_stream;
  RefPtr<Gio::File> file = Gio::File::create_tmp ("output-XXXXXX", &file_io_stream, &error);
  if (G_UNLIKELY (error))
    {
      g_printerr ("Failed to create a temp file: %s\n", error->message);
      return 1;
    }

  Soup::MessageHeaders *response_headers = message->get_response_headers ();
  g_print ("Fetching %s of type %s to %s (%zu bytes)\n", url,
    response_headers->get_content_type (nullptr),
    file->peek_path (),
    response_headers->get_content_length ());

  file_io_stream->get_output_stream ()->splice (input_stream,
    Gio::OutputStream::SpliceFlags::CLOSE_SOURCE | Gio::OutputStream::SpliceFlags::CLOSE_TARGET,
    nullptr, &error);
  if (G_UNLIKELY (error))
    {
      g_printerr ("Failed to download: %s\n", error->message);
      return 1;
    }
}
