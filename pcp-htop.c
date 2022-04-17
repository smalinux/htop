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

static void DynamicColumn_compare(ht_key_t key, void* value, void* data) {
    fprintf(stderr, "ffffffffffffffffff\n");
}

int main(int argc, char** argv) {
   const char* name = "pcp-htop";
   pmSetProgname(name);

   /* .......... Hashtable_new .......... */
   Hashtable *ht = Hashtable_new(20, false);


   /* .......... Hashtable_put .......... */
   int x = 11;
   int *xp = &x;
   Hashtable_put(ht, 1, xp);

   int y = 22;
   int *yp = &y;
   Hashtable_put(ht, 2, yp);

   int z = 33;
   int *zp = &z;
   Hashtable_put(ht, 3, zp);


   /* .......... Hashtable_get .......... */
   int *get = Hashtable_get(ht, 2);
   fprintf(stderr, "Hashtable_count = %d\n", *get);


   /* .......... Hashtable_foreach .......... */
   int iter = 2;
   Hashtable_foreach(ht, DynamicColumn_compare, NULL);


   /* .......... Hashtable_remove .......... */
   int *r = Hashtable_remove(ht, 2);
   fprintf(stderr, "Hashtable_count = %d\n", *r);



   /* .......... Hashtable_delete .......... */
   Hashtable_delete(ht);


   fflush( stderr );

   /* extract environment variables */
   opts.flags |= PM_OPTFLAG_ENV_ONLY;
   (void)pmGetOptions(argc, argv, &opts);

   return CommandLine_run(name, argc, argv);
}
