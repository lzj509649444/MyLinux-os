#ifndef _PROTECT_H
#define _PROTECT_H

#define     GDT_SIZE        GDT_ITEMS * DESC_SIZE
#define     IDT_SIZE        IDT_ITEMS * DESC_SIZE
/*selector for GDT*/
#define     GDT_SELECTOR    GDT_INDEX * DESC_SIZE    /**/
#define     IDT_SELECTOR    IDT_INDEX * DESC_SIZE
#define     DS_SELECTOR     DS_INDEX * DESC_SIZE	/* Kernel data selector */
#define     ES_SELECTOR     ES_INDEX * DESC_SIZE	/* Flat 4 Gb  */
#define     SS_SELECTOR     SS_INDEX * DESC_SIZE	/* Monitor stack */
#define     CS_SELECTOR     CS_INDEX * DESC_SIZE	/* Kernel code  */
#define     GS_SELECTOR     GS_INDEX * DESC_SIZE    /* vedio */
#define     TSS_SELECTOR    TSS_INDEX * DESC_SIZE   /* TSS */

/* Privileges. */
#define INTR_PRIVILEGE       0  /* kernel and interrupt handlers */
#define TASK_PRIVILEGE       1  /* kernel tasks */
#define USER_PRIVILEGE       3  /* servers and user processes */

/* access-byte and type-byte bits */
#define PRESENT           0x80  /* set for descriptor present */
#define DPL               0x60  /* descriptor Privilege level mask */
#define DPL_SHIFT         5
#define SEGMENT           0x10  /* set for segment-type descriptor */

/* Access-byte bits. */
#define EXECUTABLE        0x08  /* set for executable segment */
#define CONFORMING        0x04  /* set for conforming segment if executable */
#define EXPAND_DOWN       0x04  /* set for expand-down segment if !executable*/
#define READABLE          0x02  /* set for readable segment if executable */
#define WRITEABLE         0x02  /* set for writeable segment if !executable */


/* type */
#define TSS_TYPE            0x09    /* TSS type : Executable|Access */
#define INT_GATE_TYPE       0x0E  /* gate type */
#define LDT_TYPE            0x02

/* Selector bits. */
#define TI                0x04  /* table indicator */
#define RPL               0x03  /* requester privilege level */



/* Granularity byte. */
#define GRANULAR          0x80  /* set for 4K granularilty */
#define DEFAULT           0x40  /* set for 32-bit defaults (executable seg) */
#define BIG               0x40  /* set for "BIG" (expand-down seg) */
#define AVL               0x10  /* 0 for available */
#define LIMIT_HIGH        0x0F  /* mask for high bits of limit */

#define vir2phys(seg_base, vir)	(u32_t)(((u32_t)seg_base) + (u32_t)(vir))




#endif
