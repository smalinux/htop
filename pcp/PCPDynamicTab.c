/*
htop - PCPDynamicTab.c
(C) 2022 Sohaib Mohammed
(C) 2022 htop dev team
(C) 2022 Red Hat, Inc.
Released under the GNU GPLv2+, see the COPYING file
in the source distribution for its full text.
*/
#include "config.h" // IWYU pragma: keep

#include "pcp/PCPDynamicTab.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <pcp/pmapi.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Macros.h"
#include "Platform.h"
#include "RichString.h"
#include "XUtils.h"

#include "pcp/PCPMetric.h"


static PCPDynamicTabMetric* PCPDynamicTab_lookupMetric(PCPDynamicTabs* tabs, PCPDynamicTab* tab, const char* name) {
   size_t bytes = 16 + strlen(tab->super.name) + strlen(name);
   char* metricName = xMalloc(bytes);
   xSnprintf(metricName, bytes, "htop.tab.%s.%s", tab->super.name, name); // SMA FIXME Remove derived metric

   PCPDynamicTabMetric* metric;
   for (size_t i = 0; i < tab->totalMetrics; i++) {
      metric = &tab->metrics[i];
      if (String_eq(metric->name, metricName)) {
         free(metricName);
         return metric;
      }
   }

   /* not an existing metric in this tab - add it */
   size_t n = tab->totalMetrics + 1;
   tab->metrics = xReallocArray(tab->metrics, n, sizeof(PCPDynamicTabMetric));
   tab->totalMetrics = n;
   metric = &tab->metrics[n - 1];
   memset(metric, 0, sizeof(PCPDynamicTabMetric));
   metric->name = metricName;
   metric->label = String_cat(name, ": ");
   metric->id = tabs->offset + tabs->cursor;
   tabs->cursor++;

   Platform_addMetric(metric->id, metricName);

   return metric;
}

// // SMA FIXME remove derived metric
static void PCPDynamicTab_parseMetric(PCPDynamicTabs* tabs, PCPDynamicTab* tab, const char* path, unsigned int line, char* key, char* value) {
   PCPDynamicTabMetric* metric;
   char* p;

   if ((p = strchr(key, '.')) == NULL)
      return;
   *p++ = '\0'; /* end the name, p is now the attribute, e.g. 'label' */

   if (String_eq(p, "metric")) {
      /* lookup a dynamic metric with this name, else create */
      metric = PCPDynamicTab_lookupMetric(tabs, tab, key);

      /* use derived metrics in dynamic tabs for simplicity */
      char* error;
      if (pmRegisterDerivedMetric(metric->name, value, &error) < 0) {
         char* note;
         xAsprintf(&note,
                   "%s: failed to parse expression in %s at line %u\n%s\n%s",
                   pmGetProgname(), path, line, error, pmGetProgname());
         free(error);
         errno = EINVAL;
         CRT_fatalError(note);
         free(note);
      }
   } else {
      /* this is a property of a dynamic metric - the metric expression */
      /* may not have been observed yet - i.e. we allow for any ordering */
      metric = PCPDynamicTab_lookupMetric(tabs, tab, key);
      if (String_eq(p, "color")) {
         if (String_eq(value, "gray"))
            metric->color = DYNAMIC_GRAY;
         else if (String_eq(value, "darkgray"))
            metric->color = DYNAMIC_DARKGRAY;
         else if (String_eq(value, "red"))
            metric->color = DYNAMIC_RED;
         else if (String_eq(value, "green"))
            metric->color = DYNAMIC_GREEN;
         else if (String_eq(value, "blue"))
            metric->color = DYNAMIC_BLUE;
         else if (String_eq(value, "cyan"))
            metric->color = DYNAMIC_CYAN;
         else if (String_eq(value, "magenta"))
            metric->color = DYNAMIC_MAGENTA;
         else if (String_eq(value, "yellow"))
            metric->color = DYNAMIC_YELLOW;
         else if (String_eq(value, "white"))
            metric->color = DYNAMIC_WHITE;
      } else if (String_eq(p, "label")) {
         char* label = String_cat(value, ": ");
         free_and_xStrdup(&metric->label, label);
         free(label);
      } else if (String_eq(p, "suffix")) {
         free_and_xStrdup(&metric->suffix, value);
      }
   }
}

// Ensure a valid name for use in a PCP metric name and in htoprc
static bool PCPDynamicTab_validateTabName(char* key, const char* path, unsigned int line) {
   char* p = key;
   char* end = strrchr(key, ']');

   if (end) {
      *end = '\0';
   } else {
      fprintf(stderr,
              "%s: no closing brace on tab name at %s line %u\n\"%s\"\n",
              pmGetProgname(), path, line, key);
      return false;
   }

   while (*p) {
      if (p == key) {
         if (!isalpha(*p) && *p != '_')
            break;
      } else {
         if (!isalnum(*p) && *p != '_')
            break;
      }
      p++;
   }
   if (*p != '\0') { /* badness */
      fprintf(stderr,
              "%s: invalid tab name at %s line %u\n\"%s\"\n",
              pmGetProgname(), path, line, key);
      return false;
   }
   return true;
}

// Ensure a tab name has not been defined previously
static bool PCPDynamicTab_uniqueName(char* key, PCPDynamicTabs* tabs) {
   return !DynamicTab_search(tabs->table, key, NULL); // SMA FIXME
}

