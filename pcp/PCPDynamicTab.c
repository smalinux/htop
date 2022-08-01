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
#include <pcp/pmapi.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Macros.h"
#include "Settings.h"
#include "XUtils.h"


static bool PCPDynamicTab_validateTabName(char* key, const char* path, unsigned int line) {
   char* p = key;
   char* end = strrchr(key, ']');

   if (end) {
      *end = '\0';
   } else {
      fprintf(stderr,
            "%s: no closing brace on tab name at %s line %u\n\"%s\"",
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
            "%s: invalid tab name at %s line %u\n\"%s\"",
            pmGetProgname(), path, line, key);
      return false;
   }
   return true;
}

// Ensure a tab name has not been defined previously
static bool PCPDynamicTab_uniqueName(char* key, PCPDynamicTabs* tabs) {
   return !DynamicTab_search(tabs->table, key, NULL);
}

static PCPDynamicTab* PCPDynamicTab_new(PCPDynamicTabs* tabs, const char* name) {
   PCPDynamicTab* tab = xCalloc(1, sizeof(*tab));
   String_safeStrncpy(tab->super.name, name, sizeof(tab->super.name));

   size_t id = tabs->count;
   Hashtable_put(tabs->table, id, tab);
   tabs->count++;

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
      if (!trimmed || !trimmed[0] || trimmed[0] == '#') {
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
         ok = PCPDynamicTab_validateTabName(key + 1, path, lineno);
         if (ok)
            ok = PCPDynamicTab_uniqueName(key + 1, tabs);
         if (ok)
            tab = PCPDynamicTab_new(tabs, key + 1);
      } else if (value && tab && String_eq(key, "caption")) {
         free_and_xStrdup(&tab->super.caption, value);
      } else if (value && tab && String_eq(key, "columns")) {
         free_and_xStrdup(&tab->super.fields, value);
      } else if (value && tab && String_eq(key, "enabled")) {
         if (strcmp(value, "true") || strcmp(value, "True"))
            tab->enabled = 1;
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

static void PCPDynamicTabs_free(ATTR_UNUSED ht_key_t key, void* value, ATTR_UNUSED void* data) {
   PCPDynamicTab* tab = (PCPDynamicTab*) value;
   free(tab->instances);
   free(tab->super.caption);
   free(tab->super.fields);
}

void PCPDynamicTabs_done(Hashtable* table) {
   Hashtable_foreach(table, PCPDynamicTabs_free, NULL);
}

static char* formatFields(char* fields) {
   char* trim = String_trim(fields);
   char** ids = String_split(trim, ' ', NULL);
   char* columns = strdup("");
   free(trim);

   int len = 0;
   for (int j = 0; ids[j]; j++) {
      len++;
   }

   for (int j = 0; j < len; j++)
      xAsprintf(&columns, "%s Dynamic(%s)", columns, ids[j]);

   return columns;
}

void PCPDynamicTab_appendScreens(PCPDynamicTabs* tabs, Settings* settings) {
   PCPDynamicTab* dt;
   ScreenSettings* ss;

   for (size_t i = 0; i < tabs->count; i++) {
      dt = (PCPDynamicTab*)Hashtable_get(tabs->table, i);
      if (!dt)
         continue;
      if (!dt->enabled)
         continue;

      char* columns = formatFields(dt->super.fields);

      ScreenDefaults sd = {
         .name = dt->super.caption,
         .columns = columns,
      };

      ss = Settings_newScreen(settings, &sd);
      ss->generic = true;
      free(columns);
   }
}
