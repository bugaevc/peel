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
  RecMutexLocker (const RecMutexLocker &) = delete;
  RecMutexLocker &
  operator = (RecMutexLocker &other) = delete;

  ::GRecMutex *mutex;

public:
  constexpr RecMutexLocker () noexcept
    : mutex (nullptr)
  { }

  explicit RecMutexLocker (GLib::RecMutex &m) noexcept
    : mutex (reinterpret_cast<::GRecMutex *> (&m))
  {
    g_rec_mutex_lock (mutex);
  }

  explicit RecMutexLocker (GLib::RecMutex *m) noexcept
    : mutex (reinterpret_cast<::GRecMutex *> (m))
  {
    g_rec_mutex_lock (mutex);
  }

  ~RecMutexLocker () noexcept
  {
    unlock ();
  }

  void
  unlock () noexcept
  {
    if (mutex)
      g_rec_mutex_unlock (mutex);
    mutex = nullptr;
  }

  RecMutexLocker (RecMutexLocker &&other) noexcept
  {
    mutex = other.mutex;
    other.mutex = nullptr;
  }

  RecMutexLocker &
  operator = (RecMutexLocker &&other) noexcept
  {
    unlock ();
    mutex = other.mutex;
    other.mutex = nullptr;
    return *this;
  }
};

} /* namespace GLib */
} /* namespace peel */

peel_end_header
