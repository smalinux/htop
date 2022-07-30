/*
htop - DynamicTab.c
(C) 2022 Sohaib Mohammed
(C) 2022 htop dev team
(C) 2022 Red Hat, Inc.  All Rights Reserved.
Released under the GNU GPLv2+, see the COPYING file
in the source distribution for its full text.
*/

#include "config.h" // IWYU pragma: keep

#include "DynamicTab.h"

#include <stdbool.h>
#include <stddef.h>

#include "Hashtable.h"
#include "Platform.h"
#include "XUtils.h"
#include "config.h"


Hashtable* DynamicTabs_new(Settings* settings) {
   return Platform_dynamicTabs(settings);
}

void DynamicTabs_delete(Hashtable* dynamics) {
   if (dynamics) {
      Platform_dynamicTabsDone(dynamics);
      Hashtable_delete(dynamics);
   }
}

typedef struct {
   unsigned int key;
   const char* name;
   bool found;
} DynamicIterator;

static void DynamicTab_compare(ht_key_t key, void* value, void* data) {
   const DynamicTab* tab = (const DynamicTab*)value;
   DynamicIterator* iter = (DynamicIterator*)data;
   if (String_eq(iter->name, tab->name)) {
      iter->found = true;
      iter->key = key;
   }
}

bool DynamicTab_search(Hashtable* dynamics, const char* name, unsigned int* key) {
   DynamicIterator iter = { .key = 0, .name = name, .found = false };
   if (dynamics)
      Hashtable_foreach(dynamics, DynamicTab_compare, &iter);
   if (key)
      *key = iter.key;
   return iter.found;
}

const char* DynamicTab_lookup(Hashtable* dynamics, unsigned int key) {
   const DynamicTab* tab = Hashtable_get(dynamics, key);
   return tab ? tab->name : NULL;
}
