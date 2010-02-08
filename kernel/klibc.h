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

/*
 *  This file contains replacement functions for the standard C library
 *  that must be used when building libcaca and libcaca into a kernel.
 */

#if !defined HAVE_KLIBC_H
#define HAVE_KLIBC_H

/* Various typedefs -- some are x86-specific */
#define CUSTOM_INTTYPES

#define outb(port,value) __asm__ __volatile__ ("outb %%al,%%dx"::"d" (port), "a" (value));
#define outbp(port,value) __asm __volatile__ ("outb %%al,%%dx; jmp 1f; 1:"::"d" (port), "a" (value));
#define inb(port) ({unsigned char _v; __asm__ __volatile__ ("inb %%dx,%%al" : "=a" (_v) : "d" (port)); _v;	})


/* Various defines */
#define NULL ((void *)0)
#define EOF (-1)
#define BUFSIZ 4096
#define RAND_MAX ((unsigned int)0x8000000)
#define INT_MAX ((int)0x7fffffff)
#define M_PI 3.14159265358979323846
#define __BYTE_ORDER 1
#define __BIG_ENDIAN 2

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

#ifndef size_t
typedef unsigned int size_t;
#endif
typedef struct file
{
    void *mem;
} FILE;

struct timeval
{
    int tv_sec;
    int tv_usec;
};

struct timezone
{
    int tz_minuteswest;
    int tz_dsttime;
};

/* stdlib.h functions */
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
char *getenv(const char *name);
int rand(void);
int abs(int j);
void exit(int status);
void srand(unsigned int s);
int atexit(void (*function) (void));
FILE *stdin, *stdout, *stderr;

/* string.h functions */
void *memset(void *s, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
size_t strlen(const char *s);
int strcmp(const char *s1, const char *s2);
int strcasecmp(const char *s1, const char *s2);
int memcmp(const void *s1, const void *s2, size_t n);
char *strdup(const char *s);
char *strchr(const char *s, int c);

/* stdarg.h functions */
typedef void *va_list;
#define va_start(v,a) v = (void *)((uintptr_t)(&a) + sizeof(a))
#define va_end(v)
int vsnprintf(char *str, size_t size, const char *format, va_list ap);
/* va_arg */
#define args_list char *
#define _arg_stack_size(type)    (((sizeof(type)-1)/sizeof(int)+1)*sizeof(int))
#define args_start(ap, fmt) do {    \
ap = (char *)((unsigned int)&fmt + _arg_stack_size(&fmt));   \
} while (0)
#define args_end(ap)
#define args_next(ap, type) (((type *)(ap+=_arg_stack_size(type)))[-1])

/* stdio.h functions */
FILE *fopen(const char *path, const char *mode);
int feof(FILE * stream);
char *fgets(char *s, int size, FILE * stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE * stream);
int fclose(FILE * fp);

int printf(const char *format, ...);

int fprintf(FILE * stream, const char *format, ...);
int fflush(FILE * stream);
int sprintf(char *str, const char *format, ...);
int sscanf(const char *str, const char *format, ...);
void itoa(int n, char s[]);

void clearscreen(void);

/* unistd.h functions */
void usleep(unsigned long usec);
void sleep(unsigned long sec);
int getpid(void);

/* time.h functions */
int gettimeofday(struct timeval *tv, struct timezone *tz);
int time(void *);

/* math.h functions */
double cos(double x);
double sin(double x);
double sqrt(double x);

/* errno.h functions */
#define ENOENT   2              /* No such file or directory */
#define ENOMEM  12              /* Out of memory */
#define EBUSY   16              /* Device or resource busy */
#define ENODEV  19              /* No such device */
#define EINVAL  22              /* Invalid argument */
#define ENOTTY  25              /* Not a typewriter */
#define ENOSYS  38              /* Function not implemented */
extern int errno;

/* arpa/inet.h functions */
unsigned int htonl(unsigned int hostlong);
unsigned short htons(unsigned short hostlong);

void print(char *str);

#endif /* HAVE_KLIBC_H */

