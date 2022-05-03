/*
htop - ColumnsPanel.c
(C) 2004-2015 Hisham H. Muhammad
(C) 2020 Red Hat, Inc.  All Rights Reserved.
Released under the GNU GPLv2+, see the COPYING file
in the source distribution for its full text.
*/

#include "BaseLists.h"

#include <ctype.h>
#include <stdlib.h>

#include "CRT.h"
#include "FunctionBar.h"
#include "Platform.h"
#include "Process.h"
#include "ProcessList.h"
#include "ProvideCurses.h"
#include "Settings.h"
#include "XUtils.h"


static const char* const MainFunctions[]     = {"Help  ", "Setup ", "Search", "Filter", "Tree  ", "SortBy", "Nice -", "Nice +", "Kill  ", "Quit  ", NULL};
static const char* const MainFunctions_ro[]  = {"Help  ", "Setup ", "Search", "Filter", "Tree  ", "SortBy", "      ", "      ", "      ", "Quit  ", NULL};

void BaseLists_updateLabels(BaseLists* this, bool list, bool filter) {
   FunctionBar* bar = BaseLists_getFunctionBar(this);
   FunctionBar_setLabel(bar, KEY_F(5), list   ? "List  " : "Tree  ");
   FunctionBar_setLabel(bar, KEY_F(4), filter ? "FILTER" : "Filter");
}

static void BaseLists_pidSearch(BaseLists* this, int ch) {
   Panel* super = (Panel*) this;
   pid_t pid = ch - 48 + this->pidSearch;
   for (int i = 0; i < Panel_size(super); i++) {
      const Process* p = (const Process*) Panel_get(super, i);
      if (p && p->pid == pid) {
         Panel_setSelected(super, i);
         break;
      }
   }
   this->pidSearch = pid * 10;
   if (this->pidSearch > 10000000) {
      this->pidSearch = 0;
   }
}

static const char* BaseLists_getValue(Panel* this, int i) {
   const Process* p = (const Process*) Panel_get(this, i);
   return Process_getCommand(p);
}

static HandlerResult BaseLists_eventHandler(Panel* super, int ch) {
}

int BaseLists_selectedPid(BaseLists* this) {
   const Process* p = (const Process*) Panel_getSelected((Panel*)this);
   if (p) {
      return p->pid;
   }
   return -1;
}

bool BaseLists_foreachProcess(BaseLists* this, BaseLists_ForeachProcessFn fn, Arg arg, bool* wasAnyTagged) {
   Panel* super = (Panel*) this;
   bool ok = true;
   bool anyTagged = false;
   for (int i = 0; i < Panel_size(super); i++) {
      Process* p = (Process*) Panel_get(super, i);
      if (p->tag) {
         ok = fn(p, arg) && ok;
         anyTagged = true;
      }
   }
   if (!anyTagged) {
      Process* p = (Process*) Panel_getSelected(super);
      if (p) {
         ok &= fn(p, arg);
      }
   }

   if (wasAnyTagged)
      *wasAnyTagged = anyTagged;

   return ok;
}

static void BaseLists_drawFunctionBar(Panel* super, bool hideFunctionBar) {
}

static void BaseLists_printHeader(Panel* super) {
}

const PanelClass BaseLists_class = {
   .super = {
      .extends = Class(Panel),
      .delete = BaseLists_delete
   },
   .eventHandler = BaseLists_eventHandler,
   .drawFunctionBar = BaseLists_drawFunctionBar,
   .printHeader = BaseLists_printHeader
};

BaseLists* BaseLists_new() {
   BaseLists* this = AllocThis(BaseLists);
   Panel_init((Panel*) this, 1, 1, 1, 1, Class(Process), false, FunctionBar_new(Settings_isReadonly() ? MainFunctions_ro : MainFunctions, NULL, NULL));
   this->keys = xCalloc(KEY_MAX, sizeof(Htop_Action));
   this->inc = IncSet_new(BaseLists_getFunctionBar(this));

   Action_setBindings(this->keys);
   Platform_setBindings(this->keys);

   return this;
}

void BaseLists_setState(BaseLists* this, State* state) {
   this->state = state;
}

void BaseLists_delete(Object* object) {
}
