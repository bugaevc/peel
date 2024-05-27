// This is a C file that uses DemoPerson.
#include "DemoPerson.h"

int
main ()
{
  DemoPerson *person1 = demo_person_new ("John", "Doe");
  g_assert (G_IS_OBJECT (person1));
  g_assert (DEMO_IS_PERSON (person1));
  g_print ("G_OBJECT_TYPE_NAME -> %s\n", G_OBJECT_TYPE_NAME (person1));
  g_print ("demo_person_get_first_name -> %s\n",
    demo_person_get_first_name (person1));
  g_object_unref (person1);

  DemoPerson *person2 = g_object_new (DEMO_TYPE_PERSON,
    "first-name", "Jane",
    "last-name", "Doe",
    NULL);

  char *full_name = NULL;
  g_object_get (person2,
    "full-name", &full_name,
    NULL);
  g_print ("full-name -> %s\n", full_name);
  g_free (full_name);

  GType type = G_OBJECT_TYPE (person2);
  g_assert (type == DEMO_TYPE_PERSON);
  g_object_unref (person2);
}
