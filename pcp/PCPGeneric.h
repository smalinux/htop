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
#include "Platform.h"
#include "Hashtable.h"


typedef struct PCPGenericField_ {
   pmAtomValue* value;

   pmID pmid;

   int offset;

   int type;

   int index; // SMA should not use pmid as column index because the user can dublicate the same column;
} PCPGenericField;

typedef struct PCPGeneric_ {
   Generic super;

   Hashtable* fields;  /* PCPGenericFields */

   int fieldsCount;

   int sortKey;

   int love; // SMA REMOVEME
} PCPGeneric;


void PCPGeneric_delete(Object* cast);

int PCPGeneric_compare(const void* v1, const void* v2);

extern const GenericClass PCPGeneric_class;

Generic* PCPGeneric_new(const Settings* settings);

void Generic_delete(Object* cast);

PCPGenericField* PCPGeneric_addField(PCPGeneric* this, const Settings* settings);

void PCPGeneric_removeField(PCPGeneric* this);

#endif
