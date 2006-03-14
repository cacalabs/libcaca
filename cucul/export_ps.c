/*
 *  libcucul      Unicode canvas library
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/** \file export.c
 *  \version \$Id: export_irc.c 384 2006-03-13 18:07:35Z sam $
 *  \author Sam Hocevar <sam@zoy.org>
 *  \author Jean-Yves Lamoureux <jylam@lnxscene.org>
 *  \brief Export function
 *
 *  This file contains export functions for Postscript files
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdlib.h>
#   include <stdio.h>
#   include <string.h>
#endif

#include "cucul.h"
#include "cucul_internals.h"


char const *ps_header = "%%! \n"
		        "%%%% libcaca PDF export      \n"
		        "%%%%LanguageLevel: 2 \n"
		        "%%%%Pages: 1 \n"
		        "%%%%DocumentData: Clean7Bit \n"
		        "/csquare { \n"
		        "        newpath \n"
		        "        0 0 moveto \n"
		        "        0 1 rlineto \n"
		        "        1 0 rlineto \n"
		        "        0 -1 rlineto \n"
		        "        closepath \n"
		        "        setrgbcolor \n"
		        "        fill \n"
		        "} def \n"
		        "/S { \n"
		        "  Show \n"
		        "} bind def \n"
		        "/Courier-Bold findfont \n"
		        "8 scalefont \n"
		        "setfont\n"
		        "gsave \n"
		        "6 10 scale \n";


/** \brief Generate Postscript representation of current image.
 *
 *  This function generates and returns a Postscript representation of
 *  the current image.
 */
char* cucul_get_ps(cucul_t *qq, int *size)
{
    char *cur;
    int x, y;

    static float const paletteR[] =
	{
	    0,   0,   0,   0,
	    0.5, 0.5, 0.5, 0.5,
	    0.5, 0.5, 0.5, 0.5,
	    1.0, 1.0, 1.0, 1.0,
	};
    static float const paletteG[] =
	{
	    0, 0, 0.5, 0.5,
	    0, 0, 0.5, 0.5,
	    0, 0, 1.0, 1.0,
	    0, 0, 1.0, 1.0,
	};
    static float const paletteB[] =
	{
	    0, 0.5, 0,   0.5,
	    0, 0.5, 0,   0.5,
	    0, 1.0, 0.5, 1.0,
	    0, 1.0, 0,   1.0,
	};


    if(qq->ps_buffer)
        free(qq->ps_buffer);

    /* 200 is arbitrary but should be ok */
    qq->ps_buffer = malloc((strlen(ps_header) + (qq->height*qq->width)*200) * sizeof(char));
    cur = qq->ps_buffer;

    /* Header */
    cur += sprintf(cur, "%s", ps_header);


    /* Background, drawn using csquare macro defined in header */
    for(y=(int)(qq->height-1);y>=0; y--) {
        uint8_t *lineattr = qq->attr + y * qq->width;

        for(x = 0; x < (int)qq->width; x++) {
	    float bgR = paletteR[lineattr[x] >> 4];
	    float bgG = paletteG[lineattr[x] >> 4];
	    float bgB = paletteB[lineattr[x] >> 4];


	    cur += sprintf(cur, "1 0 translate \n %f %f %f csquare\n", bgR, bgG, bgB);
  	}

	/* Return to beginning of the line, and jump to the next one */
	cur += sprintf(cur, "%d 1 translate \n", -qq->width);


    }

    cur += sprintf(cur, "grestore\n");  // Restore normal transformation matrix
    for(y=(int)(qq->height-1);y>=0; y--) {
        uint8_t  *lineattr = qq->attr + y * qq->width;
        uint32_t *linechar = qq->chars + y * qq->width;

        for(x = 0; x < (int)qq->width; x++) {
	    uint32_t cR = linechar[x];
	    float fgR = paletteR[lineattr[x] & 0x0f];
	    float fgG = paletteG[lineattr[x] & 0x0f];
	    float fgB = paletteB[lineattr[x] & 0x0f];

	    cur += sprintf(cur, "newpath\n%d %d moveto\n", (x+1)*6, (y)*10);
	    cur += sprintf(cur, "%f %f %f setrgbcolor\n", fgR, fgG, fgB);
	    cur += sprintf(cur, "(%c) show\n", cR);

	}
    }


    cur += sprintf(cur, "showpage");

    /* Crop to really used size */
    *size = (strlen(qq->ps_buffer) + 1) * sizeof(char);

    qq->ps_buffer = realloc(qq->ps_buffer, *size);

    return qq->ps_buffer;

}



