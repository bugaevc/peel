#pragma once

#include <peel/lang.h>
#include <glib.h>

peel_begin_header

namespace peel
{
namespace GLib
{

struct RWLock;

class RWLockReaderLocker final
{
private:
  RWLockReaderLocker (const RWLockReaderLocker &) = delete;
  RWLockReaderLocker &
  operator = (RWLockReaderLocker &other) = delete;

  ::GRWLock *mutex;

public:
  constexpr RWLockReaderLocker () noexcept
    : mutex (nullptr)
  { }

  explicit RWLockReaderLocker (GLib::RWLock &m) noexcept
    : mutex (reinterpret_cast<::GRWLock *> (&m))
  {
    g_rw_lock_reader_lock (mutex);
  }

  explicit RWLockReaderLocker (GLib::RWLock *m) noexcept
    : mutex (reinterpret_cast<::GRWLock *> (m))
  {
    g_rw_lock_reader_lock (mutex);
  }

  ~RWLockReaderLocker () noexcept
  {
    unlock ();
  }

  void
  unlock () noexcept
  {
    if (mutex)
      g_rw_lock_reader_unlock (mutex);
    mutex = nullptr;
  }

  RWLockReaderLocker (RWLockReaderLocker &&other) noexcept
  {
    mutex = other.mutex;
    other.mutex = nullptr;
  }

  RWLockReaderLocker &
  operator = (RWLockReaderLocker &&other) noexcept
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
