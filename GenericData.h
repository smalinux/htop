#ifndef HEADER_GenericData
#define HEADER_GenericData
/*
htop - GenericData.h
(C) 2022 Sohaib Mohammed
(C) 2022 htop dev team
(C) 2022 Red Hat, Inc.
Released under the GNU GPLv2+, see the COPYING file
in the source distribution for its full text.
*/

#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

#include "Object.h"
#include "RichString.h"
#include "Settings.h"


typedef struct GenericData_ {
   /* Super object for emulated OOP */
   Object super;

   /* Pointer to quasi-global data structures */
   const struct GenericDataList_* GenericDataList;
   const struct Settings_* settings;

} GenericData;


// Implemented in platform-specific code:
void GenericData_writeField(const GenericData* this, RichString* str, int field);
int GenericData_compare(const void* v1, const void* v2);
void GenericData_delete(Object* cast);

typedef GenericData* (*GenericData_New)(const struct Settings_*);
typedef void (*GenericData_WriteField)(const GenericData*, RichString*, int field);
typedef int (*GenericData_CompareByKey)(const GenericData*, const GenericData*, int key);

typedef struct GenericDataClass_ {
   const ObjectClass super;
   const GenericData_WriteField writeField;
   const GenericData_CompareByKey compareByKey;
} GenericDataClass;

#define As_GenericData(this_)                              ((const GenericDataClass*)((this_)->super.klass))

#define GenericData_compareByKey(p1_, p2_, key_)           (As_GenericData(p1_)->compareByKey ? (As_GenericData(p1_)->compareByKey(p1_, p2_, key_)) : GenericData_compareByKey_Base(p1_, p2_, key_))

void GenericData_display(const Object* cast, RichString* out);
extern const GenericDataClass GenericData_class;

void GenericData_init(GenericData* this, const Settings* settings);

void GenericData_done(GenericData* this); // TODO

void GenericData_addField(GenericData* this); // TODO
void GenericData_removeField(GenericData* this); // TODO
void GenericData_updateField(GenericData* this); // TODO
void GenericData_rebuildFields(GenericData* this); // TODO

#endif
