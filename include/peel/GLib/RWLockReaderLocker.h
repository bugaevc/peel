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
  RWLockReaderLocker () = delete;
  RWLockReaderLocker (const RWLockReaderLocker &) = delete;
  RWLockReaderLocker (RWLockReaderLocker &&) = delete;

  ::GRWLock *mutex;

public:
  explicit RWLockReaderLocker (GLib::RWLock *m) noexcept
    : mutex (reinterpret_cast<::GRWLock *> (m))
  {
    g_rw_lock_reader_lock (mutex);
  }

  ~RWLockReaderLocker () noexcept
  {
    g_rw_lock_reader_unlock (mutex);
  }
};

} /* namespace GLib */
} /* namespace peel */

peel_end_header
