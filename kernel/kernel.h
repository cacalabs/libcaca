/*
 *  libcucul      Canvas for ultrafast compositing of Unicode letters
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/** \file kernel.h
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief Kernel features
 *
 *  This file contains replacement functions for the standard C library
 *  that must be used when building libcucul and libcaca into a kernel.
 */

/* Various defines */
#define NULL ((void *)0)
#define BUFSIZ 4096
#define RAND_MAX ((unsigned int)0x8000000)
#define INT_MAX ((int)0x7fffffff)
#define M_PI 3.14159265358979323846
#define __BYTE_ORDER 1
#define __BIG_ENDIAN 2

/* Assembly code for outb and inb */
extern inline void outb(unsigned char val, unsigned short port);
extern inline unsigned char inb(unsigned short port);

extern inline void outb(unsigned char val, unsigned short port)
{
    __asm__ __volatile__ ("outb %b0,%w1" : : "a" (val), "Nd" (port));
}

extern inline unsigned char inb(unsigned short port)
{
    unsigned char tmp;
    __asm__ __volatile__ ("inb %w1,%0" : "=a" (tmp) : "Nd" (port));
    return tmp;
}

/* Various typedefs -- some are x86-specific */
#define CUSTOM_INTTYPES
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long int uint32_t;
typedef long int intptr_t;
typedef long unsigned int uintptr_t;

typedef unsigned int size_t;

typedef struct file
{
    void *mem;
} FILE;

struct timeval {
    int tv_sec;
    int tv_usec;
};

struct timezone {
    int tz_minuteswest;
    int tz_dsttime;
};

/* Multiboot kernel entry point */
void cmain(unsigned long int magic, unsigned long int addr);

/* The application's entry point */
int main(int argc, char *argv[]);

/* stdlib.h functions */
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
char *getenv(const char *name);
int rand(void);
int abs(int j);
void exit(int status);

/* string.h functions */
void *memset(void *s, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
size_t strlen(const char *s);
int strcasecmp(const char *s1, const char *s2);

/* stdarg.h functions */
typedef void * va_list;
#define va_start(v,a) v = (void *)((uintptr_t)(&a) + sizeof(a))
#define va_end(v)
int vsnprintf(char *str, size_t size, const char *format, va_list ap);

/* stdio.h functions */
FILE *fopen(const char *path, const char *mode);
int feof(FILE *stream);
char *fgets(char *s, int size, FILE *stream);
int fclose(FILE *fp);
int printf(const char *format, ...);
int sprintf(char *str, const char *format, ...);
int sscanf(const char *str, const char *format, ...);

/* unistd.h functions */
void usleep(unsigned long usec);

/* time.h functions */
int gettimeofday(struct timeval *tv, struct timezone *tz);

/* math.h functions */
double cos(double x);
double sin(double x);
double sqrt(double x);

