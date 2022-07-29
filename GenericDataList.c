/*
htop - GenericDataList.h
(C) 2022 Sohaib Mohammed
(C) 2022 htop dev team
(C) 2022 Red Hat, Inc.
Released under the GNU GPLv2+, see the COPYING file
in the source distribution for its full text.
*/

#include "config.h" // IWYU pragma: keep

#include "GenericDataList.h"

#include "CRT.h"
#include "Hashtable.h"
#include "Macros.h"
#include "Platform.h"
#include "GenericData.h"
#include "Vector.h"
#include "XUtils.h"
#include "Settings.h"


GenericDataList* GenericDataList_new()
{
   GenericDataList* gl = xCalloc(1, sizeof(GenericDataList));
   gl = GenericDataList_addPlatformList(gl);

   gl->GenericDataRow = Vector_new(Class(GenericData), false, DEFAULT_SIZE);
   gl->displayList = Vector_new(Class(GenericData), false, DEFAULT_SIZE);
   gl->GenericDataTable = Hashtable_new(200, false);

   gl->totalRows = 0;

   return gl;
}

//void GenericDataList_delete(GenericDataList* this)
//{
//   // loop & GenericDataList_removeGenericData()
//   Hashtable_delete(this->GenericDataTable);
//   Vector_delete(this->GenericDataRow);
//
//   GenericDataList_removePlatformList(this);
//}

void GenericDataList_addGenericData(GenericDataList* this, GenericData* g)
{
   Vector_add(this->GenericDataRow, g);
   Hashtable_put(this->GenericDataTable, this->totalRows, g);

   this->totalRows++;
}

void GenericDataList_removeGenericData(GenericDataList* this)
{
   int idx = this->totalRows -1;
   Object* last = Vector_get(this->GenericDataRow, idx);

   GenericData_delete(last);

   Vector_remove(this->GenericDataRow, idx);
   Hashtable_remove(this->GenericDataTable, idx);

   this->totalRows--;
}

GenericData* GenericDataList_getGenericData(GenericDataList* this, GenericData_New constructor)
{
   GenericData* g;
   g = constructor(this->settings);

   return g;
}

void GenericDataList_scan(GenericDataList* this, bool pauseUpdate)
{
   GenericDataList_goThroughEntries(this, pauseUpdate);
}

void GenericDataList_setPanel(GenericDataList* this, Panel* panel) {
   this->panel = panel;
}

static void GenericDataList_updateDisplayList(GenericDataList* this) {
   if (this->needsSort)
      Vector_insertionSort(this->GenericDataRow);
   Vector_prune(this->displayList);
   int size = Vector_size(this->GenericDataRow);
   for (int i = 0; i < size; i++)
      Vector_add(this->displayList, Vector_get(this->GenericDataRow, i));
   this->needsSort = false;
}

void GenericDataList_rebuildPanel(GenericDataList* this)
{
   GenericDataList_updateDisplayList(this);
   const int GenericDataCount = Vector_size(this->displayList);
   int idx = 0;

   for (int i = 0; i < GenericDataCount; i++) {
      GenericData* g = (GenericData*) Vector_get(this->displayList, i);

      Panel_set(this->panel, idx, (Object*)g);

      idx++;
   }
}
