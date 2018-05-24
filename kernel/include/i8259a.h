/* constant for interrupt */
/* i8259a.h */
#ifndef _I8259A_H
#define _I8259A_H

/* 8259A interrupt controller prots. */
#define INT_M_CTL         0x20        /* I/O prot for interrupt controller <Master> */
#define INT_M_CTLMASK     0x21        /* setting bits in this port disables ints <Master> */
#define INT_S_CTL         0xA0        /* I/O prot for second interrupt controller <Slave> */
#define INT_S_CTLMASK     0xA1        /* setting bits in this port disables ints <Slave>*/


#define BIOS_IRQ0_VEC   0x08    /* base of IRQ0-7 vectors used by BIOS */
#define BIOS_IRQ8_VEC   0x70    /* base of IRQ8-15 vectors used by BIOS */
#define IRQ0_VECTOR     0x50    /* nice vectors to relocate IRQ0-7 to */
#define IRQ8_VECTOR     0x70    /* no need to move IRQ8-15 */

/* interrupt number to hardware vector */
#define VECTOR(irq)     \
        (((irq) < 8 ? IRQ0_VECTOR : IRQ8_VECTOR) + ((irq) & 0x07))



/* exception vector numbers*/
#define DIVIDE_VECTOR      0    /* divide error */
#define DEBUG_VECTOR       1    /* single step (trace) */
#define NMI_VECTOR         2    /* non-maskable interrupt */

#define BREAKPOINT_VECTOR  3    /* software breakpoint */
#define OVERFLOW_VECTOR    4    /* from INTO */

#define BOUNDS_VECTOR        5  /* bounds check failed */
#define INVAL_OP_VECTOR      6  /* invalid opcode */
#define COPROC_NOT_VECTOR    7  /* coprocessor not available */
#define DOUBLE_FAULT_VECTOR  8
#define COPROC_SEG_VECTOR    9  /* coprocessor segment overrun */
#define INVAL_TSS_VECTOR    10  /* invalid TSS */
#define SEG_NOT_VECTOR      11  /* segment not present */
#define STACK_FAULT_VECTOR  12  /* stack exception */
#define PROTECTION_VECTOR   13  /* general protection */

#define PAGE_FAULT_VECTOR   14
#define COPROC_ERR_VECTOR   16  /* coprocessor error */



#endif
