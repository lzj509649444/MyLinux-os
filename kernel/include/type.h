/* type.h */

typedef unsigned char   u8_t;       /* 8 bit type */
typedef unsigned short  u16_t;      /* 16 bit type */
typedef unsigned long   u32_t;      /* 32 bit type */
typedef unsigned long   reg_t;      /* machine register */


struct segdesc_s {              /* segment descriptor for protected mode */
    u16_t limit_low;
    u16_t base_low;
    u8_t base_middle;
    u8_t access;                  /* |P|DL|1|X|E|R|A| */
    u8_t granularity;             /* |G|X|0|A|LIMT| */
    u8_t base_high;
};

struct stackframe_s {           /* proc_ptr points here */
  reg_t gs;                     /* last item pushed by save */
  reg_t fs;                     /*  ^ */
  reg_t es;                     /*  | */
  reg_t ds;                     /*  | */
  reg_t edi;			/* di through cx are not accessed in C */
  reg_t esi;			/* order is to match pusha/popa */
  reg_t ebp;			/* bp */
  reg_t k_stack;			/* hole for another copy of sp */
  reg_t ebx;                     /*  | */
  reg_t edx;                     /*  | */
  reg_t ecx;                     /*  | */
  reg_t eax;                     /*  | */
  reg_t retadr;			/* return address for assembly code savecpuinfo() */
  reg_t eip;			/*  ^  last item pushed by interrupt */
  reg_t cs;                     /*  | */
  reg_t eflags;                    /*  pushed by CPU during interrupt */
  reg_t esp;                     /*  | */
  reg_t ss;                     /* these are pushed by CPU during interrupt */
};


