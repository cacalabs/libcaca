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

#include "kernel.h"
#include "klibc.h"

extern int kmain(void);


/* Entry point bootsect.asm loaded this file at 0x0100:0x0, which is mapped
   at 0x8:0x1000 (selector+8bytes, offset 1000 (0x100 + 0x0) */

/* 0x1000 */
void _start(void)
{
    clearscreen();

    init_gdt();
    print("Loading IDT\n");
    init_idt();
    print("Loading PIC\n");
    init_pic();
    print("Running kmain()\n");
    sti;

    kmain();                    /* Call kernel's kmain() */

    while (1)
    {                           /* Never return */
        print("hlt;\n");
    }
}
