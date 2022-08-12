/*
htop - PCPGenericDataList.c
(C) 2022 Sohaib Mohammed
(C) 2022 htop dev team
(C) 2022 Red Hat, Inc.
Released under the GNU GPLv2+, see the COPYING file
in the source distribution for its full text.
*/

#include "config.h" // IWYU pragma: keep

#include "pcp/PCPGenericDataList.h"

#include <stddef.h>
#include <stdlib.h>

#include "DynamicColumn.h"
#include "GenericData.h"
#include "GenericDataList.h"
#include "Hashtable.h"
#include "Object.h"
#include "Process.h"
#include "Settings.h"
#include "Vector.h"
#include "XUtils.h"

#include "pcp/PCPDynamicColumn.h"
#include "pcp/PCPGenericData.h"
#include "pcp/PCPMetric.h"


static size_t defineKeyMetric(const Settings* settings) {
   const PCPDynamicColumn* column;
   column = (PCPDynamicColumn*)Hashtable_get(settings->dynamicColumns, settings->ss->fields[0]);

   return column->id;
}

static int getColumnCount(const ProcessField* fields) {
   int count = 0;
   for (unsigned int i = 0; fields[i]; i++)
      count = i;

   return count + 1;
}

static int getRowCount(int keyMetric) {
   return PCPMetric_instanceCount(keyMetric);
}

static void allocRows(GenericDataList* gl, int rowsCount) {
   GenericData* g;

   if (Vector_size(gl->GenericDataRow) != rowsCount) {
      int diff = Vector_size(gl->GenericDataRow) - rowsCount;

      if (Vector_size(gl->GenericDataRow) > rowsCount) {
         for (int c = 0; c < diff; c++) {
            //PCPGenericData_removeAllFields(gg); // SMA FIXME
            GenericDataList_removeGenericData(gl);
         }
      }

      if (Vector_size(gl->GenericDataRow) < rowsCount) {
         for (int r = Vector_size(gl->GenericDataRow); r < rowsCount; r++) {
            g = GenericDataList_getGenericData(gl, PCPGenericData_new);
            GenericDataList_addGenericData(gl, g);
         }
      }

   }
}

static void allocColumns(GenericDataList* gl, size_t columnsCount, int rowsCount) {
   PCPGenericData* firstrow = (PCPGenericData*)Vector_get(gl->GenericDataRow, 0);

   if (firstrow->fieldsCount < rowsCount) { // SMA FIXME  != instead of <
      for (int r = 0; r < Vector_size(gl->GenericDataRow); r++) {
         PCPGenericData* gg = (PCPGenericData*)Vector_get(gl->GenericDataRow, r);
         for (size_t c = 0; c < columnsCount; c++)
            PCPGenericData_addField(gg);
      }
   }
}

static bool PCPGenericDataList_updateGenericDataList(PCPGenericDataList* this) {
   GenericDataList* gl = (GenericDataList*) this;
   const Settings* settings = gl->settings;
   const ProcessField* fields = settings->ss->fields;
   size_t keyMetric;

   if (!settings->ss->generic)
      return 0;

   // When one or more columns are added to pcp/screens and do not already exist in
   // pcp/columns, continue.
   for (unsigned int i = 0 ; !fields[i]; i++)
      return 0;

   // TODO inDom Validation
   //for (unsigned int i = 1 ; !fields[i]; i++)
   // check all columns should be from the same inDom
   // check all columns == inDom(keyMetric)
   // if not, continue:
   // https://github.com/smalinux/htop/commit/8956270aef82c4340e15d8be1f9457ad7e033e2a#r78946285
   //

   keyMetric = defineKeyMetric(settings);
   int columnsCount = getColumnCount(fields);
   int rowsCount = getRowCount(keyMetric);

   /* alloc */
   allocRows(gl, rowsCount);
   allocColumns(gl, columnsCount, rowsCount);

   /* fill */
   int interInst = -1, offset = -1;
   while (PCPMetric_iterate(keyMetric, &interInst, &offset)) {
      for (unsigned int i = 0; fields[i]; i++) {
         int metricType;

         DynamicColumn* dc = Hashtable_get(settings->dynamicColumns, fields[i]);
         if (!dc)
            continue;

         PCPDynamicColumn* column = (PCPDynamicColumn*) dc;
         metricType = PCPMetric_type(column->id);

         pmAtomValue value;
         if (PCPMetric_instance(column->id, interInst, offset, &value, metricType)) {
            GenericData* g;

            g = Hashtable_get(gl->GenericDataTable, offset);
            if (!g)
               continue;

            PCPGenericData* gg = (PCPGenericData*) g;

            PCPGenericDataField* field = (PCPGenericDataField*)Hashtable_get(gg->fields, i);
            *field->value  =  value;
            field->type    =  metricType;
            field->pmid    =  column->id;
            field->offset  =  offset;
         }
      }
   }

   return 0;
}

void GenericDataList_goThroughEntries(GenericDataList* super, bool pauseUpdate)
{
   bool enabled = !pauseUpdate;
   PCPGenericDataList* this = (PCPGenericDataList*) super;

   if (enabled)
      PCPGenericDataList_updateGenericDataList(this);
}

GenericDataList* GenericDataList_addPlatformList(GenericDataList* super)
{
   PCPGenericDataList* this = xCalloc(1, sizeof(PCPGenericDataList));
   super = &(this->super);

   super->GenericDataRow = Vector_new(Class(GenericData), false, DEFAULT_SIZE);
   super->displayList = Vector_new(Class(GenericData), false, DEFAULT_SIZE);
   super->GenericDataTable = Hashtable_new(200, false);

   super->totalRows = 0;

   return super;
}

void GenericDataList_removePlatformList(GenericDataList* gl)
{
   // SMA FIXME loop & GenericDataList_removeGenericData()
   Hashtable_delete(gl->GenericDataTable);
   Vector_delete(gl->GenericDataRow);

   PCPGenericDataList* this = (PCPGenericDataList*) gl;
   free(this);
}
