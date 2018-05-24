/* process.c */

#include "kernel.h"
#include "proc.h"


PUBLIC void schedule();

PUBLIC void schedule()
{
    next_proc++;
    if(next_proc >= proc_cpu + NR_TASKS + NR_PROCS)
        next_proc = BEG_PROC_ADDR;
}


