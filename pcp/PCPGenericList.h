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

#include "GenericList.h"
#include "Hashtable.h"

typedef struct PCPGenericList_ {
   GenericList super;
   // SMA; keymetric;   PCPMetric_iterate
} PCPGenericList;

void GenericList_goThroughEntries(GenericList * super,bool pauseUpdate);

GenericList* GenericList_addPlatformList(GenericList *super);

void GenericList_removePlatformList(GenericList *super);

#endif
