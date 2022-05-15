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

   this->fieldsCount = 0;

   Generic_init(&this->super, settings);
   return &this->super;
}

void Generic_delete(Object* cast) {
   PCPGeneric* this = (PCPGeneric*) cast;

   for(int i = 0; i <= this->fieldsCount; i++)
      PCPGeneric_removeField(this);

   Generic_done((Generic*)cast);
   free(this);
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
   int idx = this->fieldsCount -1;

   PCPGenericField* field = Hashtable_get(this->fields, idx);
   free(field->value);
   Hashtable_remove(this->fields, idx);
   this->fieldsCount--;
}

static void PCPGeneric_writeField(const Generic* this, RichString* str, int field) {
   const PCPGeneric* gg = (const PCPGeneric*) this;
   bool coloring = this->settings->highlightMegabytes;
   char buffer[256]; buffer[255] = '\0';
   int attr = CRT_colors[DEFAULT_COLOR];

   int n = sizeof(buffer) - 1;
   fprintf(stderr, ".......PCPGeneric_writeField %d\n", gg->fieldsCount);
   Generic_writeField(this, str, field);
}

void PCPGeneric_display(const Object* cast, RichString* out) {
   fprintf(stderr, "PCPGeneric_display\n");
}

static int PCPGeneric_compareByKey(const Generic* v1, const Generic* v2, int key) {
   const PCPGeneric* g1 = (const PCPGeneric*)v1;
   const PCPGeneric* g2 = (const PCPGeneric*)v2;

   switch (key) {
      case 0:
         return 0;
      default:
         return 0;
   }
}

const GenericClass PCPGeneric_class = {
   .super = {
      .extends = Class(Generic),
      .display = Generic_display,
      .compare = Generic_compare,
   },
   .writeField = PCPGeneric_writeField,
   .compareByKey = PCPGeneric_compareByKey,
};
