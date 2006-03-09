/*
 *  libcaca       ASCII-Art library
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/** \file driver_network.c
 *  \version \$Id: driver_gl.c 330 2006-03-07 09:17:35Z sam $
 *  \author Jean-Yves Lamoureux <jylam@lnxscene.org>
 *  \brief Network driver
 *
 *  This file contains the libcaca network input and output driver
 */

#include "config.h"

#if defined(USE_NETWORK)
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#if defined(HAVE_UNISTD_H)
#   include <unistd.h>
#endif
#include <stdarg.h>

#include "caca.h"
#include "caca_internals.h"
#include "cucul.h"
#include "cucul_internals.h"



struct driver_private
{ 
    unsigned int width, height;
    unsigned int port;
    int sockfd, new_fd;
    struct sockaddr_in my_addr;
    struct sockaddr_in remote_addr;
    socklen_t sin_size;
    int clilen;
    char buffer[256];
};



#define BACKLOG 1337    /* Number of pending connections */



/* Following vars are static */
static char codes[] = {0xff, 0xfb, 0x01,  // WILL ECHO                                                              
                       0xff, 0xfb, 0x03,  // WILL SUPPRESS GO AHEAD
                       0xff, 253, 31,     // DO NAWS
                       0xff, 254, 31,     // DON'T NAWS
                       0xff, 31, 250, 0, 30, 0, 0xFF, // to be replaced 
                       0xff, 240};


static int network_init_graphics(caca_t *kk)
{
    int yes=1;
    printf("Initing network stack.\n");

    kk->drv.p = malloc(sizeof(struct driver_private));


    kk->drv.p->width = 80;
    kk->drv.p->height = 24;
    kk->drv.p->port = 7575; // 75 75 decimal ASCII -> KK   // FIXME, sadly


    cucul_set_size(kk->qq, kk->drv.p->width, kk->drv.p->height);


    printf("socket\n");
    if ((kk->drv.p->sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return -1;
    }
    printf("setsockopt\n");
    if (setsockopt(kk->drv.p->sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
        perror("setsockopt");
        return -1;
    }


    kk->drv.p->my_addr.sin_family = AF_INET;
    kk->drv.p-> my_addr.sin_port = htons(kk->drv.p->port); 
    kk->drv.p->my_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(kk->drv.p->my_addr.sin_zero), '\0', 8);

    printf("bind\n");
    if (bind(kk->drv.p->sockfd, (struct sockaddr *)&kk->drv.p->my_addr, sizeof(struct sockaddr))
                                                                   == -1) {
        perror("bind");
        return -1;
    }
    printf("listen\n");
    if (listen(kk->drv.p->sockfd, BACKLOG) == -1) {
        perror("listen");
        return -1;
    }

    printf("accept\n");
    kk->drv.p->clilen = sizeof(kk->drv.p->remote_addr);
    kk->drv.p->new_fd = accept(kk->drv.p->sockfd, (struct sockaddr *) &kk->drv.p->remote_addr, &kk->drv.p->clilen);
    if (kk->drv.p->new_fd < 0) {
        perror("ERROR on accept");
        return -1;
    }


    printf("Got connexion from %d.%d.%d.%d\n", 
           (unsigned int)((kk->drv.p->remote_addr.sin_addr.s_addr)&0x000000FF),
           (unsigned int)((kk->drv.p->remote_addr.sin_addr.s_addr)&0x0000FF00)>>8,
           (unsigned int)((kk->drv.p->remote_addr.sin_addr.s_addr)&0x00FF0000)>>16,
           (unsigned int)((kk->drv.p->remote_addr.sin_addr.s_addr)&0xFF000000)>>24);
    
    /* FIXME, handle >255 sizes */
    codes[16] = (unsigned char) kk->drv.p->width&0xff;
    codes[18] = (unsigned char) kk->drv.p->height&0xff;

    /* Send basic telnet codes */
    if (send(kk->drv.p->new_fd, codes,sizeof(codes) , 0) == -1) {
        perror("send");
        return -1;
    }

    printf("network ok.\n");

    return 0;
}

static int network_end_graphics(caca_t *kk)
{
    printf("network end graphics\n");
    return 0;
}

static int network_set_window_title(caca_t *kk, char const *title)
{
    printf("network_set_window_title(%s) not implemented yet.\n", title);
    return 0;
}

static unsigned int network_get_window_width(caca_t *kk)
{
    return kk->drv.p->width * 6;
}

static unsigned int network_get_window_height(caca_t *kk)
{
    return kk->drv.p->height * 10;
}

static void network_display(caca_t *kk)
{
    int size;
    char *to_send = cucul_get_ansi(kk->qq, 0, &size);;
  
    
    /* ANSI code for move(0,0)*/
    if (send(kk->drv.p->new_fd, "\033[1,1H", 6, 0) == -1) {
        perror("send");
        return;
    }
    
    if (send(kk->drv.p->new_fd, to_send, size, 0) == -1) {
        perror("send");
        return;
    }

}
static void network_handle_resize(caca_t *kk)
{
    printf("Resize\n");
 
}

static unsigned int network_get_event(caca_t *kk)
{
    return 0;
}


/*
 * Driver initialisation
 */

void network_init_driver(caca_t *kk)
{
    kk->drv.driver = CACA_DRIVER_NETWORK;

    kk->drv.init_graphics = network_init_graphics;
    kk->drv.end_graphics = network_end_graphics;
    kk->drv.set_window_title = network_set_window_title;
    kk->drv.get_window_width = network_get_window_width;
    kk->drv.get_window_height = network_get_window_height;
    kk->drv.display = network_display;
    kk->drv.handle_resize = network_handle_resize;
    kk->drv.get_event = network_get_event;
}

#endif // USE_NETWORK
