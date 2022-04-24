/*
htop - GenericList.h
(C) 2022 Sohaib Mohammed
(C) 2022 htop dev team
(C) 2022 Red Hat, Inc.
Released under the GNU GPLv2+, see the COPYING file
in the source distribution for its full text.
*/

#include "config.h" // IWYU pragma: keep

#include "GenericList.h"

#include "CRT.h"
#include "Hashtable.h"
#include "Macros.h"
#include "Platform.h"
#include "Vector.h"
#include "XUtils.h"


/* SMA: Start GenericList(s) ----------------------------------------------- */
GenericLists *genericlists;

// SMA: this is just a hash hold other GenericLists
GenericLists* GenericLists_new(void) { // aka PCPDynamicColumn_new
   //return Platform_genericLists(); // SMA: don't call platform here,
   genericlists = xCalloc(1, sizeof(GenericLists));
   genericlists->table =  Hashtable_new(0, false);

   return genericlists;
}

void GenericLists_add(GenericLists* this, GenericList* g)
{
   // SMA: split this to 2 functions: new & init
   GenericList* gl = xCalloc(1, sizeof(GenericList));

   gl->ttt = g->ttt;
   gl->ss = g->ss;

   Hashtable_put(genericlists->table, gl->ttt, gl);
}

void GenericLists_remove(GenericList* this,  GenericList* g)
{

}

static void GenericLists_delete(Hashtable* gls) // aka PCPDynamicColumns_delete
{

}

void GenericLists_done(Hashtable* gls) // aka PCPDynamicColumns_done
{
   //GenericLists_delete
}

/* SMA: End GenericList(s) ------------------------------------------------- */

GenericList* GenericList_init(GenericList* this, const ObjectClass* klass, UsersTable* usersTable, Hashtable* dynamicMeters, Hashtable* dynamicColumns, Hashtable* pidMatchList, uid_t userId) {
   return this;
}

void GenericList_done(GenericList* this) {
}

void GenericList_printHeader(const GenericList* this, RichString* header) {
}

void GenericList_add(GenericList* this, Process* p) {
}

void GenericList_remove(GenericList* this, const Process* p) {
}

ProcessField GenericList_keyAt(const GenericList* this, int at) {
}

Process* GenericList_getProcess(GenericList* this, pid_t pid, bool* preExisting, Process_New constructor) {
   //Process* proc = (Process*) Hashtable_get(this->processTable, pid);
   //return generic;
}

void GenericList_scan(GenericList* this, bool pauseProcessUpdate) {
}
