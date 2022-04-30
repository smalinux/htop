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
   const GenericLists* gls = GenericLists_getGenericLists();

   GenericList *myscreen = Hashtable_get(gls->table, 100);

   // Check Why this is wrrrrrrrrrrrrrrrrrrrong
   GenericList* gl = (GenericList*) this; // SMA xxg this
   // Check Why this is wrrrrrrrrrrrrrrrrrrrong
   const Settings* settings = gl->settings;

   int pid = -1, offset = -1;
   //while (PCPMetric_iterate(PCP_PROC_PID, &pid, &offset)) {
      Generic* g = GenericList_getGeneric(myscreen, pid, PCPGeneric_new);
      PCPGeneric* gg = (PCPGeneric*) g;

      // -------------------------- Fill -------------------------------

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
      fprintf(stderr, ">>>>>>>>>>>>>>>>>>>>>>>>> row size %d\n\n", Vector_size(myscreen->genericRow));
      for (int i = 0; i < Vector_size(myscreen->genericRow); i++)
      {
         Generic* row = (Generic*)Vector_get(myscreen->genericRow, i);
         PCPGeneric* gr = (PCPGeneric*) row;
         fprintf(stderr, "Generic row %d, has %d columns\n", row->id, gr->fieldsCount);

         for (int n = 0; n < gg->fieldsCount; n++)
         {
            PCPGenericField* f = Hashtable_get(gg->fields, n);
            fprintf(stderr, "field %d its value = %d\n", n, f->value->ul);
         }
         fprintf(stderr, "\n");
      }
      // SMA: Test: End print all rows & its node for testing

      // -------------------------- Add --------------------------------
         GenericList_add(myscreen, g); // keep this last line
      // ---------------------------------------------------------------
   //}
   return 0; // bool ??!!
}

void GenericList_goThroughEntries(GenericList * super, bool pauseProcessUpdate)
{
   PCPGenericList* this = (PCPGenericList*) super;
   const Settings* settings = super->settings;
   //fprintf(stderr, "hiiiiii\n");
   //PCPGenericList_updateGenerics(this, period, &timestamp);
   PCPGenericList_updateGenericList(this);
}
