;kernel.asm
%include "include/kasm.inc"

;external funcations
extern  cstart
extern  main
extern  exception
extern	spurious_irq
extern  delay
extern  irq_table
extern  call_table

;external variables
extern  gdt
extern  position
extern  next_proc
extern  tss


[section .text]

;exported funcations
global  _start  ;export _start for ld linker
global  restart ;start a process


global	divide_error
global	single_step_exception
global	nmi
global	breakpoint_exception
global	overflow
global	bounds_check
global	inval_opcode
global	copr_not_available
global	double_fault
global	copr_seg_overrun
global	inval_tss
global	segment_not_present
global	stack_exception
global	general_protection
global	page_fault
global	copr_error
global  hwint00
global  hwint00
global  hwint01
global  hwint02
global  hwint03
global  hwint04
global  hwint05
global  hwint06
global  hwint07
global  hwint08
global  hwint09
global  hwint10
global  hwint11
global  hwint12
global  hwint13
global  hwint14
global  hwint15
global	sys_call

global  int_reenter

_start:

    mov     dword [ssseg],eax   ;save ss segment

    mov     esp,k_stacktop
    mov     dword [position],0

    sgdt    [gdt + GDT_SELECTOR]
    call    cstart
    lgdt    [gdt + GDT_SELECTOR]

    lidt    [gdt + IDT_SELECTOR]
    jmp     CS_SELECTOR:csinit
csinit:
    
    ;load tss 
    xor     eax,eax
    mov     ax,TSS_SELECTOR
    ltr     ax

    jmp     main

restart:
    mov     esp,[next_proc]         ; esp point to current process table
    lldt    [esp + P_LDT_SEL]
    lea     eax,[esp + P_STACKTOP]
    mov     dword [ tss + TSS3_S_SP0],eax
int_reenter_f:
    dec     byte [int_reenter]
    pop     gs
    pop     fs 
    pop     es 
    pop     ds 
    popad
    add     esp,4   ;skip return address
    iretd
    
    
savecpuinfo:
    pushad
    ;current process's ss segment was already saved in itself satck when clock Interrupt happened.
    push    ds
    push    es 
    push    fs
    push    gs
    
    ;now cpu get the ss and esp from TSS 
    ;ss was initialized in protect.c
    mov     dx,ss   ; get kernel ss from TSS  
    mov     ds,dx   ; set kernel data segment
    mov     es,dx   ; kernel dose not use fs,gs 

    mov     dx,GS_SELECTOR
    mov     gs,dx

    mov     ebx,esp ;prepare to return
    
    inc     byte [int_reenter]      ;will affect general register
    cmp     byte [int_reenter],1
    jg     .reenter
    mov     esp,k_stacktop
    push    restart                 ;for hwint_master return
    xor     ebp,ebp
    jmp     [ebx + RETADR - P_STACKBASE]    ; savecpuinfo funcation return

.reenter:
    push    int_reenter_f
    jmp     [ebx + RETADR - P_STACKBASE]    ; savecpuinfo funcation return

;system call (software Interrupt)do not excute process schedule
;so when the call was finished,it will return to current process
%macro	s_call	1
    ; save current cpu infomation and switch to kernel space
    call    savecpuinfo
    sti
    push    ebx
	push	%1
    call    [call_table + 0 * 4]
	pop		ecx
    pop     ebx
    mov     [ebx + EAXREG - P_STACKTOP],eax
    cli
    ret
%endmacro

sys_call:
	s_call	0x90
    
    
; ---------------------------------
; hardware Interrupt will excute process schedule
; and a ready process will runing when Interrupt return
%macro  hwint_master    1
    call    savecpuinfo
    ;disable the irq
    in      al,INT_M_CTLMASK
    or      al,(1<<%1)
    out     INT_M_CTLMASK,al 
    ;reenable maser 8259
    mov     al,EOI
    out     INT_M_CTL,al 
    sti
    ;Interrupt handler
    push    %1
    call    [irq_table + 4 * %1]
    pop     ecx
    cli
    ;enable the irq
    in      al,INT_M_CTLMASK
    and     al,~(1 << %1)
    out     INT_M_CTLMASK,al 
    ret
