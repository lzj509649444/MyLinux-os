#ifndef _CONFIG_H
#define _CONFIG_H
/* kernel/include/config.h*/

/* How many bytes for the kernel stack. Space allocated in kernel.asm*/
#define K_STACK_SPACE   1024



/* defined for task */
#define NR_TASKS    1   /* number of tasks */
#define NR_PROCS    2

/* number of interrupt */
#define NR_IRQ      16


#define GDT_INDEX            1  /* GDT descriptor */
#define IDT_INDEX            2  /* IDT descriptor */
#define DS_INDEX             3  /* kernel DS */
#define ES_INDEX             4  /* kernel ES (386: flag 4 Gb at startup) */
#define SS_INDEX             5  /* kernel SS (386: monitor SS at startup) */
#define CS_INDEX             6  /* kernel CS */
#define GS_INDEX             7  /* vedio */
#define TSS_INDEX            8  /* kernel TSS */
#define FIRST_LDT_INDEX       15 /* rest of Descriptor are LDT's */

#define CS_LDT_INDEX         0  /* process CS */
#define DS_LDT_INDEX         1  /* process DS=ES=FS=GS=SS */
#define GS_LDT_INDEX         2  /* process SS */



/* Descriptor structure offsets. */
#define DESC_BASE            2  /* to base_low */
#define DESC_BASE_MIDDLE     4  /* to base_middle */
#define DESC_ACCESS          5  /* to access byte */
#define DESC_SIZE            8  /* sizeof (struct segdesc_s) */

#define GDT_ITEMS            (FIRST_LDT_INDEX + NR_TASKS +NR_PROCS) /* how many items in GDT table */
#define IDT_ITEMS            256 /* how many items in IDT table */
#define LDT_ITEMS            3

#endif
