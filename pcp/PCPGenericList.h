#ifndef HEADER_PCPGenericList
#define HEADER_PCPGenericList
/*
htop - PCPGenericList.h
(C) 2022 Sohaib Mohammed
(C) 2022 htop dev team
(C) 2022 Red Hat, Inc.
Released under the GNU GPLv2+, see the COPYING file
in the source distribution for its full text.
*/

#include <stddef.h>

#include "Hashtable.h"

typedef struct PCPGenericLists_ {
   Hashtable* table;
   size_t count;  /* count of dynamic meters discovered by scan */ // SMALINUX:
   size_t offset; /* start offset into the Platform metric array */ // SMALINUX
   size_t cursor; /* identifier allocator for each new metric used */ // SMALINUX
} PCPGenericLists;

void PCPGenericLists_init(PCPGenericLists* genericlists);

void PCPGenericLists_done(Hashtable* table);

#endif
