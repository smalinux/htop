/*
htop - GenericData.c
(C) 2022 Sohaib Mohammed
(C) 2022 htop dev team
(C) 2022 Red Hat, Inc.
Released under the GNU GPLv2+, see the COPYING file
in the source distribution for its full text.
*/

#include "config.h" // IWYU pragma: keep

#include "pcp/PCPGenericData.h"

#include <stdbool.h>
#include <stdlib.h>

#include "CRT.h"
#include "DynamicColumn.h"
#include "DynamicScreen.h"
#include "GenericData.h"
#include "Hashtable.h"
#include "Macros.h"
#include "PCPDynamicColumn.h"
#include "PCPDynamicScreen.h"
#include "PCPMetric.h"
#include "Platform.h"
#include "Process.h"
#include "RichString.h"
#include "XUtils.h"


GenericData* PCPGenericData_new(const Settings* settings) {
   PCPGenericData* this = xCalloc(1, sizeof(PCPGenericData));
   Object_setClass(this, Class(PCPGenericData));

   this->fields = Hashtable_new(0, false);

   this->fieldsCount = 0;

   GenericData_init(&this->super, settings);
   return &this->super;
}

void GenericData_delete(Object* cast) {
   PCPGenericData* this = (PCPGenericData*) cast;

   for (int i = 1; i <= this->fieldsCount; i++)
      PCPGenericData_removeField(this);

   GenericData_done((GenericData*)cast);
   free(this);
}

PCPGenericDataField* PCPGenericData_addField(PCPGenericData* this)
{
   PCPGenericDataField* field = xCalloc(1, sizeof(PCPGenericDataField));
   pmAtomValue* atom = xCalloc(1, sizeof(pmAtomValue));

   field->value = atom;
   Hashtable_put(this->fields, this->fieldsCount, field);

   this->fieldsCount++;

   return field;
}

void PCPGenericData_removeField(PCPGenericData* this)
{
   int idx = this->fieldsCount - 1;

   PCPGenericDataField* field = Hashtable_get(this->fields, idx);
   free(field->value);
   Hashtable_remove(this->fields, idx);
   this->fieldsCount--;
}

void PCPGenericData_removeAllFields(PCPGenericData* this)
{
   for (int i = this->fieldsCount; i > 0; i--) {
      PCPGenericData_removeField(this);
   }
}

typedef struct {
   char* name;
   DynamicScreen* data;
   unsigned int key;
} DynamicIterator;

static void PCPGenericData_writeField(const GenericData* this, RichString* str, int field) {
   const PCPGenericData* gg = (const PCPGenericData*) this;
   PCPGenericDataField* gf = (PCPGenericDataField*)Hashtable_get(gg->fields, field);
   const ProcessField* fields = this->settings->ss->fields;
   char buffer[256];
   buffer[255] = '\0';
   int attr = CRT_colors[DEFAULT_COLOR];

   DynamicColumn* dc = Hashtable_get(this->settings->dynamicColumns, fields[field]);
   if (!dc)
      return;
   PCPDynamicColumn* column = (PCPDynamicColumn*) dc;
   bool instances = column->instances;

   int width = column->super.width;
   if (!width || abs(width) > DYNAMIC_MAX_COLUMN_WIDTH)
      width = DYNAMIC_DEFAULT_COLUMN_WIDTH;
   int abswidth = abs(width);
   if (abswidth > DYNAMIC_MAX_COLUMN_WIDTH) {
      abswidth = DYNAMIC_MAX_COLUMN_WIDTH;
      width = -abswidth;
   }

   if (instances) {
      char* instName;
      attr = CRT_colors[PROCESS_SHADOW];

      PCPMetric_externalName(gf->pmid, gf->offset, &instName);

      xSnprintf(buffer, sizeof(buffer), "%*s ", width, instName);
      RichString_appendAscii(str, attr, buffer);
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

static int PCPGenericData_compareByKey(ATTR_UNUSED const GenericData* v1, ATTR_UNUSED const GenericData* v2, ATTR_UNUSED int key) {

   return 0;
}

const GenericDataClass PCPGenericData_class = {
   .super = {
      .extends = Class(GenericData),
      .display = GenericData_display,
      .compare = GenericData_compare,
   },
   .writeField = PCPGenericData_writeField,
   .compareByKey = PCPGenericData_compareByKey,
};
