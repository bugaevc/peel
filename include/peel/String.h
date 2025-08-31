#pragma once

#include <peel/lang.h>
#include <glib.h>

peel_begin_header

namespace peel
{

class String final
{
  char *str;

  void
  do_free () noexcept
  {
#ifdef __GNUC__
    /* If we can statically see it's nullptr, don't do anything. */
    if (__builtin_constant_p (str == nullptr) && (str == nullptr))
      return;
#endif
    g_free (str);
  }

public:
  constexpr String () noexcept
    : str (nullptr)
  { }

  constexpr String (decltype (nullptr)) noexcept
    : str (nullptr)
  { }

  String (String &&other) noexcept
    : str (other.str)
  {
    other.str = nullptr;
  }

  String (const String &other) noexcept
    : str (g_strdup (other.str))
  { }

  String (const char *str) noexcept
    : str (g_strdup (str))
  { }

  String (const char *ptr, size_t n_chars) noexcept
    : str (g_strndup (ptr, n_chars))
  { }

  ~String () noexcept
  {
    do_free ();
  }

  constexpr
  operator const char * () const noexcept
  {
    return str;
  }

  constexpr const char *
  c_str () const noexcept
  {
    return str;
  }

  explicit constexpr
  operator bool () const noexcept
  {
    return str != nullptr;
  }

  String &
  operator = (String &&other) noexcept
  {
    do_free ();
    str = other.str;
    other.str = nullptr;
    return *this;
  }

  String &
  operator = (const String &other) noexcept
  {
    do_free ();
    str = g_strdup (other.str);
    return *this;
  }

  bool
  set (const char *s) noexcept
  {
    /* This is basically g_set_str, but without requiring GLib 2.76. */
    if (str == s)
      return false;
    if (g_strcmp0 (str, s) == 0)
      return false;

    /* Must copy first, free later. */
    char *copy = g_strdup (s);
    do_free ();
    str = copy;
    return true;
  }

  bool
  set (String &&other) noexcept
  {
    if (g_strcmp0 (str, other.str))
      return false;

    do_free ();
    str = other.str;
    other.str = nullptr;
    return true;
  }

  bool
  operator == (const char *s) const noexcept
  {
    return g_strcmp0 (str, s) == 0;
  }

  bool
  operator != (const char *s) const noexcept
  {
    return g_strcmp0 (str, s) != 0;
  }

  static String
  adopt_string (char *str) noexcept
  {
    String s;
    s.str = str;
    return s;
  }

  peel_nodiscard ("the string will leak if unused")
  char *
  release_string () && noexcept
  {
    char *s = str;
    str = nullptr;
    return s;
  }
};

} /* namespace peel */

peel_end_header
