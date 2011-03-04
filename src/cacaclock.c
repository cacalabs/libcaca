/*
 *  cacaclock     Text-mode clock display
 *  Copyright (c) 2011 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                All Rights Reserved
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <time.h>   
#   include <stdio.h>
#   include <string.h>
#   include <stdlib.h>
#endif

#include "caca.h"

#define CACACLOCKVERSION "0.1"


static char* get_date(void) {
	time_t  currtime;                                                    
	char *charTime = malloc(101);                                      

	time(&currtime);                                                     
	strftime(charTime, 100,"%R:%S",localtime(&currtime)); 

	return charTime;
}

int main(int argc, char *argv[]) {

	caca_canvas_t *cv;
	caca_canvas_t *figcv;
	caca_display_t *dp;
	uint32_t w, h;


	char *font = "/usr/share/figlet/mono12.tlf";
	if(argc==2) font = argv[1];


	cv = caca_create_canvas(0, 0);
	figcv = caca_create_canvas(0, 0);
	if(!cv || !figcv)
	{  
		fprintf(stderr, "%s: unable to initialise libcaca\n", argv[0]);
		return 1;
	}

	if(caca_canvas_set_figfont(figcv, font))
	{  
		fprintf(stderr, "Could not open font\n");
		return -1;
	}


	dp = caca_create_display(cv);
	if(!dp) {
		printf("Can't open window. CACA_DRIVER problem ?\n");
		return -1;
	}

	w = caca_get_canvas_width (cv);
	h = caca_get_canvas_height(cv);	

	for(;;) {
		caca_event_t ev;

		while(caca_get_event(dp, CACA_EVENT_KEY_PRESS
					| CACA_EVENT_QUIT, &ev, 1))
		{  
			if(caca_get_event_type(&ev))
				goto end;
		}
		char *d = get_date();
		uint32_t o = 0;

		// figfont API is not complete, and does not alloq us to put a string
		// at another position than 0,0
		// So, we have to create a canvas which will hold the figfont string,
		// then blit this canvas to the main one at the desired position.
		caca_clear_canvas(cv);
		caca_clear_canvas(figcv);
		while(d[o])
		{  
			caca_set_color_ansi(figcv, CACA_WHITE, CACA_TRANSPARENT);
			caca_put_figchar(figcv, d[o++]);
		}
		caca_flush_figlet (figcv);
		free(d);
		//TODO center etc.
		caca_blit(cv, 0, 0, figcv, NULL);
		caca_refresh_display(dp);
		usleep(250000);
	}
end:

	caca_free_canvas(figcv);
	caca_free_canvas(cv);
	caca_free_display(dp);

	return 0;
}
