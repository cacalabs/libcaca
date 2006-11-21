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
#   if defined HAVE_PTY_H
#       include <pty.h>  /* for openpty and forkpty */
#   endif
#endif

#include "cucul.h"
#include "caca.h"

static int run_in_pty(char *cmd);

int main(int argc, char **argv)
{
    static cucul_canvas_t *cv, *app[2];
    static caca_display_t *dp;
    unsigned char *buf[2];
    long int total[2];
    int ptyfd[2], pty = 0;
    int w, h, eof = 0;

    if(argc < 3)
    {
        fprintf(stderr, "usage: %s <cmd1> <cmd2>\n", argv[0]);
        fprintf(stderr, "eg. \"%s bash bash\"\n", argv[0]);
        return 1;
    }

    cv = cucul_create_canvas(80 + 7, 24 + 7);
    dp = caca_create_display(cv);
    if(!dp)
        return 1;

//    w = cucul_get_canvas_width(cv) - 7;
//    h = cucul_get_canvas_height(cv) - 7;
    w = 80; h = 24;

    if(w < 0 || h < 0)
        return 1;

    app[0] = cucul_create_canvas(w, h);
    app[1] = cucul_create_canvas(w, h);

    cucul_set_color_ansi(cv, CUCUL_LIGHTRED, CUCUL_BLACK);
    cucul_draw_thin_box(cv, pty * 5, pty * 5,
                        80 + pty * 5 + 1, 24 + pty * 5 + 1);

    caca_refresh_display(dp);

    buf[0] = buf[1] = NULL;
    total[0] = total[1] = 0;

    ptyfd[0] = run_in_pty(argv[1]);
    ptyfd[1] = run_in_pty(argv[2]);

    if(ptyfd[0] < 0 || ptyfd[1] < 0)
        return -1;

    fcntl(ptyfd[0], F_SETFL, O_NDELAY);
    fcntl(ptyfd[1], F_SETFL, O_NDELAY);

    for(;;)
    {
        struct timeval tv;
        fd_set fdset;
        caca_event_t ev;
        int i, ret, refresh = 0;

        ret = caca_get_event(dp, CACA_EVENT_ANY, &ev, 0);
        if(ret && ev.type & CACA_EVENT_KEY_PRESS)
        {
            switch(ev.data.key.ch)
            {
            case CACA_KEY_CTRL_A:
                cucul_draw_box(cv, pty * 5, pty * 5,
                               80 + pty * 5 + 1, 24 + pty * 5 + 1, ' ');
                pty = 1 - pty;
                refresh = 1;
                break;
            case CACA_KEY_UP:
                write(ptyfd[pty], "\x1b[A", 3); break;
            case CACA_KEY_DOWN:
                write(ptyfd[pty], "\x1b[B", 3); break;
            case CACA_KEY_RIGHT:
                write(ptyfd[pty], "\x1b[C", 3); break;
            case CACA_KEY_LEFT:
                write(ptyfd[pty], "\x1b[D", 3); break;
            default:
                write(ptyfd[pty], &ev.data.key.ch, 1); break;
            }
        }

        /* Read data, if any */
        FD_ZERO(&fdset);
        FD_SET(ptyfd[0], &fdset);
        FD_SET(ptyfd[1], &fdset);
        tv.tv_sec = 0;
        tv.tv_usec = 50000;
        ret = select(ptyfd[0] + ptyfd[1] + 1, &fdset, NULL, NULL, &tv);

        if(ret < 0)
        {
            if(!total[0] && !total[1])
                break;
        }
        else if(ret) for(i = 0; i < 2; i++)
        {
            if(FD_ISSET(ptyfd[i], &fdset))
            {
                ssize_t n;

                buf[i] = realloc(buf[i], total[i] + 4096);
                n = read(ptyfd[i], buf[i] + total[i], 4096);

                if(n > 0)
                    total[i] += n;
                else if(n == 0 || errno != EWOULDBLOCK)
                    eof = 1;
            }
        }

        for(i = 0; i < 2; i++) if(total[i])
        {
            unsigned long int bytes;

            bytes = cucul_import_memory(app[i], buf[i], total[i], "utf8");

            if(bytes > 0)
            {
                total[i] -= bytes;
                memmove(buf[i], buf[i] + bytes, total[i]);
                refresh = 1;
            }
        }

        if(refresh)
        {
            cucul_blit(cv, 5 * !pty + 1, 5 * !pty + 1, app[!pty], NULL);
            cucul_blit(cv, 5 * pty + 1, 5 * pty + 1, app[pty], NULL);
            cucul_draw_thin_box(cv, pty * 5, pty * 5,
                                80 + pty * 5 + 1, 24 + pty * 5 + 1);
            caca_refresh_display(dp);
        }

        if(eof && !total[0] && !total[1])
            break;
    }

    caca_get_event(dp, CACA_EVENT_KEY_PRESS, NULL, -1);

    /* Clean up */
    caca_free_display(dp);
    cucul_free_canvas(cv);
    cucul_free_canvas(app[0]);
    cucul_free_canvas(app[1]);

    return 0;
}

static int run_in_pty(char *cmd)
{
#if defined HAVE_PTY_H
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
        putenv("CACA_DRIVER=slang");
        argv = malloc(2 * sizeof(char *));
        argv[0] = cmd;
        argv[1] = NULL;
        execvp(cmd, argv);
        fprintf(stderr, "execvp() error\n");
        return -1;
    }

    return fd;
#else
    fprintf(stderr, "forkpty() not available\n");
    return -1;
#endif
}

