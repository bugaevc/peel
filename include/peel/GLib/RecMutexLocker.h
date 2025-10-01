#pragma once

#include <peel/lang.h>
#include <glib.h>

peel_begin_header

namespace peel
{
namespace GLib
{

struct RecMutex;
struct RWLock;

class RecMutexLocker final
{
private:
  RecMutexLocker () = delete;
  RecMutexLocker (const RecMutexLocker &) = delete;
  RecMutexLocker (RecMutexLocker &&) = delete;

  ::GRecMutex *mutex;

public:
  explicit RecMutexLocker (GLib::RecMutex *m) noexcept
    : mutex (reinterpret_cast<::GRecMutex *> (m))
  {
    g_rec_mutex_lock (mutex);
  }

  ~RecMutexLocker () noexcept
  {
    g_rec_mutex_unlock (mutex);
  }
};

} /* namespace GLib */
} /* namespace peel */

peel_end_header
