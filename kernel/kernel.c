/*
 *  libcucul      Canvas for ultrafast compositing of Unicode letters
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2006 Sam Hocevar <sam@zoy.org>
 *                2006 Jean-Yves Lamoureux <jylam@lnxscene.org>
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
 *  that must be used when building libcucul and libcaca into a kernel.
 */

#include "config.h"

#include "cucul_types.h"

#ifdef __KERNEL__

#define IS_DIGIT(x) (x>='0' && x<='9')
#define IS_ALPHA(x) (x>='A' && x<='z')
#define IS_UPPER(x) (x>='A' && x<='Z')
#define IS_LOWER(x) (x>='a' && x<='z')
#define UPPER(x) (IS_LOWER(x)?(x+('A'-'a')):x)
#define LOWER(x) (IS_UPPER(x)?(x-('a'-'A')):x)

/* Our default seed for random number generator */
static int seed = 0x68743284;

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
    if(!size)
        return NULL;
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
    uint32_t oldsize;
    void *p;

    if(!size)
        return NULL;

    if(!ptr)
        oldsize = 0;
    else
    {
        oldsize = ((uint32_t *)ptr)[-1];
        if(oldsize >= size)
            return ptr;
    }

    p = malloc(size);
    memcpy(p, ptr, oldsize);
    return p;
}

char *getenv(const char *name)
{
    return NULL;
}

int getpid(void)
{
    return 0x1337;
}

void srand(unsigned int s)
{
    seed = rand();
}

int time(void *dummy)
{
    return rand();
}

int rand(void)
{
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

int strcmp(const char *s1, const char *s2)
{
    while(*s1 && *s1 == *s2)
    {
        s1++;
        s2++;
    }

    return (int)*s1 - (int)*s2;
}

int strcasecmp(const char *s1, const char *s2)
{
    while(*s1 && *s2 && UPPER(*s1) == UPPER(*s2))
    {
        s1++;
        s2++;
    }

    return (int)UPPER(*s1) - (int)UPPER(*s2);
}

int memcmp(const void *_s1, const void *_s2, size_t n)
{
    unsigned char const *s1 = _s1, *s2 = _s2;

    while(n--)
    {
        if(*s1 != *s2)
            return (int)*s1 - (int)*s2;
        s1++;
        s2++;
    }
    return 0;
}

char *strdup(const char *s)
{
    char *new;
    unsigned int len = strlen(s);

    new = malloc(len + 1);
    memcpy(new, s, len + 1);

    return new;
}

char *strchr(const char *s, int c)
{
    do
        if(*s == c)
            return (char *)(intptr_t)s;
    while(*s++);

    return NULL;
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

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    return 0;
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

int fprintf(FILE *stream, const char *format, ...)
{
    /* FIXME */
    return 0;
}

int fflush(FILE *stream)
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

/* math.h functions */
double cos(double x)
{
    double ret = 0.0;
#ifdef HAVE_FSIN_FCOS
    asm volatile("fcos" : "=t" (ret) : "0" (x));
#else
    double x2;
    double num = 1.0;
    double fact = 1.0;
    int i;

    x = x - ((double)(int)(x / (2 * M_PI))) * (2 * M_PI);
    x2 = x * x;

    /* cos(x) = 1/0! - x^2/2! + x^4/4! - x^6/6! ... */
    for(i = 0; i < 10; i++)
    {
        ret += num / fact;
        num *= - x2;
        fact *= (2 * i + 1) * (2 * i + 2);
    }
#endif
    return ret;
}

double sin(double x)
{
    double ret = 0.0;
#ifdef HAVE_FSIN_FCOS
    asm volatile("fsin" : "=t" (ret) : "0" (x));
#else
    double x2;
    double num;
    double fact = 1.0;
    int i;

    x = x - ((double)(int)(x / (2 * M_PI))) * (2 * M_PI);
    x2 = x * x;
    num = x;

    /* sin(x) = x/1! - x^3/3! + x^5/5! - x^7/7! ... */
    for(i = 0; i < 10; i++)
    {
        ret += num / fact;
        num *= - x2;
        fact *= (2 * i + 2) * (2 * i + 3);
    }
#endif
    return ret;
}

double sqrt(double x)
{
    double ret = x;
    int i;

    /* This is Newton's method */
    for(i = 0; i < 10; i++)
        ret = (ret * ret + x) / (ret * 2.0);

    return ret;
}

/* errno.h stuff */
int errno = 0;

/* arpa/inet.h functions */

/* XXX FIXME Converts only from little endian to big endian (x86) */
unsigned int htonl(unsigned int hostlong)
{
    return ((hostlong&0xFFFF0000)>>16)|((hostlong&0x0000FFFFFF)<<16);
}

/* XXX FIXME Converts only from little endian to big endian (x86) */
unsigned short htons(unsigned short hostlong)
{
    return ((hostlong&0xFF00)>>8)|((hostlong&0x00FF)<<8);
}

#endif /* __KERNEL__ */