static PCPDynamicTab* PCPDynamicTab_new(PCPDynamicTabs* tabs, const char* name) {
   PCPDynamicTab* tab = xCalloc(1, sizeof(*tab));
   String_safeStrncpy(tab->super.name, name, sizeof(tab->super.name));
   Hashtable_put(tabs->table, ++tabs->count, tab);
   return tab;
}

static void PCPDynamicTab_parseFile(PCPDynamicTabs* tabs, const char* path) {
   FILE* file = fopen(path, "r");
   if (!file)
      return;

   PCPDynamicTab* tab = NULL;
   unsigned int lineno = 0;
   bool ok = true;
   for (;;) {
      char* line = String_readLine(file);
      if (!line)
         break;
      lineno++;

      /* cleanup whitespace, skip comment lines */
      char* trimmed = String_trim(line);
      free(line);
      if (trimmed[0] == '#' || trimmed[0] == '\0') {
         free(trimmed);
         continue;
      }

      size_t n;
      char** config = String_split(trimmed, '=', &n);
      free(trimmed);
      if (config == NULL)
         continue;

      char* key = String_trim(config[0]);
      char* value = n > 1 ? String_trim(config[1]) : NULL;
      if (key[0] == '[') {  /* new section heading - i.e. new tab */
         ok = PCPDynamicTab_validateTabName(key + 1, path, lineno); // SMA FIXME
         if (ok)
            ok = PCPDynamicTab_uniqueName(key + 1, tabs); // SMA FIXME
         if (ok)
            tab = PCPDynamicTab_new(tabs, key + 1); // SMA FIXME
      } else if (!ok) {
         ;  /* skip this one, we're looking for a new header */
      } else if (value && tab && String_eq(key, "caption")) {
         char* caption = String_cat(value, ": ");
         if (caption) {
            free_and_xStrdup(&tab->super.caption, caption);
            free(caption);
            caption = NULL;
         }
      } else if (value && tab && String_eq(key, "description")) {
         free_and_xStrdup(&tab->super.description, value);
      } else if (value && tab && String_eq(key, "type")) {
         if (String_eq(config[1], "bar"))
            tab->super.type = BAR_METERMODE;
         else if (String_eq(config[1], "text"))
            tab->super.type = TEXT_METERMODE;
         else if (String_eq(config[1], "graph"))
            tab->super.type = GRAPH_METERMODE;
         else if (String_eq(config[1], "led"))
            tab->super.type = LED_METERMODE;
      } else if (value && tab && String_eq(key, "maximum")) {
         tab->super.maximum = strtod(value, NULL);
      } else if (value && tab) {
         PCPDynamicTab_parseMetric(tabs, tab, path, lineno, key, value); // SMA FIXME
      }
      String_freeArray(config);
      free(value);
      free(key);
   }
   fclose(file);
}

static void PCPDynamicTab_scanDir(PCPDynamicTabs* tabs, char* path) {
   DIR* dir = opendir(path);
   if (!dir)
      return;

   struct dirent* dirent;
   while ((dirent = readdir(dir)) != NULL) {
      if (dirent->d_name[0] == '.')
         continue;

      char* file = String_cat(path, dirent->d_name);
      PCPDynamicTab_parseFile(tabs, file);
      free(file);
   }
   closedir(dir);
}

void PCPDynamicTabs_init(PCPDynamicTabs* tabs) {
   const char* share = pmGetConfig("PCP_SHARE_DIR");
   const char* sysconf = pmGetConfig("PCP_SYSCONF_DIR");
   const char* xdgConfigHome = getenv("XDG_CONFIG_HOME");
   const char* override = getenv("PCP_HTOP_DIR");
   const char* home = getenv("HOME");
   char* path;

   tabs->table = Hashtable_new(0, true);

   /* developer paths - PCP_HTOP_DIR=./pcp ./pcp-htop */
   if (override) {
      path = String_cat(override, "/tabs/");
      PCPDynamicTab_scanDir(tabs, path);
      free(path);
   }

   /* next, search in home directory alongside htoprc */
   if (xdgConfigHome)
      path = String_cat(xdgConfigHome, "/htop/tabs/");
   else if (home)
      path = String_cat(home, "/.config/htop/tabs/");
   else
      path = NULL;
   if (path) {
      PCPDynamicTab_scanDir(tabs, path);
      free(path);
   }

   /* next, search in the system tabs directory */
   path = String_cat(sysconf, "/htop/tabs/");
   PCPDynamicTab_scanDir(tabs, path);
   free(path);

   /* next, try the readonly system tabs directory */
   path = String_cat(share, "/htop/tabs/");
   PCPDynamicTab_scanDir(tabs, path);
   free(path);
}

static void PCPDynamicTab_free(ATTR_UNUSED ht_key_t key, void* value, ATTR_UNUSED void* data) {
   PCPDynamicTab* tab = (PCPDynamicTab*) value;
   for (size_t i = 0; i < tab->totalMetrics; i++) {
      free(tab->metrics[i].name);
      free(tab->metrics[i].label);
      free(tab->metrics[i].suffix);
   }
   free(tab->metrics);
   free(tab->super.caption);
   free(tab->super.description);
}

void PCPDynamicTabs_done(Hashtable* table) {
   Hashtable_foreach(table, PCPDynamicTab_free, NULL);
}

void PCPDynamicTab_enable(PCPDynamicTab* this) {
   for (size_t i = 0; i < this->totalMetrics; i++)
      PCPMetric_enable(this->metrics[i].id, true);
}
