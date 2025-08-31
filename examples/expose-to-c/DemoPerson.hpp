#pragma once
// This is the C++ header for Demo::Person.

#include <peel/GObject/Object.h>
#include <peel/String.h>
#include <peel/class.h>

namespace Demo
{

class Person final : public peel::Object
{
  PEEL_SIMPLE_CLASS (Person, peel::Object)

  peel::String first_name;
  peel::String last_name;

  template<typename F>
  static void
  define_properties (F &f)
  {
    f.prop (prop_first_name (), nullptr)
      .get (&Person::get_first_name)
      .set (&Person::set_first_name);
    f.prop (prop_last_name (), nullptr)
      .get (&Person::get_last_name)
      .set (&Person::set_last_name);
    f.prop (prop_full_name (), nullptr)
      .get (&Person::get_full_name);
  }

public:
  const char *
  get_first_name ()
  {
    return first_name;
  }

  void
  set_first_name (const char *);

  PEEL_PROPERTY (const char *, first_name, "first-name")

  const char *
  get_last_name ()
  {
    return last_name;
  }

  void
  set_last_name (const char *);

  PEEL_PROPERTY (const char *, last_name, "last-name")

  peel::String
  get_full_name ();

  PEEL_PROPERTY (const char *, full_name, "full-name")

  static peel::RefPtr<Person>
  create (const char *first_name, const char *last_name);
};

}
