/* TTY.C */
#include "kernel.h"
#include "tty.h"
#include "keyboard.h"

/* from global.h */
extern int position;

/* from keyboard.c */
extern void scan_keyboard();
extern u32_t read_input();

extern void init_keyboard();

/* funcations declaration */
PRIVATE void init_buf(struct buf_t *buf);
PRIVATE void set_cursor(unsigned int position);
PRIVATE void init_console();
PRIVATE void keyboard_input();
PRIVATE void keyboard_output();
PRIVATE void console_switch(struct console_t *console);
PRIVATE void scroll_screen(struct console_t *console, int direction);

PUBLIC void out_char(struct console_t *console, char ch);
PUBLIC void do_ctrl_alt_up();
PUBLIC void do_ctrl_alt_down();
PUBLIC void do_ctrl_alt_right();
PUBLIC void do_ctrl_alt_left();
PUBLIC void tty_task();

/* data */
PUBLIC struct console_t *next_console;

PUBLIC void tty_task()
{
    init_console();
    init_keyboard();

    console_switch(next_console);

	out_char(next_console,'T');
	out_char(next_console,'T');
	out_char(next_console,'Y');
    for(;;)
    {
		keyboard_input();
		keyboard_output();
		for(int i = 0; i < 10000; ++i);
    }
}

PUBLIC void do_ctrl_alt_up()
{
    int limit = next_console - BEG_CONSOLE_ADDR;
    if(limit == 0 || limit == 1)
        return;
    else if (limit >= 2)
    {
        next_console -=2;
        console_switch(next_console);
    }
}

PUBLIC void do_ctrl_alt_down()
{
    int limit = next_console - BEG_CONSOLE_ADDR;
    if(limit == 2 || limit == 3)
        return;
    else if( limit <= 1)
    {
        next_console +=2;
        console_switch(next_console);
    }
}

PUBLIC void do_ctrl_alt_right()
{
    int limit = next_console - BEG_CONSOLE_ADDR;
    if(limit ==1 || limit == 3)
        return;
    else
    {
        ++next_console;
        console_switch(next_console);
    }
}

PUBLIC void do_ctrl_alt_left()
{
    int limit = next_console - BEG_CONSOLE_ADDR;
    if(limit == 0 || limit == 2)
        return;
    else
    {
        --next_console;
        console_switch(next_console);
    }
}

PRIVATE void keyboard_input()
{
	
	scan_keyboard();
}

PRIVATE void keyboard_output()
{
    u32_t key = read_input();

    /* control command */
    if ( key&0xFE00 ) /* one of shift, ctrl, alt key pressed */
    {
        /* both alt and ctrl key pressed */
        if( key & (FLAG_CTRL_L | FLAG_CTRL_R | FLAG_ALT_L | FLAG_ALT_R) )
        {
            switch( key & (FLAG_EXT | 0xFF))
            {
                case 'W' :
                    do_ctrl_alt_up();
                    break;
                case 'S' :
                    do_ctrl_alt_down();
                    break;
                case 'D' :
                    do_ctrl_alt_right();
                    break;
                case 'A' :
                    do_ctrl_alt_left();
                    break;
                default:
                    break;
            }
        }
    }
    /* printable characters */
    else if( !(key & FLAG_EXT) )
    {
        out_char(next_console, (key & 0xFF));
    }
    /* unprintable characters */
    else
    {}
}

PUBLIC void out_char(struct console_t *console, char ch)
{
    u8_t *p = (u8_t *)(console->cursor_pos) + V_MEM_BASE;
    *p++ = ch;
    *p = DEFAULT_CHAR_COLOR;

    console->cursor_pos += 2;
    set_cursor(console->cursor_pos);
}

PRIVATE void init_buf(struct buf_t *buf)
{
    buf->count = 0;
    buf->p_head = buf->p_tail = buf->in_buf;
}

PRIVATE void set_cursor(unsigned int position)
{
    lock_irq();
    out_byte(CRTC_ADDR_REG,CURSOR_H);
    out_byte(CRTC_DATA_REG,(position>>8)&&0xFF);    /* set high */
    out_byte(CRTC_ADDR_REG,CURSOR_L);
    out_byte(CRTC_DATA_REG,position && 0xFF);   /* set low */
    unlock_irq();
}

PRIVATE void init_console()
{
    struct console_t *p;
    int nr_cons = 0;
    

    for(p = BEG_CONSOLE_ADDR; p <= END_CONSOLE_ADDR && nr_cons < NR_CONSOLE; ++p, ++nr_cons)
    {
        init_buf(&p->buf);
        p->mem_addr = CONSOLE_SIZE * nr_cons;
        p->mem_limt = CONSOLE_SIZE;
        p->current_add = p->mem_addr;
        p->cursor_pos = p->mem_addr;        

		out_char(p, nr_cons + '0');
		out_char(p, '#');
		out_char(p, ':');
    }

    next_console = BEG_CONSOLE_ADDR;

}

PRIVATE void console_switch(struct console_t *console)
{
    set_cursor(console->cursor_pos);
    /* set video start address */
    lock_irq();
    out_byte(CRTC_ADDR_REG, VIDEO_ADDR_H);
    out_byte(CRTC_DATA_REG, (console->current_add>>8) & 0xFF);
    out_byte(CRTC_ADDR_REG, VIDEO_ADDR_L);
    out_byte(CRTC_DATA_REG, (console->current_add) & 0xFF);
    unlock_irq();
}


PRIVATE void scroll_screen(struct console_t *console, int direction)
{

}
