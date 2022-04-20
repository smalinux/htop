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
#include "ProcessList.h"
#include "DynamicColumn.h"
#include "RichString.h"
#include "Settings.h"
#include "XUtils.h"

#if defined(MAJOR_IN_MKDEV)
#include <sys/mkdev.h>
#endif

void Generic_display(const Object* cast, RichString* out) {
   fprintf(stderr, "hiiiiiiiiii\n");
}

int Generic_compare(const void* v1, const void* v2) {
   return 0;
}

//void Process_writeField(const Process* this, RichString* str, ProcessField field) {
void Generic_writeField(const Generic* this, RichString* str) {
}

const GenericClass Generic_class = {
   .super = {
      .extends = Class(Object),
      .display = Generic_display,
      //.delete = Generic_delete,
      .compare = Generic_compare
   },
   .writeField = Generic_writeField,
};

