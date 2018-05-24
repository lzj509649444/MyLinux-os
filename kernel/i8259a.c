/* i8259a.c */
#include "kernel.h"
#include "i8259a.h"
extern void init_clock();
extern void init_keyboard();
extern void init_keyboard();

PUBLIC void init_8259A()
{
    out_byte(INT_M_CTL, 0x11);  /* Master 8259A, ICW1. */
    out_byte(INT_S_CTL, 0x11);  /* Slave 8259A, ICW1. */
    out_byte(INT_M_CTLMASK, IRQ0_VECTOR);   /* Master ICW2. IRQ0_VECTOR is base of IRQ0-7 */
    out_byte(INT_S_CTLMASK, IRQ8_VECTOR);   /* Slave ICW2. IRQ8_VECTOR is base of IRQ8-15 */
	out_byte(INT_M_CTLMASK,	0x4);			/* Master 8259, ICW3. IR2 correspond to Slave */
	out_byte(INT_S_CTLMASK,	0x2);			/* Slave  8259, ICW3. IR2 correspond to Master */
	out_byte(INT_M_CTLMASK,	0x1);			/* Master 8259, ICW4. */
	out_byte(INT_S_CTLMASK,	0x1);			/* Slave  8259, ICW4. */

	out_byte(INT_M_CTLMASK,	0xFF);	/* Master 8259, OCW1. */
	out_byte(INT_S_CTLMASK,	0xFF);	/* Slave  8259, OCW1. */

    /* Initialize clock interrupt */
    init_clock();
    
}

