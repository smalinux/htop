#ifndef HEADER_PCPDynamicTab
#define HEADER_PCPDynamicTab

#include <stddef.h>

#include <stdbool.h>

#include "DynamicTab.h"
#include "Hashtable.h"
#include "Settings.h"


typedef struct PCPDynamicTab_ {
   DynamicTab super;
   char* instances;
   bool* enableInstance;
   bool enabled;
} PCPDynamicTab;

typedef struct PCPDynamicTabs_ {
   Hashtable* table;
   size_t count;  /* count of dynamic tabs discovered by scan */
} PCPDynamicTabs;

void PCPDynamicTabs_init(PCPDynamicTabs* tabs);

void PCPDynamicTabs_done(Hashtable* table);

void PCPDynamicTab_appendScreens(PCPDynamicTabs* tabs, Settings* settings);

#endif
