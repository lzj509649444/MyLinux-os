/* exception.c */
#include "kernel.h"

/* exception */
struct ex_s{
    char *msg;
};

PUBLIC void exception(unsigned vec_nr)
{
    static struct ex_s ex_table[] = {
        {"#DE Divide Error"},
		{"#DB RESERVED"},
		{"¡ª  NMI Interrupt"},
		{"#BP Breakpoint"},
		{"#OF Overflow"},
		{"#BR BOUND Range Exceeded"},
		{"#UD Invalid Opcode (Undefined Opcode)"},
		{"#NM Device Not Available (No Math Coprocessor)"},
		{"#DF Double Fault"},
		{"    Coprocessor Segment Overrun (reserved)"},
		{"#TS Invalid TSS"},
		{"#NP Segment Not Present"},
		{"#SS Stack-Segment Fault"},
		{"#GP General Protection"},
		{"#PF Page Fault"},
		{"¡ª  (Intel reserved. Do not use.)"},
		{"#MF x87 FPU Floating-Point Error (Math Fault)"},
		{"#AC Alignment Check"},
		{"#MC Machine Check"},
		{"#XF SIMD Floating-Point Exception"}
    };
    print_str(ex_table[vec_nr].msg);
}
