/*
 * Adapted from libdex:examples/tcp-echo.c, which is:
 * Copyright 2022 Christian Hergert <chergert@redhat.com>
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <peel/Dex/Dex.h>
#include <peel/Gio/Gio.h>
#include <peel/GLib/Error.h>
#include <peel/GLib/MainLoop.h>

using namespace peel;

static RefPtr<Dex::Scheduler> thread_pool;

static RefPtr<Dex::Future>
socket_connection_fiber (Gio::SocketConnection *connection)
{
  Gio::InputStream *input = connection->get_input_stream ();
  Gio::OutputStream *output = connection->get_output_stream ();
  UniquePtr<GLib::Error> error;
  unsigned char buffer[1024];

  while (!error)
    {
      RefPtr<Dex::Future> future = Dex::input_stream_read (input, buffer, G_PRIORITY_DEFAULT);
      gssize n_read = Dex::Future::await_int64 (std::move (future), &error);
      if (n_read == 0 || error)
        break;

      ArrayRef<unsigned char> to_write { buffer, static_cast<size_t> (n_read) };

      while (to_write)
        {
          RefPtr<Dex::Future> future = Dex::output_stream_write (output, to_write, G_PRIORITY_HIGH);
          gssize n_written = Dex::Future::await_int64 (std::move (future), &error);
          if (n_written == 0 || error)
            break;
          to_write = to_write.slice (n_written, to_write.size () - n_written);
        }
    }

  return error ? Dex::Future::create_for_error (std::move (error)) : nullptr;
}

static RefPtr<Dex::Future>
socket_listener_fiber (Gio::SocketListener *socket_listener)
{
  while (true)
    {
      UniquePtr<GLib::Error> error;

      RefPtr<Dex::Future> future = Dex::socket_listener_accept (socket_listener);
      RefPtr<Gio::SocketConnection> connection = Dex::Future::await_object (std::move (future), &error).cast<Gio::SocketConnection> ();
      if (!connection)
        return Dex::Future::create_for_error (std::move (error));

      thread_pool->spawn (0,
        [connection]
        {
          return socket_connection_fiber (connection);
        });
    }
  return nullptr;
}

int
main ()
{
  Dex::init ();

  UniquePtr<GLib::Error> error;

  RefPtr<GLib::MainLoop> main_loop = GLib::MainLoop::create (nullptr, false);
  thread_pool = Dex::ThreadPoolScheduler::create ();
  RefPtr<Gio::SocketListener> socket_listener = Gio::SocketListener::create ();

  unsigned port = 8080;

  if (!socket_listener->add_inet_port (port, nullptr, &error))
    g_error ("Failed to listen on port %u: %s", port, error->message);

  g_print ("Listening on 0.0.0.0:%u\n", port);

  RefPtr<Dex::Future> future = Dex::Scheduler::get_default ()->spawn (0,
    [socket_listener]
    {
      return socket_listener_fiber (socket_listener);
    });

  future = Dex::Future::finally_ (std::move (future),
    [main_loop] (Dex::Future *completed) -> RefPtr<Dex::Future>
    {
      (void) completed;
      main_loop->quit ();
      return nullptr;
    });

  main_loop->run ();

  return 0;
}
