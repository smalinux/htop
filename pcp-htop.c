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

   int size = 3; // == p1->myPCPProcess->size

   myPCPProcess *p1 = malloc(sizeof(myPCPProcess));
   p1->v_list = malloc(size * sizeof(pmAtomValue));

   myPCPProcess *p2 = malloc(sizeof(myPCPProcess));
   p2->v_list = malloc(size * sizeof(pmAtomValue));

   myPCPProcess *p3 = malloc(sizeof(myPCPProcess));
   p3->v_list = malloc(size * sizeof(pmAtomValue));

   myPCPProcess *p4 = malloc(sizeof(myPCPProcess));
   p4->v_list = malloc(size * sizeof(pmAtomValue));


   p1->v_list[0].ul = 555;
   p2->v_list[0].ul = 8888;
   p3->v_list[0].ul = 4;
   p4->v_list[0].ul = 66;

   p1->v_list[1].ul = 55;
   p2->v_list[1].ul = 888;
   p3->v_list[1].ul = 44;
   p4->v_list[1].ul = 6;

   p1->v_list[2].ul = 5;
   p2->v_list[2].ul = 88;
   p3->v_list[2].ul = 444;
   p4->v_list[2].ul = 666;

   Vector_insert(v, 1, p1);
   Vector_insert(v, 2, p2);
   Vector_insert(v, 3, p3);
   Vector_insert(v, 4, p4);

   fprintf(stderr, "-------------------Before----------------------\n");
   myPCPProcess *tmp;
   for (int i = 0; i < Vector_size(v); i++) {
       tmp = (myPCPProcess*)Vector_get(v, i);
       fprintf(stderr, "idx %d=>    elm[0] %d,    elm[1] %d,    elm[2] %d\n",
               i, tmp->v_list[0].ul, tmp->v_list[1].ul, tmp->v_list[2].ul);
   }


   //Vector_quickSortCustomCompare(this->processes, compareProcessByKnownParentThenNatural);
   Vector_quickSortCustomCompare(v, mycompareProcessByKnownParentThenNatural);

   fprintf(stderr, "-----------------------After-----------------------\n");

   for (int i = 0; i < Vector_size(v); i++) {
       tmp = (myPCPProcess*)Vector_get(v, i);
       fprintf(stderr, "idx %d=>    elm[0] %d,    elm[1] %d,    elm[2] %d\n",
               i, tmp->v_list[0].ul, tmp->v_list[1].ul, tmp->v_list[2].ul);
   }









   fflush(stderr);

   /* extract environment variables */
   opts.flags |= PM_OPTFLAG_ENV_ONLY;
   (void)pmGetOptions(argc, argv, &opts);

   return CommandLine_run(name, argc, argv);
}
