#ifndef HEADER_Generic
#define HEADER_Generic
/*
htop - Generic.h
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
#include "ProcessField.h" // SMA REMOVEME
#include "RichString.h"
#include "Settings.h"


typedef struct Generic_ {
   /* Super object for emulated OOP */
   Object super;

   /* Pointer to quasi-global data structures */
   const struct GenericList_* GenericList;
   const struct Settings_* settings;

   int id; // SMA REMOVEME

   /* Parent process identifier */
   pid_t ppid; // SMA REMOVEME

} Generic;


// Implemented in platform-specific code:
void Generic_writeField(const Generic* this, RichString* str, int field);
int Generic_compare(const void* v1, const void* v2);
void Generic_delete(Object* cast);

typedef Generic* (*Generic_New)(const struct Settings_*);
typedef void (*Generic_WriteField)(const Generic*, RichString*, int field);
typedef int (*Generic_CompareByKey)(const Generic*, const Generic*);

typedef struct GenericClass_ {
   const ObjectClass super;
   const Generic_WriteField writeField;
   const Generic_CompareByKey compareByKey;
} GenericClass;

#define As_Generic(this_)                              ((const GenericClass*)((this_)->super.klass))

#define Generic_compareByKey(p1_, p2_, key_)           (As_Generic(p1_)->compareByKey ? (As_Generic(p1_)->compareByKey(p1_, p2_, key_)) : Generic_compareByKey_Base(p1_, p2_, key_))

void Generic_display(const Object* cast, RichString* out);
extern const GenericClass Generic_class;

void Generic_init(Generic* this, const Settings* settings);

/* Just a wrapper to Platform/ PCPGeneric_addField
 * I will need those in Columns Panel
 * Maybe I will need Generic_updateField also ... */
void Generic_addField(Generic* this /* maybe I will add more params */);
void Generic_removeField(Generic* this);

#endif
