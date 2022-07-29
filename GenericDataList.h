#ifndef HEADER_GenericDataList
#define HEADER_GenericDataList
/*
htop - GenericDataList.h
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
#include "GenericData.h"
#include "RichString.h"
#include "Settings.h"
#include "UsersTable.h"
#include "Vector.h"
#include "Settings.h"


typedef struct GenericDataList_ {
   const Settings* settings;

   Vector* displayList;
   Vector* GenericDataRow; /* each elem is sturct GenericData */
   Hashtable* GenericDataTable;

   bool needsSort;

   Panel* panel;

   Hashtable* fieldUI;

   bool rebuildFields;

   int offset;

   int id; /* GenericDataList id == offset */

   unsigned int totalRows;
} GenericDataList;

/* Implemented by platforms */
GenericDataList* GenericDataList_addPlatformList(GenericDataList *super);
void GenericDataList_removePlatformList(GenericDataList *super);
void GenericDataList_goThroughEntries(GenericDataList* super, bool pauseUpdate);


/* GenericData Lists */
GenericDataList* GenericDataList_new(void);

void GenericDataList_delete(GenericDataList* gl);

/* One GenericData List */
void GenericDataList_addList(void);

//void GenericDataList_removeList(GenericDataList* g);

/* struct GenericData */
GenericData* GenericDataList_getGenericData(GenericDataList* this, GenericData_New constructor);

void GenericDataList_addGenericData(GenericDataList* this, GenericData* g);

void GenericDataList_removeGenericData(GenericDataList* this);

/* helpers functions */
void GenericDataList_setPanel(GenericDataList* this, Panel* panel);

void GenericDataList_printHeader(const GenericDataList* this, RichString* header); // TODO

//void GenericDataList_updateDisplayList(GenericDataList* this); // REMOVEME I thik this should be static function

//GenericDataField GenericDataList_keyAt(const GenericDataList* this, int at); // TODO

void GenericDataList_expandTree(GenericDataList* this); // TODO

void GenericDataList_collapseAllBranches(GenericDataList* this); // TODO

void GenericDataList_rebuildPanel(GenericDataList* this);

void GenericDataList_scan(GenericDataList* this, bool pauseUpdate);

/*
static inline Process* GenericDataList_findProcess(GenericDataList* this, pid_t pid) { // TODO
   return (Process*) Hashtable_get(this->GenericDataTable, pid);
}
*/

#endif
