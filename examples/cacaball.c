/*
 *   cacaball       Metaballs effect using libcaca
 *   Copyright (c) 2003-2004 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                 All Rights Reserved
 *
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *   02111-1307  USA
 */

#include "../config.h"

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#include "caca.h"

/* Virtual buffer size */
#define XSIZ 128
#define YSIZ 128

#define METASIZE 64
#define METACENTER (METASIZE/2)

static struct caca_bitmap *caca_bitmap;
static unsigned char *bitmap;
unsigned char MetaBoule[METASIZE*METASIZE];



void drawBalls(unsigned int bx, unsigned int by);
void cutBalls(unsigned int bx, unsigned int by);
void generateBalls(void);

/* Screen palette */
const static int pal[] =
{
  0, 0, 0, 0, 0, 6, 0, 0, 6, 0, 0, 7, 0, 0, 8, 0, 0, 8, 0, 0, 9, 0, 0, 10,
2, 0, 10, 4, 0, 9, 6, 0, 9, 8, 0, 8, 10, 0, 7, 12, 0, 7, 14, 0, 6, 16, 0, 5,
  18, 0, 5, 20, 0, 4, 22, 0, 4, 24, 0, 3, 26, 0, 2, 28, 0, 2, 30, 0, 1, 32, 0, 0,
  32, 0, 0, 33, 0, 0, 34, 0, 0, 35, 0, 0, 36, 0, 0, 36, 0, 0, 37, 0, 0, 38, 0, 0,
  39, 0, 0, 40, 0, 0, 40, 0, 0, 41, 0, 0, 42, 0, 0, 43, 0, 0, 44, 0, 0, 45, 0, 0,
  46, 1, 0, 47, 1, 0, 48, 2, 0, 49, 2, 0, 50, 3, 0, 51, 3, 0, 52, 4, 0, 53, 4, 0,
  54, 5, 0, 55, 5, 0, 56, 6, 0, 57, 6, 0, 58, 7, 0, 59, 7, 0, 60, 8, 0, 61, 8, 0,
  63, 9, 0, 63, 9, 0, 63, 10, 0, 63, 10, 0, 63, 11, 0, 63, 11, 0, 63, 12, 0, 63, 12, 0,
  63, 13, 0, 63, 13, 0, 63, 14, 0, 63, 14, 0, 63, 15, 0, 63, 15, 0, 63, 16, 0, 63, 16, 0,
  63, 17, 0, 63, 17, 0, 63, 18, 0, 63, 18, 0, 63, 19, 0, 63, 19, 0, 63, 20, 0, 63, 20, 0,
  63, 21, 0, 63, 21, 0, 63, 22, 0, 63, 22, 0, 63, 23, 0, 63, 24, 0, 63, 24, 0, 63, 25, 0,
  63, 25, 0, 63, 26, 0, 63, 26, 0, 63, 27, 0, 63, 27, 0, 63, 28, 0, 63, 28, 0, 63, 29, 0,
  63, 29, 0, 63, 30, 0, 63, 30, 0, 63, 31, 0, 63, 31, 0, 63, 32, 0, 63, 32, 0, 63, 33, 0,
  63, 33, 0, 63, 34, 0, 63, 34, 0, 63, 35, 0, 63, 35, 0, 63, 36, 0, 63, 36, 0, 63, 37, 0,
  63, 38, 0, 63, 38, 0, 63, 39, 0, 63, 39, 0, 63, 40, 0, 63, 40, 0, 63, 41, 0, 63, 41, 0,
  63, 42, 0, 63, 42, 0, 63, 43, 0, 63, 43, 0, 63, 44, 0, 63, 44, 0, 63, 45, 0, 63, 45, 0,
  63, 46, 0, 63, 46, 0, 63, 47, 0, 63, 47, 0, 63, 48, 0, 63, 48, 0, 63, 49, 0, 63, 49, 0,
  63, 50, 0, 63, 50, 0, 63, 51, 0, 63, 52, 0, 63, 52, 0, 63, 52, 0, 63, 52, 0, 63, 52, 0,
  63, 53, 0, 63, 53, 0, 63, 53, 0, 63, 53, 0, 63, 54, 0, 63, 54, 0, 63, 54, 0, 63, 54, 0,
  63, 54, 0, 63, 55, 0, 63, 55, 0, 63, 55, 0, 63, 55, 0, 63, 56, 0, 63, 56, 0, 63, 56, 0,
  63, 56, 0, 63, 57, 0, 63, 57, 0, 63, 57, 0, 63, 57, 0, 63, 57, 0, 63, 58, 0, 63, 58, 0,
  63, 58, 0, 63, 58, 0, 63, 59, 0, 63, 59, 0, 63, 59, 0, 63, 59, 0, 63, 60, 0, 63, 60, 0,
  63, 60, 0, 63, 60, 0, 63, 60, 0, 63, 61, 0, 63, 61, 0, 63, 61, 0, 63, 61, 0, 63, 62, 0,
  63, 62, 0, 63, 62, 0, 63, 62, 0, 63, 63, 0, 63, 63, 1, 63, 63, 2, 63, 63, 3, 63, 63, 4,
  63, 63, 5, 63, 63, 6, 63, 63, 7, 63, 63, 8, 63, 63, 9, 63, 63, 10, 63, 63, 10, 63, 63, 11,
  63, 63, 12, 63, 63, 13, 63, 63, 14, 63, 63, 15, 63, 63, 16, 63, 63, 17, 63, 63, 18, 63, 63, 19,
  63, 63, 20, 63, 63, 21, 63, 63, 21, 63, 63, 22, 63, 63, 23, 63, 63, 24, 63, 63, 25, 63, 63, 26,
  63, 63, 27, 63, 63, 28, 63, 63, 29, 63, 63, 30, 63, 63, 31, 63, 63, 31, 63, 63, 32, 63, 63, 33,
  63, 63, 34, 63, 63, 35, 63, 63, 36, 63, 63, 37, 63, 63, 38, 63, 63, 39, 63, 63, 40, 63, 63, 41,
  63, 63, 42, 63, 63, 42, 63, 63, 43, 63, 63, 44, 63, 63, 45, 63, 63, 46, 63, 63, 47, 63, 63, 48,
  63, 63, 49, 63, 63, 50, 63, 63, 51, 63, 63, 52, 63, 63, 52, 63, 63, 53, 63, 63, 54, 63, 63, 55,
  63, 63, 56, 63, 63, 57, 63, 63, 58, 63, 63, 59, 63, 63, 60, 63, 63, 61, 63, 63, 62, 63, 63, 63};


