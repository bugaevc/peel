#pragma once

/* Auto-generated, do not modify */
/* Package peel-test */

#include <peel/GObject/Type.h>
#include <peel/RefPtr.h>
#include <peel/FloatPtr.h>
#include <peel/UniquePtr.h>
#include <peel/ArrayRef.h>
#include <peel/String.h>
#include <peel/signal.h>
#include <peel/callback.h>
#include <peel/property.h>
#include <peel/lang.h>
#include <cstdint>
#include <utility>
#include <peel-test/peel-test.h>
#include <peel/GObject/Value.h>

peel_begin_header

namespace peel
{
namespace GObject
{
class Object;
struct Value;
} /* namespace GObject */

namespace Test
{
struct Rec;
} /* namespace Test */


namespace Test
{
struct Rec
{
private:
  Rec () = delete;
  Rec (const Rec &) = delete;
  Rec (Rec &&) = delete;
  ~Rec ();

public:
  int int_field;
  gboolean bool_field;
  GObject::Value inline_value_field;
  GObject::Value *value_pointer_field;
  GObject::Object *object_field;
  int array_of_ints[4];
  GObject::Value array_of_inline_values[4];
  GObject::Value *array_of_value_pointers[4];
  GObject::Object *array_of_object_pointers[4];

  peel_arg_inout (2) peel_nonnull_args (2)
  void
  pass_inout_int (int *i) noexcept
  {
    ::TestRec *_peel_this = reinterpret_cast<::TestRec *> (this);
    test_rec_pass_inout_int (_peel_this, i);
  }

  void
  const_self () const noexcept
  {
    const ::TestRec *_peel_this = reinterpret_cast<const ::TestRec *> (this);
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

static_assert (sizeof (Rec) == sizeof (::TestRec),
               "Rec size mismatch");
static_assert (alignof (Rec) == alignof (::TestRec),
               "Rec align mismatch");

} /* namespace Test */
} /* namespace peel */

peel_end_header
