/* proto.h */

/* kernel.asm */
void restart();

/* syscall.asm */
int get_ticks();

/* klib.asm */
PUBLIC void out_byte(u16_t port, u8_t value);
PUBLIC u8_t in_byte(u16_t port);
PUBLIC void print_str(char *str);
PUBLIC void cprint_str(char *str, int color);
PUBLIC void enable_irq(int irq);
PUBLIC void disable_irq(int irq);
PUBLIC void lock_irq();
PUBLIC void unlock_irq();

/* klibf.c */
PUBLIC void delay(int time);
PUBLIC void print_int(int num);


/* i8259a.c */
PUBLIC void init_8259A();
