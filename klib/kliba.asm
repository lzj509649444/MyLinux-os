;kliba.asm

extern  position


INT_M_CTLMASK	equ 0x21
INT_S_CTLMASK	equ 0xA1

[section .text]

global  out_byte
global  in_byte
global  print_str
global  cprint_str  ; display string with color
global  enable_irq
global  disable_irq
global  lock_irq
global  unlock_irq

;void print_str(char *str);
print_str:
    push    ebp
    mov     ebp,esp

    mov     esi,[ebp + 8]   ;esi point to string address
    mov     edi,[position]
    mov     ah,0x0F

.1:
    lodsb
    test    al,al
    jz      .2
    cmp     al,0x0A ; is enter ?
    jnz     .3
    push    eax
    mov     eax,edi
    mov     bl,160
    div     bl
    and     eax,0xFF
    inc     eax
    mov     bl,160
    mul     bl
    mov     edi,eax
    pop     eax
    jmp     .1
.3:
    mov     [gs:edi],ax
    add     edi,2
    jmp     .1
.2:
    mov     [position],edi
    pop     ebp
    ret

; ========================================================================
;                  void disp_color_str(char * info, int color);
; ========================================================================
cprint_str:
	push	ebp
	mov	ebp, esp

	mov	esi, [ebp + 8]	; esi point to string address
	mov	edi, [position]
	mov	ah, [ebp + 12]	; color
.1:
	lodsb
	test	al, al
	jz	.2
	cmp	al, 0x0A	; is enter ?
	jnz	.3
	push	eax
	mov	eax, edi
	mov	bl, 160
	div	bl
	and	eax, 0xFF
	inc	eax
	mov	bl, 160
	mul	bl
	mov	edi, eax
	pop	eax
	jmp	.1
.3:
	mov	[gs:edi], ax
	add	edi, 2
	jmp	.1

.2:
	mov	[position], edi

	pop	ebp
	ret



;void out_byte(u16_t port, u8_t value)
out_byte:
    mov     edx,[esp + 4]       ; port
    mov     al,[esp + 4 + 4]    ; value
    out     dx,al
    nop         ; short delay
    nop
    ret


;void in_byte(u16_t prot)
in_byte:
    mov     edx,[esp + 4]       ; port
    xor     eax,eax
    in      al,dx
    nop         ; short delay
    nop
    ret
   
; ========================================================================
;                  void disable_irq(int irq);
; ========================================================================
; Disable an interrupt request line by setting an 8259 bit.
; Equivalent code:
;	if(irq < 8)
;		out_byte(INT_M_CTLMASK, in_byte(INT_M_CTLMASK) | (1 << irq));
;	else
;		out_byte(INT_S_CTLMASK, in_byte(INT_S_CTLMASK) | (1 << irq));
disable_irq:
        mov     ecx, [esp + 4]          ; irq
        pushf
        cli
        mov     ah, 1
        rol     ah, cl                  ; ah = (1 << (irq % 8))
        cmp     cl, 8
        jae     disable_8               ; disable irq >= 8 at the slave 8259
disable_0:
        in      al, INT_M_CTLMASK
        test    al, ah
        jnz     dis_already             ; already disabled?
        or      al, ah
        out     INT_M_CTLMASK, al       ; set bit at master 8259
        popf
        mov     eax, 1                  ; disabled by this function
        ret
disable_8:
        in      al, INT_S_CTLMASK
        test    al, ah
        jnz     dis_already             ; already disabled?
        or      al, ah
        out     INT_S_CTLMASK, al       ; set bit at slave 8259
        popf
        mov     eax, 1                  ; disabled by this function
        ret
dis_already:
        popf
        xor     eax, eax                ; already disabled
        ret

; ========================================================================
;                  void enable_irq(int irq);
; ========================================================================
; Enable an interrupt request line by clearing an 8259 bit.
; Equivalent code:
;       if(irq < 8)
;               out_byte(INT_M_CTLMASK, in_byte(INT_M_CTLMASK) & ~(1 << irq));
;       else
;               out_byte(INT_S_CTLMASK, in_byte(INT_S_CTLMASK) & ~(1 << irq));
;
enable_irq:
        mov     ecx, [esp + 4]          ; irq
        pushf
        cli
        mov     ah, ~1
        rol     ah, cl                  ; ah = ~(1 << (irq % 8))
        cmp     cl, 8
        jae     enable_8                ; enable irq >= 8 at the slave 8259
enable_0:
        in      al, INT_M_CTLMASK
        and     al, ah
        out     INT_M_CTLMASK, al       ; clear bit at master 8259
        popf
        ret
enable_8:
        in      al, INT_S_CTLMASK
        and     al, ah
        out     INT_S_CTLMASK, al       ; clear bit at slave 8259
        popf
        ret


; void lock();
lock_irq:
    cli
    ret

; void unlock();
unlock_irq:
    sti
    ret
