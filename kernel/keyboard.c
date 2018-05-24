/* keyboard.c */
#include "kernel.h"
#include "keyboard.h"
#include "keymap.h"
#include "tty.h"

/* below external funcation is come from clock.c */
extern void put_irq_handler(int irq, void (*handler)());

/* data from tty.c */
extern struct console_t *next_console;

/* funcations declaration */
PRIVATE void keyboard_handler(int irq);
PUBLIC u32_t read_input();
PRIVATE void read_kb_buf();

PUBLIC void init_keyboard();
PUBLIC void scan_keyboard();

PRIVATE struct buf_t kb_buf;


/* data declaration */

PUBLIC void init_keyboard()
{
	kb_buf.p_head = kb_buf.p_tail = kb_buf.in_buf;
	kb_buf.count = 0;
    /* set keyboard interrupt handler */
    put_irq_handler(KEYBOARD_IRQ, keyboard_handler);
    enable_irq(KEYBOARD_IRQ);
}

/* press a key will interrupt two times, key down and key up */
PUBLIC void keyboard_handler(int irq)
{
    u8_t scan_code = in_byte(KEYBOARD_PORT);

    /* read scan code into kb_in buffer */
    /* return if input buffer is full */
    if(kb_buf.count < BUFFER_BYTES)
    {
        *(kb_buf.p_head) = scan_code;
        ++kb_buf.p_head;
        if(kb_buf.p_head == kb_buf.in_buf + BUFFER_BYTES)
        {
            kb_buf.p_head = kb_buf.in_buf;
        }
        ++kb_buf.count;
    }
}

PRIVATE void read_kb_buf()
{
	while(kb_buf.count > 0)
	{
		if(next_console->buf.count < BUFFER_BYTES)
		{
			if(next_console->buf.p_head == next_console->buf.in_buf + BUFFER_BYTES)
				next_console->buf.p_head = next_console->buf.in_buf;
			*next_console->buf.p_head = *(kb_buf.p_tail);
			++next_console->buf.p_head;
			++next_console->buf.count;
		}
        /* disable keyboard interrupt */
        lock_irq();
		--kb_buf.count;
        /* enable keyboard interrupt */
        unlock_irq();

		++kb_buf.p_tail;
	}
	if(kb_buf.p_tail == kb_buf.in_buf + BUFFER_BYTES)
		kb_buf.p_tail = kb_buf.in_buf;
}

/* analyse scan code in buffer */
PUBLIC u32_t read_input()
{

    u8_t normal = 0;
    u8_t e0flg  = 0;

    u8_t shift_l = 0;
    u8_t shift_r = 0;
    u8_t ctrl_l = 0;
    u8_t ctrl_r = 0;
    u8_t alt_l = 0;
    u8_t alt_r = 0;

    u8_t scan_code;
    u32_t key;
    while(!normal)
    {
		/* wait for next character */
		if(next_console->buf.count <= 0)
			scan_keyboard();

        scan_code = *next_console->buf.p_tail;
        ++next_console->buf.p_tail;
        --next_console->buf.count;
        if(next_console->buf.p_tail == next_console->buf.in_buf + BUFFER_BYTES)
        {
            next_console->buf.p_tail = next_console->buf.in_buf;
        }

         if( scan_code == 0xE0)
        {
            e0flg = 1;
            continue;
        }

        /* ignor break code */
        if( scan_code & 0x80)
        {
            if(e0flg)
            {
                if(scan_code == 0x9D)
                    ctrl_r = 0;
                else if(scan_code == 0xB8)
                    alt_r = 0;
                e0flg = 0;
            }
            else
            {
                if(scan_code == 0x9D)
                    ctrl_l = 0;
                else if(scan_code == 0xB8)
                    alt_l = 0;
                else if(scan_code == AA)
                    shift_l = 0;
                else if(scan_code == B6)
                    shift_r = 0;
            }

            continue;
        }

        /* command key pressed */
        if(scan_code == 0x1D)
        {
            if(e0flg)
            {
                ctrl_r = 1;
                e0flg = 0;
            }else ctrl_l = 1;
            continue;
        }else if( scan_code == 0x38)
        {
            if(e0flg)
            {
                alt_r = 1;
                e0flg = 0;
            }else alt_l = 1;
            continue;
        }else if( scan_code == 0x2A)
        {
            shift_l = 1;
            continue;
        }else if( scan_code == 0x38)
        {
            shift_r = 1;
            continue;
        }

        /* normal key pressed */
        int column = 0;
        if( shift_l | shift_r )
            column = 1;
        if(e0flg)
            column = 2;
        key = keymap[ (scan_code & 0x7F) * MAP_COLS + column];
				
        key |= shift_l ? FLAG_SHIFT_L	: 0;
        key |= shift_r ? FLAG_SHIFT_R	: 0;
        key |= ctrl_l ? FLAG_CTRL_L	: 0;
        key |= ctrl_r ? FLAG_CTRL_R	: 0;
        key |= alt_l	? FLAG_ALT_L	: 0;
        key |= alt_r	? FLAG_ALT_R	: 0;

        normal = 1;
        return key;
    }
}


PUBLIC void scan_keyboard()
{
    /* wait for keyboard input */
    for( ; kb_buf.count <= 0 ; );

    read_kb_buf();

}

