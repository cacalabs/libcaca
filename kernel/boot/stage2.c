#include "kernel.h"
#include "klibc.h"

extern int kmain(void);


/* Entry point 
 * bootsect.asm loaded this file at 0x0100:0x0, which is mapped
 * at 0x8:0x1000 (selector+8bytes, offset 1000 (0x100 + 0x0)
 */

/* 0x1000 */
void _start(void) {

   	clearscreen();
    
    init_gdt();
    print("Loading IDT\n");
    init_idt();
    print("Loading PIC\n");
    init_pic();
    print("Running kmain()\n");
    sti;
	kmain(); /* Call kernel's kmain() */
    
    while(1) { /* Never return */
		print("hlt;\n");
    }
}

