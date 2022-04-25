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

static bool PCPGenericList_updateGenerics(PCPGenericList* this) // SMA xxg this
{
   /*
    * PCPGenericList ===> this
    * GenericList ======> gl
    */
   const GenericLists* gls = GenericLists_getGenericLists();

   GenericList *myscreen = Hashtable_get(gls->table, 100);

   // Check Why this is wrrrrrrrrrrrrrrrrrrrong
   GenericList* gl = (GenericList*) this; // Check Why this is wrrrrrrrrrrrrrrrrrrrong // SMA xxg this
   // Check Why this is wrrrrrrrrrrrrrrrrrrrong
   const Settings* settings = gl->settings;

   int pid = -1, offset = -1;
   //while (PCPMetric_iterate(PCP_PROC_PID, &pid, &offset)) {
      Generic* g = GenericList_getGeneric(myscreen, pid, PCPGeneric_new);
      PCPGeneric* gg = (PCPGeneric*) g;

      // -------------------------- Fill -------------------------------
      fprintf(stderr, "hiiiiii\n");
      gg->love = 44;
      g->id = 777;
      // -------------------------- Add --------------------------------
      /*
         GenericList_add(myscreen, g);
      */
      // ---------------------------------------------------------------
      fprintf(stderr, "%d\n", gg->love);
      fprintf(stderr, "%d\n", gg->GenericFieldsCount);
      PCPGenericField* gf = (PCPGenericField*)Hashtable_get(gg->genericFields, 0);
      fprintf(stderr, "value %d, pmid %d, offset %d, index %d\n",
            gf->value->ul, gf->pmid, gf->offset, gf->index);

      // genericRow Vector
      /*
         for (int i = 0; i < Vector_size(myscreen->genericRow); i++)
         {
            Generic* genericRow = (Generic*)Vector_get(myscreen->genericRow, i);
            fprintf(stderr, "genericRow %d\n", genericRow->id);
         }
      */
   //}
   return 0; // bool ??!!
}

void GenericList_goThroughEntries(GenericList * super, bool pauseProcessUpdate)
{
   PCPGenericList* this = (PCPGenericList*) super;
   const Settings* settings = super->settings;
   //fprintf(stderr, "hiiiiii\n");
   //PCPGenericList_updateGenerics(this, period, &timestamp);
   PCPGenericList_updateGenerics(this);
}
