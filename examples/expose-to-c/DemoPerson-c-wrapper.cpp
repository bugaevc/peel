#include "DemoPerson.h"
#include "DemoPerson.hpp"

using namespace peel;

GType
demo_person_get_type ()
{
  return Type::of<Demo::Person> ();
}

const char *
demo_person_get_first_name (DemoPerson *person)
{
  return reinterpret_cast<Demo::Person *> (person)->get_first_name ();
}

void
demo_person_set_first_name (DemoPerson *person, const char *name)
{
  reinterpret_cast<Demo::Person *> (person)->set_first_name (name);
}

const char *
demo_person_get_last_name (DemoPerson *person)
{
  return reinterpret_cast<Demo::Person *> (person)->get_last_name ();
}

void
demo_person_set_last_name (DemoPerson *person, const char *name)
{
  reinterpret_cast<Demo::Person *> (person)->set_last_name (name);
}

DemoPerson *
demo_person_new (const char *first_name, const char *last_name)
{
  RefPtr<Demo::Person> person = Demo::Person::create (first_name, last_name);
  return reinterpret_cast<DemoPerson *> (std::move (person).release_ref ());
}
