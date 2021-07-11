#ifndef HEADER_PCPDynamicColumn
#define HEADER_PCPDynamicColumn

#include "CRT.h"
#include "DynamicColumn.h"
#include "Hashtable.h"
#include "Process.h"
#include "RichString.h"

#include "pcp/PCPProcess.h"


typedef struct {
   DynamicColumn super;
   char* metricName;
   unsigned int id;  /* identifier for metric array lookups */
} PCPDynamicColumn;

typedef struct {
   Hashtable* table;
   unsigned int count;
   unsigned int offset;
   unsigned int cursor;
} PCPDynamicColumns;

void PCPDynamicColumns_init(PCPDynamicColumns* columns);

void PCPDynamicColumn_writeField(PCPDynamicColumn* this, const Process* proc, RichString* str);

int PCPDynamicColumn_compareByKey(const PCPProcess* p1, const PCPProcess* p2, ProcessField key);

#endif
