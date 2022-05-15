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

/*
 * https://github.com/smalinux/htop/commit/2afaf57d0ee18696f47f8df51801610148677157
 */
static bool PCPGenericList_updateGenericList(PCPGenericList* this) // SMA xxg this
{
   GenericList* gl = (GenericList*) this;
   const Settings* settings = gl->settings;
   const ProcessField* fields = settings->ss->fields; // SMA: change "ProcessField" to just "Field"
   Generic* g;
   const DynamicColumn* dc;
   const PCPDynamicColumn* pcp_dc;
   unsigned int dc_key;
   int lastField = 0;
   int keyMatric;
   pmAtomValue value;
   int interInst = -1, offset = -1;
   int rowsCount;
   int metricType;

   /*
    * > What is the info you need here?
    * - 1. Info from *Panel.ch ( like AvailableColumnsPanel.ch .. etc )
    * - 2. this info will be stored in settings->ss
    * - 3. DynamicColumn. ( fetch matrics via its name)
    */


   // SMALINUX Step 1.
   // here catch all the current screen setup...



   // SMALINUX Step 1. follow
   /*
    * Imagine you finished step one, for now...
    * Here, I will set some hardCoded setup settings...
    * this setup settings should be came from step 1 dynamically
    */
   int columnsCount = 5; // total fields[max] number (+1 for ExternalName Column) // SMA: hardCoded FIXME

   // {{ fetch keyMatric ... keyMatric == fields[0] , move this to static func
   if (settings->ss->generic)
   {
      DynamicColumn* column = Hashtable_get(settings->dynamicColumns, fields[0]);
      dc = DynamicColumn_search(settings->dynamicColumns, column->name, &dc_key);
      pcp_dc = (const PCPDynamicColumn*) dc;
      keyMatric = pcp_dc->id;

   } else {
      return 0;
   }
   // }}

   rowsCount = PCPMetric_instanceCount(keyMatric); // instance count

   // -------------------------- Start: Alloc -------------------------------
   if (Vector_size(gl->genericRow)< rowsCount)
   {
      for (int r = 0; r < rowsCount; r++)
      {
         g = GenericList_getGeneric(gl, PCPGeneric_new);
         PCPGeneric* gg = (PCPGeneric*) g;

         for (int c = 0; c < columnsCount; c++)
         {
            PCPGeneric_addField(gg, NULL);
         }

         GenericList_addGeneric(gl, g); // keep this last line
      }
   }

   // -------------------------- End: Alloc ---------------------------------


   /* for every generic row ... */
   while (PCPMetric_iterate(keyMatric, &interInst, &offset)) {


   // -------------------------- Fill -------------------------------

   // SMA: {{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{
   for(unsigned int i = 0; fields[i]; i++)
   {
      DynamicColumn* column = Hashtable_get(settings->dynamicColumns, fields[i]);
      if (!column)
         continue;
      //fprintf(stderr, "#> %s, ", column->name);

      /* column */
      PCPDynamicColumn* pcp_column = (PCPDynamicColumn*) column;

      //dc = DynamicColumn_search(settings->dynamicColumns, column->name, NULL);
      dc = DynamicColumn_search(settings->dynamicColumns, column->name, &dc_key);
      pcp_dc = (const PCPDynamicColumn*) dc;

      metricType = PCPMetric_type(pcp_column->id);

      if (PCPMetric_instance(pcp_dc->id, interInst, offset, &value, metricType))
      {
          //process->mycgroup = value.ull; // SMA: This func not good at all with cgroup
         //fprintf(stderr, "........................ %d\n", offset);
         g = Hashtable_get(gl->genericTable, offset); // 0 != offset
         PCPGeneric* gg = (PCPGeneric*) g;

         // SMA: get field count
         PCPGenericField* gf = (PCPGenericField*)Hashtable_get(gg->fields, i);
         *gf->value = value;
         gf->type = metricType;
      }

      lastField = i;
      //DynamicColumn* ddd = (DynamicColumn*)Hashtable_get(settings->dynamicColumns, dc_key);
      //fprintf(stderr, "+++++++++++++++++++ %d - %ld - %s\n", dc_key, pcp_dc->id, ddd->caption);
   }
   // SMA: }}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}








   // -------------------------------------------------------------------------
   // -------------------------------------------------------------------------
   // -------------------------------------------------------------------------
   // -------------------------------------------------------------------------
   /* External name column */
   g = Hashtable_get(gl->genericTable, offset);
   PCPGeneric* gg = (PCPGeneric*) g;
   PCPGenericField* gf = (PCPGenericField*)Hashtable_get(gg->fields, columnsCount-1);
   //fprintf(stderr, ">>>>>>>>>>>>>>> %d\n", lastField);
   // SMA: param 1 == key matric (aka PCPMetric_iterate):
   PCPMetric_externalName(keyMatric, interInst, &gf->value->cp);
   // -------------------------------------------------------------------------
   // -------------------------------------------------------------------------
   // -------------------------------------------------------------------------
   // -------------------------------------------------------------------------








      //PCPGenericField * field = PCPGeneric_addField(gg, NULL);
      //field->value->ul = 9999;

      //field = PCPGeneric_addField(gg, NULL);
      //field->value->ul = 8888;

      //field = PCPGeneric_addField(gg, NULL);
      //field->value->ul = 7777;

      //field = PCPGeneric_addField(gg, NULL);
      //field->value->ul = 6666;

      //PCPGeneric_removeField(gg); // will remove the last one == 6666
      //PCPGeneric_removeField(gg); // will remove the last one == 7777
      // SMA End add & assing Fields.


      // -------------------------- Add --------------------------------

      // ------------- Remove Generic ---------------
      if(gl->totalRows > 10) {
         //GenericList_removeGeneric(gl);
      }
      // ---------------------------------------------------------------

   } // end: while PCPMetric_iterate


   // -------------------------- testing (fprintf) ----------------------------
      // SMA: Test: Start print all rows & its node for testing
   /*
      fprintf(stderr, ">>>>>>>>>>>>>>>>>>>>>>>>> totalRows %d\n\n", gl->totalRows);
      fprintf(stderr, "PCPGenericList->test %d\n", this->test);
      for (int i = 0; i < Vector_size(gl->genericRow); i++)
      {
         fprintf(stderr, "Generic row %d - %d\n", gl->totalRows, Vector_size(gl->genericRow));

         for (int n = 0; n < gg->fieldsCount; n++)
         {
            PCPGenericField* f = Hashtable_get(gg->fields, n);
            fprintf(stderr, "field %d its value = %d\n", n, f->value->ul);
         }
         fprintf(stderr, "\n");
      }
   */


   return 0; // bool ??!!
}

void GenericList_goThroughEntries(GenericList * super, bool pauseProcessUpdate)
{
   PCPGenericList* this = (PCPGenericList*) super;
   const Settings* settings = super->settings;

   //fprintf(stderr, "-$$$$$$$$$$$$$ >>> %s\n", super->settings->ss->name);
   PCPGenericList_updateGenericList(this);
}

GenericList* GenericList_addPlatformList(GenericList *super)
{
   PCPGenericList* this = xCalloc(1, sizeof(PCPGenericList));
   super = &(this->super);
   this->test = 66; // SMA REMOVEME

   return super;
}

void GenericList_removePlatformList(GenericList *gl)
{
   PCPGenericList* this = (PCPGenericList*) gl;
   free(this);
}
