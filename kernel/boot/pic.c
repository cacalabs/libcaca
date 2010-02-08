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


#include "klibc.h"

#define PIC_MASTER_ICW1 0x20
#define PIC_MASTER_ICW2 0x21

#define PIC_SLAVE_ICW1  0xA0
#define PIC_SLAVE_ICW2  0xA1


void init_pic(void)
{
    /* MASTER */
    outbp(PIC_MASTER_ICW1, 0x11);       // Init 8259A-1
    /* ICW2 - start vector = 32 */
    outbp(PIC_MASTER_ICW2, 0x20);       // IRQ 0-7 mapped to 0x20-0x27
    /* IICW3 */
    outbp(PIC_MASTER_ICW2, 0x04);       // 8259A-1 has slave
    /* ICW4 */
    outbp(PIC_MASTER_ICW2, 0x01);
    /* Int mask */
    outbp(PIC_MASTER_ICW2, 0xFF);

    /* SLAVE */
    outbp(PIC_SLAVE_ICW1, 0x11);
    /* ICW2 - start vector = 96 */
    outbp(PIC_SLAVE_ICW2, 0x70);
    /* ICW3 */
    outbp(PIC_SLAVE_ICW2, 0x02);
    /* ICW4 */
    outbp(PIC_SLAVE_ICW2, 0x01);
    /* Int Mask */
    outbp(PIC_SLAVE_ICW2, 0xFF);


    /* Unmask irqs */
    outbp(0x21, 0xFD);

}

static unsigned int cached_irq_mask = 0xffff;

#define __byte(x,y) 	(((unsigned char *)&(y))[x])
#define cached_21	(__byte(0,cached_irq_mask))
#define cached_A1	(__byte(1,cached_irq_mask))

void disable_interrupt(char irq)
{
    unsigned int mask = 1 << irq;

    cached_irq_mask |= mask;
    if (irq & 8)
    {
        outb(0xA1, cached_A1);
    }
    else
    {
        outb(0x21, cached_21);
    }
}

void enable_interrupt(char irq)
{
    unsigned int mask = ~(1 << irq);

    cached_irq_mask &= mask;
    if (irq & 8)
    {
        outb(0xA1, cached_A1);
    }
    else
    {
        outb(0x21, cached_21);
    }

}
