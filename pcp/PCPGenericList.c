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

#include "pcp/PCPGeneric.h"
#include "pcp/PCPMetric.h"

/*
 * https://github.com/smalinux/htop/commit/2afaf57d0ee18696f47f8df51801610148677157
 */
static bool PCPGenericList_updateGenericList(PCPGenericList* this) // SMA xxg this
{
   GenericList* gl = (GenericList*) this;
   const Settings* settings = gl->settings;
   Generic* g;

   /*
    * > What is the info you need here?
    * - 1. Info from *Panel.ch ( like AvailableColumnsPanel.ch .. etc )
    * - 2. this info will be stored in settings->ss
    * - 3. DynamicColumn. ( fetch matrics via its name)
    */


   // SMALINUX Step 1.
   // here catch all the current screen setup...

   // SMA: Start test: list all Caption under current tab
   /*
   for(unsigned int i = 0; settings->ss->fields[i]; i++)
   {
      fprintf(stderr, "#> %s, ", Process_fields[settings->ss->fields[i]].name);
   }
   fprintf(stderr, "\n");
   */
   // SMA: End test: list all Caption under current tab


   // SMALINUX Step 1. follow
   /*
    * Imagine you finished step one, for now...
    * Here, I will set some hardCoded setup settings...
    * this setup settings should be came from step 1 dynamically
    */
   int columnsCount = 10;
   int rowsCount = 300; // instance count
   int keyMatric;


   // -------------------------- Start: Alloc -------------------------------
   if (Vector_size(gl->genericRow)< 300)
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

   int interInst = -1, offset = -1;
   pmAtomValue value;
   /* for every generic row ... */
   while (PCPMetric_iterate(CGROUP_CPU_STAT_USER, &interInst, &offset)) {


   // -------------------------- Fill -------------------------------
   //fprintf(stderr, "........................ %d - %d\n", interInst, offset);


   /* CGROUP_CPU_STAT_USER column */
   if (PCPMetric_instance(CGROUP_CPU_STAT_USER, interInst, offset, &value, PM_TYPE_U64))
   {
       //process->mycgroup = value.ull; // SMA: This func not good at all with cgroup
      //fprintf(stderr, "........................ %d\n", offset);
      g = Hashtable_get(gl->genericTable, offset); // 0 != offset
      PCPGeneric* gg = (PCPGeneric*) g;



      // SMA: get field count
      PCPGenericField* gf = (PCPGenericField*)Hashtable_get(gg->fields, 0);
      gf->value->ull = value.ull;
   }







   /* CGROUP_CPU_STAT_USAGE column */
   if (PCPMetric_instance(CGROUP_CPU_STAT_USAGE, interInst, offset, &value, PM_TYPE_U64))
   {
      g = Hashtable_get(gl->genericTable, offset);
      PCPGeneric* gg = (PCPGeneric*) g;
      PCPGenericField* gf = (PCPGenericField*)Hashtable_get(gg->fields, 1);
      gf->value->ull = value.ull;
   }







   /* CGROUP_CPU_STAT_SYSTEM column */
   if (PCPMetric_instance(CGROUP_CPU_STAT_SYSTEM, interInst, offset, &value, PM_TYPE_U64))
   {
      g = Hashtable_get(gl->genericTable, offset);
      PCPGeneric* gg = (PCPGeneric*) g;
      PCPGenericField* gf = (PCPGenericField*)Hashtable_get(gg->fields, 2);
      gf->value->ull = value.ull;
   }



   /* External name column */
   g = Hashtable_get(gl->genericTable, offset);
   PCPGeneric* gg = (PCPGeneric*) g;
   PCPGenericField* gf = (PCPGenericField*)Hashtable_get(gg->fields, 3);
   // SMA: param 1 == key matric (aka PCPMetric_iterate):
   PCPMetric_externalName(CGROUP_CPU_STAT_USER, interInst, &gf->value->cp);

















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
