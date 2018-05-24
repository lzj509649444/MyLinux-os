;syscall.asm


INT_VECTROE_SYS     equ 0x90

global  get_ticks

bits 32
[section .text]

get_ticks:
    int     INT_VECTROE_SYS
    ret
