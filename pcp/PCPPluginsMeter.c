#include "config.h" // IWYU pragma: keep

#include "PCPPluginsMeter.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "CRT.h"
#include "Object.h"
#include "Platform.h"
#include "ProcessList.h"
#include "RichString.h"
#include "Settings.h"
#include "XUtils.h"


static const int PluginMeter_attributes[] = {
   PLUGINS_BLUE,
   PLUGINS_RED,
   PLUGINS_GREEN,
   PLUGINS_YELLOW,
   PLUGINS_CYAN,
   PLUGINS_SHADOW
};

static void PluginMeter_init(Meter* this) {
   int index = this->param;
   char* caption_value = Platform_plugin_caption(index);
   int totalplugins = Platform_totalplugins();
   for(int i = 0; i < totalplugins; i++) {
      char caption[100];
      xSnprintf(caption, sizeof(caption), "%s: ", caption_value);
      Meter_setCaption(this, caption);
   }
}

static void PluginMeter_updateValues(Meter* this) {
   char* buffer = this->txtBuffer;
   int pcp_metric_count = Platform_get_pcp_metric_count();
   size_t size = sizeof(this->txtBuffer);
   unsigned int index = this->param;
   pmAtomValue value = Platform_get_metric_value(index+pcp_metric_count);
   char* meter_type = Platform_plugin_type(index);
   float result = 0;
   int percent = 0;
   this->curItems = 4;
   char* color = Platform_plugin_color(index);
   char* label = Platform_plugin_label(index);
   uint64_t bar_max = Platform_plugin_bar_max(index);

   /*
    * Percent calculating
    *
   switch(meter_type) {
      case 0:
         result = ((plugins->bar_max[index] - value.l) * 100) / value.l;
         percent = (int)result;
         break;
      case 1:
         result = ((plugins->bar_max[index] - value.ul) * 100) / value.ul;
         percent = (int)result;
         break;
      case 2:
         result = ((plugins->bar_max[index] - value.ll) * 100) / value.ll;
         percent = (int)result;
         break;
      case 3:
         result = ((plugins->bar_max[index] - value.ull) * 100) / value.ull;
         percent = (int)result;
         break;
      case 4:
         result = ((plugins->bar_max[index] - value.f) * 100) / value.f;
         percent = (int)result;
         break;
      case 5:
         result = ((plugins->bar_max[index] - value.d) * 100) / value.d;
         percent = (int)result;
         break;
      default: // err case
         this->values[0] = 25; // blue
         this->values[1] = 25; // green
         this->values[2] = 25; // red
         this->values[3] = 25; // yellow
         break;
   }
   this->values[0] = percent;
   */

   // Start: temp. placeholder
   if(String_eq(color, "blue")) {
      //this->values[0] = percent;
      this->values[0] = 50; // 50 == %50
   } else if(String_eq(color, "green")) {
      //this->values[1] = percent;
      this->values[1] = 50;
   } else if(String_eq(color, "red")) {
      //this->values[2] = percent;
      this->values[2] = 50;
   } else if(String_eq(color, "yellow")) {
      //this->values[3] = percent;
      this->values[3] = 50;
   } else {
      //this->values[0] = percent;
      this->values[0] = 50;
   }
   // End: temp. placeholder


   // label
   xSnprintf(buffer, sizeof(buffer), "%s", label);
}

static void PluginMeter_display(ATTR_UNUSED const Object* cast, RichString* out) {
   const Meter* this = (const Meter*)cast;
   int pcp_metric_count = Platform_get_pcp_metric_count();
   char buffer[200];
   int index = this->param + pcp_metric_count;
   pmAtomValue value = Platform_get_metric_value(index);
   short metric_type = Platform_get_metric_type(index);
   char* color = Platform_plugin_color(this->param);
   char* label = Platform_plugin_label(this->param);

   // FIXME: cleanup! move this block to back-end & pcp/Platform.c
   switch(metric_type) {
      case 0:
         xSnprintf(buffer, sizeof(buffer), "%d ", value.l);
         break;
      case 1:
         xSnprintf(buffer, sizeof(buffer), "%u ", value.ul);
         break;
      case 2:
         xSnprintf(buffer, sizeof(buffer), "%lu ", value.ll);
         break;
      case 3:
         xSnprintf(buffer, sizeof(buffer), "%llu ", value.ull);
         break;
      case 4:
         xSnprintf(buffer, sizeof(buffer), "%f ", value.f);
         break;
      case 5:
         xSnprintf(buffer, sizeof(buffer), "%f ", value.d);
         break;
      case 6:
         xSnprintf(buffer, sizeof(buffer), "%s ", value.cp);
         break;
      default:
         xSnprintf(buffer, sizeof(buffer), "Err: pmDesc.type not implemented");
         break;
   }

   if(String_eq(color, "blue"))
      RichString_writeAscii(out, CRT_colors[PLUGINS_BLUE] , buffer);
   else if(String_eq(color, "red"))
      RichString_writeAscii(out, CRT_colors[PLUGINS_RED] , buffer);
   else if(String_eq(color, "yellow"))
      RichString_writeAscii(out, CRT_colors[PLUGINS_YELLOW] , buffer);
   else if(String_eq(color, "green"))
      RichString_writeAscii(out, CRT_colors[PLUGINS_GREEN] , buffer);
   else if(String_eq(color, "cyan"))
      RichString_writeAscii(out, CRT_colors[PLUGINS_CYAN] , buffer);
   else if(String_eq(color, "shadow"))
      RichString_writeAscii(out, CRT_colors[PLUGINS_SHADOW] , buffer);
   else
      RichString_writeAscii(out, CRT_colors[PLUGINS_GREEN] , buffer);

   // label
   xSnprintf(buffer, sizeof(buffer), "%s", label);
   RichString_appendAscii(out, CRT_colors[PLUGINS_SHADOW], buffer);
}


const MeterClass PCPPluginsMeter_class = {
   .super = {
      .extends = Class(Meter),
      .delete = Meter_delete,
      .display = PluginMeter_display
   },
   .updateValues = PluginMeter_updateValues,
   .defaultMode = TEXT_METERMODE,
   .maxItems = PCP_METRIC_COUNT,
   .total = 100.0,
   .attributes = PluginMeter_attributes,
   .name = "PCPPlugin",
   .uiName = "PCPPlgin",
   .caption = "PCPPlugin",
   .init = PluginMeter_init,
};
