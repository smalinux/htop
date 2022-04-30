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

   this->fields = Hashtable_new(0, false);

   Generic_init(&this->super, settings);
   return &this->super;
}

static void PCPGeneric_writeField(const Process* this, RichString* str, int field) {
   fprintf(stderr, "PCPGeneric_writeField\n");
}

void PCPGeneric_display(const Object* cast, RichString* out) {
   fprintf(stderr, "PCPGeneric_display\n");
}

PCPGenericField* PCPGeneric_addField(PCPGeneric* this, const Settings* settings)
{
   PCPGenericField* field = xCalloc(1, sizeof(PCPGenericField));
   pmAtomValue *atom = xCalloc(1, sizeof(pmAtomValue));

   Hashtable_put(this->fields, this->fieldsCount, field);

   field->value = atom;
   this->fieldsCount += 1;

    //settings here if you want to read from it.. to assing...
    //Reference: Process_init()
   return field;
}

void PCPGeneric_removeField(PCPGeneric* this)
{
   PCPGenericField* field = Hashtable_get(this->fields, this->fieldsCount);
   //free(field->value); // uncomment == segment fault

   Hashtable_remove(this->fields, this->fieldsCount);
   this->fieldsCount--;
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
