#ifndef HEADER_PCPDynamicTab
#define HEADER_PCPDynamicTab

#include <stddef.h>

#include "CRT.h"
#include "DynamicTab.h" // SMA FIXME
#include "Hashtable.h"
#include "Meter.h" // SMA REMOVEME
#include "RichString.h"
#include "Settings.h"



typedef struct PCPDynamicTab_ {
   DynamicTab super;
   char* metricName;
   size_t id;  /* identifier for metric array lookups */
} PCPDynamicTab;

typedef struct PCPDynamicTabs_ {
   Hashtable* table;
   size_t count;  /* count of dynamic meters discovered by scan */
   size_t offset; /* start offset into the Platform metric array */
   size_t cursor; /* identifier allocator for each new metric used */
} PCPDynamicTabs;

void PCPDynamicTabs_init(PCPDynamicTabs* tabs);

void PCPDynamicTabs_done(Hashtable* table);

void PCPDynamicTab_appendScreens(PCPDynamicTabs* tabs, Settings* settings);

#endif

