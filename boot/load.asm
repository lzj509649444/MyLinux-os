;load.asm
BOOTSEC     equ     0x1000      ; Secondary boot code segment
BOOTOFF     equ     0x0030      ; Offset 
BUFFER      equ     0x0600      ; First free memory
KERNELBUFFER    equ 0x1F00      ; kernel code Offset
KERNELSEG       equ BOOTSEC 
KERNELOFF       equ BOOTOFF

BPB_SecPerTrk       equ     18
rootdirectoryoff    equ     19
fattboff            equ     1
fattbsecs           equ     9
rootdirsecs         equ     14
rootdiritems        equ     224
dataoff             equ     19 + 14 - 2

DS_SELECTOR     equ	   3*8	; Kernel data selector
ES_SELECTOR     equ	   4*8	; Flat 4 Gb
SS_SELECTOR     equ	   5*8	; Monitor stack
CS_SELECTOR     equ	   6*8	; Kernel code
GS_SELECTOR     equ	   7*8	; Monitor code

org	0x0030
boot:

    mov     ax,cs
    mov	    ds,ax
    mov     es,ax

    pop     ax
    mov     word [loadsecs],ax
    mov     ax,cs
    cli
    mov     ss,ax
    mov     sp,BOOTOFF
    sti

    ;print load begin message
    mov     bx,loadmessage
    call    print           
    
    ;reset floppy
    xor     ah,ah
    xor     dl,dl
    int     0x0013

    ;read root directory data to buffer 
    mov     byte [sec_t],0
    mov     word [sec_t + 1],rootdirectoryoff
    mov     word [sec_t + 3],1
    mov     word [sec_t + 5],0
    mov     word [sec_t + 7],BUFFER
    mov     ax,sec_t
    push    ax
    call    readsec
    pop     ax


    ;search KERNEL.BIN in root directory
    mov     ax,0
    call    search      ; return first sector number in ax
    cmp	    ax,0
    je      $

    ;read fat table data to buffer
    mov     byte [sec_t],0
    mov     word [sec_t + 1],fattboff
    mov     word [sec_t + 3],fattbsecs
    mov     word [sec_t + 5],0
    mov     word [sec_t + 7],BUFFER
    push    ax
    mov     ax,sec_t
    push    ax
    call    readsec
    pop     ax
    pop	    ax

    
    ;load OS kerenl
    call    loadkernel

    ;close drive
    call    killmotor


;move this code to safety, then jump to it.

    mov     ax,[loadsecs]  ; move cx sectors
    mov     bx,512
    mul     bx
    mov     cx,ax
    cld
  
    mov     ax,BOOTSEC
    mov     ds,ax
    mov     si,BOOTOFF  ; si = start of this code
    xor     ax,ax
    mov     es,ax
    mov     di,BUFFER   ; di = buffer area
    rep     movsb
    jmp     word 0x0000:(BUFFER + reposition - 0x0030)

    ;reposition kernel code to right place
reposition:
    
    call    initkernel
    jmp     minix386    ;switch to protected mode 
    

;read sectors
;int readsec(char device,unsigned short sec,unsigned short secs,unsigned int buffer);
readsec:
    push    bp
    mov     bp,sp
    
    push    ax
    push    cx
    push    es
    push    bx

    mov     bx,[word bp + 4]
    mov     es,[word bx + 5] ;buffer segment
    
    mov     ax,word [bx + 1] ;ax = begsec
    mov     bl,BPB_SecPerTrk
    div     bl                  ; al >> 1 = clyinder , ah = begsec - 1
    inc     ah                  ; ah = begsec
    mov     cl,ah               ; cl = begsec
    mov     dh,al               
    shr     al,1                ; dh = clyinder
    mov     ch,al
    and     dh,1                ; dh & 1 = header id

    pop     bx
    push    si
    mov     si,word  [bp + 4] ; read sectors to es:bx 
    mov     bx,word  [si + 7]
    mov     dl,byte  [si]    ; dirver id
    pop     si

