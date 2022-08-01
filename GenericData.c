/*
htop - GenericData.c
(C) 2022 Sohaib Mohammed
(C) 2022 htop dev team
(C) 2022 Red Hat, Inc.
Released under the GNU GPLv2+, see the COPYING file
in the source distribution for its full text.
*/

#include "config.h" // IWYU pragma: keep

#include "GenericData.h"

#include <assert.h>

#include "Macros.h"
#include "Process.h"
#include "RichString.h"
#include "Settings.h"


void GenericData_init(GenericData* this, const Settings* settings) {
   this->settings = settings;
}

void GenericData_done(ATTR_UNUSED GenericData* this) {
   assert (this != NULL);
}

void GenericData_writeField(ATTR_UNUSED const GenericData* this, ATTR_UNUSED RichString* str, ATTR_UNUSED int field) {
   return;
}

void GenericData_display(const Object* cast, RichString* out) {
   const GenericData* this = (const GenericData*) cast;
   const ProcessField* fields = this->settings->ss->fields;
   for (int i = 0; fields[i]; i++)
      As_GenericData(this)->writeField(this, out, i);
}

int GenericData_compare(ATTR_UNUSED const void* v1, ATTR_UNUSED const void* v2) {
   return 0;
}

const GenericDataClass GenericData_class = {
   .super = {
      .extends = Class(Object),
      .display = GenericData_display,
      .compare = GenericData_compare,
   },
   .writeField = GenericData_writeField,
};
