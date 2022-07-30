#ifndef HEADER_PCPGenericData
#define HEADER_PCPGenericData
/*
htop - PCPGenericData.h
(C) 2022 Sohaib Mohammed
(C) 2022 htop dev team
(C) 2022 Red Hat, Inc.
Released under the GNU GPLv2+, see the COPYING file
in the source distribution for its full text.
*/

#include "config.h" // IWYU pragma: keep

#include "Object.h"
#include "GenericData.h"
#include "Settings.h"
#include "Platform.h"
#include "Hashtable.h"


typedef struct PCPGenericDataField_ {
   pmAtomValue* value;

   pmID pmid;

   int offset;

   int type;
} PCPGenericDataField;

typedef struct PCPGenericData_ {
   GenericData super;

   Hashtable* fields;  /* PCPGenericDataFields */

   int fieldsCount;

   int sortKey;
} PCPGenericData;


void PCPGenericData_delete(Object* cast);

int PCPGenericData_compare(const void* v1, const void* v2);

extern const GenericDataClass PCPGenericData_class;

GenericData* PCPGenericData_new(const Settings* settings);

void GenericData_delete(Object* cast);

PCPGenericDataField* PCPGenericData_addField(PCPGenericData* this);

void PCPGenericData_removeField(PCPGenericData* this);

void PCPGenericData_removeAllFields(PCPGenericData* this);

#endif