continueread:
    mov     ah,2
    push    bx
    mov     bx,word [bp + 4]
    mov     al,byte [bx + 3] ; read al sectors
    pop     bx
    int     0x13
    jc      continueread

    pop     es
    pop     cx
    pop     ax
    mov     sp,bp
    pop     bp
    ret

;load kernel code to 0x0000:KERNELBUFFER
;ax = sector number
loadkernel:

    push    es
    xor     cx,cx
    mov     es,cx
    push    di
    mov     byte [sec_t],0      ;drive id
    mov     word [sec_t + 3],1  ;sector to read
    mov     word [sec_t + 5],0    ;Secondary boot code segment

    mov     cx,KERNELBUFFER
    sub     cx,512
loadsector:
    add     cx,512
    mov     word [sec_t + 7],cx     ;secondary boot code Offset
    
    push    ax
    add     ax,dataoff
    mov     word [sec_t + 1],ax     ;logical sector number
    mov     ax,sec_t
    push    ax
    call    readsec
    pop     ax    

getsecnum:
    pop     ax
    mov     bx,3
    mul     bx
    mov     bx,2
    div     bx
    mov	    di,BUFFER
    add     di,ax
    mov     ax,word [es:di]
    cmp     dx,0
    jz      even
    shr     ax,4
    jmp     finish
even:
    and	    ax,0x0fff
finish:
    cmp     ax,0x0fff
    jl      loadsector

    pop     es
    pop     di
    ret


;print message
;void print(char *message);
print:  
    push    ax
    push    si
    mov     si,bx
printstart:
    lodsb
    test    al,al
    jz      printend
    mov     ah,0x0e     ;print character in teletype mode
    mov     bx,0x0001   ;page 0,foreground color
    int     0x10        ;call BIOS VIDEO_IO
    jmp     printstart
printend:
    pop     si
    pop     ax
    ret

; search file by compare file name
search:
    push    es
    xor	    dx,dx
    mov     es,dx
    mov     di,BUFFER
nextitem:
    cmp     ax,0    ;find 
    jne     find
    cmp     word [rootdiritems_t],0      ; root directory items
    je      notfind
    
compare:
    push    di
    mov     cx,11               ; file name length
    mov     si,kernelfilename
    repe    cmpsb
    je      find
    pop	    di
    add     di,32
    dec     word [rootdiritems_t]
    jmp     nextitem
  
find:   
    mov     ax,word [es:di + 15]   ; first sector number for loadfilename
    pop     di
    mov     bx,findkernelmessage
    call    print               ; show message " find "
    jmp     searchend
notfind:
    mov     bx,notfindkernelmessage
    call    print               ; show message " not find "
searchend:
    pop     es
    ret

;reset kernel address
initkernel:
    mov     ax,KERNELSEG
    mov     es,ax
    xor     ax,ax
    mov     ds,ax
    mov     si,KERNELBUFFER
    and     esi,0x0000ffff

    mov     bx,(BUFFER - 0x0030)
    mov     ax,[si + elf_ehsize]    ;elf header size
    mov     [bx + elfdata],ax
    mov     ax,[si + elf_phnum]     ;elf program number
    mov     [bx + elfdata + 4],ax
    mov     ax,[si + elf_phentsize]     ;each program header size
    mov     [bx + elfdata + 8],ax
    mov     eax,[si + elf_phoff]  ;first program header Offset
    mov     [bx + elfdata + 12],eax
    mov     eax,[si + elf_shoff] ;first section header offset
    mov     [bx + elfdata + 16],eax
    mov     ax,[si + elf_shentsize] ;each section header size
    mov     [bx + elfdata + 20],ax
    mov     ax,[si + elf_shnum] ;section header number
    mov     [bx + elfdata + 24],ax
    mov     dl,[si + elf_shstrndx]  ;section header sting table index
    mov     eax,[bx + elfdata + 20] ;section header size
    mul     dl
    add     eax,[bx + elfdata + 16 ] ;string table section entry
    add     eax,elf_shoffset
    mov     edx,[esi + eax]          ;string table offset
    mov     [bx + elfdata + 28],edx


    mov     bx,si
    add     bx,[BUFFER -0x0030 + elfdata+12]    ; bx point to first program header

