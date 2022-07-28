/*
htop - PCPGenericList.c
(C) 2022 Sohaib Mohammed
(C) 2022 htop dev team
(C) 2022 Red Hat, Inc.
Released under the GNU GPLv2+, see the COPYING file
in the source distribution for its full text.
*/

#include "config.h" // IWYU pragma: keep

#include "pcp/PCPGenericList.h"

#include "Hashtable.h"
#include "GenericList.h"
#include "Macros.h"
#include "Object.h"
#include "Platform.h"
#include "Process.h"
#include "Settings.h"
#include "XUtils.h"
#include "DynamicColumn.h"
#include "Vector.h"

#include "pcp/PCPGeneric.h"
#include "pcp/PCPMetric.h"
#include "pcp/PCPDynamicColumn.h"

static size_t defineKeyMetric(const Settings* settings) {
   const PCPDynamicColumn* column;
   column = (PCPDynamicColumn*)Hashtable_get(settings->dynamicColumns, settings->ss->fields[0]);

   return column->id;
}

static size_t getColumnCount(const ProcessField* fields) {
   size_t count;
   for (unsigned int i = 0; fields[i]; i++)
      count = i;

   return count+1;
}

static int getRowCount(int keyMetric) {
   return PCPMetric_instanceCount(keyMetric);
}

static void allocRows(GenericList* gl, int rowsCount) {
   Generic* g;

   if (Vector_size(gl->genericRow) != rowsCount) {
      int diff = Vector_size(gl->genericRow) - rowsCount;

      if (Vector_size(gl->genericRow) > rowsCount) {

         for (int c = 0; c < diff; c++) {
            //PCPGeneric_removeAllFields(gg); // SMA FIXME
            GenericList_removeGeneric(gl);
         }
      }

      if (Vector_size(gl->genericRow) < rowsCount) {
         for (int r = Vector_size(gl->genericRow); r < rowsCount; r++) {
            g = GenericList_getGeneric(gl, PCPGeneric_new);
            GenericList_addGeneric(gl, g);
         }
      }

   }
}

static void allocColumns(GenericList* gl, size_t columnsCount, int rowsCount) {
   PCPGeneric* firstrow = (PCPGeneric*)Vector_get(gl->genericRow, 0);

   if (firstrow->fieldsCount < rowsCount) { // SMA FIXME  != instead of <
      for (int r = 0; r < Vector_size(gl->genericRow); r++) {
         PCPGeneric* gg = (PCPGeneric*)Vector_get(gl->genericRow, r);
         for (size_t c = 0; c < columnsCount; c++) {
            PCPGeneric_addField(gg, NULL);
         }
      }
   }
}

static bool PCPGenericList_updateGenericList(PCPGenericList* this) {
   GenericList* gl = (GenericList*) this;
   const Settings* settings = gl->settings;
   const ProcessField* fields = settings->ss->fields; // SMA: change name from "ProcessField" to just "Field"
   size_t keyMetric;

   if (!settings->ss->generic)
      return 0;

   // inDom Validation
   // TODO
   //

   keyMetric = defineKeyMetric(settings);
   size_t columnsCount = getColumnCount(fields);
   int rowsCount = getRowCount(keyMetric);

   /* alloc */
   allocRows(gl, rowsCount);
   allocColumns(gl, columnsCount, rowsCount);

   /* fill */
   int interInst = -1, offset = -1;
   while (PCPMetric_iterate(keyMetric, &interInst, &offset)) {
      for(unsigned int i = 0; fields[i]; i++) {
         int metricType;

         DynamicColumn* dc = Hashtable_get(settings->dynamicColumns, fields[i]);
         if (!dc)
            continue;

         PCPDynamicColumn* column = (PCPDynamicColumn*) dc;
         metricType = PCPMetric_type(column->id);

         pmAtomValue value;
         if (PCPMetric_instance(column->id, interInst, offset, &value, metricType)) {
            Generic* g;

            g = Hashtable_get(gl->genericTable, offset);
            if (!g)
               continue;

            PCPGeneric* gg = (PCPGeneric*) g;

            PCPGenericField* field = (PCPGenericField*)Hashtable_get(gg->fields, i);
            *field->value = value;
            field->type = metricType;
         }
      }
   }

   return 0;
}

void GenericList_goThroughEntries(GenericList * super, bool pauseUpdate)
{
   bool enabled = !pauseUpdate;
   PCPGenericList* this = (PCPGenericList*) super;
   const Settings* settings = super->settings;

   if (enabled)
      PCPGenericList_updateGenericList(this);
}

GenericList* GenericList_addPlatformList(GenericList *super)
{
   PCPGenericList* this = xCalloc(1, sizeof(PCPGenericList));
   super = &(this->super);

   return super;
}

void GenericList_removePlatformList(GenericList *gl)
{
   PCPGenericList* this = (PCPGenericList*) gl;
   free(this);
}
