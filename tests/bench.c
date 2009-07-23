#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "caca.h"


#define TIME(desc,code) \
{\
    time_t start, end;\
    start = time(NULL);\
    code ;\
    end = time(NULL);\
    printf("%-25s:%4ld\n", desc, end-start);\
}

static void blit(int mask, int clear)
{
    caca_canvas_t *cv, *cv2;
    int i;
    cv = caca_create_canvas(40, 40);
    cv2 = caca_create_canvas(16, 16);
    caca_fill_box(cv2, 0, 0, 16, 16, 'x');
    for (i=0; i<1000000; i++) {
      if(clear)
	      caca_clear_canvas(cv);
      caca_blit(cv, 1, 1, cv2, mask?cv2:NULL);
    }
    caca_free_canvas(cv);
    caca_free_canvas(cv2);
}

static void putchars(void)
{
    caca_canvas_t *cv;
    int i;
    cv = caca_create_canvas(40, 40);
    for (i=0; i<50000000; i++) {
      caca_put_char(cv, 1, 1, 'x');
      caca_put_char(cv, 1, 1, 'o');
    }
    caca_free_canvas(cv);
}

int main(int argc, char *argv[])
{
    TIME("blit no mask, no clear",blit(0, 0));
    TIME("blit no mask, clear",blit(0, 1));
    TIME("blit mask, no clear",blit(1, 0));
    TIME("blit mask, clear",blit(1, 1));
    TIME("putchars",putchars());
    return 0;
}

