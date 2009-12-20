/* 
 *  libcaca       
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2006 Sam Hocevar <sam@hocevar.net>
 *                2009 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                All Rights Reserved
 *
 *  $Id: kernel.h 4154 2009-12-20 13:33:11Z jylam $
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

#define GDTBASE	0x800           /* Physical address of GDT */
#define GDTSIZE	0xFF            /* Maximum table size (in entries) */

/* Segment descriptor */
struct gdtdesc
{
    u16 lim0_15;
    u16 base0_15;
    u8 base16_23;
    u8 acces;
    u8 lim16_19:4;
    u8 other:4;
    u8 base24_31;
} __attribute__ ((packed));

/* GDTR register */
struct gdtr
{
    u16 limite;
    u32 base;
} __attribute__ ((packed));
struct gdtr kgdtr;


struct gdtdesc kgdt[GDTSIZE] = { {0, 0, 0, 0, 0, 0, 0} };

unsigned int kgdtptr = 1;

void init_code_desc(u32 base, u32 limite, struct gdtdesc *desc);
void init_data_desc(u32 base, u32 limite, struct gdtdesc *desc);
void add_gdt_desc(struct gdtdesc desc);
void init_gdt(void);



void init_gdt_desc(u32 base, u32 limite, u8 acces, u8 other,
                   struct gdtdesc *desc)
{
    desc->lim0_15 = (limite & 0xffff);
    desc->base0_15 = (base & 0xffff);
    desc->base16_23 = (base & 0xff0000) >> 16;
    desc->acces = acces;
    desc->lim16_19 = (limite & 0xf0000) >> 16;
    desc->other = (other & 0xf);
    desc->base24_31 = (base & 0xff000000) >> 24;
    return;
}


void init_code_desc(u32 base, u32 limite, struct gdtdesc *desc)
{
    init_gdt_desc(base, limite, 0x9B, 0x0D, desc);
}

void init_data_desc(u32 base, u32 limite, struct gdtdesc *desc)
{
    init_gdt_desc(base, limite, 0x93, 0x0D, desc);
}

void add_gdt_desc(struct gdtdesc desc)
{
    kgdt[kgdtptr] = desc;
    kgdtptr++;
}

void init_gdt(void)
{
    struct gdtdesc code, data, stack;

    /* initialisation des descripteurs de segment */
    init_code_desc(0x0, 0xFFFFF, &code);
    init_data_desc(0x0, 0xFFFFF, &data);
    init_gdt_desc(0, 0x10, 0x97, 0x0D, &stack);
    add_gdt_desc(code);
    add_gdt_desc(data);
    add_gdt_desc(stack);

    /* initialisation de la structure pour GDTR */
    kgdtr.limite = GDTSIZE * 8;
    kgdtr.base = GDTBASE;

    /* recopie de la GDT a son adresse */
    memcpy((void *)kgdtr.base, kgdt, kgdtr.limite);

    /* chargement du registre GDTR */
    asm("lgdtl (kgdtr)");

    /* initialisation des segments */
    asm("	movw $0x10,%ax	\n \
		movw %ax, %ds	\n \
		movw %ax, %es	\n \
		movw %ax, %fs	\n \
		movw %ax, %gs	\n \
		movw $0x18,%ax	\n \
		movw %ax, %ss	\n \
		movl $0x1FFFF,%esp	\n \
		nop	\n \
		nop	\n \
		ljmp $0x08,$next	\n \
		next:	\n");
}
