#include <peel/coro/AsyncResult.h>
#include <peel/coro/SimpleTask.h>
#include <peel/Gio/File.h>
#include <peel/Gio/FileInputStream.h>
#include <peel/GLib/Bytes.h>
#include <peel/GLib/MainContext.h>

using namespace peel;

coro::SimpleTask
read_file (RefPtr<Gio::File> file)
{
  coro::AsyncResult async_result;
  UniquePtr<GLib::Error> error;

  file->read_async (G_PRIORITY_DEFAULT, nullptr, async_result.callback ());
  RefPtr<Gio::FileInputStream> stream = file->read_finish (co_await async_result, &error);
  if (error) [[unlikely]]
    {
      g_print ("Error reading file: %s\n", error->message);
      co_return;
    }

  stream->read_bytes_async (100, G_PRIORITY_DEFAULT, nullptr, async_result.callback ());
  RefPtr<GLib::Bytes> bytes = stream->read_bytes_finish (co_await async_result, &error);
  if (error) [[unlikely]]
    {
      g_print ("Error reading file: %s\n", error->message);
      co_return;
    }

  g_print ("Read %d bytes\n", bytes->get_size ());
}

int
main ()
{
  GLib::MainContext *context = GLib::MainContext::default_ ();
  read_file (Gio::File::create_for_path ("/etc/passwd"));
  while (true)
    context->iteration (true);
}
