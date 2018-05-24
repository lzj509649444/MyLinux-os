/*clock.h*/
#ifndef CLOCK_H
#define CLOCK_H

/* Hardware interrupts */
#define	NR_IRQ		    16	/* Number of IRQs */
#define NR_CALL         1
#define	CLOCK_IRQ	    0
#define	KEYBOARD_IRQ	1
#define	CASCADE_IRQ 	2	/* cascade enable for 2nd AT controller */
#define	ETHER_IRQ	    3	/* default ethernet interrupt vector */
#define	SECONDARY_IRQ	3	/* RS232 interrupt vector for port 2 */
#define	RS232_IRQ   	4	/* RS232 interrupt vector for port 1 */
#define	XT_WINI_IRQ 	5	/* xt winchester */
#define	FLOPPY_IRQ  	6	/* floppy disk */
#define	PRINTER_IRQ	    7
#define	AT_WINI_IRQ	    14	/* at winchester */


/* 8253/8254 PIT (Programmable Interval Timer) */
#define TIMER0         0x40 /* I/O port for timer channel 0 */
#define TIMER_MODE     0x43 /* I/O port for timer mode control */
#define RATE_GENERATOR 0x34 /* 00-11-010-0 :* Counter0 - LSB then MSB - rate generator - binary */
#define TIMER_FREQ     1193182L/* clock frequency for timer in PC and AT */
#define HZ             100  /* clock freq (software settable on IBM-PC) */



void (*irq_table[NR_IRQ])(int irq);

int (*call_table[NR_CALL])(int irq);



#endif
