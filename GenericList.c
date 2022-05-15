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

void GenericList_addList(GenericList* l)
{
   GenericList* gl = GenericList_addPlatformList(l);

   gl->genericRow = Vector_new(Class(Generic), false, DEFAULT_SIZE);
   gl->displayList = Vector_new(Class(Generic), false, DEFAULT_SIZE);
   gl->genericTable = Hashtable_new(200, false);

   gl->ss = l->ss; // SMA

   Hashtable_put(genericlists->table, 100, gl); // SMA FIXME 100 hard code

   /* init */
   gl->totalRows = 0;
}

void GenericList_removeList(GenericList* this)
{
   // loop & GenericList_removeGeneric()
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
   g->gtest = 5;

   Vector_add(this->genericRow, g);
   Hashtable_put(this->genericTable, this->totalRows, g);
   this->totalRows++;
}

void GenericList_removeGeneric(GenericList* this)
{
   int idx = this->totalRows -1;
   Object* last = Vector_get(this->genericRow, idx);

   Generic_delete(last);

   Vector_remove(this->genericRow, idx);
   Hashtable_remove(this->genericTable, idx);

   this->totalRows--;
}

Generic* GenericList_getGeneric(GenericList* this, Generic_New constructor)
{
   Generic* g;
   g = constructor(this->settings);

   return g;
}

void GenericList_scan(GenericList* this, bool pauseGenericUpdate) // SMA xxxg // TODO
{
   GenericList_goThroughEntries(this, 0); // SMA xxg this
}

void GenericList_setPanel(GenericLists* this, Panel* panel) {
   this->panel = panel;
}

static void GenericList_updateDisplayList(GenericList* this) {
   if (this->needsSort)
      Vector_insertionSort(this->genericRow);
   Vector_prune(this->displayList);
   int size = Vector_size(this->genericRow);
   for (int i = 0; i < size; i++)
      Vector_add(this->displayList, Vector_get(this->genericRow, i));
   this->needsSort = false;
}

void GenericList_rebuildPanel(GenericLists* gls, GenericList* this)
{
   GenericList_updateDisplayList(this);
   const int genericCount = Vector_size(this->displayList);
   int idx = 0;

   for (int i = 0; i < genericCount; i++) {
      Generic* g = (Generic*) Vector_get(this->displayList, i);

      Panel_set(gls->panel, idx, (Object*)g);

      idx++;
   }
}
