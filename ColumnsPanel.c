/*
htop - ColumnsPanel.c
(C) 2004-2011 Hisham H. Muhammad
Released under the GNU GPLv2, see the COPYING file
in the source distribution for its full text.
*/

#include "ColumnsPanel.h"

#include <ctype.h>
#include <stdlib.h>

#include "CRT.h"
#include "DynamicColumn.h"
#include "FunctionBar.h"
#include "Hashtable.h"
#include "ListItem.h"
#include "Object.h"
#include "Process.h"
#include "ProvideCurses.h"
#include "XUtils.h"


static const char* const ColumnsFunctions[] = {"      ", "      ", "      ", "      ", "      ", "      ", "MoveUp", "MoveDn", "Remove", "Done  ", NULL};

static void ColumnsPanel_delete(Object* object) {
   Panel* super = (Panel*) object;
   ColumnsPanel* this = (ColumnsPanel*) object;
   Panel_done(super);
   free(this);
}

static HandlerResult ColumnsPanel_eventHandler(Panel* super, int ch) {
   ColumnsPanel* const this = (ColumnsPanel*) super;

   int selected = Panel_getSelectedIndex(super);
   HandlerResult result = IGNORED;
   int size = Panel_size(super);

   switch(ch) {
      case 0x0a:
      case 0x0d:
      case KEY_ENTER:
      case KEY_MOUSE:
      case KEY_RECLICK:
      {
         if (selected < size - 1) {
            this->moving = !(this->moving);
            Panel_setSelectionColor(super, this->moving ? PANEL_SELECTION_FOLLOW : PANEL_SELECTION_FOCUS);
            ListItem* selectedItem = (ListItem*) Panel_getSelected(super);
            if (selectedItem)
               selectedItem->moving = this->moving;
            result = HANDLED;
         }
         break;
      }
      case KEY_UP:
      {
         if (!this->moving) {
            break;
         }
      }
         /* else fallthrough */
      case KEY_F(7):
      case '[':
      case '-':
      {
         if (selected < size - 1)
            Panel_moveSelectedUp(super);
         result = HANDLED;
         break;
      }
      case KEY_DOWN:
      {
         if (!this->moving) {
            break;
         }
      }
         /* else fallthrough */
      case KEY_F(8):
      case ']':
      case '+':
      {
         if (selected < size - 2)
            Panel_moveSelectedDown(super);
         result = HANDLED;
         break;
      }
      case KEY_F(9):
      case KEY_DC:
      {
         if (selected < size - 1) {
            Panel_remove(super, selected);
         }
         result = HANDLED;
         break;
      }
      default:
      {
         if (0 < ch && ch < 255 && isgraph((unsigned char)ch))
            result = Panel_selectByTyping(super, ch);
         if (result == BREAK_LOOP)
            result = IGNORED;
         break;
      }
   }
   if (result == HANDLED)
      ColumnsPanel_update(super);
   return result;
}

const PanelClass ColumnsPanel_class = {
   .super = {
      .extends = Class(Panel),
      .delete = ColumnsPanel_delete
   },
   .eventHandler = ColumnsPanel_eventHandler
};

typedef struct {
   Panel* super;
   unsigned int id;
   unsigned int offset;
} DynamicIterator;

static void ColumnsPanel_add(Panel* super, unsigned int key, Hashtable* columns) {
   const char* name;
   if (key < LAST_PROCESSFIELD) {
      name = Process_fields[key].name;
   } else {
      const DynamicColumn* column = Hashtable_get(columns, key);
      assert(column);
      if (!column) {
         name = NULL;
      } else {
         name = column->caption ? column->caption : column->heading;
         if (!name)
            name = column->name; /* name is a mandatory field */
      }
   }
   if (name == NULL)
      name = "- ";
   Panel_add(super, (Object*) ListItem_new(name, key));
}

ColumnsPanel* ColumnsPanel_new(Settings* settings) {
   ColumnsPanel* this = AllocThis(ColumnsPanel);
   Panel* super = (Panel*) this;
   FunctionBar* fuBar = FunctionBar_new(ColumnsFunctions, NULL, NULL);
   Panel_init(super, 1, 1, 1, 1, Class(ListItem), true, fuBar);

   this->settings = settings;
   this->moving = false;
   Panel_setHeader(super, "Active Columns");

   Hashtable* dynamicColumns = settings->dynamicColumns;
   const ProcessField* fields = settings->fields;
   for (; *fields; fields++)
      ColumnsPanel_add(super, *fields, dynamicColumns);

   return this;
}

void ColumnsPanel_update(Panel* super) {
   ColumnsPanel* this = (ColumnsPanel*) super;
   int size = Panel_size(super);
   this->settings->changed = true;
   this->settings->fields = xRealloc(this->settings->fields, sizeof(ProcessField) * (size + 1));
   this->settings->flags = 0;
   for (int i = 0; i < size; i++) {
      int key = ((ListItem*) Panel_get(super, i))->key;
      this->settings->fields[i] = key;
      if (key < LAST_PROCESSFIELD)
         this->settings->flags |= Process_fields[key].flags;
   }
   this->settings->fields[size] = 0;
}
