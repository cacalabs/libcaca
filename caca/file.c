/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2006-2012 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

/*
 *  This file contains functions for compressed file I/O.
 */

#include "config.h"

#if !defined __KERNEL__
#   include <stdio.h>
#   include <stdlib.h>
#   include <string.h>
#   if defined HAVE_ZLIB_H
#       include <zlib.h>
#       define READSIZE  128 /* Read buffer size */
#       define WRITESIZE 128 /* Inflate buffer size */
#   endif
#endif

#include "caca.h"
#include "caca_internals.h"

#if !defined __KERNEL__ && defined HAVE_ZLIB_H
static int zipread(caca_file_t *, void *, unsigned int);
#endif

#if !defined __KERNEL__
struct caca_file
{
#   if defined HAVE_ZLIB_H
    uint8_t read_buffer[READSIZE];
    z_stream stream;
    gzFile gz;
    int eof, zip, total;
#   endif
    FILE *f;
    int readonly;
};
#endif

/** \brief Open a file for reading or writing
 *
 *  Create a caca file handle for a file. If the file is zipped, it is
 *  decompressed on the fly.
 *
 *  If an error occurs, NULL is returned and \b errno is set accordingly:
 *  - \c ENOSTS Function not implemented.
 *  - \c EINVAL File not found or permission denied.
 *
 *  \param path The file path
 *  \param mode The file open mode
 *  \return A file handle to \e path.
 */
caca_file_t *caca_file_open(char const *path, const char *mode)
{
#if defined __KERNEL__
    seterrno(ENOSYS);
    return NULL;
#else
    caca_file_t *fp = malloc(sizeof(*fp));

    fp->readonly = !!strchr(mode, 'r');

#   if defined HAVE_ZLIB_H
    uint8_t buf[4];
    unsigned int skip_size = 0;

    fp->gz = gzopen(path, fp->readonly ? "rb" : "wb");
    if(!fp->gz)
    {
        free(fp);
        seterrno(EINVAL);
        return NULL;
    }

    fp->eof = 0;
    fp->zip = 0;
    fp->total = 0;

    if(fp->readonly)
    {
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
            gzclose(fp->gz);
            free(fp);
            seterrno(EINVAL);
            return NULL;
        }
    }
#   else
    fp->f = fopen(path, mode);

    if(!fp->f)
    {
        free(fp);
        seterrno(EINVAL);
        return NULL;
    }
#   endif

    return fp;
#endif
}

/** \brief Close a file handle
 *
 *  Close and destroy the resources associated with a caca file handle.
 *
 *  This function is a wrapper for fclose() or, if available, gzclose().
 *
 *  \param fp The file handle
 *  \return The return value of fclose() or gzclose().
 */
int caca_file_close(caca_file_t *fp)
{
#if defined __KERNEL__
    seterrno(ENOSYS);
    return 0;
#elif defined HAVE_ZLIB_H
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

/** \brief Return the position in a file handle
 *
 *  Return the file handle position, in bytes.
 *
 *  \param fp The file handle
 *  \return The current offset in the file handle.
 */
uint64_t caca_file_tell(caca_file_t *fp)
{
#if defined __KERNEL__
    seterrno(ENOSYS);
    return 0;
#elif defined HAVE_ZLIB_H
    if(fp->zip)
        return fp->total;
    return gztell(fp->gz);
#else
    return ftell(fp->f);
#endif
}

/** \brief Read data from a file handle
 *
 *  Read data from a file handle and copy them into the given buffer.
 *
 *  \param fp The file handle
 *  \param ptr The destination buffer
 *  \param size The number of bytes to read
 *  \return The number of bytes read
 */
size_t caca_file_read(caca_file_t *fp, void *ptr, size_t size)
{
#if defined __KERNEL__
    seterrno(ENOSYS);
    return 0;
#elif defined HAVE_ZLIB_H
    if(fp->zip)
        return zipread(fp, ptr, size);
    return gzread(fp->gz, ptr, size);
#else
    return fread(ptr, 1, size, fp->f);
#endif
}

/** \brief Write data to a file handle
 *
 *  Write the contents of the given buffer to the file handle.
 *
 *  \param fp The file handle
 *  \param ptr The source buffer
 *  \param size The number of bytes to write
 *  \return The number of bytes written
 */
size_t caca_file_write(caca_file_t *fp, const void *ptr, size_t size)
{
#if defined __KERNEL__
    seterrno(ENOSYS);
    return 0;
#else
    if(fp->readonly)
        return 0;

#   if defined HAVE_ZLIB_H
    if(fp->zip)
    {
        /* FIXME: zip files are not supported */
        seterrno(ENOSYS);
        return 0;
    }
    return gzwrite(fp->gz, ptr, size);
#   else
    return fwrite(ptr, 1, size, fp->f);
#   endif
#endif
}

/** \brief Read a line from a file handle
 *
 *  Read one line of data from a file handle, up to one less than the given
 *  number of bytes. A trailing zero is appended to the data.
 *
 *  \param fp The file handle
 *  \param s The destination buffer
 *  \param size The maximum number of bytes to read
 *  \return The number of bytes read, including the trailing zero
 */
char *caca_file_gets(caca_file_t *fp, char *s, int size)
{
#if defined __KERNEL__
    seterrno(ENOSYS);
    return NULL;
#elif defined HAVE_ZLIB_H
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

/** \brief Tell whether a file handle reached end of file
 *
 *  Return the end-of-file status of the file handle.
 *
 *  This function is a wrapper for feof() or, if available, gzeof().
 *
 *  \param fp The file handle
 *  \return 1 if EOF was reached, 0 otherwise
 */
int caca_file_eof(caca_file_t *fp)
{
#if defined __KERNEL__
    return 1;
#elif defined HAVE_ZLIB_H
    return fp->zip ? fp->eof : gzeof(fp->gz);
#else
    return feof(fp->f);
#endif
}

#if !defined __KERNEL__ && defined HAVE_ZLIB_H
static int zipread(caca_file_t *fp, void *buf, unsigned int len)
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
            fp->total += total_read;
            return total_read;
        }

        if(ret != Z_OK)
            return ret;
    }

    fp->total += total_read;
    return total_read;
}
#endif

/*
 * XXX: The following functions are aliases.
 */

cucul_file_t *cucul_file_open(char const *, const char *)
         CACA_ALIAS(caca_file_open);
int cucul_file_close(cucul_file_t *) CACA_ALIAS(caca_file_close);
uint64_t cucul_file_tell(cucul_file_t *) CACA_ALIAS(caca_file_tell);
size_t cucul_file_read(cucul_file_t *, void *, size_t)
         CACA_ALIAS(caca_file_read);
size_t cucul_file_write(cucul_file_t *, const void *, size_t)
         CACA_ALIAS(caca_file_write);
char * cucul_file_gets(cucul_file_t *, char *, int)
         CACA_ALIAS(caca_file_gets);
int cucul_file_eof(cucul_file_t *) CACA_ALIAS(caca_file_eof);

