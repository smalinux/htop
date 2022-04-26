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
#include "Platform.h"
#include "Hashtable.h"
#include "XUtils.h"

Generic* PCPGeneric_new(const Settings* settings) {
   PCPGeneric* this = xCalloc(1, sizeof(PCPGeneric));
   Object_setClass(this, Class(PCPGeneric));

   // SMA: Start Hard code
   // this->GenericFieldsCount = settings->ss->ColumnsCount; // SMA do it
   this->GenericFieldsCount = 3;
   this->genericFields = Hashtable_new(3, false); // 3 -> 0  && add/remove via Columns panel

   // SMA this should be done via PCPMetric_iterate,
   // SMA this function just for mallocs...
   pmAtomValue atom = {.ul = 1};
   PCPGenericField gf = {.index = 1, .offset = 1, .pmid = 111, .value = &atom};
   Hashtable_put(this->genericFields, 0, &gf);
   // SMA: End Hard code

   Generic_init(&this->super, settings);
   return &this->super;
}

static void PCPGeneric_writeField(const Process* this, RichString* str, int field) {
   fprintf(stderr, "PCPGeneric_writeField\n");
}

void PCPGeneric_display(const Object* cast, RichString* out) {
   fprintf(stderr, "PCPGeneric_display\n");
}

const GenericClass PCPGeneric_class = {
   .super = {
      .extends = Class(Generic),
      .display = PCPGeneric_display,
      //.delete = PCPGeneric_delete,
      //.compare = PCPGeneric_compare,
   },
   .writeField = PCPGeneric_writeField,
   //.compareByKey = PCPGeneric_compareByKey,
};
