#pragma once

#include <peel/lang.h>
#include <cstdint>
#include <glib.h>

peel_begin_header

namespace peel
{
namespace GLib
{

struct Quark final
{
public:
  uint32_t quark;

  constexpr
  Quark () noexcept
    : quark (0)
  { }

  constexpr
  Quark (::GQuark quark) noexcept
    : quark (quark)
  { }

  constexpr
  operator ::GQuark () const noexcept
  {
    return quark;
  }

  Quark (const char *string) noexcept
  {
#ifdef __GNUC__
    if ((__builtin_constant_p (!string) && !string) || __builtin_constant_p (__builtin_strlen (string)))
      quark = g_quark_from_static_string (string);
    else
#endif
      quark = g_quark_from_string (string);
  }

  static Quark
  create_from_static_string (const char *string) noexcept
  {
    return g_quark_from_static_string (string);
  }

  static Quark
  try_string (const char *string) noexcept
  {
    return g_quark_try_string (string);
  }

  const char *
  to_string () const noexcept
  {
    return g_quark_to_string (quark);
  }
};

static_assert (sizeof (Quark) == sizeof (::GQuark),
               "Quark size mismatch");
static_assert (alignof (Quark) == alignof (::GQuark),
               "Quark align mismatch");

} /* namespace GLib */
} /* namespace peel */

peel_end_header
