/* 
 *  libcaca      
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2006 Sam Hocevar <sam@zoy.org>
 *                2006-2009 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/* 
 *  This file contains replacement functions for the standard C library
 *  that must be used when building libcaca and libcaca into a kernel.
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

/* C entry point, called from stage2 */
int kmain(void)
{
    struct processor_info processor_info;
    struct floppy_info    floppy_info;
    
    printf("_start at 0x%x\n", _start);
    printf("kmain() at 0x%x\n", kmain);
    
    processor_get_info(&processor_info);
    processor_print_info(&processor_info);

    floppy_get_info(&floppy_info);
    floppy_print_info(&floppy_info);

    enable_interrupt(1);  // Enable Keyboard Interrupt (IRQ1)
    enable_interrupt(0);  // Enable IRQ0 (timer)
    enable_interrupt(13);
    timer_phase(100);     // Fire IRQ0 each 1/100s
    
  
    printf("Waiting 1s\n");
    sleep(1);
    printf("Waiting 2s\n");
    sleep(2);
    printf("Waiting 3s\n");
    sleep(3);
    printf("Ok\n");
    //caca_get_display_driver_list();
    
    
    while (1)
    {
        
    }
}


#endif /* __KERNEL__ */
