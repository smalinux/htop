/*
htop - PCPDynamicScreen.c
(C) 2022 Sohaib Mohammed
(C) 2022 htop dev team
(C) 2022 Red Hat, Inc.
Released under the GNU GPLv2+, see the COPYING file
in the source distribution for its full text.
*/
#include "config.h" // IWYU pragma: keep

#include "pcp/PCPDynamicScreen.h"

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


static bool PCPDynamicScreen_validateScreenName(char* key, const char* path, unsigned int line) {
   char* p = key;
   char* end = strrchr(key, ']');

   if (end) {
      *end = '\0';
   } else {
      fprintf(stderr,
            "%s: no closing brace on screen name at %s line %u\n\"%s\"",
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
            "%s: invalid screen name at %s line %u\n\"%s\"",
            pmGetProgname(), path, line, key);
      return false;
   }
   return true;
}

// Ensure a screen name has not been defined previously
static bool PCPDynamicScreen_uniqueName(char* key, PCPDynamicScreens* screens) {
   return !DynamicScreen_search(screens->table, key, NULL);
}

static PCPDynamicScreen* PCPDynamicScreen_new(PCPDynamicScreens* screens, const char* name) {
   PCPDynamicScreen* screen = xCalloc(1, sizeof(*screen));
   String_safeStrncpy(screen->super.name, name, sizeof(screen->super.name));

   size_t id = screens->count;
   Hashtable_put(screens->table, id, screen);
   screens->count++;

   return screen;
}

static void PCPDynamicScreen_parseFile(PCPDynamicScreens* screens, const char* path) {
   FILE* file = fopen(path, "r");
   if (!file)
      return;

   PCPDynamicScreen* screen = NULL;
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
      if (key[0] == '[') {  /* new section heading - i.e. new screen */
         ok = PCPDynamicScreen_validateScreenName(key + 1, path, lineno);
         if (ok)
            ok = PCPDynamicScreen_uniqueName(key + 1, screens);
         if (ok)
            screen = PCPDynamicScreen_new(screens, key + 1);
      } else if (value && screen && String_eq(key, "caption")) {
         free_and_xStrdup(&screen->super.caption, value);
      } else if (value && screen && String_eq(key, "columns")) {
         free_and_xStrdup(&screen->super.fields, value);
      } else if (value && screen && String_eq(key, "sortKey")) {
         free_and_xStrdup(&screen->super.sortKey, value);
      } else if (value && screen && String_eq(key, "sortDirection")) {
         screen->super.direction = strtoul(value, NULL, 10);
      } else if (value && screen && String_eq(key, "enabled")) {
         if (strcmp(value, "true") || strcmp(value, "True"))
            screen->enabled = 1;
      }
      String_freeArray(config);
      free(value);
      free(key);
   }
   fclose(file);
}

static void PCPDynamicScreen_scanDir(PCPDynamicScreens* screens, char* path) {
   DIR* dir = opendir(path);
   if (!dir)
      return;

   struct dirent* dirent;
   while ((dirent = readdir(dir)) != NULL) {
      if (dirent->d_name[0] == '.')
         continue;

      char* file = String_cat(path, dirent->d_name);
      PCPDynamicScreen_parseFile(screens, file);
      free(file);
   }
   closedir(dir);
}

void PCPDynamicScreens_init(PCPDynamicScreens* screens) {
   const char* share = pmGetConfig("PCP_SHARE_DIR");
   const char* sysconf = pmGetConfig("PCP_SYSCONF_DIR");
   const char* xdgConfigHome = getenv("XDG_CONFIG_HOME");
   const char* override = getenv("PCP_HTOP_DIR");
   const char* home = getenv("HOME");
   char* path;

   screens->table = Hashtable_new(0, true);

   /* developer paths - PCP_HTOP_DIR=./pcp ./pcp-htop */
   if (override) {
      path = String_cat(override, "/screens/");
      PCPDynamicScreen_scanDir(screens, path);
      free(path);
   }

   /* next, search in home directory alongside htoprc */
   if (xdgConfigHome)
      path = String_cat(xdgConfigHome, "/htop/screens/");
   else if (home)
      path = String_cat(home, "/.config/htop/screens/");
   else
      path = NULL;
   if (path) {
      PCPDynamicScreen_scanDir(screens, path);
      free(path);
   }

   /* next, search in the system screens directory */
   path = String_cat(sysconf, "/htop/screens/");
   PCPDynamicScreen_scanDir(screens, path);
   free(path);

   /* next, try the readonly system screens directory */
   path = String_cat(share, "/htop/screens/");
   PCPDynamicScreen_scanDir(screens, path);
   free(path);
}

static void PCPDynamicScreens_free(ATTR_UNUSED ht_key_t key, void* value, ATTR_UNUSED void* data) {
   PCPDynamicScreen* screen = (PCPDynamicScreen*) value;
   free(screen->instances);
   free(screen->super.caption);
   free(screen->super.fields);
   free(screen->super.sortKey);
}

void PCPDynamicScreens_done(Hashtable* table) {
   Hashtable_foreach(table, PCPDynamicScreens_free, NULL);
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

void PCPDynamicScreen_appendScreens(PCPDynamicScreens* screens, Settings* settings) {
   PCPDynamicScreen* ds;
   ScreenSettings* ss;
   char* sortKey;

   for (size_t i = 0; i < screens->count; i++) {
      ds = (PCPDynamicScreen*)Hashtable_get(screens->table, i);
      if (!ds)
         continue;
      if (!ds->enabled)
         continue;

      char* columns = formatFields(ds->super.fields);

      xAsprintf(&sortKey, "Dynamic(%s)", ds->super.sortKey);

      ScreenDefaults screen = {
         .name    = ds->super.caption,
         .columns = columns,
         .sortKey = sortKey,
      };

      ss = Settings_newScreen(settings, &screen);

      ss->generic = true;
      if (ds->super.direction)
         ss->direction = ds->super.direction;

      free(columns);
   }
}
