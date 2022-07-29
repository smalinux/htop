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
#include <limits.h>
#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/resource.h>

#include "CRT.h"
#include "Macros.h"
#include "Platform.h"
#include "DynamicColumn.h"
#include "GenericDataList.h"
#include "RichString.h"
#include "Settings.h"
#include "XUtils.h"

#if defined(MAJOR_IN_MKDEV)
#include <sys/mkdev.h>
#endif


void GenericData_init(GenericData* this, const Settings* settings) {
   this->settings = settings;

   // this->
   // this->
}

void GenericData_done(GenericData* this) {
   assert (this != NULL);
   // freeFields here
   // Reference: Process_done
}

void GenericData_writeField(const GenericData* this, RichString* str, int field) {
   char buffer[256];
   size_t n = sizeof(buffer);
   int attr = CRT_colors[DEFAULT_COLOR];
   //fprintf(stderr, "- GenericData_writeField\n");

   switch (field) {
      case 0: {
                 //xSnprintf(buffer, n, "%*d ", Process_pidDigits, this->pid);
                 //xSnprintf(buffer, n, "%*d", 10, this->gtest);
                 break;
              }
      case 1: fprintf(stderr, "1- GenericData_writeField\n"); return;
      case 2: fprintf(stderr, "2- GenericData_writeField\n"); return;
   }
   //RichString_appendAscii(str, attr, buffer);
}

void GenericData_display(const Object* cast, RichString* out) {
   const GenericData* this = (const GenericData*) cast;
   const ProcessField* fields = this->settings->ss->fields;
   for (int i = 0; fields[i]; i++)
      As_GenericData(this)->writeField(this, out, i);

   assert(RichString_size(out) > 0);
}

int GenericData_compare(const void* v1, const void* v2) {
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