%endmacro
; ---------------------------------

ALIGN	16
hwint00:		; Interrupt routine for irq 0 (the clock).
    hwint_master    0

ALIGN	16
hwint01:		; Interrupt routine for irq 1 (keyboard)
	hwint_master	1

ALIGN	16
hwint02:		; Interrupt routine for irq 2 (cascade!)
	hwint_master	2

ALIGN	16
hwint03:		; Interrupt routine for irq 3 (second serial)
	hwint_master	3

ALIGN	16
hwint04:		; Interrupt routine for irq 4 (first serial)
	hwint_master	4

ALIGN	16
hwint05:		; Interrupt routine for irq 5 (XT winchester)
	hwint_master	5

ALIGN	16
hwint06:		; Interrupt routine for irq 6 (floppy)
	hwint_master	6

ALIGN	16
hwint07:		; Interrupt routine for irq 7 (printer)
	hwint_master	7


; ---------------------------------
%macro  hwint_slave     1
        push    %1
        call    spurious_irq
        add     esp, 4
        hlt
%endmacro
; ---------------------------------

ALIGN	16
hwint08:		; Interrupt routine for irq 8 (realtime clock).
	hwint_slave	8

ALIGN	16
hwint09:		; Interrupt routine for irq 9 (irq 2 redirected)
	hwint_slave	9

ALIGN	16
hwint10:		; Interrupt routine for irq 10
	hwint_slave	10

ALIGN	16
hwint11:		; Interrupt routine for irq 11
	hwint_slave	11

ALIGN	16
hwint12:		; Interrupt routine for irq 12
	hwint_slave	12

ALIGN	16
hwint13:		; Interrupt routine for irq 13 (FPU exception)
	hwint_slave	13

ALIGN	16
hwint14:		; Interrupt routine for irq 14 (AT winchester)
	hwint_slave	14

ALIGN	16
hwint15:		; Interrupt routine for irq 15
	hwint_slave	15





;*===========================================================================*
;*				exception handlers			     *
;*===========================================================================*
divide_error:
	push	DIVIDE_VECTOR
	jmp     exceptionentry

single_step_exception:
	push	DEBUG_VECTOR
	jmp     exceptionentry

nmi:
	push	NMI_VECTOR
	jmp	exceptionentry

breakpoint_exception:
	push	BREAKPOINT_VECTOR
	jmp     exceptionentry

overflow:
	push	OVERFLOW_VECTOR
	jmp     exceptionentry

bounds_check:
	push	BOUNDS_VECTOR
	jmp     exceptionentry

inval_opcode:
	push	INVAL_OP_VECTOR
	jmp     exceptionentry

copr_not_available:
	push	COPROC_NOT_VECTOR
	jmp     exceptionentry

double_fault:
	push	DOUBLE_FAULT_VECTOR
	jmp     errexceptionentry

copr_seg_overrun:
	push	COPROC_SEG_VECTOR
	jmp	exceptionentry

inval_tss:
	push	INVAL_TSS_VECTOR
	jmp     errexceptionentry

segment_not_present:
	push	SEG_NOT_VECTOR
	jmp     errexceptionentry

stack_exception:
	push	STACK_FAULT_VECTOR
	jmp     errexceptionentry

general_protection:
	push	PROTECTION_VECTOR
	jmp     errexceptionentry

page_fault:
	push	PAGE_FAULT_VECTOR
	jmp     errexceptionentry

copr_error:
	push	COPROC_ERR_VECTOR
	jmp     exceptionentry


errexceptionentry:
exceptionentry:
    call    exception
    add     esp,4*2
    hlt


[section .data]
ssseg:
    dd  0
ring0msg:
    db  "  ring0...",0xA,0xD,0
int_reenter:
    db  0

[section .bss]
k_stackbottom:
k_stack     resb    K_STACK_SPACE;   2k space
k_stacktop:
