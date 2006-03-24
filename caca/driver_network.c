/*
 *  libcaca       Colour ASCII-Art library
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

#define BACKLOG 1337     /* Number of pending connections */
#define INBUFFER 32      /* Size of per-client input buffer */
#define OUTBUFFER 300000 /* Size of per-client output buffer */

/* Following vars are static */
#define INIT_PREFIX \
    "\xff\xfb\x01"     /* WILL ECHO */ \
    "\xff\xfb\x03"     /* WILL SUPPRESS GO AHEAD */ \
    "\xff\xfd\x31"     /* DO NAWS */ \
    "\xff\x1f\xfa____" /* SB NAWS */ \
    "\xff\xf0"         /* SE */  \
    "\x1b]2;caca for the network\x07" /* Change window title */ \
    "\x1b[H\x1b[J" /* Clear screen */
    /*"\x1b[?25l"*/ /* Hide cursor */

#define ANSI_PREFIX \
    "\x1b[1;1H" /* move(0,0) */ \
    "\x1b[1;1H" /* move(0,0) again */

#define ANSI_RESET \
    "    " /* Garbage */ \
    "\x1b[?1049h" /* Clear screen */ \
    "\x1b[?1049h" /* Clear screen again */


static char const telnet_commands[16][5] =
    {"SE  ", "NOP ", "DM  ", "BRK ", "IP  ", "AO  ", "AYT ", "EC  ",
     "EL  ", "GA  ", "SB  ", "WILL", "WONT", "DO  ", "DONT", "IAC "};

static char const telnet_options[37][5] =
    {"????", "ECHO", "????", "SUGH", "????", "STTS", "TIMK", "????", "????", "????",
     "????", "????", "????", "????", "????", "????", "????", "????", "????", "????",
     "????", "????", "????", "????", "TTYP", "????", "????", "????", "????", "????",
     "????", "NAWS", "TRSP", "RMFC", "LIMO", "????", "EVAR"};


#define COMMAND_NAME(x) (x>=240)?telnet_commands[x-240]:"????"
#define OPTION_NAME(x) (x<=36)?telnet_options[x]:"????"


struct client
{
    int fd;
    int ready;
    uint8_t inbuf[INBUFFER];
    int inbytes;
    uint8_t outbuf[OUTBUFFER];
    int start, stop;
};

struct driver_private
{
    unsigned int width, height;
    unsigned int port;
    int sockfd;
    struct sockaddr_in my_addr;
    socklen_t sin_size;

    char prefix[sizeof(INIT_PREFIX)];

    struct cucul_export *ex;

    int client_count;
    struct client *clients;

    RETSIGTYPE (*sigpipe_handler)(int);
};

static void manage_connections(caca_t *kk);
static int send_data(caca_t *kk, struct client *c);
ssize_t nonblock_write(int fd, void *buf, size_t len);

