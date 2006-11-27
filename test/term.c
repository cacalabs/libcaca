/*
 *  term          swallow a terminal-based application
 *  Copyright (c) 2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#include "config.h"
#include "common.h"

#if !defined __KERNEL__
#   include <stdio.h>
#   include <string.h>
#   include <stdlib.h>
#   include <unistd.h>
#   include <fcntl.h>
#   include <sys/ioctl.h>
#   if defined HAVE_PTY_H
#       include <pty.h>  /* for openpty and forkpty */
#   else
#       include <util.h> /* for OS X */
#   endif
#endif

#include "cucul.h"
#include "caca.h"

#define XTAB 3
#define YTAB 2

struct screen
{
    cucul_canvas_t *cv;
    int fd;
    unsigned char *buf;
    long int total;
    int w, h;
};

static int create_pty(char *cmd, unsigned int w, unsigned int h);
static int set_tty_size(int fd, unsigned int w, unsigned int h);

int main(int argc, char **argv)
{
    static cucul_canvas_t *cv;
    static caca_display_t *dp;
    static struct screen *screen;
    int pty = 0, prevpty = 0, i, n, w, h, eof = 0, refresh = 1, command = 0;

    if(argc < 2)
    {
        fprintf(stderr, "usage: %s <cmd1> [<cmd2> [<cmd3> ...]]\n", argv[0]);
        fprintf(stderr, "eg. \"%s zsh bash\"\n", argv[0]);
        return 1;
    }

    cv = cucul_create_canvas(0, 0);
    dp = caca_create_display(cv);
    if(!dp)
        return 1;

    n = argc - 1;
    screen = malloc(n * sizeof(struct screen));

    w = cucul_get_canvas_width(cv);
    h = cucul_get_canvas_height(cv);

    w = w <= XTAB * n ? 1 : w - XTAB * n;
    h = h <= YTAB * n ? 1 : h - YTAB * n;

    for(i = 0; i < n; i++)
        screen[i].cv = cucul_create_canvas(w, h);

    cucul_set_color_ansi(cv, CUCUL_LIGHTRED, CUCUL_BLACK);
    cucul_draw_cp437_box(cv, (n - 1 - pty) * XTAB, pty * YTAB,
                         w + (n - 1 - pty) * XTAB + 1, h + pty * YTAB + 1);

    caca_refresh_display(dp);

    for(i = 0; i < n; i++)
    {
        screen[i].buf = NULL;
        screen[i].total = 0;
        screen[i].fd = create_pty(argv[i + 1], w, h);
        if(screen[i].fd < 0)
            return -1;
    }

    for(;;)
    {
        struct timeval tv;
        fd_set fdset;
        caca_event_t ev;
        int i, maxfd = 0, ret;

        /* Read data, if any */
        FD_ZERO(&fdset);
        for(i = 0; i < n; i++)
        {
            FD_SET(screen[i].fd, &fdset);
            if(screen[i].fd > maxfd)
                maxfd = screen[i].fd;
        }
        tv.tv_sec = 0;
        tv.tv_usec = 50000;
        ret = select(maxfd + 1, &fdset, NULL, NULL, &tv);

        if(ret < 0)
        {
            if(errno == EINTR)
                ; /* We probably got a SIGWINCH, ignore it */
            else
            {
                for(i = 0; i < n; i++)
                    if(screen[i].total)
                        break;
                if(i == n)
                    break;
            }
        }
        else if(ret) for(i = 0; i < n; i++)
        {
            if(FD_ISSET(screen[i].fd, &fdset))
            {
                ssize_t n;

                screen[i].buf = realloc(screen[i].buf, screen[i].total + 4096);
                n = read(screen[i].fd, screen[i].buf + screen[i].total, 4096);

                if(n > 0)
                    screen[i].total += n;
                else if(n == 0 || errno != EWOULDBLOCK)
                    eof = 1;
            }
        }

        for(i = 0; i < n; i++) if(screen[i].total)
        {
            unsigned long int bytes;

            bytes = cucul_import_memory(screen[i].cv, screen[i].buf, screen[i].total, "utf8");

            if(bytes > 0)
            {
                screen[i].total -= bytes;
                memmove(screen[i].buf, screen[i].buf + bytes, screen[i].total);
                refresh = 1;
            }
        }

        /* Get events, if any */
        ret = caca_get_event(dp, CACA_EVENT_ANY, &ev, 0);
        if(ret && (ev.type & CACA_EVENT_KEY_PRESS))
        {
            if(command)
            {
                command = 0;

                switch(ev.data.key.ch)
                {
                case CACA_KEY_CTRL_A:
                    pty ^= prevpty;
                    prevpty ^= pty;
                    pty ^= prevpty;
                    refresh = 1;
                    break;
                case 'n':
                case ' ':
                case '\0':
                case CACA_KEY_CTRL_N:
                    prevpty = pty;
                    pty = (pty + 1) % n;
                    refresh = 1;
                    break;
                case 'p':
                case CACA_KEY_CTRL_P:
                    prevpty = pty;
                    pty = (pty + n - 1) % n;
                    refresh = 1;
                    break;
                }
            }
            else
            {
                switch(ev.data.key.ch)
                {
                case CACA_KEY_CTRL_A:
                    command = 1; break;
                case CACA_KEY_UP:
                    write(screen[pty].fd, "\x1b[A", 3); break;
                case CACA_KEY_DOWN:
                    write(screen[pty].fd, "\x1b[B", 3); break;
                case CACA_KEY_RIGHT:
                    write(screen[pty].fd, "\x1b[C", 3); break;
                case CACA_KEY_LEFT:
                    write(screen[pty].fd, "\x1b[D", 3); break;
                default:
                    write(screen[pty].fd, &ev.data.key.ch, 1); break;
                }
            }
        }
        else if(ret && (ev.type & CACA_EVENT_RESIZE))
        {
            w = cucul_get_canvas_width(cv);
            h = cucul_get_canvas_height(cv);
            w = w <= XTAB * n ? 1 : w - XTAB * n;
            h = h <= YTAB * n ? 1 : h - YTAB * n;
            for(i = 0; i < n; i++)
            {
                cucul_set_canvas_size(screen[i].cv, w, h);
                set_tty_size(screen[i].fd, w, h);
            }
            cucul_clear_canvas(cv);
            refresh = 1;
        }

        /* Refresh screen */
        if(refresh)
        {
            refresh = 0;

            for(i = 0; i < n; i++)
            {
                int j = (pty + n - 1 - i) % n;
                cucul_blit(cv, (n - 1 - j) * XTAB + 1,
                               j * YTAB + 1, screen[j].cv, NULL);
                cucul_draw_cp437_box(cv, (n - 1 - j) * XTAB, j * YTAB,
                                     w + (n - 1 - j) * XTAB + 1, h + j * YTAB + 1);
            }
            caca_refresh_display(dp);
        }

        if(eof)
        {
            for(i = 0; i < n; i++)
                if(screen[i].total)
                    break;
            if(i == n)
                break;
        }
    }

    caca_get_event(dp, CACA_EVENT_KEY_PRESS, NULL, -1);

    /* Clean up */
    caca_free_display(dp);
    cucul_free_canvas(cv);
    for(i = 0; i < n; i++)
        cucul_free_canvas(screen[i].cv);

    return 0;
}

static int create_pty(char *cmd, unsigned int w, unsigned int h)
{
    char **argv;
    int fd;
    pid_t pid;

    pid = forkpty(&fd, NULL, NULL, NULL);
    if(pid < 0)
    {
        fprintf(stderr, "forkpty() error\n");
        return -1;
    }
    else if(pid == 0)
    {
        set_tty_size(0, w, h);
        putenv("CACA_DRIVER=slang");
        putenv("TERM=xterm");
        argv = malloc(2 * sizeof(char *));
        argv[0] = cmd;
        argv[1] = NULL;
        execvp(cmd, argv);
        fprintf(stderr, "execvp() error\n");
        return -1;
    }

    fcntl(fd, F_SETFL, O_NDELAY);
    return fd;
#if 0
    fprintf(stderr, "forkpty() not available\n");
    return -1;
#endif
}

static int set_tty_size(int fd, unsigned int w, unsigned int h)
{
    struct winsize ws;

    memset(&ws, 0, sizeof(ws));
    ws.ws_row = h;
    ws.ws_col = w;
    ioctl(fd, TIOCSWINSZ, (char *)&ws);

    return 0;
}

