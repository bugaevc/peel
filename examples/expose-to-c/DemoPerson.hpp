#pragma once
// This is the C++ header for Demo::Person.

#include <peel/GObject/Object.h>
#include <peel/GObject/Value.h>
#include <peel/class.h>
#include <string>

namespace Demo
{

class Person final : public peel::Object
{
  PEEL_SIMPLE_CLASS (Person, peel::Object)

  inline void
  init (Class *);

  std::string first_name;
  std::string last_name;

  void
  get_full_name (peel::Value *);

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
    return first_name.c_str ();
  }

  void
  set_first_name (const char *);

  PEEL_PROPERTY (const char *, first_name, "first-name")

  const char *
  get_last_name ()
  {
    return last_name.c_str ();
  }

  void
  set_last_name (const char *);

  PEEL_PROPERTY (const char *, last_name, "last-name")

  PEEL_PROPERTY (const char *, full_name, "full-name")

  static peel::RefPtr<Person>
  create (const char *first_name, const char *last_name);
};

}
