/*
htop - PCPGenericList.c
(C) 2022 Sohaib Mohammed
(C) 2022 htop dev team
(C) 2022 Red Hat, Inc.
Released under the GNU GPLv2+, see the COPYING file
in the source distribution for its full text.
*/

#include "config.h" // IWYU pragma: keep

#include "Hashtable.h"
#include "pcp/PCPGenericList.h"
#include "pcp/PCPGeneric.h"
#include "GenericList.h"

static bool PCPGenericList_updateGenericList(PCPGenericList* this) // SMA xxg this
{
   /*
    * PCPGenericList ===> this
    * GenericList ======> gl
    */
   const GenericLists* gls = GenericList_getGenericLists();

   GenericList *myscreen = Hashtable_get(gls->table, 100);
   PCPGenericList *pcpmyscreen = (PCPGenericList*) myscreen;

   // Check Why this is wrrrrrrrrrrrrrrrrrrrong
   GenericList* gl = (GenericList*) this; // SMA xxg this
   // Check Why this is wrrrrrrrrrrrrrrrrrrrong
   const Settings* settings = gl->settings;

   int pid = -1, offset = -1;
   //while (PCPMetric_iterate(PCP_PROC_PID, &pid, &offset)) {
      Generic* g = GenericList_getGeneric(myscreen, PCPGeneric_new);

      PCPGeneric* gg = (PCPGeneric*) g;

      // -------------------------- Fill -------------------------------

      g->gtest = 666666;

      // SMA Start add & assing fields(columns).
      fprintf(stderr, "test: add new field w/ loop over all fields:\n");

      PCPGenericField * field = PCPGeneric_addField(gg, NULL);
      field->value->ul = 9999;

      field = PCPGeneric_addField(gg, NULL);
      field->value->ul = 8888;

      field = PCPGeneric_addField(gg, NULL);
      field->value->ul = 7777;

      field = PCPGeneric_addField(gg, NULL);
      field->value->ul = 6666;

      PCPGeneric_removeField(gg); // will remove the last one == 6666
      PCPGeneric_removeField(gg); // will remove the last one == 7777
      // SMA End add & assing Fields.


      // SMA: Test: Start print all rows & its node for testing
      //fprintf(stderr, ">>>>>>>>>>>>>>>>>>>>>>>>> row size %d\n\n", Vector_size(myscreen->genericRow));
      fprintf(stderr, ">>>>>>>>>>>>>>>>>>>>>>>>> totalRows %d\n\n", myscreen->totalRows);
      fprintf(stderr, "PCPGenericList->test %d\n", pcpmyscreen->test);
      for (int i = 0; i < Vector_size(myscreen->genericRow); i++)
      {
         fprintf(stderr, "Generic row %d - %d\n", myscreen->totalRows, Vector_size(myscreen->genericRow));

         for (int n = 0; n < gg->fieldsCount; n++)
         {
            PCPGenericField* f = Hashtable_get(gg->fields, n);
            fprintf(stderr, "field %d its value = %d\n", n, f->value->ul);
         }
         fprintf(stderr, "\n");
      }
      // SMA: Test: End print all rows & its node for testing

      // -------------------------- Add --------------------------------
         GenericList_addGeneric(myscreen, g); // keep this last line
      // ---------------------------------------------------------------

      // ------------- Remove Generic (just for testing) ---------------
      if(myscreen->totalRows > 5) {
         GenericList_removeGeneric(myscreen);
      }
      // ---------------------------------------------------------------

   //}
   return 0; // bool ??!!
}

void GenericList_goThroughEntries(GenericList * super, bool pauseProcessUpdate)
{
   PCPGenericList* this = (PCPGenericList*) super;
   const Settings* settings = super->settings;

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
