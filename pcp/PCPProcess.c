/*
htop - PCPProcess.c
(C) 2014 Hisham H. Muhammad
(C) 2020 htop dev team
(C) 2020-2021 Red Hat, Inc.  All Rights Reserved.
Released under the GNU GPLv2, see the COPYING file
in the source distribution for its full text.
*/

#include "pcp/PCPProcess.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syscall.h>
#include <unistd.h>

#include "CRT.h"
#include "Process.h"
#include "ProvideCurses.h"
#include "XUtils.h"

const ProcessFieldData Process_fields[] = {
   [0] = { .name = "", .title = NULL, .description = NULL, .flags = 0, },
   [1] = { .name = "PID", .title = "PID", .description = "Process/thread ID", .flags = 0, .pidColumn = true, },
   [2] = { .name = "Command", .title = "Command ", .description = "Command line", .flags = 0, },
   [3] = { .name = "STATE", .title = "S ", .description = "Process state (S sleeping, R running, D disk, Z zombie, T traced, W paging, I idle)", .flags = 0, },
   [4] = { .name = "PPID", .title = "PPID", .description = "Parent process ID", .flags = 0, },
   [5] = { .name = "PGRP", .title = "PGRP", .description = "Process group ID", .flags = 0, },
   [6] = { .name = "SESSION", .title = "SID", .description = "Process's session ID", .flags = 0, },
   [7] = { .name = "TTY", .title = "TTY      ", .description = "Controlling terminal", .flags = 0, },
   [8] = { .name = "TPGID", .title = "TPGID", .description = "Process ID of the fg process group of the controlling terminal", .flags = 0, },
   [10] = { .name = "MINFLT", .title = "     MINFLT ", .description = "Number of minor faults which have not required loading a memory page from disk", .flags = 0, .defaultSortDesc = true, },
   [11] = { .name = "CMINFLT", .title = "    CMINFLT ", .description = "Children processes' minor faults", .flags = 0, .defaultSortDesc = true, },
   [12] = { .name = "MAJFLT", .title = "     MAJFLT ", .description = "Number of major faults which have required loading a memory page from disk", .flags = 0, .defaultSortDesc = true, },
   [13] = { .name = "CMAJFLT", .title = "    CMAJFLT ", .description = "Children processes' major faults", .flags = 0, .defaultSortDesc = true, },
   [14] = { .name = "UTIME", .title = " UTIME+  ", .description = "User CPU time - time the process spent executing in user mode", .flags = 0, .defaultSortDesc = true, },
   [15] = { .name = "STIME", .title = " STIME+  ", .description = "System CPU time - time the kernel spent running system calls for this process", .flags = 0, .defaultSortDesc = true, },
   [16] = { .name = "CUTIME", .title = " CUTIME+ ", .description = "Children processes' user CPU time", .flags = 0, .defaultSortDesc = true, },
   [17] = { .name = "CSTIME", .title = " CSTIME+ ", .description = "Children processes' system CPU time", .flags = 0, .defaultSortDesc = true, },
   [18] = { .name = "PRIORITY", .title = "PRI ", .description = "Kernel's internal priority for the process", .flags = 0, },
   [19] = { .name = "NICE", .title = " NI ", .description = "Nice value (the higher the value, the more it lets other processes take priority)", .flags = 0, },
   [21] = { .name = "STARTTIME", .title = "START ", .description = "Time the process was started", .flags = 0, },
   [54] = { .name = "ELAPSED", .title = "ELAPSED  ", .description = "Time since the process was started", .flags = 0, },
   [38] = { .name = "PROCESSOR", .title = "CPU ", .description = "If of the CPU the process last executed on", .flags = 0, },
   [39] = { .name = "M_VIRT", .title = " VIRT ", .description = "Total program size in virtual memory", .flags = 0, .defaultSortDesc = true, },
   [40] = { .name = "M_RESIDENT", .title = "  RES ", .description = "Resident set size, size of the text and data sections, plus stack usage", .flags = 0, .defaultSortDesc = true, },
   [41] = { .name = "M_SHARE", .title = "  SHR ", .description = "Size of the process's shared pages", .flags = 0, .defaultSortDesc = true, },
   [42] = { .name = "M_TRS", .title = " CODE ", .description = "Size of the text segment of the process", .flags = 0, .defaultSortDesc = true, },
   [43] = { .name = "M_DRS", .title = " DATA ", .description = "Size of the data segment plus stack usage of the process", .flags = 0, .defaultSortDesc = true, },
   [44] = { .name = "M_LRS", .title = "  LIB ", .description = "The library size of the process (unused since Linux 2.6; always 0)", .flags = 0, .defaultSortDesc = true, },
   [45] = { .name = "M_DT", .title = " DIRTY ", .description = "Size of the dirty pages of the process (unused since Linux 2.6; always 0)", .flags = 0, .defaultSortDesc = true, },
   [46] = { .name = "ST_UID", .title = "  UID ", .description = "User ID of the process owner", .flags = 0, },
   [47] = { .name = "PERCENT_CPU", .title = "CPU% ", .description = "Percentage of the CPU time the process used in the last sampling", .flags = 0, .defaultSortDesc = true, },
   [53] = { .name = "PERCENT_NORM_CPU", .title = "NCPU%", .description = "Normalized percentage of the CPU time the process used in the last sampling (normalized by cpu count)", .flags = 0, .defaultSortDesc = true, },
   [48] = { .name = "PERCENT_MEM", .title = "MEM% ", .description = "Percentage of the memory the process is using, based on resident memory size", .flags = 0, .defaultSortDesc = true, },
   [49] = { .name = "USER", .title = "USER      ", .description = "Username of the process owner (or user ID if name cannot be determined)", .flags = 0, },
   [50] = { .name = "TIME", .title = "  TIME+  ", .description = "Total time the process has spent in user and system time", .flags = 0, .defaultSortDesc = true, },
   [51] = { .name = "NLWP", .title = "NLWP ", .description = "Number of threads in the process", .flags = 0, .defaultSortDesc = true, },
   [52] = { .name = "TGID", .title = "TGID", .description = "Thread group ID (i.e. process ID)", .flags = 0, },
   [103] = { .name = "RCHAR", .title = "RCHAR ", .description = "Number of bytes the process has read", .flags = PROCESS_FLAG_IO, .defaultSortDesc = true, },
   [104] = { .name = "WCHAR", .title = "WCHAR ", .description = "Number of bytes the process has written", .flags = PROCESS_FLAG_IO, .defaultSortDesc = true, },
   [105] = { .name = "SYSCR", .title = "  READ_SYSC ", .description = "Number of read(2) syscalls for the process", .flags = PROCESS_FLAG_IO, .defaultSortDesc = true, },
   [106] = { .name = "SYSCW", .title = " WRITE_SYSC ", .description = "Number of write(2) syscalls for the process", .flags = PROCESS_FLAG_IO, .defaultSortDesc = true, },
   [107] = { .name = "RBYTES", .title = " IO_R ", .description = "Bytes of read(2) I/O for the process", .flags = PROCESS_FLAG_IO, .defaultSortDesc = true, },
   [108] = { .name = "WBYTES", .title = " IO_W ", .description = "Bytes of write(2) I/O for the process", .flags = PROCESS_FLAG_IO, .defaultSortDesc = true, },
   [109] = { .name = "CNCLWB", .title = " IO_C ", .description = "Bytes of cancelled write(2) I/O", .flags = PROCESS_FLAG_IO, .defaultSortDesc = true, },
   [110] = { .name = "IO_READ_RATE", .title = " DISK READ ", .description = "The I/O rate of read(2) in bytes per second for the process", .flags = PROCESS_FLAG_IO, .defaultSortDesc = true, },
   [111] = { .name = "IO_WRITE_RATE", .title = " DISK WRITE ", .description = "The I/O rate of write(2) in bytes per second for the process", .flags = PROCESS_FLAG_IO, .defaultSortDesc = true, },
   [112] = { .name = "IO_RATE", .title = "   DISK R/W ", .description = "Total I/O rate in bytes per second", .flags = PROCESS_FLAG_IO, .defaultSortDesc = true, },
   [113] = { .name = "CGROUP", .title = "    CGROUP ", .description = "Which cgroup the process is in", .flags = PROCESS_FLAG_LINUX_CGROUP, },
   [114] = { .name = "OOM", .title = " OOM ", .description = "OOM (Out-of-Memory) killer score", .flags = PROCESS_FLAG_LINUX_OOM, .defaultSortDesc = true, },
   [116] = { .name = "PERCENT_CPU_DELAY", .title = "CPUD% ", .description = "CPU delay %", .flags = 0, .defaultSortDesc = true, },
   [117] = { .name = "PERCENT_IO_DELAY", .title = "IOD% ", .description = "Block I/O delay %", .flags = 0, .defaultSortDesc = true, },
   [118] = { .name = "PERCENT_SWAP_DELAY", .title = "SWAPD% ", .description = "Swapin delay %", .flags = 0, .defaultSortDesc = true, },
   [119] = { .name = "M_PSS", .title = "  PSS ", .description = "proportional set size, same as M_RESIDENT but each page is divided by the number of processes sharing it.", .flags = PROCESS_FLAG_LINUX_SMAPS, .defaultSortDesc = true, },
   [120] = { .name = "M_SWAP", .title = " SWAP ", .description = "Size of the process's swapped pages", .flags = PROCESS_FLAG_LINUX_SMAPS, .defaultSortDesc = true, },
   [121] = { .name = "M_PSSWP", .title = " PSSWP ", .description = "shows proportional swap share of this mapping, Unlike \"Swap\", this does not take into account swapped out page of underlying shmem objects.", .flags = PROCESS_FLAG_LINUX_SMAPS, .defaultSortDesc = true, },
   [122] = { .name = "CTXT", .title = " CTXT ", .description = "Context switches (incremental sum of voluntary_ctxt_switches and nonvoluntary_ctxt_switches)", .flags = PROCESS_FLAG_LINUX_CTXT, .defaultSortDesc = true, },
   [123] = { .name = "SECATTR", .title = " Security Attribute ", .description = "Security attribute of the process (e.g. SELinux or AppArmor)", .flags = PROCESS_FLAG_LINUX_SECATTR, },
   [124] = { .name = "COMM", .title = "COMM            ", .description = "comm string of the process", .flags = 0, },
   [125] = { .name = "EXE", .title = "EXE             ", .description = "Basename of exe of the process", .flags = 0, },
   [126] = { .name = "CWD", .title = "CWD                       ", .description = "The current working directory of the process", .flags = PROCESS_FLAG_CWD, },
};

