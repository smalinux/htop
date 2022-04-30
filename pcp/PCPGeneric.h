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
   int offset; // Add this to Generic.h not PCPGeneric.h
   int index; // SMA should not use pmid as column index because the user can dublicate the same column;
} PCPGenericField;

typedef struct PCPGeneric_ {
   Generic super;
   int love; // SMA REMOVEME

   Hashtable* fields; // rename to just fields
   int fieldsCount; // rename to FieldCount
   int sortKey;
} PCPGeneric;

void PCPGeneric_delete(Object* cast);
void PCPGeneric_display(const Object* cast, RichString* out);
int PCPGeneric_compare(const void* v1, const void* v2);

extern const GenericClass PCPGeneric_class;

Generic* PCPGeneric_new(const Settings* settings);

// Reference:  GenericList_addGeneric ...
// do alloc. - this is used in PCPMetric_iterate
// to add new column (aka, node. aka, one field, aka PCPGenericField
PCPGenericField* PCPGeneric_addField(PCPGeneric* this, const Settings* settings);
void PCPGeneric_removeField(PCPGeneric* this);

void Generic_delete(Object* cast);

#endif
