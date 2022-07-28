/*
htop - Generic.c
(C) 2022 Sohaib Mohammed
(C) 2022 htop dev team
(C) 2022 Red Hat, Inc.
Released under the GNU GPLv2+, see the COPYING file
in the source distribution for its full text.
*/

#include "config.h" // IWYU pragma: keep

#include "pcp/PCPGeneric.h"

#include "RichString.h"
#include "CRT.h"
#include "Macros.h"
#include "Generic.h"
#include "ProvideCurses.h"
#include "RichString.h"
#include "Platform.h"
#include "Hashtable.h"
#include "XUtils.h"


Generic* PCPGeneric_new(const Settings* settings) {
   PCPGeneric* this = xCalloc(1, sizeof(PCPGeneric));
   Object_setClass(this, Class(PCPGeneric));

   this->fields = Hashtable_new(0, false);

   this->fieldsCount = 0;

   Generic_init(&this->super, settings);
   return &this->super;
}

void Generic_delete(Object* cast) {
   PCPGeneric* this = (PCPGeneric*) cast;

   for(int i = 0; i <= this->fieldsCount; i++)
      PCPGeneric_removeField(this);

   Generic_done((Generic*)cast);
   free(this);
}

PCPGenericField* PCPGeneric_addField(PCPGeneric* this, const Settings* settings)
{
   PCPGenericField* field = xCalloc(1, sizeof(PCPGenericField));
   pmAtomValue *atom = xCalloc(1, sizeof(pmAtomValue));

   field->value = atom;
   Hashtable_put(this->fields, this->fieldsCount, field);

   this->fieldsCount += 1;
    //settings here if you want to read from it.. to assing...
    //Reference: Process_init()
   return field;
}

void PCPGeneric_removeField(PCPGeneric* this)
{
   int idx = this->fieldsCount -1;

   PCPGenericField* field = Hashtable_get(this->fields, idx);
   free(field->value);
   Hashtable_remove(this->fields, idx);
   this->fieldsCount--;
}

void PCPGeneric_removeAllFields(PCPGeneric* this)
{
   for (int i = this->fieldsCount; i >= 0; i--) {
      PCPGeneric_removeField(this);
   }
}

typedef struct {
   char* name;
   DynamicTab* data;
   unsigned int key;
} DynamicIterator;

static void DynamicTab_compare(ht_key_t key, void* value, void* data) {
   DynamicTab* tab = (DynamicTab*)value;
   DynamicIterator* iter = (DynamicIterator*)data;
   if (String_eq(iter->name, tab->name)) {
      iter->data = tab;
      iter->key = key;
   }
}

static DynamicTab* getCurrentTab(Hashtable* tabs, char* name) {
   DynamicIterator iter = { .key = 0, .name = name };
   if (tabs)
      Hashtable_foreach(tabs, DynamicTab_compare, &iter);
   if (name)
      name = iter.name;
   return iter.data;
}

static void PCPGeneric_writeField(const Generic* this, RichString* str, int field) {
   const PCPGeneric* gg = (const PCPGeneric*) this;
   PCPGenericField* gf = (PCPGenericField*)Hashtable_get(gg->fields, field);
   const ProcessField* fields = this->settings->ss->fields;
   char buffer[256]; buffer[255] = '\0';
   int attr = CRT_colors[DEFAULT_COLOR];
   char* tab_username = this->settings->ss->username;
   Hashtable* pTabs = Platform_getDynamicTabs();

   DynamicColumn* dc = Hashtable_get(this->settings->dynamicColumns, fields[field]);
   if (!dc)
      return;
   PCPDynamicColumn* column = (PCPDynamicColumn*) dc;

   int width = column->super.width;
   if (!width || abs(width) > DYNAMIC_MAX_COLUMN_WIDTH)
      width = DYNAMIC_DEFAULT_COLUMN_WIDTH;
   int abswidth = abs(width);
   if (abswidth > DYNAMIC_MAX_COLUMN_WIDTH) {
      abswidth = DYNAMIC_MAX_COLUMN_WIDTH;
      width = -abswidth;
   }

   DynamicTab* tab = getCurrentTab(pTabs, tab_username);
   PCPDynamicTab* p_tab = (PCPDynamicTab*) tab;
   bool printInstance = p_tab->enableInstance[field];


   if (printInstance) {
      char* instName;
      attr = CRT_colors[PROCESS_SHADOW];

      PCPMetric_externalName(gf->pmid, gf->offset, &instName);

      xSnprintf(buffer, sizeof(buffer), "%*s ", width, instName);
      RichString_appendAscii(str, attr, buffer);
      free(instName);
   } else {
      switch (gf->type) {
         case PM_TYPE_STRING:
            attr = CRT_colors[PROCESS_SHADOW];
            xSnprintf(buffer, sizeof(buffer), "%*s ", width, gf->value->cp);
            RichString_appendAscii(str, attr, buffer);
            break;
         case PM_TYPE_32:
            xSnprintf(buffer, sizeof(buffer), "%*d ", width, gf->value->l);
            RichString_appendAscii(str, attr, buffer);
            break;
         case PM_TYPE_U32:
            xSnprintf(buffer, sizeof(buffer), "%*u ", width, gf->value->ul);
            RichString_appendAscii(str, attr, buffer);
            break;
         case PM_TYPE_64:
            xSnprintf(buffer, sizeof(buffer), "%*lld ", width, (long long) gf->value->ll);
            RichString_appendAscii(str, attr, buffer);
            break;
         case PM_TYPE_U64:
            xSnprintf(buffer, sizeof(buffer), "%*llu ", width, (unsigned long long) gf->value->ull);
            RichString_appendAscii(str, attr, buffer);
            break;
         case PM_TYPE_FLOAT:
            xSnprintf(buffer, sizeof(buffer), "%*.2f ", width, (double) gf->value->f);
            RichString_appendAscii(str, attr, buffer);
            break;
         case PM_TYPE_DOUBLE:
            xSnprintf(buffer, sizeof(buffer), "%*.2f ", width, gf->value->d);
            RichString_appendAscii(str, attr, buffer);
            break;
         default:
            attr = CRT_colors[METER_VALUE_ERROR];
            RichString_appendAscii(str, attr, "no type");
            break;
      }
   }
}

static int PCPGeneric_compareByKey(const Generic* v1, const Generic* v2, int key) {
   const PCPGeneric* g1 = (const PCPGeneric*)v1;
   const PCPGeneric* g2 = (const PCPGeneric*)v2;

   switch (key) {
      case 0:
         return 0;
      default:
         return 0;
   }
}

const GenericClass PCPGeneric_class = {
   .super = {
      .extends = Class(Generic),
      .display = Generic_display,
      .compare = Generic_compare,
   },
   .writeField = PCPGeneric_writeField,
   .compareByKey = PCPGeneric_compareByKey,
};
