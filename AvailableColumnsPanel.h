#ifndef HEADER_AvailableColumnsPanel
#define HEADER_AvailableColumnsPanel
/*
htop - AvailableColumnsPanel.h
(C) 2004-2011 Hisham H. Muhammad
Released under the GNU GPLv2, see the COPYING file
in the source distribution for its full text.
*/

#include "Hashtable.h"
#include "Panel.h"
#include "ProcessList.h"


typedef struct AvailableColumnsPanel_ {
   Panel super;
   Panel* columns;
   Hashtable* dynamicColumns;
} AvailableColumnsPanel;

extern const PanelClass AvailableColumnsPanel_class;

AvailableColumnsPanel* AvailableColumnsPanel_new(Panel* columns, const ProcessList* pl);

#endif
