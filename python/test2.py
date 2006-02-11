#!/usr/bin/python2.3

import caca
import math
from random import Random
from math import *
import Numeric as N

ret = caca.init()

r = N.zeros(256)
g = N.zeros(256)
b = N.zeros(256)
a = N.zeros(256)

rand = Random()

# Our pixel array
pixels = N.zeros(32*32*4)
#pixels = pixelst.tolist()

for i in range(0,256):
    r[i] = i
    g[i] = i
    b[i] = i
    a[i] = 128

        
bitmap = caca.create_bitmap(32,32,32,32*4,0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000)
#caca.set_bitmap_palette(bitmap, r, g, b, a)


color = 0

while caca.get_event(caca.CACA_EVENT_KEY_PRESS) != caca.CACA_EVENT_KEY_PRESS|caca.CACA_KEY_ESCAPE:

    for y in range(0,32):
         for x in range(0,(32*4), 4):
            offset = x + y * (32*4)
            pixels[offset]   = rand.random()*256
            pixels[offset+1] = rand.random()*256
            pixels[offset+2] = rand.random()*256
            pixels[offset+3] = 128
            

        
    color = color + 1

    caca.draw_bitmap(0,0,caca.get_width() - 1, caca.get_height() - 1,
                     bitmap, pixels)
        
    caca.refresh();



caca.end();
