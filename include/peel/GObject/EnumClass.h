#pragma once

#include <peel/GObject/TypeClass.h>
#include <peel/ArrayRef.h>
#include <peel/lang.h>
#include <glib-object.h>

peel_begin_header

namespace peel
{
namespace GObject
{
struct EnumValue;
} /* namespace GObject */


namespace GObject
{
class EnumClass : public TypeClass
{
private:
  EnumClass () = delete;
  EnumClass (const EnumClass &) = delete;
  EnumClass (EnumClass &&) = delete;
  ~EnumClass ();

public:
  int minimum;
  int maximum;
private:
  unsigned n_values;
  EnumValue *values;

public:
  peel::ArrayRef<EnumValue>
  get_values () noexcept
  {
      return peel::ArrayRef<EnumValue> (this->values, this->n_values);
  }

  const EnumValue *
  get_value (int value) noexcept
  {
      return reinterpret_cast<const EnumValue *> (g_enum_get_value (reinterpret_cast<::GEnumClass*> (this), value));
  }

  peel_nonnull_args (2)
  const EnumValue *
  get_value_by_name (const char *name) noexcept
  {
      return reinterpret_cast<const EnumValue *> (g_enum_get_value_by_name (reinterpret_cast<::GEnumClass*> (this), name));
  }

  peel_nonnull_args (2)
  const EnumValue *
  get_value_by_nick (const char *nick) noexcept
  {
      return reinterpret_cast<const EnumValue *> (g_enum_get_value_by_name (reinterpret_cast<::GEnumClass*> (this), nick));
  }
}; /* record EnumClass */

static_assert (sizeof (EnumClass) == sizeof (::GEnumClass),
               "EnumClass size mismatch");
static_assert (alignof (EnumClass) == alignof (::GEnumClass),
               "EnumClass align mismatch");

} /* namespace GObject */
} /* namespace peel */

peel_end_header
