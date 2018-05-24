/* start.c */
#include "kernel.h"
extern int position;

extern  void prot_init();

PUBLIC  void cstart()
{
    print_str("----cstart excute now-----\n");
    prot_init();
    print_str("---- cstart finished-----\n");
	for(int i = 0; i < 20000; ++i);
}

