/* clock.c */
#include "kernel.h"
#include "clock.h"
#include "proc.h"

extern unsigned char int_reenter;
extern void schedule();
/* interrupt */
PUBLIC void spurious_irq(int irq);
PUBLIC void init_clock();
PUBLIC void put_irq_handler(int irq, void (*handler)());
PUBLIC void clock_handler(int irq);

/* system call */
PUBLIC  int s_getticks(int irq);


/* global data member */
static int ticks = 0;

PUBLIC int s_getticks(int irq)
{
    return ticks;
}

PUBLIC void clock_handler(int irq)
{
    ++ticks;

    if(int_reenter > 1)
    {
        print_str("!");
        return;
    }
    
    schedule();
}
 

PUBLIC void spurious_irq(int irq)
{
    print_str("spurious_irq: ");
    print_str((char *)irq);
    print_str("\n");
}

PUBLIC  void init_clock()
{
    /* Initialize 8253 PIT clock timer */
    out_byte(TIMER_MODE, RATE_GENERATOR);
    out_byte(TIMER0, (u8_t)(TIMER_FREQ/HZ) );
    out_byte(TIMER0, (u8_t)((TIMER_FREQ/HZ) >> 8));
    
    /* Initialize the table of interrupt handlers. */
    for(int i = 0; i < NR_IRQ; ++i)
    {
        irq_table[i] = spurious_irq;
    }

    /* init system call  */
    call_table[0] = s_getticks;

    /* set the interrupt handler */
    put_irq_handler(CLOCK_IRQ,clock_handler);

    /* ready for clock interrupts */
    enable_irq(CLOCK_IRQ);
}

PUBLIC void put_irq_handler(int irq, void (*handler)())
{
    disable_irq(irq);
    irq_table[irq] = handler;
}
