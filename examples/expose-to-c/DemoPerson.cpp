#include "DemoPerson.hpp"
#include <peel/GLib/functions.h>

namespace Demo
{

PEEL_CLASS_IMPL (Person, "DemoPerson", peel::Object)

inline void
Person::Class::init ()
{
}

void
Person::set_first_name (const char *name)
{
  if (first_name.set (name))
    {
      notify (prop_first_name ());
      notify (prop_full_name ());
    }
}

void
Person::set_last_name (const char *name)
{
  if (last_name.set (name))
    {
      notify (prop_last_name ());
      notify (prop_full_name ());
    }
}

peel::String
Person::get_full_name ()
{
  return peel::GLib::strdup_printf ("%s %s", (const char *) first_name, (const char *) last_name);
}

peel::RefPtr<Person>
Person::create (const char *first_name, const char *last_name)
{
  return peel::Object::create<Person> (
    prop_first_name (), first_name,
    prop_last_name (), last_name);
}

}
