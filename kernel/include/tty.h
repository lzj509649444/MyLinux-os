/* tty.h */
#ifndef TTY_H
#define TTY_H

#define BUFFER_BYTES    256
#define NR_CONSOLE      4

/* VGA */
#define	CRTC_ADDR_REG	0x3D4	/* CRT Controller Registers - Addr Register */
#define	CRTC_DATA_REG	0x3D5	/* CRT Controller Registers - Data Register */
#define	VIDEO_ADDR_H	0x0C	/* reg index of video mem start addr (MSB) */
#define	VIDEO_ADDR_L	0x0D	/* reg index of video mem start addr (LSB) */
#define	CURSOR_H	0x0E	/* reg index of cursor position (MSB) */
#define	CURSOR_L	0x0F	/* reg index of cursor position (LSB) */
#define	V_MEM_BASE	0xB8000	/* base of color video memory */
#define	V_MEM_SIZE	0x8000	/* 32K: B8000H -> BFFFFH */
#define CONSOLE_SIZE    (V_MEM_SIZE / NR_CONSOLE)

#define DEFAULT_CHAR_COLOR	0x07    /* black background, while foreground */




struct buf_t
{
    u32_t in_buf[BUFFER_BYTES];
    u32_t *p_head;
    u32_t *p_tail;
    int   count;
};

struct console_t
{
    unsigned int mem_addr;
    unsigned int mem_limt;
    unsigned int current_add;
    unsigned int cursor_pos;
    struct buf_t buf;
};

struct console_t consoles[NR_CONSOLE];

#define BEG_CONSOLE_ADDR    (&consoles[0])
#define END_CONSOLE_ADDR    (&consoles[NR_CONSOLE-1])

#endif
