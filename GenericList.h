//
//
//
//
// SMALINUX: rename any thing refer to process like pidMatchList,
// pauseProcessUpdate
//
// add missing includes... && compare with ProcessList.h file
//
//
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

   Vector* processes;
   Hashtable* processTable;

   bool needsSort;

   //Hashtable* dynamicMeters;  /* runtime-discovered meters */
   //Hashtable* dynamicColumns; /* runtime-discovered Columns */

   Panel* panel;
   uid_t userId;
   int ttt;

   unsigned int totalTasks;
} GenericList;

typedef struct GenericLists_ {
   Hashtable *table;
   void *ontherhash;
   int tst;
   char* name;
} GenericLists;


/* Implemented by platforms */
GenericList* GenericList_new(UsersTable* usersTable, Hashtable* dynamicMeters, Hashtable* dynamicColumns, Hashtable* pidMatchList, uid_t userId);
void GenericList_delete(GenericList* gl);
void GenericList_goThroughEntries(GenericList* super, bool pauseProcessUpdate);
bool GenericList_isCPUonline(const GenericList* super, unsigned int id);

/* GenericLists */
GenericLists* GenericLists_new(void);

void GenericLists_done(Hashtable* gls);

void GenericLists_add(GenericLists* this, GenericList* g);

void GenericLists_remove(GenericList* this,  GenericList* g);

/* GenericList */
GenericList* GenericList_init(GenericList* this, const ObjectClass* klass, UsersTable* usersTable, Hashtable* dynamicMeters, Hashtable* dynamicColumns, Hashtable* pidMatchList, uid_t userId);

void GenericList_done(GenericList* this);

void GenericList_setPanel(GenericList* this, Panel* panel);

void GenericList_printHeader(const GenericList* this, RichString* header);

void GenericList_add(GenericList* this, Process* p);

void GenericList_remove(GenericList* this, const Process* p);

void GenericList_updateDisplayList(GenericList* this);

ProcessField GenericList_keyAt(const GenericList* this, int at);

void GenericList_expandTree(GenericList* this);

void GenericList_collapseAllBranches(GenericList* this);

void GenericList_rebuildPanel(GenericList* this);

Process* GenericList_getProcess(GenericList* this, pid_t pid, bool* preExisting, Process_New constructor);

void GenericList_scan(GenericList* this, bool pauseProcessUpdate);

static inline Process* GenericList_findProcess(GenericList* this, pid_t pid) {
   return (Process*) Hashtable_get(this->processTable, pid);
}


#endif
