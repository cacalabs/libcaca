/*
 *  libcucul      Unicode canvas library
 *  libcaca       ASCII-Art library
 *  Copyright (c) 2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/** \file kernel.c
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief Kernel features
 *
 *  This file contains replacement functions for the standard C library
 *  that must be used when building libcucul and libcaca into a kernel.
 */

#include "config.h"

#ifdef __KERNEL__

/* Our memory mapping */
static uint32_t *freemem = (uint32_t*) 0x00200000;

/* Multiboot kernel entry point */
void cmain(unsigned long int magic, unsigned long int addr)
{
    static char const text[] = "Booting libcaca kernel...";
    char const *ptr = text;
    char *video = (char*)0xB8000;

    char *argv[] = { NULL };
    int argc = 0;

    /* Print a text message to say hello */
    while(*ptr)
        *video = *ptr++; video += 2;

    /* Launch the main program */
    main(argc, argv);
}

/* stdlib.h functions */
void *malloc(size_t size)
{
    uint32_t *p = freemem;
    size = (size + 0x7) / 4;
    *p = size;
    freemem += size + 1;
    return p + 1;
}

void free(void *ptr)
{
    return;
}

void *realloc(void *ptr, size_t size)
{
    uint32_t oldsize = ((uint32_t *)ptr)[-1];
    void *p;
    if(oldsize >= size)
        return ptr;

    p = malloc(size);
    memcpy(p, ptr, oldsize);
    return p;
}

char *getenv(const char *name)
{
    return NULL;
}

int rand(void)
{
    static int seed = 0x68743284;
    seed = (seed * 0x7f32ba17) ^ 0xf893a735;
    return seed % RAND_MAX;
}

int abs(int j)
{
    if(j < 0)
        return -j;
    return j;
}

void exit(int status)
{
    /* FIXME: reboot? */
    while(1);
}

/* string.h functions */
void *memset(void *s, int c, size_t n)
{
    uint8_t *ptr = s;

    while(n--)
        *ptr++ = c;

    return s;
}

void *memcpy(void *dest, const void *src, size_t n)
{
    uint8_t *destptr = dest;
    uint8_t const *srcptr = src;

    while(n--)
        *destptr++ = *srcptr++;

    return dest;
}

size_t strlen(const char *s)
{
    int len = 0;

    while(*s++)
        len++;

    return len;
}

int strcasecmp(const char *s1, const char *s2)
{
    /* FIXME */
    return -1;
}

/* stdarg.h functions */
int vsnprintf(char *str, size_t size, const char *format, va_list ap)
{
    /* FIXME */
    return 0;
}

/* stdio.h functions */
FILE *fopen(const char *path, const char *mode)
{
    /* FIXME */
    return NULL;
}

int feof(FILE *stream)
{
    /* FIXME */
    return 0;
}

char *fgets(char *s, int size, FILE *stream)
{
    /* FIXME */
    return NULL;
}

int fclose(FILE *fp)
{
    /* FIXME */
    return 0;
}

int printf(const char *format, ...)
{
    /* FIXME */
    return 0;
}

int sprintf(char *str, const char *format, ...)
{
    /* FIXME */
    return 0;
}

int sscanf(const char *str, const char *format, ...)
{
    /* FIXME */
    return 0;
}

/* unistd.h functions */
void usleep(unsigned long usec)
{
    /* FIXME */
    return;
}

/* time.h functions */
int gettimeofday(struct timeval *tv, struct timezone *tz)
{
    static int usec = 0;
    static int sec = 0;

    /* FIXME */
    usec += 10000;
    if(usec > 1000000)
    {
        sec++;
        usec -= 1000000;
    }

    tv->tv_sec = sec;
    tv->tv_usec = usec;

    return 0;
}

#endif /* __KERNEL__ */
