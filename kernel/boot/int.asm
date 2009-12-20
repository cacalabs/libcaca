extern idt_default_int,idt_int0, idt_int1, idt_int2, idt_int3, idt_int4, idt_int5, idt_int6, idt_int7, idt_int8, idt_int9, idt_int10, idt_int11, idt_int12, idt_int13, idt_int14, idt_int15, idt_int16, idt_int17, idt_int18, idt_irq0, kbd_int, idt_irq2, idt_irq3, idt_irq4, idt_irq5, idt_irq6, idt_irq7, idt_irq8
global default_int, k_int0, k_int1, k_int2, k_int3, k_int4, k_int5, k_int6, k_int7, k_int8, k_int9, k_int10, k_int11, k_int12, k_int13, k_int14, k_int15, k_int16, k_int17, k_int18, k_irq0, k_irq1, k_irq2, k_irq3, k_irq4, k_irq5, k_irq6, k_irq7, k_irq8
default_int:
	call idt_default_int     ; Call C function
	mov al,0x20              ; End Of Interruption
	out 0x20,al              ; Send EOI to PIC
	iret
k_int0:
	call idt_int0
	mov al,0x20
	out 0x20,al
	iret
k_int1:
	call idt_int1
	mov al,0x20
	out 0x20,al
	iret
k_int2:
	call idt_int2
	mov al,0x20
	out 0x20,al
	iret
k_int3:
	call idt_int3
	mov al,0x20
	out 0x20,al
	iret
k_int4:
	call idt_int4
	mov al,0x20
	out 0x20,al
	iret
k_int5:
	call idt_int5
	mov al,0x20
	out 0x20,al
	iret
k_int6:
	call idt_int6
	mov al,0x20
	out 0x20,al
	iret
k_int7:
	call idt_int7
	mov al,0x20
	out 0x20,al
	iret
k_int8:
	call idt_int8
	mov al,0x20
	out 0x20,al
	iret
k_int9:
	call idt_int9
	mov al,0x20
	out 0x20,al
	iret
k_int10:
	call idt_int10
	mov al,0x20
	out 0x20,al
	iret
k_int11:
	call idt_int11
	mov al,0x20
	out 0x20,al
	iret
k_int12:
	call idt_int12
	mov al,0x20
	out 0x20,al
	iret
k_int13:
	call idt_int13
	mov al,0x20
	out 0x20,al
	iret
k_int14:
	call idt_int14
	mov al,0x20
	out 0x20,al
	iret
k_int15:
	call idt_int15
	mov al,0x20
	out 0x20,al
	iret
k_int16:
	call idt_int16
	mov al,0x20
	out 0x20,al
	iret
k_int17:
	call idt_int17
	mov al,0x20
	out 0x20,al
	iret
k_int18:
	call idt_int18
	mov al,0x20
	out 0x20,al
	iret
k_irq0:
	call idt_irq0
	mov al,0x20
	out 0x20,al
	iret
k_irq1:
	call kbd_int
	mov al,0x20
	out 0x20,al
	iret
k_irq2:
	call idt_irq2
	mov al,0x20
	out 0x20,al
	iret
k_irq3:
	call idt_irq3
	mov al,0x20
	out 0x20,al
	iret
k_irq4:
	call idt_irq4
	mov al,0x20
	out 0x20,al
	iret
k_irq5:
	call idt_irq5
	mov al,0x20
	out 0x20,al
	iret
k_irq6:
	call idt_irq6
	mov al,0x20
	out 0x20,al
	iret
k_irq7:
	call idt_irq7
	mov al,0x20
	out 0x20,al
	iret
k_irq8:
	call idt_irq8
	mov al,0x20
	out 0x20,al
	iret