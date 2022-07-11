#ifndef HEADER_DynamicTab
#define HEADER_DynamicTab

#include <stdbool.h>

#include "Hashtable.h"


typedef struct DynamicTab_ {
   char name[32];  /* unique name, cannot contain spaces */
   char* caption;
   char* description;
   unsigned int type;
   double maximum;
} DynamicTab;

Hashtable* DynamicTabs_new(void);

void DynamicTabs_delete(Hashtable* dynamics);

const char* DynamicTab_lookup(Hashtable* dynamics, unsigned int key);

bool DynamicTab_search(Hashtable* dynamics, const char* name, unsigned int* key);

#endif
