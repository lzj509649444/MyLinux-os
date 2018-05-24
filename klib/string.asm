; string.asm
[section .text]
global  memcpy
global  memset
global  strcpy


;void *memcpy(void *es:p_dst, void *ds:p_src, int size);
memcpy:
    push    ebp
    mov     ebp,esp

    push    esi
    push    edi
    push    ecx

    mov     edi,[ebp + 8]   ;destination
    mov     esi,[ebp + 12]  ;source
    mov     ecx,[ebp + 16]  ;size
.1:
    cmp     ecx,0
    jz      .2

    mov     al,[ds:esi]
    inc     esi
    mov     byte [es:edi],al
    inc     edi
    dec     ecx
    jmp     .1
.2:
    mov     eax,[ebp+8] ;return

    pop     ecx
    pop     edi
    pop     esi
    mov     esp,ebp
    pop     ebp

    ret



;void memste(void *p_dst, char ch, int size);
memset:
    push    ebp
    mov     ebp,esp

    push    esi
    push    edi
    push    ecx

    mov     edi,[ebp+8] ; destination
    mov     edx,[ebp+12]    ;char to be setted
    mov     ecx,[ebp+16]    ;size
.1:
    cmp     ecx,0
    jz      .2

    mov     byte [edi],dl
    inc     edi

    dec     ecx
    jmp     .1
.2:
	pop	ecx
	pop	edi
	pop	esi
	mov	esp, ebp
	pop	ebp
    
    ret


;void *strcpy(char *p_dst, char *p_src);
strcpy:
	push    ebp
	mov     ebp, esp

	mov     esi, [ebp + 12] ; source
	mov     edi, [ebp + 8]  ; destination

.1:
    mov     al,[esi]
    inc     esi

    mov     byte [edi],al
    inc     edi

    cmp     al,0
    jnz     .1

    mov     eax,[ebp + 8]  ;return

    mov     esp,ebp
    pop     ebp
    ret

    

