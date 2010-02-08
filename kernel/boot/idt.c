/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2006 Sam Hocevar <sam@hocevar.net>
 *                2009 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/*
 *'freely' inspired by http://jojo.ouvaton.org/dossiers/boot_sector/tutorial02.html
 * (actually, that's mostly copied, with minor compilation fixes)
 */
#include "kernel.h"
#include "klibc.h"

#define IDTBASE	0               /* GDT Physical address */
#define IDTSIZE	256             /* Max descriptor count */
#define INTGATE 0x8E00          /* Interruptions */
#define TRAPGATE 0x8F00         /* Syscalls */
#define TASKGATE 0x8500         /* Task switching */

/* segment descriptor */
struct idtdesc
{
    u16 offset0_15;
    u16 select;
    u16 type;
    u16 offset16_31;
} __attribute__ ((packed));

/* IDTR register */
struct idtr
{
    u16 limite;
    u32 base;
} __attribute__ ((packed));

struct idtr kidtr;

/* IDT table */
struct idtdesc kidt[IDTSIZE] = { {0, 0, 0, 0} };

/* pointer on a free IDT entry */
unsigned int kidtptr = 0;

void default_int(void);
void k_int0(void);
void k_int1(void);
void k_int2(void);
void k_int3(void);
void k_int4(void);
void k_int5(void);
void k_int6(void);
void k_int7(void);
void k_int8(void);
void k_int9(void);
void k_int10(void);
void k_int11(void);
void k_int12(void);
void k_int13(void);
void k_int14(void);
void k_int15(void);
void k_int16(void);
void k_int17(void);
void k_int18(void);
void k_irq0(void);
void k_irq1(void);
void k_irq2(void);
void k_irq3(void);
void k_irq4(void);
void k_irq5(void);
void k_irq6(void);
void k_irq7(void);
void k_irq8(void);


void init_idt_desc(u32 offset, u16 select, u16 type, struct idtdesc *desc)
{
    desc->offset0_15 = (offset & 0xffff);
    desc->select = select;
    desc->type = type;
    desc->offset16_31 = (offset & 0xffff0000) >> 16;
    return;
}


void add_idt_desc(struct idtdesc desc)
{
    kidt[kidtptr++] = desc;
    return;
}


void init_idt(void)
{
    struct idtdesc desc;
    int i;


    for (i = 0; i < IDTSIZE; i++)
    {
        init_idt_desc((u32) default_int, 0x08, INTGATE, &desc);
        add_idt_desc(desc);
    }

    init_idt_desc((u32) k_int0, 0x08, INTGATE, &kidt[0]);
    init_idt_desc((u32) k_int1, 0x08, INTGATE, &kidt[1]);
    init_idt_desc((u32) k_int2, 0x08, INTGATE, &kidt[2]);
    init_idt_desc((u32) k_int3, 0x08, INTGATE, &kidt[3]);
    init_idt_desc((u32) k_int4, 0x08, INTGATE, &kidt[4]);
    init_idt_desc((u32) k_int5, 0x08, INTGATE, &kidt[5]);
    init_idt_desc((u32) k_int6, 0x08, INTGATE, &kidt[6]);
    init_idt_desc((u32) k_int7, 0x08, INTGATE, &kidt[7]);
    init_idt_desc((u32) k_int8, 0x08, INTGATE, &kidt[8]);
    init_idt_desc((u32) k_int9, 0x08, INTGATE, &kidt[9]);
    init_idt_desc((u32) k_int10, 0x08, INTGATE, &kidt[10]);
    init_idt_desc((u32) k_int11, 0x08, INTGATE, &kidt[11]);
    init_idt_desc((u32) k_int12, 0x08, INTGATE, &kidt[12]);
    init_idt_desc((u32) k_int13, 0x08, INTGATE, &kidt[13]);
    init_idt_desc((u32) k_int14, 0x08, INTGATE, &kidt[14]);
    init_idt_desc((u32) k_int15, 0x08, INTGATE, &kidt[15]);
    init_idt_desc((u32) k_int16, 0x08, INTGATE, &kidt[16]);
    init_idt_desc((u32) k_int17, 0x08, INTGATE, &kidt[17]);
    init_idt_desc((u32) k_int18, 0x08, INTGATE, &kidt[18]);

    init_idt_desc((u32) k_irq0, 0x08, INTGATE, &kidt[32]);
    init_idt_desc((u32) k_irq1, 0x08, INTGATE, &kidt[33]);
    init_idt_desc((u32) k_irq2, 0x08, INTGATE, &kidt[34]);
    init_idt_desc((u32) k_irq3, 0x08, INTGATE, &kidt[35]);
    init_idt_desc((u32) k_irq4, 0x08, INTGATE, &kidt[36]);
    init_idt_desc((u32) k_irq5, 0x08, INTGATE, &kidt[37]);
    init_idt_desc((u32) k_irq6, 0x08, INTGATE, &kidt[38]);
    init_idt_desc((u32) k_irq7, 0x08, INTGATE, &kidt[39]);
    init_idt_desc((u32) k_irq8, 0x08, INTGATE, &kidt[40]);

    kidtr.limite = IDTSIZE * 8;
    kidtr.base = IDTBASE;

    memcpy((void *)kidtr.base, kidt, kidtr.limite);
#define printf(format, ...) { char __str[255]; sprintf (__str, format, __VA_ARGS__); print(__str);}

    printf("Loading IDT from 0x%x\n", kidtr);
#undef printf

    asm("lidtl (kidtr)");
}
