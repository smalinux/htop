#ifndef HEADER_PCPGeneric
#define HEADER_PCPGeneric
/*
htop - PCPGeneric.h
(C) 2022 Sohaib Mohammed
(C) 2022 htop dev team
(C) 2022 Red Hat, Inc.
Released under the GNU GPLv2+, see the COPYING file
in the source distribution for its full text.
*/

#include "config.h" // IWYU pragma: keep

#include <stdbool.h>

#include "Object.h"
#include "Generic.h"
#include "Settings.h"

typedef struct PCPGeneric_ {
   Generic super;
   // SMALINUX: Define node & vector .. here :)
} PCPGeneric;

void PCPGeneric_delete(Object* cast);
void PCPGeneric_display(const Object* cast, RichString* out);
int PCPGeneric_compare(const void* v1, const void* v2);

extern const GenericClass PCPGeneric_class;

Generic* PCPGeneric_new(const Settings* settings);

void Generic_delete(Object* cast);

#endif