static int network_init_graphics(caca_t *kk)
{
    int yes = 1, flags;
    int port = 0xCACA; /* 51914 */
    unsigned int width = 0, height = 0;
    char *tmp;

    kk->drv.p = malloc(sizeof(struct driver_private));
    if(kk->drv.p == NULL)
        return -1;

#if defined(HAVE_GETENV)
    tmp = getenv("CACA_PORT");
    if(tmp && *tmp)
    {
        int new_port = atoi(tmp);
        if(new_port)
            port = new_port;
    }

    tmp = getenv("CACA_GEOMETRY");
    if(tmp && *tmp)
        sscanf(tmp, "%ux%u", &width, &height);
#endif

    if(width && height)
    {
        kk->drv.p->width = width;
        kk->drv.p->height = height;
    }
    else
    {
        kk->drv.p->width = 80;
        kk->drv.p->height = 24;
    }

    kk->drv.p->client_count = 0;
    kk->drv.p->clients = NULL;
    kk->drv.p->port = port;

    _cucul_set_size(kk->qq, kk->drv.p->width, kk->drv.p->height);

    /* FIXME, handle >255 sizes */
    memcpy(kk->drv.p->prefix, INIT_PREFIX, sizeof(INIT_PREFIX));
    tmp = strstr(kk->drv.p->prefix, "____");
    tmp[0] = (unsigned char) (kk->drv.p->width & 0xff00) >> 8;
    tmp[1] = (unsigned char) kk->drv.p->width & 0xff;
    tmp[2] = (unsigned char) (kk->drv.p->height & 0xff00) >> 8;
    tmp[3] = (unsigned char) kk->drv.p->height & 0xff;

    if ((kk->drv.p->sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        return -1;
    }

    if (setsockopt(kk->drv.p->sockfd, SOL_SOCKET,
                   SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        perror("setsockopt SO_REUSEADDR");
        return -1;
    }

    kk->drv.p->my_addr.sin_family = AF_INET;
    kk->drv.p-> my_addr.sin_port = htons(kk->drv.p->port);
    kk->drv.p->my_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(kk->drv.p->my_addr.sin_zero), '\0', 8);

    if (bind(kk->drv.p->sockfd, (struct sockaddr *)&kk->drv.p->my_addr,
             sizeof(struct sockaddr)) == -1)
    {
        perror("bind");
        return -1;
    }

    /* Non blocking socket */
    flags = fcntl(kk->drv.p->sockfd, F_GETFL, 0);
    fcntl(kk->drv.p->sockfd, F_SETFL, flags | O_NONBLOCK);

    if (listen(kk->drv.p->sockfd, BACKLOG) == -1)
    {
        perror("listen");
        return -1;
    }

    kk->drv.p->ex = NULL;

    /* Ignore SIGPIPE */
    kk->drv.p->sigpipe_handler = signal(SIGPIPE, SIG_IGN);

    fprintf(stderr, "initialised network, listening on port %i\n",
                    kk->drv.p->port);

    return 0;
}

static int network_end_graphics(caca_t *kk)
{
    int i;

    for(i = 0; i < kk->drv.p->client_count; i++)
    {
        close(kk->drv.p->clients[i].fd);
        kk->drv.p->clients[i].fd = -1;
    }

    if(kk->drv.p->ex)
        cucul_free_export(kk->drv.p->ex);

    /* Restore SIGPIPE handler */
    signal(SIGPIPE, kk->drv.p->sigpipe_handler);

    free(kk->drv.p);

    return 0;
}

static int network_set_window_title(caca_t *kk, char const *title)
{
    /* Not handled (yet) */
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

    /* Free the previous export buffer, if any */
    if(kk->drv.p->ex)
    {
        cucul_free_export(kk->drv.p->ex);
        kk->drv.p->ex = NULL;
    }

    /* Get ANSI representation of the image and skip the end-of buffer
     * linefeed ("\r\n\0", 3 bytes) */
    kk->drv.p->ex = cucul_create_export(kk->qq, CUCUL_FORMAT_ANSI);
    kk->drv.p->ex->size -= 3;

    for(i = 0; i < kk->drv.p->client_count; i++)
    {
        if(kk->drv.p->clients[i].fd == -1)
            continue;

        if(send_data(kk, &kk->drv.p->clients[i]))
        {
            fprintf(stderr, "client %i dropped connection\n",
                            kk->drv.p->clients[i].fd);
            close(kk->drv.p->clients[i].fd);
            kk->drv.p->clients[i].fd = -1;
        }
    }

    manage_connections(kk);
}

static void network_handle_resize(caca_t *kk)
{
    /* Not handled */
}

static int network_get_event(caca_t *kk, struct caca_event *ev)
{
    /* Manage new connections as this function will be called sometimes
     * more often than display */
    manage_connections(kk);

    /* Event not handled */
    return 0;
}

/*
 * XXX: The following functions are local
 */

static void manage_connections(caca_t *kk)
{
    int fd, flags;
    struct sockaddr_in remote_addr;
    socklen_t len = sizeof(struct sockaddr_in);

    fd = accept(kk->drv.p->sockfd, (struct sockaddr *)&remote_addr, &len);
    if(fd == -1)
        return;

    fprintf(stderr, "client %i connected from %s\n",
                    fd, inet_ntoa(remote_addr.sin_addr));

    /* Non blocking socket */
    flags = fcntl(fd, F_SETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    if(kk->drv.p->clients == NULL)
    {
        kk->drv.p->clients = malloc(sizeof(struct client));
        if(kk->drv.p->clients == NULL)
            return;
    }
    else
    {
        kk->drv.p->clients = realloc(kk->drv.p->clients,
                                     (kk->drv.p->client_count+1) * sizeof(struct client));
    }

    kk->drv.p->clients[kk->drv.p->client_count].fd = fd;
    kk->drv.p->clients[kk->drv.p->client_count].ready = 0;
    kk->drv.p->clients[kk->drv.p->client_count].inbytes = 0;
    kk->drv.p->clients[kk->drv.p->client_count].start = 0;
    kk->drv.p->clients[kk->drv.p->client_count].stop = 0;

    /* If we already have data to send, send it to the new client */
    if(send_data(kk, &kk->drv.p->clients[kk->drv.p->client_count]))
    {
        fprintf(stderr, "client %i dropped connection\n", fd);
        close(fd);
        kk->drv.p->clients[kk->drv.p->client_count].fd = -1;
        return;
    }

    kk->drv.p->client_count++;
}

static int send_data(caca_t *kk, struct client *c)
{
    ssize_t ret;

    /* Not for us */
    if(c->fd < 0)
        return -1;

    /* Listen to incoming data */
    for(;;)
    {
        ret = read(c->fd, c->inbuf + c->inbytes, 1);
        if(ret <= 0)
            break;

        c->inbytes++;

        /* Check for telnet sequences */
        if(c->inbuf[0] == 0xff)
        {
            if(c->inbytes == 1)
            {
                ;
            }
            else if(c->inbuf[1] == 0xfd || c->inbuf[1] == 0xfc)
            {
                if(c->inbytes == 3)
                {
                    fprintf(stderr, "client %i said: %.02x %.02x %.02x (%s %s %s)\n",
                            c->fd, c->inbuf[0], c->inbuf[1], c->inbuf[2],
                            COMMAND_NAME(c->inbuf[0]), COMMAND_NAME(c->inbuf[1]), OPTION_NAME(c->inbuf[2]));
                    /* Just ignore, lol */
                    c->inbytes = 0;
                }
            }
            else
                c->inbytes = 0;
        }
        else if(c->inbytes == 1)
        {
            if(c->inbuf[0] == 0x03)
            {
                fprintf(stderr, "client %i pressed C-c\n", c->fd);
                return -1; /* User requested to quit */
            }

            c->inbytes = 0;
        }
    }

    /* Send the telnet initialisation commands */
    if(!c->ready)
    {
        ret = nonblock_write(c->fd, kk->drv.p->prefix, sizeof(INIT_PREFIX));
        if(ret == -1)
            return (errno == EAGAIN) ? 0 : -1;

        if(ret < (ssize_t)sizeof(INIT_PREFIX))
            return 0;

        c->ready = 1;
    }

    /* No error, there's just nothing to send yet */
    if(!kk->drv.p->ex)
        return 0;

    /* If we have backlog, send the backlog */
    if(c->stop)
    {
        ret = nonblock_write(c->fd, c->outbuf + c->start, c->stop - c->start);

        if(ret == -1)
        {
            if(errno == EAGAIN)
                ret = 0;
            else
                return -1;
        }

        if(ret == c->stop - c->start)
        {
            /* We got rid of the backlog! */
            c->start = c->stop = 0;
        }
        else
        {
            c->start += ret;

            if(c->stop - c->start + strlen(ANSI_PREFIX) + kk->drv.p->ex->size
                > OUTBUFFER)
            {
                /* Overflow! Empty buffer and start again */
                memcpy(c->outbuf, ANSI_RESET, strlen(ANSI_RESET));
                c->start = 0;
                c->stop = strlen(ANSI_RESET);
                return 0;
            }

            /* Need to move? */
            if(c->stop + strlen(ANSI_PREFIX) + kk->drv.p->ex->size > OUTBUFFER)
            {
                memmove(c->outbuf, c->outbuf + c->start, c->stop - c->start);
                c->stop -= c->start;
                c->start = 0;
            }

            memcpy(c->outbuf + c->stop, ANSI_PREFIX, strlen(ANSI_PREFIX));
            c->stop += strlen(ANSI_PREFIX);
            memcpy(c->outbuf + c->stop, kk->drv.p->ex->buffer, kk->drv.p->ex->size);
            c->stop += kk->drv.p->ex->size;

            return 0;
        }
    }

    /* We no longer have backlog, send our new data */

    /* Send ANSI prefix */
    ret = nonblock_write(c->fd, ANSI_PREFIX, strlen(ANSI_PREFIX));
    if(ret == -1)
    {
        if(errno == EAGAIN)
            ret = 0;
        else
            return -1;
    }

    if(ret < (ssize_t)strlen(ANSI_PREFIX))
    {
        if(strlen(ANSI_PREFIX) + kk->drv.p->ex->size > OUTBUFFER)
        {
            /* Overflow! Empty buffer and start again */
            memcpy(c->outbuf, ANSI_RESET, strlen(ANSI_RESET));
            c->start = 0;
            c->stop = strlen(ANSI_RESET);
            return 0;
        }

        memcpy(c->outbuf, ANSI_PREFIX, strlen(ANSI_PREFIX) - ret);
        c->stop = strlen(ANSI_PREFIX) - ret;
        memcpy(c->outbuf + c->stop, kk->drv.p->ex->buffer, kk->drv.p->ex->size);
        c->stop += kk->drv.p->ex->size;

        return 0;
    }

    /* Send actual data */
    ret = nonblock_write(c->fd, kk->drv.p->ex->buffer, kk->drv.p->ex->size);
    if(ret == -1)
    {
        if(errno == EAGAIN)
            ret = 0;
        else
            return -1;
    }

    if(ret < (int)kk->drv.p->ex->size)
    {
        if(kk->drv.p->ex->size > OUTBUFFER)
        {
            /* Overflow! Empty buffer and start again */
            memcpy(c->outbuf, ANSI_RESET, strlen(ANSI_RESET));
            c->start = 0;
            c->stop = strlen(ANSI_RESET);
            return 0;
        }

        memcpy(c->outbuf, kk->drv.p->ex->buffer, kk->drv.p->ex->size - ret);
        c->stop = kk->drv.p->ex->size - ret;

        return 0;
    }

    return 0;
}

ssize_t nonblock_write(int fd, void *buf, size_t len)
{
    size_t total = 0;
    ssize_t ret;

    while(total < len)
    {
        do
        {
            ret = write(fd, buf, len);
        }
        while(ret < 0 && errno == EINTR);

        if(ret < 0)
            return ret;
        else if(ret == 0)
            return total;

        total += len;
        buf = (void *)((uintptr_t)buf + len);
    }

    return total;
}

/*
 * Driver initialisation
 */

int network_install(caca_t *kk)
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

    return 0;
}


#endif /* USE_NETWORK */

