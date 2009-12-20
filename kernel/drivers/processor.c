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

#include "kernel.h"
#include "klibc.h"
#include "processor.h"


int processor_get_info(struct processor_info *processor_info)
{
    processor_info->id = 0;

    /* Vendor String */
    int code = CPUID_GETVENDORSTRING;
    unsigned int where[5];
    char *s;
    asm volatile ("cpuid":"=a" (*where), "=b"(*(where + 1)),
                  "=c"(*(where + 2)), "=d"(*(where + 3)):"0"(code));

    s = (char *)where;

    unsigned int a = 0;
    unsigned int b = where[1];
    unsigned int d = where[3];
    unsigned int c = where[2];

    where[0] = b;
    where[1] = d;
    where[2] = c;
    where[3] = 0;

    memcpy(processor_info->vendor, where, 13);

    /* Features */
    code = CPUID_GETFEATURES;
    asm volatile ("cpuid":"=a" (a), "=d"(d):"0"(code):"ecx", "ebx");
    processor_info->features = a;

    return 0;
}

void processor_print_info(struct processor_info *processor_info)
{
    printf("CPU%d\n", processor_info->id);
    printf("Vendor ID : %s\n", processor_info->vendor);
    printf("Features : 0x%x\n", processor_info->features);
}
