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
#include <fcntl.h>
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

    int client_count;
    int *fd_list;

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
    if(kk->drv.p == NULL)
        return -1;

    kk->drv.p->width = 80;
    kk->drv.p->height = 24;
    kk->drv.p->port = 7575; // 75 75 decimal ASCII -> KK   // FIXME, sadly
    kk->drv.p->client_count = 0;
    kk->drv.p->fd_list = NULL;



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

    /* Non blocking socket */
    fcntl(kk->drv.p->sockfd, F_SETFL, O_NONBLOCK);


    printf("listen\n");
    if (listen(kk->drv.p->sockfd, BACKLOG) == -1) {
        perror("listen");
        return -1;
    }

    printf("network ok.\n");

    return 0;
}

static int network_end_graphics(caca_t *kk)
{
    int i;
    for(i = 0; i < kk->drv.p->client_count; i++) {
        close(kk->drv.p->fd_list[i]);
    }
    return 0;
}

static int network_set_window_title(caca_t *kk, char const *title)
{
    /* Not handled (yet)*/
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
    int size, i;
    char *to_send = cucul_get_ansi(kk->qq, 0, &size);;
  
    kk->drv.p->clilen = sizeof(kk->drv.p->remote_addr);
    kk->drv.p->new_fd = accept(kk->drv.p->sockfd, (struct sockaddr *) &kk->drv.p->remote_addr, &kk->drv.p->clilen);
    if(kk->drv.p->new_fd != -1)
        {

            if(kk->drv.p->fd_list == NULL) {
                kk->drv.p->fd_list = malloc(sizeof(int));
                if(kk->drv.p->fd_list == NULL)
                    return;
                kk->drv.p->fd_list[kk->drv.p->client_count] = kk->drv.p->new_fd;
            } else {
                kk->drv.p->fd_list = realloc(kk->drv.p->fd_list, (kk->drv.p->client_count+1) * sizeof(int));
                kk->drv.p->fd_list[kk->drv.p->client_count] = kk->drv.p->new_fd;
            }

            kk->drv.p->client_count++;

        }

    for(i = 0; i < kk->drv.p->client_count; i++) {
        if(kk->drv.p->fd_list[i] == -1)
           continue;
        
        /* FIXME, handle >255 sizes */
            codes[16] = (unsigned char) kk->drv.p->width&0xff;
            codes[18] = (unsigned char) kk->drv.p->height&0xff;
            
            /* Send basic telnet codes */
            if (send(kk->drv.p->fd_list[i], codes,sizeof(codes) , 0) == -1) {
                kk->drv.p->fd_list[i] = -1;
             }
            
            /* ANSI code for move(0,0)*/
            if (send(kk->drv.p->fd_list[i], "\033[1,1H", 6, 0) == -1) {
                kk->drv.p->fd_list[i] = -1;
            }
            
            if (send(kk->drv.p->fd_list[i], to_send, size, 0) == -1) {
                kk->drv.p->fd_list[i] = -1;
            }
        }
    
}
static void network_handle_resize(caca_t *kk)
{
    /* Not handled */
}

static unsigned int network_get_event(caca_t *kk)
{
    /* Not handled */
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

