#ifndef HEADER_GenericList
#define HEADER_GenericList
/*
htop - GenericList.h
(C) 2022 Sohaib Mohammed
(C) 2022 htop dev team
(C) 2022 Red Hat, Inc.
Released under the GNU GPLv2+, see the COPYING file
in the source distribution for its full text.
*/

#include "config.h" // IWYU pragma: keep

#include "Hashtable.h"
#include "Object.h"
#include "Panel.h"
#include "Generic.h"
#include "RichString.h"
#include "Settings.h"
#include "UsersTable.h"
#include "Vector.h"

typedef struct GenericList_ {
   const Settings* settings;

   Vector* genericRow; /* every row == One Sturct Generic */
   Hashtable* genericTable;

   ScreenSettings ss; /* each GenericList has its own screenSettings */

   bool needsSort;

   Panel* panel;

   int ttt; // SMALINUX REMOVEME

   int offset;
   int id; /* GenericList id == offset */

   unsigned int totalRows;
} GenericList;

typedef struct GenericLists_ {
   Hashtable *table;
   int tst; // SMALINUX REMOVEME
   size_t count; /* total count for all GenericList */
} GenericLists;


/* Implemented by platforms */
GenericList* GenericList_addPlatformList(GenericList *super);
void GenericList_removePlatformList(GenericList *super);
void GenericList_goThroughEntries(GenericList* super, bool pauseProcessUpdate);


/* Generic Lists */
GenericLists* GenericList_new(void);

void GenericList_delete(GenericLists* gls);

const GenericLists* GenericList_getGenericLists(void);

/* One Generic List */
void GenericList_addList(GenericList* g);

void GenericList_removeList(GenericList* g);

/* struct Generic */
Generic* GenericList_getGeneric(GenericList* this, Generic_New constructor);

void GenericList_addGeneric(GenericList* this, Generic* g);

void GenericList_removeGeneric(GenericList* this);

/* helpers functions */
void GenericList_setPanel(GenericList* this, Panel* panel);

void GenericList_printHeader(const GenericList* this, RichString* header); // TODO

void GenericList_updateDisplayList(GenericList* this); // TODO

//GenericField GenericList_keyAt(const GenericList* this, int at); // TODO

void GenericList_expandTree(GenericList* this); // TODO

void GenericList_collapseAllBranches(GenericList* this); // TODO

void GenericList_rebuildPanel(GenericList* this); // TODO

void GenericList_scan(GenericList* this, bool pauseGenericUpdate);

/*
static inline Process* GenericList_findProcess(GenericList* this, pid_t pid) { // TODO
   return (Process*) Hashtable_get(this->genericTable, pid);
}
*/

#endif
