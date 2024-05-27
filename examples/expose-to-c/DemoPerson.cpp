#include "DemoPerson.hpp"
#include <new>

namespace Demo
{

PEEL_CLASS_IMPL (Person, "DemoPerson", peel::Object)

inline void
Person::init (Class *)
{
  new (&first_name) std::string;
  new (&last_name) std::string;
}

inline void
Person::Class::init ()
{
}

void
Person::set_first_name (const char *name)
{
  if (first_name == name)
    return;
  first_name = name;
  notify (prop_first_name ());
  notify (prop_full_name ());
}

void
Person::set_last_name (const char *name)
{
  if (last_name == name)
    return;
  last_name = name;
  notify (prop_last_name ());
  notify (prop_full_name ());
}

void
Person::get_full_name (peel::Value *value)
{
  std::string full_name = first_name + " " + last_name;
  value->set<const char *> (full_name.c_str ());
}

peel::RefPtr<Person>
Person::create (const char *first_name, const char *last_name)
{
  return peel::Object::create<Person> (
    prop_first_name (), first_name,
    prop_last_name (), last_name);
}

}
