/*
 *  libcucul      Canvas for ultrafast compositing of Unicode letters
 *  Copyright (c) 2006-2007 Sam Hocevar <sam@zoy.org>
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
 *  This file contains functions for compressed file I/O.
 */

#include "config.h"
#include "common.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#   include <stdlib.h>
#   include <string.h>
#   if defined HAVE_ZLIB_H
#       include <zlib.h>
#       define READSIZE  128 /* Read buffer size */
#       define WRITESIZE 128 /* Inflate buffer size */
#   endif
#endif

#include "cucul.h"
#include "cucul_internals.h"

#if defined HAVE_ZLIB_H
static int zipread(cucul_file_t *, void *, unsigned int);
#endif

struct cucul_file
{
#if defined HAVE_ZLIB_H
    unsigned char read_buffer[READSIZE];
    z_stream stream;
    gzFile gz;
    int eof, zip;
#endif
    FILE *f;
};

cucul_file_t *_cucul_file_open(char const *path, const char *mode)
{
    cucul_file_t *fp = malloc(sizeof(*fp));

#if defined HAVE_ZLIB_H
    uint8_t buf[4];
    unsigned int skip_size = 0;

    fp->gz = gzopen(path, "rb");
    if(!fp->gz)
    {
        free(fp);
        return NULL;
    }

    fp->eof = 0;
    fp->zip = 0;

    /* Parse ZIP file and go to start of first file */
    gzread(fp->gz, buf, 4);
    if(memcmp(buf, "PK\3\4", 4))
    {
        gzseek(fp->gz, 0, SEEK_SET);
        return fp;
    }

    fp->zip = 1;

    gzseek(fp->gz, 22, SEEK_CUR);

    gzread(fp->gz, buf, 2); /* Filename size */
    skip_size += (uint16_t)buf[0] | ((uint16_t)buf[1] << 8);
    gzread(fp->gz, buf, 2); /* Extra field size */
    skip_size += (uint16_t)buf[0] | ((uint16_t)buf[1] << 8);

    gzseek(fp->gz, skip_size, SEEK_CUR);

    /* Initialise inflate stream */
    fp->stream.total_out = 0;
    fp->stream.zalloc = NULL;
    fp->stream.zfree = NULL;
    fp->stream.opaque = NULL;
    fp->stream.next_in = NULL;
    fp->stream.avail_in = 0;

    if(inflateInit2(&fp->stream, -MAX_WBITS))
    {
        free(fp);
        gzclose(fp->gz);
        return NULL;
    }
#else
    fp->f = fopen(path, mode);

    if(!fp->f)
    {
        free(fp);
        return NULL;
    }
#endif

    return fp;
}

int _cucul_file_close(cucul_file_t *fp)
{
#if defined HAVE_ZLIB_H
    gzFile gz = fp->gz;
    if(fp->zip)
        inflateEnd(&fp->stream);
    free(fp);
    return gzclose(gz);
#else
    FILE *f = fp->f;
    free(fp);
    return fclose(f);
#endif
}

int _cucul_file_eof(cucul_file_t *fp)
{
#if defined HAVE_ZLIB_H
    return fp->zip ? fp->eof : gzeof(fp->gz);
#else
    return feof(fp->f);
#endif
}

char *_cucul_file_gets(char *s, int size, cucul_file_t *fp)
{
#if defined HAVE_ZLIB_H
    if(fp->zip)
    {
        int i;

        for(i = 0; i < size; i++)
        {
            int ret = zipread(fp, s + i, 1);

            if(ret < 0)
                return NULL;

            if(ret == 0 || s[i] == '\n')
            {
                if(i + 1 < size)
                    s[i + 1] = '\0';
                return s;
            }
        }

        return s;
    }

    return gzgets(fp->gz, s, size);
#else
    return fgets(s, size, fp->f);
#endif
}

#if defined HAVE_ZLIB_H
static int zipread(cucul_file_t *fp, void *buf, unsigned int len)
{
    unsigned int total_read = 0;

    if(len == 0)
        return 0;

    fp->stream.next_out = buf;
    fp->stream.avail_out = len;

    while(fp->stream.avail_out > 0)
    {
        unsigned int tmp;
        int ret = 0;

        if(fp->stream.avail_in == 0 && !gzeof(fp->gz))
        {
            int bytes_read;

            bytes_read = gzread(fp->gz, fp->read_buffer, READSIZE);
            if(bytes_read < 0)
                return -1;

            fp->stream.next_in = fp->read_buffer;
            fp->stream.avail_in = bytes_read;
        }

        tmp = fp->stream.total_out;
        ret = inflate(&fp->stream, Z_SYNC_FLUSH);
        total_read += fp->stream.total_out - tmp;

        if(ret == Z_STREAM_END)
        {
            fp->eof = 1;
            return total_read;
        }

        if(ret != Z_OK)
            return ret;
    }

    return total_read;
}
#endif

