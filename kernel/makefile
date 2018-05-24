entry	= 0x10030
entryoff	= 0x0030
ASM	= nasm
DASM	= ndisasm
CC	= gcc
LD	= ld
asmflags	= -f elf
bootflags	= -I boot/include/
kernelflags	= -I include/ -f elf
cflags	= -I include/ -c -g -fno-builtin -Wall -std=c99 -fno-stack-protector
ldflags	= -verbose -Ttext $(entry)
CCEXE	= $(CC) $(cflags) -o $@ $<

i=include/
head=$(i)kernel.h
lib=../klib/
kernel=kernel.bin

OBJS	= kernel.o start.o i8259a.o exception.o protect.o main.o $(lib)kliba.o $(lib)string.o $(lib)klibf.o syscall.o clock.o keyboard.o tty.o

$(lib)klibf.o:	$(lib)klibf.c $(head)
	$(CCEXE)

$(lib)kliba.o: $(lib)kliba.asm
	$(ASM) $(asmflags) -o $@ $<

$(lib)string.o: $(lib)string.asm
	$(ASM) $(asmflags) -o $@ $<

tty.o	: tty.c $(head) $(i)keyboard.h $(i)tty.h
	$(CCEXE)

keyboard.o:	keyboard.c $(head) $(i)keyboard.h $(i)keymap.h $(i)tty.h
	$(CCEXE)

clock.o: clock.c $(head) $(i)clock.h
	$(CCEXE)

main.o:	main.c $(head)
	$(CCEXE)

exception.o: exception.c $(head)
	$(CCEXE)

i8259a.o: i8259a.c $(head) $(i)i8259a.h
	$(CCEXE)

protect.o: protect.c $(head) $(i)protect.h $(i)i8259a.h $(i)string.h $(i)proc.h $(i)global.h
	$(CCEXE)

start.o: start.c $(head) $(i)global.h
	$(CCEXE)

kernel.o: kernel.asm
	$(ASM) $(asmflags) -o $@ $<

syscall.o: syscall.asm
	$(ASM) $(asmflags) -o $@ $<

#############
buildobjs: $(OBJS)

buildkernel: $(OBJS)
	$(LD) $(ldflags) -o $(kernel) $(OBJS)

deleteobjs: 
	rm -f $(OBJS)