Process* PCPProcess_new(const Settings* settings) {
   PCPProcess* this = xCalloc(1, sizeof(PCPProcess));
   Object_setClass(this, Class(PCPProcess));
   Process_init(&this->super, settings);
   return &this->super;
}

void Process_delete(Object* cast) {
   PCPProcess* this = (PCPProcess*) cast;
   Process_done((Process*)cast);
   free(this->cgroup);
   free(this->secattr);
   free(this);
}

static void PCPProcess_printDelay(float delay_percent, char* buffer, int n) {
   if (isnan(delay_percent)) {
      xSnprintf(buffer, n, " N/A  ");
   } else {
      xSnprintf(buffer, n, "%4.1f  ", delay_percent);
   }
}

static void PCPProcess_writeField(const Process* this, RichString* str, ProcessField field) {
   const PCPProcess* pp = (const PCPProcess*) this;
   bool coloring = this->settings->highlightMegabytes;
   char buffer[256]; buffer[255] = '\0';
   int attr = CRT_colors[DEFAULT_COLOR];
   int n = sizeof(buffer) - 1;
   switch ((int)field) {
   case 11: Process_printCount(str, pp->cminflt, coloring); return;
   case 13: Process_printCount(str, pp->cmajflt, coloring); return;
   case 43: Process_printBytes(str, pp->m_drs, coloring); return;
   case 45: Process_printBytes(str, pp->m_dt, coloring); return;
   case 44: Process_printBytes(str, pp->m_lrs, coloring); return;
   case 42: Process_printBytes(str, pp->m_trs, coloring); return;
   case 41: Process_printBytes(str, pp->m_share, coloring); return;
   case 119: Process_printKBytes(str, pp->m_pss, coloring); return;
   case 120: Process_printKBytes(str, pp->m_swap, coloring); return;
   case 121: Process_printKBytes(str, pp->m_psswp, coloring); return;
   case 14: Process_printTime(str, pp->utime, coloring); return;
   case 15: Process_printTime(str, pp->stime, coloring); return;
   case 16: Process_printTime(str, pp->cutime, coloring); return;
   case 17: Process_printTime(str, pp->cstime, coloring); return;
   case 103:  Process_printBytes(str, pp->io_rchar, coloring); return;
   case 104:  Process_printBytes(str, pp->io_wchar, coloring); return;
   case 105:  Process_printCount(str, pp->io_syscr, coloring); return;
   case 106:  Process_printCount(str, pp->io_syscw, coloring); return;
   case 107: Process_printBytes(str, pp->io_read_bytes, coloring); return;
   case 108: Process_printBytes(str, pp->io_write_bytes, coloring); return;
   case 109: Process_printBytes(str, pp->io_cancelled_write_bytes, coloring); return;
   case 110:  Process_printRate(str, pp->io_rate_read_bps, coloring); return;
   case 111: Process_printRate(str, pp->io_rate_write_bps, coloring); return;
   case 112: {
      double totalRate = NAN;
      if (!isnan(pp->io_rate_read_bps) && !isnan(pp->io_rate_write_bps))
         totalRate = pp->io_rate_read_bps + pp->io_rate_write_bps;
      else if (!isnan(pp->io_rate_read_bps))
         totalRate = pp->io_rate_read_bps;
      else if (!isnan(pp->io_rate_write_bps))
         totalRate = pp->io_rate_write_bps;
      else
         totalRate = NAN;
      Process_printRate(str, totalRate, coloring); return;
   }
   case 113: xSnprintf(buffer, n, "%-10s ", pp->cgroup ? pp->cgroup : ""); break;
   case 114: xSnprintf(buffer, n, "%4u ", pp->oom); break;
   case 116:
      PCPProcess_printDelay(pp->cpu_delay_percent, buffer, n);
      break;
   case 117:
      PCPProcess_printDelay(pp->blkio_delay_percent, buffer, n);
      break;
   case 118:
      PCPProcess_printDelay(pp->swapin_delay_percent, buffer, n);
      break;
   case 122:
      if (pp->ctxt_diff > 1000) {
         attr |= A_BOLD;
      }
      xSnprintf(buffer, n, "%5lu ", pp->ctxt_diff);
      break;
   case 123: snprintf(buffer, n, "%-30s   ", pp->secattr ? pp->secattr : "?"); break;
   default:
      Process_writeField(this, str, field);
      return;
   }
   RichString_appendWide(str, attr, buffer);
}

