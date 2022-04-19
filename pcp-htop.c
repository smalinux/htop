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

   Vector* v = Vector_new(Class(myProcess), false, DEFAULT_SIZE);

   Process p1 = {.pid = 555};
   Process p2 = {.pid = 8888};
   Process p3 = {.pid = 4};
   Process p4 = {.pid = 66};


   Vector_insert(v, 1, &p1);
   Vector_insert(v, 2, &p2);
   Vector_insert(v, 3, &p3);
   Vector_insert(v, 4, &p4);

   fprintf(stderr, "-------------------Before----------------------\n");
   Process *tmp;
   for (int i = 0; i < Vector_size(v); i++) {
       tmp = (Process*)Vector_get(v, i);
       fprintf(stderr, "%d = %d\n", i, tmp->pid);
   }


   //Vector_quickSortCustomCompare(this->processes, compareProcessByKnownParentThenNatural);
   Vector_quickSortCustomCompare(v, compareProcessByKnownParentThenNatural);

   fprintf(stderr, "-----------------------After-----------------------\n");

   for (int i = 0; i < Vector_size(v); i++) {
       tmp = (Process*)Vector_get(v, i);
       fprintf(stderr, "%d = %d\n", i, tmp->pid);
   }









   fflush(stderr);

   /* extract environment variables */
   opts.flags |= PM_OPTFLAG_ENV_ONLY;
   (void)pmGetOptions(argc, argv, &opts);

   return CommandLine_run(name, argc, argv);
}
