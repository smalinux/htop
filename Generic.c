/*
htop - Generic.c
(C) 2022 Sohaib Mohammed
(C) 2022 htop dev team
(C) 2022 Red Hat, Inc.
Released under the GNU GPLv2+, see the COPYING file
in the source distribution for its full text.
*/

#include "config.h" // IWYU pragma: keep

#include "Generic.h"

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
#include "GenericList.h"
#include "RichString.h"
#include "Settings.h"
#include "XUtils.h"

#if defined(MAJOR_IN_MKDEV)
#include <sys/mkdev.h>
#endif


void Generic_init(Generic* this, const Settings* settings) {
   this->settings = settings;

   // this->
   // this->
}

void Generic_done(Generic* this) {
   assert (this != NULL);
   // freeFields here
   // Reference: Process_done
}

void Generic_writeField(const Generic* this, RichString* str, int field) {
   char buffer[256];
   size_t n = sizeof(buffer);
   int attr = CRT_colors[DEFAULT_COLOR];
   //fprintf(stderr, "- Generic_writeField\n");

   switch (field) {
      case 0: {
   //xSnprintf(buffer, n, "%*d ", Process_pidDigits, this->pid);
   //xSnprintf(buffer, n, "%*d", 10, this->gtest);
   break;
              }
      case 1: fprintf(stderr, "1- Generic_writeField\n"); return;
      case 2: fprintf(stderr, "2- Generic_writeField\n"); return;
   }
   //RichString_appendAscii(str, attr, buffer);
}

void Generic_display(const Object* cast, RichString* out) {
   const Generic* this = (const Generic*) cast;
   const ProcessField* fields = this->settings->ss->fields;
   for (int i = 0; fields[i]; i++) // SMA use generic fields
      //As_Generic(this)->writeField(this, out, fields[i]); // 0 == loop over fields
      As_Generic(this)->writeField(this, out, i); // 0 == loop over fields

   assert(RichString_size(out) > 0);
}

int Generic_compare(const void* v1, const void* v2) {
   return 0;
}

const GenericClass Generic_class = {
   .super = {
      .extends = Class(Object),
      .display = Generic_display,
      .compare = Generic_compare,
   },
   .writeField = Generic_writeField,
};
