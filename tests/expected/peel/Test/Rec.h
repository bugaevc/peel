#pragma once

/* Auto-generated, do not modify */
/* Package peel-test */

#include <peel/GObject/Type.h>
#include <peel/RefPtr.h>
#include <peel/FloatPtr.h>
#include <peel/UniquePtr.h>
#include <peel/ArrayRef.h>
#include <peel/signal.h>
#include <peel/callback.h>
#include <peel/property.h>
#include <peel/lang.h>
#include <cstdint>
#include <utility>
#include <peel-test/peel-test.h>

peel_begin_header

namespace peel
{
namespace Test
{
class /* record */ Rec;
} /* namespace Test */


namespace Test
{
class /* record */ Rec
{
private:
  Rec () = delete;
  Rec (const Rec &) = delete;
  Rec (Rec &&) = delete;
  ~Rec ();

public:
  peel_arg_inout (2) peel_nonnull_args (2)
  void
  pass_inout_int (int *i) noexcept
  {
    ::TestRec * _peel_this = reinterpret_cast<::TestRec *> (this);
    test_rec_pass_inout_int (_peel_this, i);
  }

  void
  const_self () const noexcept
  {
    const ::TestRec * _peel_this = reinterpret_cast<const ::TestRec *> (this);
    test_rec_const_self (_peel_this);
  }

  void
  self_gconstpointer () const noexcept
  {
    ::gconstpointer _peel_this = reinterpret_cast<::gconstpointer> (this);
    test_rec_self_gconstpointer (_peel_this);
  }

  void
  self_gpointer () noexcept
  {
    ::gpointer _peel_this = reinterpret_cast<::gpointer> (this);
    test_rec_self_gpointer (_peel_this);
  }
}; /* record Rec */

} /* namespace Test */
} /* namespace peel */

peel_end_header
