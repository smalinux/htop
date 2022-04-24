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
#include "Generic.h"
#include "Vector.h"
#include "XUtils.h"


/* SMA: Start GenericList(s) ----------------------------------------------- */
GenericLists *genericlists;

// SMA: this is just a hash hold other GenericLists
GenericLists* GenericLists_new(void) // aka PCPDynamicColumn_new
{
   //return Platform_genericLists(); // SMA: don't call platform here,
   genericlists = xCalloc(1, sizeof(GenericLists));
   genericlists->table =  Hashtable_new(0, false);

   return genericlists;
}

void GenericLists_add(GenericList* g)
{
   // SMA: split this to 2 functions: new & init
   GenericList* gl = xCalloc(1, sizeof(GenericList));

   gl->genericRow = Vector_new(Class(Generic), true, DEFAULT_SIZE);
   gl->genericTable = Hashtable_new(200, false); // SMA use PCPMetric_instanceCount

   gl->ttt = g->ttt;
   gl->ss = g->ss;

   Hashtable_put(genericlists->table, gl->ttt, gl);
}

void GenericLists_remove(GenericList* this,  GenericList* g) { }

const GenericLists* GenericLists_getGenericLists()
{
   return genericlists;
}

// aka PCPDynamicColumns_delete
static void GenericLists_delete(Hashtable* gls) { }

void GenericLists_done(Hashtable* gls) { }

/* SMA: End GenericList(s) ------------------------------------------------- */

GenericList* GenericList_init(GenericList* this, const ObjectClass* klass, UsersTable* usersTable, Hashtable* dynamicMeters, Hashtable* dynamicColumns, Hashtable* pidMatchList, uid_t userId) {
   return this;
}

void GenericList_done(GenericList* this) { }

void GenericList_printHeader(const GenericList* this, RichString* header) { }

void GenericList_add(GenericList* this, Generic* g)
{
   g->GenericList = this;

   Vector_add(this->genericRow, g);
   Hashtable_put(this->genericTable, g->id, g);
}

void GenericList_remove(GenericList* this, const Process* p) { }

ProcessField GenericList_keyAt(const GenericList* this, int at) { }

Generic* GenericList_getGeneric(GenericList* this, pid_t pid, Generic_New constructor)
{
   Generic* proc = (Generic*) Hashtable_get(this->genericTable, pid);
   fprintf(stderr, "nnnnnnnnnnnnnn\n");
   //*preExisting = proc != NULL;
   if (proc) {
      //assert(Vector_indexOf(this->processes, proc, Process_pidCompare) != -1);
      //assert(proc->pid == pid);
   } else {
      proc = constructor(this->settings);
      //assert(proc->cmdline == NULL);
      proc->ppid = 3333;
   }
   return proc;
}

void GenericList_scan(GenericList* this, bool pauseProcessUpdate) // SMA xxxg
{
   GenericList_goThroughEntries(this, 0); // SMA xxg this
}

