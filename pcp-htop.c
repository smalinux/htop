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

   myProcess p1 = {.sohaib = 555};
   myProcess p2 = {.sohaib = 8888};
   myProcess p3 = {.sohaib = 4};
   myProcess p4 = {.sohaib = 66};


   Vector_insert(v, 1, &p1);
   Vector_insert(v, 2, &p2);
   Vector_insert(v, 3, &p3);
   Vector_insert(v, 4, &p4);

   fprintf(stderr, "-------------------Before----------------------\n");
   myProcess *tmp;
   for (int i = 0; i < Vector_size(v); i++) {
       tmp = (myProcess*)Vector_get(v, i);
       fprintf(stderr, "%d = %d\n", i, tmp->sohaib);
   }


   //Vector_quickSortCustomCompare(this->processes, compareProcessByKnownParentThenNatural);
   Vector_quickSortCustomCompare(v, mycompareProcessByKnownParentThenNatural);

   fprintf(stderr, "-----------------------After-----------------------\n");

   for (int i = 0; i < Vector_size(v); i++) {
       tmp = (myProcess*)Vector_get(v, i);
       fprintf(stderr, "%d = %d\n", i, tmp->sohaib);
   }









   fflush(stderr);

   /* extract environment variables */
   opts.flags |= PM_OPTFLAG_ENV_ONLY;
   (void)pmGetOptions(argc, argv, &opts);

   return CommandLine_run(name, argc, argv);
}
