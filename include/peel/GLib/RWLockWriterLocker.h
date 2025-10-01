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
  RWLockWriterLocker () = delete;
  RWLockWriterLocker (const RWLockWriterLocker &) = delete;
  RWLockWriterLocker (RWLockWriterLocker &&) = delete;

  ::GRWLock *mutex;

public:
  explicit RWLockWriterLocker (GLib::RWLock *m) noexcept
    : mutex (reinterpret_cast<::GRWLock *> (m))
  {
    g_rw_lock_writer_lock (mutex);
  }

  ~RWLockWriterLocker () noexcept
  {
    g_rw_lock_writer_unlock (mutex);
  }
};

} /* namespace GLib */
} /* namespace peel */

peel_end_header
