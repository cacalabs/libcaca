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
#include "floppy.h"


int floppy_get_info(struct floppy_info *floppy_info)
{
    outb(0x70, 0x10);
    unsigned char c = inb(0x71);
    int a = c >> 4;
    int b = c & 0xF;

    char *drive_type[6] = {
        "none",
        "360kb 5.25in",
        "1.2mb 5.25in",
        "720kb 3.5in",
        "1.44mb 3.5in",
        "2.88mb 3.5in"
    };


    memcpy(floppy_info->drive[0].type, drive_type[a],
           strlen(drive_type[a]) + 1);
    memcpy(floppy_info->drive[1].type, drive_type[b],
           strlen(drive_type[b]) + 1);

    floppy_info->count = 0;
    if (a != 0)
        floppy_info->count++;
    if (b != 0)
        floppy_info->count++;

    return 0;
}

void floppy_print_info(struct floppy_info *floppy_info)
{
    printf("%d floppy drive(s)\n", floppy_info->count);
    if (floppy_info->count)
    {
        printf("Floppy %d type %s\n", 0, floppy_info->drive[0].type);
        if (floppy_info->count == 2)
        {
            printf("Floppy %d type %s\n", 1, floppy_info->drive[1].type);
        }
    }

}

int floppy_get_status(void)
{
    unsigned char c = inb(0x1F7);
    return c;
}
