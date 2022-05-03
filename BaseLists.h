#ifndef HEADER_BaseLists
#define HEADER_BaseLists
/*
htop - ColumnsPanel.h
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


typedef struct BaseLists_ {
   Panel super;
   State* state;
   IncSet* inc;
   Htop_Action* keys;
   pid_t pidSearch;
} BaseLists;

typedef bool(*BaseLists_ForeachProcessFn)(Process*, Arg);

#define BaseLists_getFunctionBar(this_) (((Panel*)(this_))->defaultBar)

// update the Label Keys in the BaseLists bar, list: list / tree mode, filter: filter (inc) active / inactive
void BaseLists_updateLabels(BaseLists* this, bool list, bool filter);

int BaseLists_selectedPid(BaseLists* this);

bool BaseLists_foreachProcess(BaseLists* this, BaseLists_ForeachProcessFn fn, Arg arg, bool* wasAnyTagged);

extern const PanelClass BaseLists_class;

BaseLists* BaseLists_new(void);

void BaseLists_setState(BaseLists* this, State* state);

void BaseLists_delete(Object* object);

#endif
