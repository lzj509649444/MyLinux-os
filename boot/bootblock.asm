;boot code which placed in first sector of boot device

LOADOFF     equ     0x7c00      ; 0x0000:LOADOFF is where this code is loaded
BOOTSEC     equ     0x1000      ; Secondary boot code segment
BOOTOFF     equ     0x0030      ; Offset 
BUFFER      equ     0x0600      ; First free memory


rootdirectoryoff    equ     19
fattboff            equ     1
fattbsecs           equ     9
rootdirsecs         equ     14
rootdiritems        equ     224
dataoff             equ     19 + 14 - 2

org     0x7c00
jmp short   boot
nop
%include	"include/fat12hdr.inc"


boot:
    mov     ax,cs
    mov     ds,ax
    mov     es,ax
    cli
    mov     ss,ax
    mov     sp,LOADOFF
    sti

    ;clear screen
    mov     ax,0x0600
    mov     bx,0x0700
    mov     cx,0
    mov     dx,0x0184f
    int     0x0010

    ; print boot message
    mov     bx,bootmessage
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

    ;search boot file in the root directory
    mov     ax,0
    call    search      ; return first sector number in ax

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
    ;load Secondary boot code
    call    load 
    mov	    ax,[loadsecs]
    push    ax

    jmp     word BOOTSEC:BOOTOFF



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
    mov     bl,[BPB_SecPerTrk]
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


; search file by compare file name
search:
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
    mov     si,loadfilename
    repe    cmpsb
    je      find
    pop     di
    add     di,32
    dec     word [rootdiritems_t]
    jmp     nextitem
  
find:   
    mov     ax,word [es:di + 15]   ; first sector number for loadfilename
    pop     di
    mov     bx,findmessage
    call    print               ; show message " find "
    jmp     searchend
notfind:
    mov     bx,notfindmessage
    call    print               ; show message " not find "
searchend:
    ret

;load Secondary boot code
;ax = sector number
load:
    push    di
    mov     byte [sec_t],0      ;drive id
    mov     word [sec_t + 3],1  ;sector to read
    mov     word [sec_t + 5],BOOTSEC    ;Secondary boot code segment

    mov     cx,BOOTOFF
    sub     cx,512
loadsector:
	inc		word [loadsecs]			;recorde how many sectors for the loader code
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
    


; struct for readsec funcation
sec_t:
    db  0   ;dirver id
    dw  0   ;sector number
    dw  0   ;sectors to read
    dw  0   ;es
    dw  0   ;bx

; load sectors
loadsecs:	dw	0	;

;data definition
rootdiritems_t:         dw  rootdirsecs
loadfilename:           db "LOADER  BIN", 0xA,0xD,0
bootmessage:            db "Booting",0xA,0xD,0
findmessage:            db "find",0xA,0xD,0
notfindmessage:         db "do not find",0xA,0xD,0
