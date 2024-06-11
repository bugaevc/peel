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

G_GNUC_BEGIN_IGNORE_DEPRECATIONS

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
  peel_nothrow peel_arg_inout (2) peel_nonnull_args (2)
  void
  pass_inout_int (int *i)
  {
    ::TestRec * _peel_this = reinterpret_cast<::TestRec *> (this);
    test_rec_pass_inout_int (_peel_this, i);
  }

  peel_nothrow
  void
  const_self () const
  {
    const ::TestRec * _peel_this = reinterpret_cast<const ::TestRec *> (this);
    test_rec_const_self (_peel_this);
  }

  peel_nothrow
  void
  self_gconstpointer () const
  {
    ::gconstpointer _peel_this = reinterpret_cast<::gconstpointer> (this);
    test_rec_self_gconstpointer (_peel_this);
  }

  peel_nothrow
  void
  self_gpointer ()
  {
    ::gpointer _peel_this = reinterpret_cast<::gpointer> (this);
    test_rec_self_gpointer (_peel_this);
  }
}; /* record Rec */

} /* namespace Test */
} /* namespace peel */

G_GNUC_END_IGNORE_DEPRECATIONS
