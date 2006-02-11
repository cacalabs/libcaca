#!/usr/bin/python2.4

import caca
import math
from random import Random
from math import *


ret = caca.init()
print "caca.init() returned ", ret
print "Window size is ",caca.get_window_width(),"x",caca.get_window_height()
print "Buffer size is ",caca.get_width(),"x",caca.get_height()

caca.set_size(80,25)
print "Buffer size is now",caca.get_width(),"x",caca.get_height()


print "Feature name for CACA_DITHERING_FSTEIN is "+caca.get_feature_name(caca.CACA_DITHERING_FSTEIN)


sprite = int(caca.load_sprite("./caca.txt"))


rand = Random()

i = 0
while caca.get_event(caca.CACA_EVENT_KEY_PRESS) != caca.CACA_EVENT_KEY_PRESS|caca.CACA_KEY_ESCAPE:


    i=i+1;
    
    caca.clear();
    
    caca.set_color(caca.CACA_COLOR_YELLOW, caca.CACA_COLOR_BLACK);
    xo = caca.get_width() / 4;
    yo = caca.get_height() / 4 + 5 * sin(0.03*i);
    
    for j in range(0,16):
        xa = xo - (30 + sin(0.03*i) * 8) * sin(0.03*i + 3.1415*j/8);
        ya = yo + (15 + sin(0.03*i) * 4) * cos(0.03*i + 3.1415*j/8);
        caca.draw_thin_line(int(xo), int(yo), int(xa), int(ya));

    j = 15 + sin(0.03*i) * 8;
    caca.set_color(caca.CACA_COLOR_WHITE, caca.CACA_COLOR_BLACK);
    caca.fill_ellipse(int(xo), int(yo), int(j), int(j / 2), '#');
    caca.set_color(caca.CACA_COLOR_YELLOW, caca.CACA_COLOR_BLACK);
    caca.draw_ellipse(xo, yo, j, j / 2, '0');

    xo = caca.get_width() * 5 / 8;
    yo = 2;

    xa = caca.get_width() / 8 + sin(0.03*i) * 5;
    ya = caca.get_height() / 2 + cos(0.03*i) * 5;

    xb = caca.get_width() - 10 - cos(0.02*i) * 10;
    yb = caca.get_height() * 3 / 4 - 5 + sin(0.02*i) * 5;

    xc = caca.get_width() / 4 - sin(0.02*i) * 5;
    yc = caca.get_height() * 3 / 4 + cos(0.02*i) * 5;

    caca.set_color(caca.CACA_COLOR_GREEN, caca.CACA_COLOR_BLACK);
    caca.fill_triangle(xo, yo, xb, yb, xa, ya, '%');
    caca.set_color(caca.CACA_COLOR_YELLOW, caca.CACA_COLOR_BLACK);
    caca.draw_thin_triangle(xo, yo, xb, yb, xa, ya);

    caca.set_color(caca.CACA_COLOR_RED, caca.CACA_COLOR_BLACK);
    caca.fill_triangle(xa, ya, xb, yb, xc, yc, ' ');
    caca.set_color(caca.CACA_COLOR_YELLOW, caca.CACA_COLOR_BLACK);
    caca.draw_thin_triangle(xa, ya, xb, yb, xc, yc);

    caca.set_color(caca.CACA_COLOR_BLUE, caca.CACA_COLOR_BLACK);
    caca.fill_triangle(xo, yo, xb, yb, xc, yc, '%');
    caca.set_color(caca.CACA_COLOR_YELLOW, caca.CACA_COLOR_BLACK);
    caca.draw_thin_triangle(xo, yo, xb, yb, xc, yc);

    xa = 2;
    ya = 2;

    xb = caca.get_width() - 3;
    yb = caca.get_height() / 2;

    xc = caca.get_width() / 3;
    yc = caca.get_height() - 3;

    caca.set_color(caca.CACA_COLOR_CYAN, caca.CACA_COLOR_BLACK);
    caca.draw_thin_triangle(xa, ya, xb, yb, xc, yc);

    xo = caca.get_width() / 2 + cos(0.027*i) * caca.get_width() / 3;
    yo = caca.get_height() / 2 - sin(0.027*i) * caca.get_height() / 2;

    caca.draw_thin_line(xa, ya, xo, yo);
    caca.draw_thin_line(xb, yb, xo, yo);
    caca.draw_thin_line(xc, yc, xo, yo);

    caca.draw_sprite(xo, yo, sprite, 0);

    for j in range(i - 60, i):
        delta = (rand.random()*10)-5;
        caca.set_color(rand.random()*15, rand.random()*15);
        caca.putchar(caca.get_width() / 2
                    + cos(0.02*j) * (delta + caca.get_width() / 4),
                   caca.get_height() / 2
                    + sin(0.02*j) * (delta + caca.get_height() / 3),
                   ' ');
    caca.draw_sprite(caca.get_width() / 2 + cos(0.02*i) * caca.get_width() / 4,
                   caca.get_height() / 2 + sin(0.02*i) * caca.get_height() / 3,
                   int(sprite), int(0));


    
    caca.refresh();


caca.end()
