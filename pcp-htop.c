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

// node { data & key }
typedef struct {
   int data; // could be any kind of data.
   unsigned int key;
} Iterator;

static void compare(ht_key_t key, void* value, void* data) {
    Iterator* val = (Iterator*) value;
    Iterator* user_data = (Iterator*) data;
        //fprintf(stderr, "val->data = %d - val->key = %d\n", val->data, val->key);
        //fprintf(stderr, "data->data = %d - data->key = %d\n", val->data, val->key);
    if(user_data->data == val->data) {
        //fprintf(stderr, "found in key = %d\n", key);
        user_data->key = key;
    }
}

int main(int argc, char** argv) {
   const char* name = "pcp-htop";
   pmSetProgname(name);

   /* .......... Hashtable_new .......... */
   Hashtable *ht = Hashtable_new(20, false);


   /* .......... Hashtable_put .......... */
   Iterator xp = {.data = 11, .key = 1};
   Hashtable_put(ht, xp.key, &xp);

   Iterator yp = {.data = 22, .key = 2};
   Hashtable_put(ht, yp.key, &yp);

   Iterator zp = {.data = 33, .key = 3};
   Hashtable_put(ht, zp.key, &zp);

   Iterator zzp = {.data = 33, .key = 4};
   Hashtable_put(ht, zzp.key, &zzp);


   /* .......... Hashtable_get .......... */
   int *get = Hashtable_get(ht, 2);
   fprintf(stderr, "Hashtable_count = %d\n", *get);


   /* .......... Hashtable_foreach .......... */
   Iterator iter = { .key = 0, .data = 33 };
   Hashtable_foreach(ht, compare, &iter);
   if (iter.key)
       fprintf(stderr, "item key = %d\n", iter.key);


   /* .......... Hashtable_remove .......... */
   Iterator *r = Hashtable_remove(ht, 2);
   fprintf(stderr, "removed elm = { key = %d, val = %d }\n", r->key, r->data);



   /* .......... Hashtable_delete .......... */
   Hashtable_delete(ht);


   fflush( stderr );

   /* extract environment variables */
   opts.flags |= PM_OPTFLAG_ENV_ONLY;
   (void)pmGetOptions(argc, argv, &opts);

   return CommandLine_run(name, argc, argv);
}
