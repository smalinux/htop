#ifndef HEADER_DiskUsage
#define HEADER_DiskUsage
/*
htop - DiskUsageMeter.h
(C) 2021 htop dev team
Released under the GNU GPL, see the COPYING file
in the source distribution for its full text.
*/

#include "Meter.h"

typedef struct DiskUsageData_ {
   double total;
   double available;
   double used;
   double usedPercentage;
} DiskUsageData;

static const DiskUsageData invalidDiskUsageData = { -1.0, 0.0, 0.0, 0.0 };

static inline bool isInvalidDiskUsageData(const DiskUsageData *data)
{
   return data->total < 0;
}

extern const MeterClass DiskUsageMeter_class;

#endif
