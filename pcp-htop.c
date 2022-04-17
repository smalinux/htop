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

static int c(const void* v1, const void* v2) {
   const Process* p1 = (const Process*)v1;
   const Process* p2 = (const Process*)v2;

   return SPACESHIP_NUMBER(p1->pid, p2->pid);
}

//..........................Start three level...................................
typedef struct {
   int data; // could be any kind of data. ex: pmAtomValue
   pmInDom indom; // pminfo -d cgroup.cpu.stat.user
   pmID pmid; // pminfo -m cgroup.cpu.stat.user
   size_t internalInstance;
} node;

static void hash_compare(ht_key_t key, void* value, void* data) {
    node* val = (node*) value;
    node* user_data = (node*) data;
    if(
            user_data->indom == val->indom &&
            user_data->pmid == val->pmid &&
            user_data->internalInstance == val->internalInstance) {

        user_data->data = val->data;
    }
}

//..........................End three level...................................
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

   /* ...................................................................... */
   /* .......................... Three levels key .......................... */
   /* ...........................There is no Three levels key............... */
   /* ............................ Becuase this prototype: ................. */
   /* void Hashtable_put(Hashtable* this, ht_key_t key, void* value);        */


   /* .......... Hashtable_new .......... */
   Hashtable *hash = Hashtable_new(20, false);

   /* .......... Hashtable_put .......... */
   node n1 = { .data = 1111, .indom = 1, .pmid = 11, .internalInstance = 111};
   //Hashtable_put(hash, n1.indom*n1.pmid, &n1);
   Hashtable_put(hash, 1, &n1); // SMA: search how to generate unique number from 2 num
   // indom & pmid
   // or use hashtable->hashtable->vector

   node n2 = { .data = 2222, .indom = 2, .pmid = 22, .internalInstance = 222};
   Hashtable_put(hash, 2, &n2);

   node n3 = { .data = 3333, .indom = 3, .pmid = 33, .internalInstance = 333};
   Hashtable_put(hash, 3, &n3);

   node n4 = { .data = 4444, .indom = 3, .pmid = 33, .internalInstance = 444};
   Hashtable_put(hash, 4, &n4);

   node n5 = { .data = 5555, .indom = 3, .pmid = 33, .internalInstance = 555};
   Hashtable_put(hash, 5, &n5);


   /* .......... Hashtable_foreach .......... */
   node i = { .indom = 3, .pmid = 33, .internalInstance = 444 };
   Hashtable_foreach(hash, hash_compare, &i);
   if (i.indom)
       fprintf(stderr, "3 levels item value = %d\n", i.data);

   /* ...................................................................... */
   // ............................... Vector ...............................
   /* ...................................................................... */

   Vector *v = Vector_new(Class(Process), true, DEFAULT_SIZE);

    fprintf(stderr, "Before sort: \n");
   int d1 = 11;
   Vector_insert(v, 1, &d1);

   int d3 = 33;
   Vector_insert(v, 3, &d3);

   int d2 = 22;
   Vector_insert(v, 2, &d2);

   int d4 = 44;
   Vector_insert(v, 4, &d4);

   Object* t;
   for(int idx = 0; idx <Vector_size(v); idx++) {
       t = Vector_get(v, idx);
       fprintf(stderr, "i = %d\n", *(int*)t);
   }

    fprintf(stderr, "=================\n");
    fprintf(stderr, "After sort: \n");
    Vector_quickSortCustomCompare(v, c);
   for(int idx = 0; idx <Vector_size(v); idx++) {
       t = Vector_get(v, idx);
       fprintf(stderr, "i = %d\n", *(int*)t);
   }


   fflush( stderr );

   /* extract environment variables */
   opts.flags |= PM_OPTFLAG_ENV_ONLY;
   (void)pmGetOptions(argc, argv, &opts);

   return CommandLine_run(name, argc, argv);
}
