; Boot sector, freely inspired by http://jojo.ouvaton.org/dossiers/boot_sector/tutorial02.html
; Sets a GDT and protected mode
; Copy the kernel (or stage2) to 0x1000 (linear)
; Max kernel size is 25600 bytes (50 sectors)


%define	BASE	0x100	; 0x0100:0x0 = 0x1000
%define KSIZE	50

[BITS 16]
[ORG 0x0]

; Code starts here, jump to our real entry point, skipping print
jmp start

; print string using BIOS
print:
		push ax
		push bx
	.start:
		lodsb		; ds:si -> al
		cmp al,0
		jz .fin
		mov ah,0x0E	; 0x0e function of BIOS 0x10 interruption
		mov bx,0x07	; bx -> attribute, al -> ascii
		int 0x10
        jmp .start
	
	.fin:
		pop bx
		pop ax
		ret
        
; Entry point
start:
	mov [bootdrv],dl	; boot drive

; Stack and section initialization at 0x07C0
	mov ax,0x07C0
	mov ds,ax
	mov es,ax
	mov ax,0x8000	; stack at 0xFFFF
	mov ss,ax
	mov sp, 0xf000

; print 'Loading kernel ...'
	mov si, ldKernel
	call print

; Load kernel
	xor ax,ax
	int 0x13

	push es
	mov ax,BASE
	mov es,ax
	mov bx,0
	mov ah,2
	mov al,KSIZE
	mov ch,0
	mov cl,2
	mov dh,0
	mov dl,[bootdrv]
	int 0x13
	pop es

; GDT pointer
	mov ax,gdtend	; GDT limit
	mov bx,gdt
	sub ax,bx
	mov word [gdtptr],ax

	xor eax,eax		; GDT linear address
	xor ebx,ebx
	mov ax,ds
	mov ecx,eax
	shl ecx,4
	mov bx,gdt
	add ecx,ebx
	mov dword [gdtptr+2],ecx

; pmode
	cli
	lgdt [gdtptr]	; load GDT
	mov eax, cr0
	or ax,1
	mov cr0,eax		; enter Protected Mode

	jmp next
next:
	mov ax,0x10		; data segment
	mov ds,ax
	mov fs,ax
	mov gs,ax
	mov es,ax
	mov ss,ax
	mov esp,0x9F000	

	jmp dword 0x8:0x1000    ; jump to kernel _start at 0x1000 of 'second' selector (*8)


;--------------------------------------------------------------------
bootdrv: db 0
ldKernel	db	"Loading kernel ...",13,10,0
;--------------------------------------------------------------------
gdt:
	db 0,0,0,0,0,0,0,0
gdt_cs:
	db 0xFF,0xFF,0x0,0x0,0x0,10011011b,11011111b,0x0
gdt_ds:
	db 0xFF,0xFF,0x0,0x0,0x0,10010011b,11011111b,0x0
gdtend:

;--------------------------------------------------------------------
gdtptr:
	dw	0	; limit
	dd	0	; base


;--------------------------------------------------------------------
;; fill remaining space with NOPs
times 510-($-$$) db 144
dw 0xAA55