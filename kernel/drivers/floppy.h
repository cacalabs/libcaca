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

struct floppy_drive
{
    char type[16];
    u8 status;
};

struct floppy_info
{
    int count;
    struct floppy_drive drive[2];
};

int floppy_get_info(struct floppy_info *floppy_info);
void floppy_print_info(struct floppy_info *floppy_info);

int floppy_get_status(void);
