/* global.h */
#ifndef _GLOBAL_H
#define _GLOBAL_H

/* external funcations  from main.c */
EXTERN PUBLIC void TestA();
EXTERN PUBLIC void TestB();

/* from tty.c */
EXTERN PUBLIC void tty_task();


#define TTY 0
#define TA  1
#define TB  2

PUBLIC int position;
PUBLIC struct segdesc_s gdt[GDT_ITEMS];		/* used in klib.s and mpx.s */

/* description of each task */
PUBLIC struct task_t
{
    void (*funcation)();
    char *name;
    char *stack;
};

PUBLIC struct t_stack_t
{
    char stack[K_STACK_SPACE];
};

PUBLIC struct t_stack_t t_stack[NR_TASKS + NR_PROCS];

PUBLIC struct task_t task_table[] = {
    {tty_task,"tty_task",(t_stack[TTY].stack)+K_STACK_SPACE},
    {TestA,"TestA",(t_stack[TA].stack)+K_STACK_SPACE},
    {TestB,"TestB",(t_stack[TB].stack)+K_STACK_SPACE}
};


#endif
