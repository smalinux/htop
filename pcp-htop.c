/*
htop - pcp-htop.c
(C) 2004-2011 Hisham H. Muhammad
(C) 2020-2021 htop dev team
Released under the GNU GPLv2+, see the COPYING file
in the source distribution for its full text.
*/

#include "config.h" // IWYU pragma: keep

#include <pcp/pmapi.h>

#include "CommandLine.h"
#include "Platform.h"


int main(int argc, char** argv) {
   const char* name = "pcp-htop";
   pmSetProgname(name);


   Vector* v = Vector_new(Class(myPCPProcess), false, DEFAULT_SIZE);

   myPCPProcess p1 = {.v_list.ul = 555};
   myPCPProcess p2 = {.v_list.ul = 8888};
   myPCPProcess p3 = {.v_list.ul = 4};
   myPCPProcess p4 = {.v_list.ul = 66};


   Vector_insert(v, 1, &p1);
   Vector_insert(v, 2, &p2);
   Vector_insert(v, 3, &p3);
   Vector_insert(v, 4, &p4);

   fprintf(stderr, "-------------------Before----------------------\n");
   myPCPProcess *tmp;
   for (int i = 0; i < Vector_size(v); i++) {
       tmp = (myPCPProcess*)Vector_get(v, i);
       fprintf(stderr, "%d = %d\n", i, tmp->v_list.ul);
   }


   //Vector_quickSortCustomCompare(this->processes, compareProcessByKnownParentThenNatural);
   Vector_quickSortCustomCompare(v, mycompareProcessByKnownParentThenNatural);

   fprintf(stderr, "-----------------------After-----------------------\n");

   for (int i = 0; i < Vector_size(v); i++) {
       tmp = (myPCPProcess*)Vector_get(v, i);
       fprintf(stderr, "%d = %d\n", i, tmp->v_list.ul);
   }









   fflush(stderr);

   /* extract environment variables */
   opts.flags |= PM_OPTFLAG_ENV_ONLY;
   (void)pmGetOptions(argc, argv, &opts);

   return CommandLine_run(name, argc, argv);
}
