/* main.c */
#include "kernel.h"

PUBLIC void TestA();
PUBLIC void TestB();
extern int get_ticks();
void restart();

extern int position;

PUBLIC int main()
{
/* program i8259a interrupt controller */
    init_8259A();

	position = 0;
	for(int i = 0 ; i < 25; ++i)
	{
		for(int j = 0 ; j < 80; ++j)
		{
			print_str(" ");
		}
	}
	position = 0;
    /* return to the assembly code to start running the current process */
    restart();

	return 0;
}

PUBLIC void TestA()
{
    while(1)
    {
		for(int i = 10 ;i < 10000; ++i);
    }

}

PUBLIC void TestB()
{
    for(;;)
    {
	   for(int i = 0; i < 10000; ++i);
    }

}
