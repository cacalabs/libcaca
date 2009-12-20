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

#include "config.h"
#include "caca_types.h"

#include "klibc.h"
#include "drivers/timer.h"
#include "kernel.h"


void htoa(unsigned int value, char s[]);

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
int kX = 0;
int kY = 0;

void scroll(void)
{
  	unsigned char* video, *tmp;
	
	for(video=(unsigned char*)0xB8000 ; video<(unsigned char*)0xB8FA0 ; video++){
        tmp = (unsigned char*) (video+1*160);
        
        if(tmp<(unsigned char*)0xB8FA0)
            *video = *tmp;
        else
            *video = 0;
	}
    
	kY-=1;
	if(kY<0) 
		kY=0;}

void putcar(unsigned char c)
{
	unsigned char* video;
    
	if(c==10){	
		kX=0;
		kY++;
	}
	else{
		video = (unsigned char*) (0xB8000+2*kX+160*kY);
		*video = c;
		*(video+1) = 0x07;
        
		kX++;
		if(kX>79){
			kX = 0;
			kY++;
		}
        if(kY >= 24) {
            scroll();
        }
    }
}

void print(char *str)
{
    char const *ptr = str;
    while(*ptr) {
       	putcar(*ptr++);
    }
}

void clearscreen(void)
{
    int x, y;
    kX = 0;
    kY = 0;
 	for(y = 0; y < 25; y++)
        for(x = 0; x < 80; x++) {
         	putcar(' ');   
        }
    kX = 0;
    kY = 0;        
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

int atexit(void (*function)(void))
{
    /* FIXME: register function */
    return 0;
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

void *memmove(void *dest, const void *src, size_t n)
{
    memcpy(freemem, src, n);
    memcpy(dest, freemem, n);
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
    uint8_t const *s1 = _s1, *s2 = _s2;
    
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

int printf(const char *fmt, ...)
{
    char str[200];
    char tmp[100];
    args_list args;
    args_start(args, fmt);
    
    char *s;
    int ptr = 0;
    int i = 0;
    
    for (; fmt[i]; ++i) {
        if ((fmt[i]!='%') && (fmt[i]!='\\')) {
            str[ptr++] = fmt[i];
            continue;
        } else if (fmt[i] == '\\') {
            switch (fmt[++i]) {
                case 'a': str[ptr++] = '\a'; break;
                case 'b': str[ptr++] = '\b'; break;
                case 't': str[ptr++] = '\t'; break;
                case 'n': str[ptr++] = '\n'; break;
                case 'r': str[ptr++] = '\r'; break;
                case '\\':str[ptr++] = '\\'; break;
            }
            continue;
        }
        
        switch (fmt[++i]) {
            case 's':
                s = (char *)args_next(args, char *);
                while (*s)
                    str[ptr++] = *s++;
                break;
            case 'c':
                str[ptr++] = (char)args_next(args, int);
                break;
            case 'p':
            case 'x':
                htoa((unsigned long)args_next(args, unsigned long), tmp);
                memcpy(&str[ptr], tmp, strlen(tmp));
                ptr+=strlen(tmp);
                break;
            case 'd':
                itoa((unsigned long)args_next(args, unsigned long), tmp);
                memcpy(&str[ptr], tmp, strlen(tmp));
                ptr+=strlen(tmp);
                break;
            case '%':
                str[ptr++] = '%';
                break;
            default:
                str[ptr++] = fmt[i];
                break;
        }
    }
    
    str[ptr] = '\0';
    args_end(args);
    
    print(str);
    
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

int sprintf(char *str, const char *fmt, ...)
{
    char tmp[100];
    args_list args;
    args_start(args, fmt);
    
    char *s;
    int ptr = 0;
    int i = 0;
    
    for (; fmt[i]; ++i) {
        if ((fmt[i]!='%') && (fmt[i]!='\\')) {
            str[ptr++] = fmt[i];
            continue;
        } else if (fmt[i] == '\\') {
            switch (fmt[++i]) {
                case 'a': str[ptr++] = '\a'; break;
                case 'b': str[ptr++] = '\b'; break;
                case 't': str[ptr++] = '\t'; break;
                case 'n': str[ptr++] = '\n'; break;
                case 'r': str[ptr++] = '\r'; break;
                case '\\':str[ptr++] = '\\'; break;
            }
            continue;
        }
    
        switch (fmt[++i]) {
            case 's':
                s = (char *)args_next(args, char *);
                while (*s)
                    str[ptr++] = *s++;
                break;
            case 'c':
                str[ptr++] = (char)args_next(args, int);
                break;
            case 'p':
            case 'x':
                htoa((unsigned long)args_next(args, unsigned long), tmp);
                memcpy(&str[ptr], tmp, strlen(tmp));
                ptr+=strlen(tmp);
                break;
            case 'd':
                itoa((unsigned long)args_next(args, unsigned long), tmp);
                memcpy(&str[ptr], tmp, strlen(tmp));
                ptr+=strlen(tmp);
                break;
            case '%':
                str[ptr++] = '%';
                break;
            default:
                str[ptr++] = fmt[i];
                break;
        }
    }

    str[ptr] = '\0';
    args_end(args);

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
   	u32 start = ticks;
    signed int diff  = 0;
    
    while(1) {
        diff = (signed int)(ticks - start);
        if(diff >= (signed int)(usec/20)) break;
    }
}

void sleep(unsigned long sec)	
{
	usleep(sec*1000);	
}

/* time.h functions */
  u64 rdtsc() {
    u64 x;
    __asm__ volatile ("rdtsc" : "=A" (x));
    return x;
}

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


/* reverse:  reverse string s in place */
void reverse(char s[])
{
    int i, j;
    char c;
    
    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}


/* itoa implementation, by Kernighan and Ritchie's The C Programming Language */
void itoa(int n, char s[])
{
    int i, sign;
    
    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
} 

void htoa(unsigned int value, char s[]) {
    int i = 8;
    int ptr = 0;
    while (i-- > 0) {
        s[ptr++] = "0123456789abcdef"[(value>>(i*4))&0xf];
    }
    s[ptr] = 0;
}


/* errno.h stuff */
int errno = 0;