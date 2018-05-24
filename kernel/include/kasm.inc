;
GDT_SELECTOR    equ 1*8
IDT_SELECTOR    equ 2*8
DS_SELECTOR     equ 3*8	; Kernel data selector
ES_SELECTOR   	equ 4*8	; Flat 4 Gb
SS_SELECTOR     equ 5*8	; Monitor stack
CS_SELECTOR     equ 6*8	; Kernel code
GS_SELECTOR     equ 7*8 ; vedio
TSS_SELECTOR    equ 8*8 ; TSS

K_STACK_SPACE   equ 1024

DIVIDE_VECTOR       equ     0   
DEBUG_VECTOR        equ     1   
NMI_VECTOR         equ  2  

BREAKPOINT_VECTOR  equ  3 
OVERFLOW_VECTOR    equ  4

BOUNDS_VECTOR        equ    5 
INVAL_OP_VECTOR      equ    6
COPROC_NOT_VECTOR    equ    7
DOUBLE_FAULT_VECTOR  equ    8
COPROC_SEG_VECTOR    equ    9
INVAL_TSS_VECTOR    equ     10
SEG_NOT_VECTOR      equ     11
STACK_FAULT_VECTOR  equ     12
PROTECTION_VECTOR   equ     13

PAGE_FAULT_VECTOR   equ     14
COPROC_ERR_VECTOR   equ     16

;corresponding to TSS structure
;offsett of current process cpu infomation
P_STACKBASE	equ	0
GSREG		equ	P_STACKBASE
FSREG		equ	GSREG		+ 4
ESREG		equ	FSREG		+ 4
DSREG		equ	ESREG		+ 4
EDIREG		equ	DSREG		+ 4
ESIREG		equ	EDIREG		+ 4
EBPREG		equ	ESIREG		+ 4
KERNELESPREG	equ	EBPREG		+ 4
EBXREG		equ	KERNELESPREG	+ 4
EDXREG		equ	EBXREG		+ 4
ECXREG		equ	EDXREG		+ 4
EAXREG		equ	ECXREG		+ 4
RETADR		equ	EAXREG		+ 4
EIPREG		equ	RETADR		+ 4
CSREG		equ	EIPREG		+ 4
EFLAGSREG	equ	CSREG		+ 4
ESPREG		equ	EFLAGSREG	+ 4
SSREG		equ	ESPREG		+ 4
P_STACKTOP	equ	SSREG		+ 4
P_LDT_SEL	equ	P_STACKTOP
P_LDT		equ	P_LDT_SEL	+ 4

TSS3_S_SP0  equ 4



;8259A interrupt controller prots. 
INT_M_CTL         equ   0x20        ; IO prot for interrupt controller <Master> 
INT_M_CTLMASK     equ   0x21        ; setting bits in this port disables ints <Master> 
NT_S_CTL         equ    0xA0         ; I/O prot for second interrupt controller <Slave> 
NT_S_CTLMASK     equ    0xA1         ; setting bits in this port disables ints <Slave>

EOI             equ 0x20


