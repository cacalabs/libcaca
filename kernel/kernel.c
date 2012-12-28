/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2006-2012 Sam Hocevar <sam@hocevar.net>
 *                2009-2010 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

#include "config.h"
#include "caca_types.h"

#ifdef __KERNEL__

#include "klibc.h"
#include "boot/stage2.h"
#include "kernel.h"
#include "drivers/processor.h"
#include "drivers/floppy.h"
#include "drivers/timer.h"

extern char const * const * caca_get_display_driver_list(void);
extern char end[];

/* C entry point, called from stage2 */
int kmain(void)
{
    struct processor_info processor_info;
    struct floppy_info    floppy_info;

    printf("_start at 0x%x\n", _start);
    printf("kmain() at 0x%x\n", kmain);
    printf("Types : char[%d] short[%d] int[%d] unsigned long long[%d]\n", sizeof(char), sizeof(short), sizeof(int), sizeof(unsigned long long));

    enable_interrupt(1);  // Enable Keyboard Interrupt (IRQ1)
    enable_interrupt(0);  // Enable IRQ0 (timer)
    enable_interrupt(13);
    timer_phase(100);     // Fire IRQ0 each 1/100s


    processor_get_info(&processor_info);
    processor_print_info(&processor_info);

    floppy_get_info(&floppy_info);
    floppy_print_info(&floppy_info);

    /* Caca is delicious */
    printf("Filling memory with 0xCACA, starting from 0x%x\n", end);

    char *ptr = end;
    while (1)
    {
        *ptr = 0xCA;
        *ptr++;
    }
}

char end[];

#endif /* __KERNEL__ */