setkernel:
    mov     ax,[bx + elf_poffset]   ;get current program Offset
    mov     cx,[bx + elf_memsz]     ;text size
    push    si
    add     si,ax       ;source Offset
    mov     di,[bx + elf_pvaddr]
    cld     
    rep     movsb
    add     bx,[BUFFER - 0x0030 + elfdata + 8]    ;bx point to next program header
    pop     si
    dec     word [BUFFER -0x0030 + elfdata + 4]  ; read one program text
    jnz      setkernel

    ret

;switch to Protected mode
minix386:
    ;set Global descriptor
    push    ds
    ;get date segment from elf file string table
    mov     ax,KERNELBUFFER
    shr     ax,4
    mov     ds,ax
    xor     ax,ax
    mov     es,ax
    mov     edx,[es:BUFFER -0x0030 + elfdata + 16]   ;first section header offset
    mov     cx,[es:BUFFER - 0x0030 + elfdata + 24]   ;setion number
.1:
    mov     eax,dword [es:BUFFER -0x0030 + secname]
    mov     ebx,dword [edx]                         ;get section type
    add     ebx,[es:BUFFER -0x0030 + elfdata + 28]  ;section name entry in string table
    push    edx
    mov     edx,dword [ebx] ;get section name
    cmp     eax,dword [ebx]
    je      .2
    pop     edx
    add     edx,[es:BUFFER -0x0030 + elfdata + 20]   ;next setion header
    loop    .1
.2:
    pop     edx
    mov     ebx,edx ;data section header
    mov     dx,word [ebx + elf_shaddr + 2]    ;data segment high address
    mov     ax,word [ebx + elf_shaddr]       ;data segment low address 
    
    mov     word [sssegment],ax              ; ss segment low base
    mov     byte [sssegment + 2],dl          ; ss segment high base

    pop     ds
    mov     bx,(BUFFER - 0x0030)
    ;mov     word [bx + p_ds_desc + 2],ax    ;set DS descriptor
    ;mov     byte [bx + p_ds_desc + 4],dl


    mov     word [bx + p_es_desc + 2],0x0000    ;set ES descriptor
    mov     byte [bx + p_es_desc + 4],0x00

    mov     word [bx + p_ss_desc + 2],0    ;set SS descriptor
    mov     byte [bx + p_ss_desc + 4],0

    mov     dx,KERNELSEG
    shr     dx,12
    mov     ax,KERNELOFF

    mov     word [bx + p_ds_desc + 2],0
    mov     byte [bx + p_ds_desc + 4],0

    mov     word [bx + p_cs_desc + 2],0
    mov     byte [bx + p_cs_desc + 4],0

    ;mov     word [bx + p_es_desc + 2],ax
    ;mov     byte [bx + p_es_desc + 4],dl

    ;mov     word [bx + p_ss_desc + 2],ax
    ;mov     byte [bx + p_ss_desc + 4],dl

    mov     word [bx + p_gs_desc + 2],0x8000
    mov     byte [bx + p_gs_desc + 4],0xB

    mov     word [bx + p_gdt_desc + 2],(BUFFER + p_gdt - 0x0030) 
    mov     byte [bx + p_gdt_desc + 4],0


    ;mov     ax,DS_SELECTOR
    ;mov     ds,ax
    ;mov     ax,ES_SELECTOR
    ;mov     es,ax
    ;mov     ax,SS_SELECTOR
    ;mov     ss,ax
    xor     ax,ax
    mov     ds,ax
    call    real2prot

    ;mov     ax,ES_SELECTOR
    ;mov     es,ax
    mov     ax,GS_SELECTOR
    mov     gs,ax
    mov     ax,DS_SELECTOR
    mov     ds,ax
    mov     ax,SS_SELECTOR
    mov     ss,ax
    mov     ax,ES_SELECTOR
    mov     es,ax
    xor     eax,eax
    mov     eax,dword [sssegment]
    jmp     dword CS_SELECTOR:(KERNELSEG * 16 + KERNELOFF)

