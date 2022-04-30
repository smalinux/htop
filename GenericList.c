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


GenericLists *genericlists;

GenericLists* GenericList_new(void)
{
   genericlists = xCalloc(1, sizeof(GenericLists));
   genericlists->table =  Hashtable_new(0, false);

   return genericlists;
}

void GenericList_delete(GenericLists* gls)
{
   Hashtable_delete(gls->table);
   free(gls);
}

void GenericList_addList(GenericList* g)
{
   GenericList* gl = GenericList_addPlatformList(g);

   gl->genericRow = Vector_new(Class(Generic), false, DEFAULT_SIZE);
   gl->genericTable = Hashtable_new(200, false);

   gl->ttt = g->ttt; // SMA REMOVEME
   gl->ss = g->ss; // SMA REMOVEME

   Hashtable_put(genericlists->table, gl->ttt, gl);
}

void GenericList_removeList(GenericList* this)
{
   // loop & free fields & Generics
   Hashtable_delete(this->genericTable);
   Vector_delete(this->genericRow);

   GenericList_removePlatformList(this);
}

const GenericLists* GenericList_getGenericLists()
{
   return genericlists;
}

void GenericList_addGeneric(GenericList* this, Generic* g)
{
   g->GenericList = this;

   Vector_add(this->genericRow, g);
   Hashtable_put(this->genericTable, g->id, g); // id ?? or totalRows ?
}

void GenericList_removeGeneric(GenericList* this)
{
   int idx;

   idx = Vector_size(this->genericRow);
   Vector_remove(this->genericRow, idx);

   this->totalRows--;
   // freefields(); here
   //free(g);     this last element
}

Generic* GenericList_getGeneric(GenericList* this, Generic_New constructor)
{
   Generic* g;
   g = constructor(this->settings);
   g->ppid = 3333; // SMA REMOVEME

   return g;
}

void GenericList_scan(GenericList* this, bool pauseGenericUpdate) // SMA xxxg // TODO
{
   GenericList_goThroughEntries(this, 0); // SMA xxg this
}

void GenericList_setPanel(GenericList* this, Panel* panel) {
   this->panel = panel;
}

