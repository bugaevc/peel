#pragma once

#include <peel/lang.h>
#include <glib.h>

peel_begin_header

namespace peel
{
namespace GLib
{

struct RWLock;

class RWLockWriterLocker final
{
private:
  RWLockWriterLocker (const RWLockWriterLocker &) = delete;
  RWLockWriterLocker &
  operator = (RWLockWriterLocker &other) = delete;

  ::GRWLock *mutex;

public:
  constexpr RWLockWriterLocker () noexcept
    : mutex (nullptr)
  { }

  explicit RWLockWriterLocker (GLib::RWLock &m) noexcept
    : mutex (reinterpret_cast<::GRWLock *> (&m))
  {
    g_rw_lock_writer_lock (mutex);
  }

  explicit RWLockWriterLocker (GLib::RWLock *m) noexcept
    : mutex (reinterpret_cast<::GRWLock *> (m))
  {
    g_rw_lock_writer_lock (mutex);
  }

  ~RWLockWriterLocker () noexcept
  {
    unlock ();
  }

  void
  unlock () noexcept
  {
    if (mutex)
      g_rw_lock_writer_unlock (mutex);
    mutex = nullptr;
  }

  RWLockWriterLocker (RWLockWriterLocker &&other) noexcept
  {
    mutex = other.mutex;
    other.mutex = nullptr;
  }

  RWLockWriterLocker &
  operator = (RWLockWriterLocker &&other) noexcept
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
