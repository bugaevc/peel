#pragma once
// This is a plain C header file, declaring
// the DemoPerson class, as seen from C.
#include <glib-object.h>

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE (DemoPerson, demo_person, DEMO, PERSON, GObject)
#define DEMO_TYPE_PERSON demo_person_get_type ()

const char *demo_person_get_first_name (DemoPerson *person);
void demo_person_set_first_name (DemoPerson *person, const char *name);
const char *demo_person_get_last_name (DemoPerson *person);
void demo_person_set_last_name (DemoPerson *person, const char *name);

DemoPerson *demo_person_new (const char *first_name, const char *last_name);

G_END_DECLS