; Switch from real to protected mode.
real2prot:
    lgdt    [bx + p_gdt_desc]
    cli
    call    enableA20

    mov     eax,cr0
    or      eax,1
    mov     cr0,eax

    ret


enableA20:
	in	al, 92h
	or	al, 00000010b
	out	92h, al
    ret    

;close floppy
killmotor:
    push	dx
	mov	dx, 03F2h
	mov	al, 0
	out	dx, al
	pop	dx
	ret
    

;elf header
elf_phoff       equ     28              ;program header entry
elf_shoff       equ     elf_phoff + 4   ;section header entry
elf_ehsize      equ     elf_shoff + 8   ;elf header size
elf_phentsize   equ     elf_ehsize + 2  ;each program header size
elf_phnum       equ     elf_phentsize + 2  ;program header number
elf_shentsize   equ     elf_phnum + 2    ;each section header size
elf_shnum       equ     elf_shentsize + 2   ;section header number
elf_shstrndx    equ     elf_shnum + 2       ;section header string table index
;program header
elf_poffset     equ     4                   ;text code entry
elf_pvaddr      equ     elf_poffset + 4     ;text code vaturail adderss
elf_memsz       equ     elf_pvaddr + 12    ;text code memory size
;section header
elf_shaddr      equ     12   ;section address     
elf_shoffset    equ     elf_shaddr + 4  ;section offset

    
elfdata:
    dd  0   ;elf header size
    dd  0   ;program header number
    dd  0   ;each program header size
    dd  0   ;first program header Offset    
    dd  0   ;first section header Offset
    dd  0   ;each seciton header size
    dd  0   ;section header number
    dd  0   ;string table entry

; struct for readsec funcation
sec_t:
    db  0   ;dirver id
    dw  0   ;sector number
    dw  0   ;sectors to read
    dw  0   ;es
    dw  0   ;bx

;load sectors
loadsecs:
    dw  0
sssegment:
    dw  0
    dw  0
; Global descriptor tables.
UNSET   equ 0		; Must be computed

; Protected mode descriptor table.
p_gdt:
p_null_desc:
	; Null descriptor
	dw	0x0000, 0x0000
	db	0x00, 0x00, 0x00, 0x00
p_gdt_desc:
	; Descriptor for this descriptor table
	dw	8*8-1, UNSET
	db	UNSET, 0x00, 0x00, 0x00
p_idt_desc:
	; Real mode interrupt descriptor table descriptor
	dw	0x03FF, 0x0000
	db	0x00, 0x00, 0x00, 0x00
p_ds_desc:
	; Kernel data segment descriptor (4 Gb flat)
	dw	0xFFFF, UNSET   ;*0xFFFF = 64k, one page*
	db	UNSET, 0x92, 0xCF, 0x00
p_es_desc:
	; Physical memory descriptor (4 Gb flat)
	dw	0xFFFF, 0x0000
	db	0x00, 0x92, 0xCF, 0x00
p_ss_desc:
	; stack segment descriptor (64 kb flat)
	dw	0xFFFF, UNSET
	db	UNSET, 0x92, 0xCF, 0x00
p_cs_desc:
	; Kernel code segment descriptor (4 Gb flat)
	dw	0xFFFF, UNSET
	db	UNSET, 0x9A, 0xCF, 0x00
p_gs_desc:
	; video segment descriptor (4 Gb flat)
	dw	0xFFFF, UNSET
	db	UNSET, 0x92, 0xC0, 0x00

;data elf section name
secname:    db ".data"


;data and message string
rootdiritems_t:             dw  rootdirsecs
loadmessage:                db  "start load....",0xA,0xD,0
kernelfilename:             db  "KERNEL  BIN",0xA,0xD,0
findkernelmessage:          db  "find kernel",0xA,0xD,0
notfindkernelmessage:       db  "kernel not find",0xA,0xD,0