static double adjustNaN(double num) {
   if (isnan(num))
      return -0.0005;

   return num;
}

static int PCPProcess_compareByKey(const Process* v1, const Process* v2, ProcessField key) {
   const PCPProcess* p1 = (const PCPProcess*)v1;
   const PCPProcess* p2 = (const PCPProcess*)v2;

   switch (key) {
   case 43:
      return SPACESHIP_NUMBER(p1->m_drs, p2->m_drs);
   case 45:
      return SPACESHIP_NUMBER(p1->m_dt, p2->m_dt);
   case 44:
      return SPACESHIP_NUMBER(p1->m_lrs, p2->m_lrs);
   case 42:
      return SPACESHIP_NUMBER(p1->m_trs, p2->m_trs);
   case 41:
      return SPACESHIP_NUMBER(p1->m_share, p2->m_share);
   case 119:
      return SPACESHIP_NUMBER(p1->m_pss, p2->m_pss);
   case 120:
      return SPACESHIP_NUMBER(p1->m_swap, p2->m_swap);
   case 121:
      return SPACESHIP_NUMBER(p1->m_psswp, p2->m_psswp);
   case 14:
      return SPACESHIP_NUMBER(p1->utime, p2->utime);
   case 16:
      return SPACESHIP_NUMBER(p1->cutime, p2->cutime);
   case 15:
      return SPACESHIP_NUMBER(p1->stime, p2->stime);
   case 17:
      return SPACESHIP_NUMBER(p1->cstime, p2->cstime);
   case 103:
      return SPACESHIP_NUMBER(p1->io_rchar, p2->io_rchar);
   case 104:
      return SPACESHIP_NUMBER(p1->io_wchar, p2->io_wchar);
   case 105:
      return SPACESHIP_NUMBER(p1->io_syscr, p2->io_syscr);
   case 106:
      return SPACESHIP_NUMBER(p1->io_syscw, p2->io_syscw);
   case 107:
      return SPACESHIP_NUMBER(p1->io_read_bytes, p2->io_read_bytes);
   case 108:
      return SPACESHIP_NUMBER(p1->io_write_bytes, p2->io_write_bytes);
   case 109:
      return SPACESHIP_NUMBER(p1->io_cancelled_write_bytes, p2->io_cancelled_write_bytes);
   case 110:
      return SPACESHIP_NUMBER(adjustNaN(p1->io_rate_read_bps), adjustNaN(p2->io_rate_read_bps));
   case 111:
      return SPACESHIP_NUMBER(adjustNaN(p1->io_rate_write_bps), adjustNaN(p2->io_rate_write_bps));
   case 112:
      return SPACESHIP_NUMBER(adjustNaN(p1->io_rate_read_bps) + adjustNaN(p1->io_rate_write_bps), adjustNaN(p2->io_rate_read_bps) + adjustNaN(p2->io_rate_write_bps));
   case 113:
      return SPACESHIP_NULLSTR(p1->cgroup, p2->cgroup);
   case 114:
      return SPACESHIP_NUMBER(p1->oom, p1->oom);
   case 116:
      return SPACESHIP_NUMBER(p1->cpu_delay_percent, p1->cpu_delay_percent);
   case 117:
      return SPACESHIP_NUMBER(p1->blkio_delay_percent, p1->blkio_delay_percent);
   case 118:
      return SPACESHIP_NUMBER(p1->swapin_delay_percent, p1->swapin_delay_percent);
   case 122:
      return SPACESHIP_NUMBER(p1->ctxt_diff, p1->ctxt_diff);
   case 123:
      return SPACESHIP_NULLSTR(p1->secattr, p2->secattr);
   default:
      return Process_compareByKey_Base(v1, v2, key);
   }
}

const ProcessClass PCPProcess_class = {
   .super = {
      .extends = Class(Process),
      .display = Process_display,
      .delete = Process_delete,
      .compare = Process_compare
   },
   .writeField = PCPProcess_writeField,
   .compareByKey = PCPProcess_compareByKey
};
