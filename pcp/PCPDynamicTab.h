#ifndef HEADER_PCPDynamicTab
#define HEADER_PCPDynamicTab

#include <stddef.h>

#include "CRT.h"
#include "DynamicTab.h" // SMA FIXME
#include "Hashtable.h"
#include "Meter.h" // SMA REMOVEME
#include "RichString.h"


typedef struct PCPDynamicTabMetric_ {
   size_t id; /* index into metric array */
   ColorElements color; // SMA: REMOVEME
   char* name; /* derived metric name */ // SMA: not derived! or add the original inDom field ...
   char* label;
   char* suffix;
} PCPDynamicTabMetric; // SMA: Rename this with a new name .... for tabs & generic columns

typedef struct PCPDynamicTab_ {
   DynamicTab super; // SMA FIXME
   PCPDynamicTabMetric* metrics;
   size_t totalMetrics;
} PCPDynamicTab;

typedef struct PCPDynamicTabs_ {
   Hashtable* table;
   size_t count;  /* count of dynamic tabs discovered by scan */
   size_t offset; /* start offset into the Platform metric array */
   size_t cursor; /* identifier allocator for each new metric used */
} PCPDynamicTabs;

void PCPDynamicTabs_init(PCPDynamicTabs* tabs);

void PCPDynamicTabs_done(Hashtable* table);

void PCPDynamicTab_enable(PCPDynamicTab* this);

//void PCPDynamicMeter_updateValues(PCPDynamicMeter* this, Meter* meter); // SMA
//REMOVEME

//void PCPDynamicMeter_display(PCPDynamicMeter* this, const Meter* meter, RichString* out); // SMA REMOVEME

#endif

