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
 *  \version \$Id$
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
#include <signal.h>
#include <errno.h>

#if defined(HAVE_UNISTD_H)
#   include <unistd.h>
#endif
#include <stdarg.h>

#include "caca.h"
#include "caca_internals.h"
#include "cucul.h"
#include "cucul_internals.h"

static void manage_connections(caca_t *kk);
static int send_data(caca_t *kk, int fd);
ssize_t nonblock_write(int fd, char *buf, size_t len);

struct driver_private
{
    unsigned int width, height;
    unsigned int port;
    int sockfd;
    struct sockaddr_in my_addr;
    struct sockaddr_in remote_addr;
    socklen_t sin_size;
    int clilen;

    char *buffer;
    int size;

    int client_count;
    int *fd_list;
};



#define BACKLOG 1337    /* Number of pending connections */



/* Following vars are static */
static char codes[] = {0xff, 0xfb, 0x01,  // WILL ECHO
                       0xff, 0xfb, 0x03,  // WILL SUPPRESS GO AHEAD
                       0xff, 253, 31,     // DO NAWS
                       0xff, 254, 31,     // DON'T NAWS
                       0xff, 31, 250, 0, 30, 0, 0xFF, // Set size, replaced in display
                       0xff, 240};


static int network_init_graphics(caca_t *kk)
{
    int yes=1;
    int net_port = 7575;
    char *network_port;


    kk->drv.p = malloc(sizeof(struct driver_private));
    if(kk->drv.p == NULL)
        return -1;


#if defined(HAVE_GETENV)
    network_port = getenv("CACA_NETWORK_PORT");
    if(network_port && *network_port) {
        net_port = atoi(network_port);
        if(!net_port)
            net_port = 7575;
    }
#endif


    kk->drv.p->width = 80;
    kk->drv.p->height = 23; // Avoid scrolling
    kk->drv.p->client_count = 0;
    kk->drv.p->fd_list = NULL;
    kk->drv.p->port = net_port;


    _cucul_set_size(kk->qq, kk->drv.p->width, kk->drv.p->height);


    if ((kk->drv.p->sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return -1;
    }

    if (setsockopt(kk->drv.p->sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
        perror("setsockopt SO_REUSEADDR");
        return -1;
    }

    kk->drv.p->my_addr.sin_family = AF_INET;
    kk->drv.p-> my_addr.sin_port = htons(kk->drv.p->port);
    kk->drv.p->my_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(kk->drv.p->my_addr.sin_zero), '\0', 8);

    if (bind(kk->drv.p->sockfd, (struct sockaddr *)&kk->drv.p->my_addr, sizeof(struct sockaddr))
                                                                   == -1) {
        perror("bind");
        return -1;
    }

    /* Non blocking socket */
    fcntl(kk->drv.p->sockfd, F_SETFL, O_NONBLOCK);

    if (listen(kk->drv.p->sockfd, BACKLOG) == -1) {
        perror("listen");
        return -1;
    }

    kk->drv.p->buffer = NULL;

    /* Ignore SIGPIPE */
    signal(SIGPIPE, SIG_IGN);

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
    int i;

    /* Get ANSI representation of the image */
    kk->drv.p->buffer = cucul_get_ansi(kk->qq, 0, &kk->drv.p->size);;

    for(i = 0; i < kk->drv.p->client_count; i++)
    {
        if(kk->drv.p->fd_list[i] == -1)
            continue;

        if(send_data(kk, kk->drv.p->fd_list[i]))
            kk->drv.p->fd_list[i] = -1;
    }

    manage_connections(kk);
}

static void network_handle_resize(caca_t *kk)
{
    /* Not handled */
}

static unsigned int network_get_event(caca_t *kk)
{
    /* Manage new connections as this function will be called sometimes
     *  more often than display
     */
    manage_connections(kk);

    /* Event not handled */
    return 0;
}

/*
 * XXX: The following functions are local
 */

static void manage_connections(caca_t *kk)
{
    int fd;

    kk->drv.p->clilen = sizeof(kk->drv.p->remote_addr);
    fd = accept(kk->drv.p->sockfd, (struct sockaddr *) &kk->drv.p->remote_addr, &kk->drv.p->clilen);

    if(fd == -1)
        return;

    /* Non blocking socket */
    fcntl(fd, F_SETFL, O_NONBLOCK);

    if(kk->drv.p->fd_list == NULL)
    {
        kk->drv.p->fd_list = malloc(sizeof(int));
        if(kk->drv.p->fd_list == NULL)
            return;
    }
    else
    {
        kk->drv.p->fd_list = realloc(kk->drv.p->fd_list,
                                     (kk->drv.p->client_count+1) * sizeof(int));
    }

    if(send_data(kk, fd) == 0)
    {
        kk->drv.p->fd_list[kk->drv.p->client_count] = fd;
        kk->drv.p->client_count++;
    }
}

static int send_data(caca_t *kk, int fd)
{
    ssize_t ret;

    /* No error, there's just nothing to send yet */
    if(!kk->drv.p->buffer)
        return 0;

    if(fd < 0)
        return -1;

    /* FIXME, handle >255 sizes */
    codes[15] = (unsigned char) (kk->drv.p->width & 0xff00)>>8;
    codes[16] = (unsigned char) kk->drv.p->width & 0xff;
    codes[17] = (unsigned char) (kk->drv.p->height & 0xff00)>>8;
    codes[18] = (unsigned char) kk->drv.p->height & 0xff;

    /* Send basic telnet codes */
    ret = nonblock_write(fd, codes, sizeof(codes));
    if(ret == -1)
        return (errno == EAGAIN) ? 0 : -1;

    /* ANSI code for move(0,0)*/
    ret = nonblock_write(fd, "\033[1,1H", 6);
    if(ret == -1)
        return (errno == EAGAIN) ? 0 : -1;

    /* Send actual data */
    ret = nonblock_write(fd, kk->drv.p->buffer, kk->drv.p->size);
    if(ret == -1)
        return (errno == EAGAIN) ? 0 : -1;

    return 0;
}

ssize_t nonblock_write(int fd, char *buf, size_t len)
{
    int retries = 10;
    size_t total = 0;
    ssize_t ret;

    while(total < len)
    {
        do
        {
            ret = write(fd, buf, len);
            if(ret < 0 && errno == EAGAIN)
            {
                retries--;
                if(retries < 0)
                    break;
            }
        }
        while(ret < 0 && (errno == EINTR || errno == EAGAIN));

        if(ret < 0)
            return ret;
        else if(ret == 0)
            return total;

        total += len;
        buf += len;
    }

    return total;
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

