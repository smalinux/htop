#ifndef HEADER_DynamicTab
#define HEADER_DynamicTab

#include <stdbool.h>

#include "Hashtable.h"
#include "Settings.h"


typedef struct DynamicTab_ {
   char name[32];  /* unique name, cannot contain spaces */
   char* caption;
   char* fields;
} DynamicTab;

Hashtable* DynamicTabs_new(Settings* settings);

void DynamicTabs_delete(Hashtable* dynamics);

const char* DynamicTab_lookup(Hashtable* dynamics, unsigned int key);

bool DynamicTab_search(Hashtable* dynamics, const char* name, unsigned int* key);

#endif
