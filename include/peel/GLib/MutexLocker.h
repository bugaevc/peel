#pragma once

#include <peel/lang.h>
#include <glib.h>

peel_begin_header

namespace peel
{
namespace GLib
{

union Mutex;

class MutexLocker final
{
private:
  MutexLocker () = delete;
  MutexLocker (const MutexLocker &) = delete;
  MutexLocker (MutexLocker &&) = delete;

  ::GMutex *mutex;

public:
  explicit MutexLocker (GLib::Mutex *m) noexcept
    : mutex (reinterpret_cast<::GMutex *> (m))
  {
    g_mutex_lock (mutex);
  }

  ~MutexLocker () noexcept
  {
    g_mutex_unlock (mutex);
  }
};

} /* namespace GLib */
} /* namespace peel */

peel_end_header