int main(int argc, char **argv)
{
  int quit = 0;
  float i=0, j=0, k=0;
  int p;
  unsigned int x1=0, yy1=0;
  unsigned int x2=20, y2=70;
  unsigned int x3=30, y3=20;
  

    int r[256], g[256], b[256], a[256];
  if(caca_init())
    return 1;
  
  caca_set_delay(40000);
  
  /* Make the palette eatable by libcaca */
  for (p = 0; p < 256; p++)
    {
      b[p] = pal[p * 3] * 64;
      g[p] = pal[p * 3 + 1] * 64;
      r[p] = pal[p * 3 + 2] * 64;
      a[p] = 0xfff;
    }
  
  /* Create the bitmap */
  caca_bitmap = caca_create_bitmap(8, XSIZ, YSIZ, XSIZ, 0, 0, 0, 0);

  /* Set the palette */
  caca_set_bitmap_palette(caca_bitmap, r, g, b, a);

  /* Our virtual buffer */
  bitmap = malloc(XSIZ * YSIZ * sizeof(char));
  
  
  /* Generate ball sprite */
  generateBalls();


  /* Go ! */
  while(!quit)
    {
      /* Get event and test it for a key pressure */
      int event = caca_get_event(CACA_EVENT_KEY_PRESS);
      
      if((event & CACA_EVENT_KEY_PRESS))
        {
	  quit = 1;
        }

      /* Silly paths for our balls */
      x1 = abs(sin(i)*(XSIZ-METASIZE));
      yy1 = abs(cos(j)*(YSIZ-METASIZE));

      x2 = abs(cos(j*2)*(XSIZ-METASIZE));
      y2 = abs(cos(i/2)*(YSIZ-METASIZE));

      x3 = abs(cos(k*2)*(XSIZ-METASIZE));
      y3 = abs(cos(i+j/2)*(YSIZ-METASIZE));


      /* Here is all the trick. Maybe if you're that 
	 clever you'll understand.
      */
      drawBalls(x1,x2);
      drawBalls(x2,y2);
      drawBalls(x3,y3);


      

      i+=0.01;
      j+=0.02;
      k+=0.03;

      /* Draw our virtual buffer to screen, letting libcaca resize it */
      caca_draw_bitmap(0, 0, caca_get_width() , caca_get_height(),
		       caca_bitmap, bitmap);
      caca_refresh();
      
      memset(bitmap, 0, XSIZ*YSIZ);
    }
  
  /* End, bye folks */
  caca_end();

  return 0;

}


/* You missed the trick ? */
void drawBalls(unsigned int bx, unsigned int by)
{
  unsigned int color;
  unsigned int i, e=0;
  unsigned int b = (by*XSIZ)+bx;

  for(i=0;i<METASIZE*METASIZE;i++){
      color =  bitmap[b] + MetaBoule[i];
      if(color>255) color = 255;
      bitmap[b] = color;
      if(e==METASIZE){e=0;b+=(XSIZ-(METASIZE));}
      b++;
      e++;
    }
}

/* 'Limit' the color to 100, to have the cropped effect */
void cutBalls(unsigned int bx, unsigned int by)
{
  unsigned int i, e=0;
  unsigned int b = (by*XSIZ)+bx;
  for(i=0;i<METASIZE*METASIZE;i++){
      if(bitmap[b]<100) bitmap[b] = 0;
      if(e==METASIZE){e=0;b+=(XSIZ-(METASIZE));}
      b++;
      e++;
    }
}



/* Generate ball sprite
   You should read the comments, I already wrote that before ...
 */
void generateBalls(void)
{
  int x,y,distance;
  for(y=0;y<METASIZE;y++)
    for(x=0;x<METASIZE;x++)
      {
	distance = sqrt(   ((METACENTER-x)*(METACENTER-x)) + ((METACENTER-y)*(METACENTER-y)));
	MetaBoule[x+y*METASIZE]= (255-(distance))*15;
	if(distance > 15)
	  MetaBoule[x+y*METASIZE] = 0;
      }
}
