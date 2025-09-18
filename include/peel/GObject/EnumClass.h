#pragma once

#include <peel/GObject/TypeClass.h>
#include <peel/ArrayRef.h>
#include <peel/String.h>
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
struct Enum final
{
public:
  int value;

  constexpr
  Enum (int value) noexcept
    : value (value)
  {  }

  Enum (const EnumValue *value) noexcept
    : value (reinterpret_cast<const ::GEnumValue *> (value)->value)
  {  }

  constexpr
  operator int () const noexcept
  {
    return value;
  }

  class Class : public TypeClass
  {
  private:
    Class () = delete;
    Class (const Class &) = delete;
    Class (Class &&) = delete;
    ~Class ();

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

    peel::String
    to_string (int value) noexcept
    {
        return peel::String::adopt_string (g_enum_to_string (this->get_type (), value));
    }
  }; /* class Class */
}; /* struct Enum */

static_assert (sizeof (Enum::Class) == sizeof (::GEnumClass),
               "Enum::Class size mismatch");
static_assert (alignof (Enum::Class) == alignof (::GEnumClass),
               "Enum::Class align mismatch");

} /* namespace GObject */
} /* namespace peel */

peel_end_header
