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
struct FlagsValue;
} /* namespace GObject */


namespace GObject
{
struct Flags final
{
public:
  unsigned value;

  constexpr
  Flags (unsigned value) noexcept
    : value (value)
  { }

  Flags (const FlagsValue *value) noexcept
    : value (reinterpret_cast<const ::GFlagsValue *> (value)->value)
  { }

  constexpr operator unsigned () const noexcept
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
    unsigned mask;
  private:
    unsigned n_values;
    FlagsValue *values;

  public:
    peel::ArrayRef<FlagsValue>
    get_values () const noexcept
    {
        return peel::ArrayRef<FlagsValue> (this->values, this->n_values);
    }

    const FlagsValue *
    get_first_value (unsigned value) noexcept
    {
        return reinterpret_cast<const FlagsValue *> (g_flags_get_first_value (reinterpret_cast<::GFlagsClass*> (this), value));
    }

    peel_nonnull_args (2)
    const FlagsValue *
    get_value_by_name (const char *name) noexcept
    {
        return reinterpret_cast<const FlagsValue *> (g_flags_get_value_by_name (reinterpret_cast<::GFlagsClass*> (this), name));
    }

    peel_nonnull_args (2)
    const FlagsValue *
    get_value_by_nick (const char *nick) noexcept
    {
        return reinterpret_cast<const FlagsValue *> (g_flags_get_value_by_name (reinterpret_cast<::GFlagsClass*> (this), nick));
    }

    peel::String
    to_string (int value) noexcept
    {
        return peel::String::adopt_string (g_flags_to_string (this->get_type (), value));
    }
  }; /* class Class */
}; /* struct Flags */

static_assert (sizeof (Flags::Class) == sizeof (::GFlagsClass),
               "Flags::Class size mismatch");
static_assert (alignof (Flags::Class) == alignof (::GFlagsClass),
               "Flags::Class align mismatch");

} /* namespace GObject */
} /* namespace peel */

peel_end_header
