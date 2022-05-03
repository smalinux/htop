#ifndef HEADER_ProcessListPanel
#define HEADER_ProcessListPanel
/*
htop - ProcessListPanel.h
(C) 2004-2015 Hisham H. Muhammad
(C) 2020 Red Hat, Inc.  All Rights Reserved.
Released under the GNU GPLv2+, see the COPYING file
in the source distribution for its full text.
*/

#include "config.h" // IWYU pragma: keep

#include <stdbool.h>
#include <sys/types.h>

#include "Action.h"
#include "IncSet.h"
#include "Object.h"
#include "Panel.h"
#include "Process.h"


typedef struct ProcessListPanel_ {
   Panel super;
   State* state;
   IncSet* inc;
   Htop_Action* keys;
   pid_t pidSearch;
} ProcessListPanel;

typedef bool(*ProcessListPanel_ForeachProcessFn)(Process*, Arg);

#define ProcessListPanel_getFunctionBar(this_) (((Panel*)(this_))->defaultBar)

// update the Label Keys in the ProcessListPanel bar, list: list / tree mode, filter: filter (inc) active / inactive
void ProcessListPanel_updateLabels(ProcessListPanel* this, bool list, bool filter);

int ProcessListPanel_selectedPid(ProcessListPanel* this);

bool ProcessListPanel_foreachProcess(ProcessListPanel* this, ProcessListPanel_ForeachProcessFn fn, Arg arg, bool* wasAnyTagged);

extern const PanelClass ProcessListPanel_class;

ProcessListPanel* ProcessListPanel_new(void);

void ProcessListPanel_setState(ProcessListPanel* this, State* state);

void ProcessListPanel_delete(Object* object);

#endif

