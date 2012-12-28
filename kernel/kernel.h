/*
 *  libcaca
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2006-2012 Sam Hocevar <sam@hocevar.net>
 *                2009-2010 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */




extern void init_gdt(void);
void init_pic(void);
void init_idt(void);
void putcar(unsigned char c);
void dump_gdt(void);

void disable_interrupt(char i);
void enable_interrupt(char i);

#define cli __asm__("cli" : : )
#define sti __asm__("sti" : : )

#define rdtsc(low, high) \
    __asm__ __volatile__("rdtsc" : "=a" (low), "=d" (high))

/* The application's entry point */
int main(int argc, char *argv[]);

