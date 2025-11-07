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
  MutexLocker (const MutexLocker &) = delete;
  MutexLocker &
  operator = (MutexLocker &other) = delete;

  ::GMutex *mutex;

public:
  constexpr MutexLocker () noexcept
    : mutex (nullptr)
  { }

  explicit MutexLocker (GLib::Mutex &m) noexcept
    : mutex (reinterpret_cast<::GMutex *> (&m))
  {
    g_mutex_lock (mutex);
  }

  explicit MutexLocker (GLib::Mutex *m) noexcept
    : mutex (reinterpret_cast<::GMutex *> (m))
  {
    g_mutex_lock (mutex);
  }

  ~MutexLocker () noexcept
  {
    unlock ();
  }

  void
  unlock () noexcept
  {
    if (mutex)
      g_mutex_unlock (mutex);
    mutex = nullptr;
  }

  MutexLocker (MutexLocker &&other) noexcept
  {
    mutex = other.mutex;
    other.mutex = nullptr;
  }

  MutexLocker &
  operator = (MutexLocker &&other) noexcept
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
