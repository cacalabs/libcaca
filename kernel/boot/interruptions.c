/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2006 Sam Hocevar <sam@hocevar.net>
 *                2009 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */


#include "kernel.h"
#include "klibc.h"
#include "drivers/timer.h"


void idt_default_int(void)
{
    print("!! Unknown interrupt\n");
}

void idt_int0(void)
{
    print("!! Divide by 0\n ");
}

void idt_int1(void)
{
    print("!! Debug exception ");
}

void idt_int2(void)
{
    print("!! NMI ");
}

void idt_int3(void)
{
    print("!! Breakpoint ");
}

void idt_int4(void)
{
    print("!! Overflow ");
}

void idt_int5(void)
{
    print("!! BoundCheck ");
}

void idt_int6(void)
{
    print("!! Invalid Opcode ");
}

void idt_int7(void)
{
    print("!! Coprocessor not available ");
}

void idt_int8(void)
{
    print("!! Double Fault ");
}

void idt_int9(void)
{
    print("!! Coprocessor segment overrun ");
}

void idt_int10(void)
{
    print("!! Invalid TSS ");
}

void idt_int11(void)
{
    print("!! Segment not present ");
}

void idt_int12(void)
{
    print("!! Stack exception ");
}

void idt_int13(void)
{
    print("!! General protection exception ");
}

void idt_int14(void)
{
    print("!! Page fault ");
}

void idt_int15(void)
{
    print("!! Intel reserved int ");
}

void idt_int16(void)
{
    print("!! Coprocessor error ");
}

void idt_int17(void)
{
    print("!! Intel reserved (2) ");
}

void idt_int18(void)
{
    print("i18 ");
}


/* Used by Channel0 timer */
void idt_irq0(void)
{
    ticks++;
}

void idt_irq2(void)
{
    print("IRQ 2");
}

void idt_irq3(void)
{
    print("IRQ 3");
}

void idt_irq4(void)
{
    print("IRQ 4");
}

void idt_irq5(void)
{
    print("IRQ 5");
}

void idt_irq6(void)
{
    print("IRQ 6");
}

void idt_irq7(void)
{
    print("IRQ 7");
}

void idt_irq8(void)
{
    print("IRQ 8");
}


extern unsigned char kbdmap[];

/* Keyboard irq is 1 */
void kbd_int(void)
{
    unsigned char i;
    do
    {
        i = inb(0x64);
    }
    while ((i & 0x01) == 0);

    i = inb(0x60);
    i--;

    if (i < 0x80)
    {
        putcar(kbdmap[i * 4]);
    }
}
