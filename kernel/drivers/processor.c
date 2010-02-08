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
#include "drivers/timer.h"
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

    processor_info->frequency = 0;
    processor_info->frequency = processor_get_frequency(processor_info);

    return 0;
}

u32 processor_get_frequency(struct processor_info * processor_info)
{
    if (processor_info->frequency)
        return processor_info->frequency;
    u64 srdtsc64, erdtsc64;
    u32 srdtsc_l, srdtsc_h;
    u32 erdtsc_l, erdtsc_h;

    rdtsc(srdtsc_l, srdtsc_h);  /* Get RDTSC */
    sleep(2);                   /* Sleep for 2 seconds */
    rdtsc(erdtsc_l, erdtsc_h);  /* Get RDTSC again */

    srdtsc64 = srdtsc_h;
    srdtsc64 <<= 32;
    srdtsc64 |= srdtsc_l;
    erdtsc64 = erdtsc_h;
    erdtsc64 <<= 32;
    erdtsc64 |= erdtsc_l;


    u32 diff = erdtsc64 - srdtsc64;     /* Cycle count for 2 seconds */
    diff /= 2;                  /* Divide by 2 to get cycles per sec */
    return diff;
}

void processor_print_info(struct processor_info *processor_info)
{
    printf("CPU%d\n", processor_info->id);
    printf("Vendor ID : %s\n", processor_info->vendor);
    printf("Frequency : ");
    if (processor_info->frequency > 1000000000)
    {
        printf("%dGhz",
               processor_info->frequency / 1000000000);
    }
    else if (processor_info->frequency > 1000000)
    {
        printf("%dMhz",
               processor_info->frequency / 1000000);
    }
    else if (processor_info->frequency > 1000)
    {
        printf("%dKhz\n",
               processor_info->frequency / 1000);
    }
    else
    {
        printf("%dhz\n",
               processor_info->frequency);
    }
    printf(" (or something like that)\n");
    printf("Features : 0x%x\n", processor_info->features);
}
