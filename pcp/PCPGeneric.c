/*
htop - Generic.c
(C) 2022 Sohaib Mohammed
(C) 2022 htop dev team
(C) 2022 Red Hat, Inc.
Released under the GNU GPLv2+, see the COPYING file
in the source distribution for its full text.
*/

#include "config.h" // IWYU pragma: keep

#include "pcp/PCPGeneric.h"

#include "RichString.h"
#include "CRT.h"
#include "Macros.h"
#include "Generic.h"
#include "ProvideCurses.h"
#include "RichString.h"
#include "XUtils.h"


void PCPGeneric_display(const Object* cast, RichString* out) {
   fprintf(stderr, "hiiiiiiiiii\n");
}

const GenericClass PCPGeneric_class = {
   .super = {
      .extends = Class(Generic),
      .display = PCPGeneric_display,
      //.delete = PCPGeneric_delete,
      //.compare = PCPGeneric_compare,
   },
   //.writeField = PCPGeneric_writeField,
   //.compareByKey = PCPGeneric_compareByKey,
};
