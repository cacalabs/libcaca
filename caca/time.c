/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2002-2012 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

/*
 *  This file contains simple timer routines.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdlib.h>
#   if defined(HAVE_SYS_TIME_H)
#       include <sys/time.h>
#   endif
#   include <time.h>
#   if defined(USE_WIN32)
#       include <windows.h>
#   endif
#   if defined(HAVE_UNISTD_H)
#       include <unistd.h>
#   endif
#endif

#include "caca.h"
#include "caca_internals.h"

void _caca_sleep(int usec)
{
#if defined(HAVE_SLEEP)
    Sleep((usec + 500) / 1000);
#elif defined(HAVE_USLEEP)
    usleep(usec);
#else
    /* SLEEP */
#endif
}

int _caca_getticks(caca_timer_t *timer)
{
#if defined(USE_WIN32)
    LARGE_INTEGER tmp;
    static double freq = -1.0; /* FIXME: can this move to caca_context? */
    double seconds;
#elif defined(HAVE_GETTIMEOFDAY)
    struct timeval tv;
#endif
    int ticks = 0;
    int new_sec, new_usec;

#if defined(USE_WIN32)
    if (freq < 0.0)
    {
        if(!QueryPerformanceFrequency(&tmp))
            freq = 0.0;
        else
            freq = 1.0 / (double)tmp.QuadPart;
    }

    QueryPerformanceCounter(&tmp);
    seconds = freq * (double)tmp.QuadPart;
    new_sec = (int)seconds;
    new_usec = (int)((seconds - new_sec) * 1000000.0);
#elif defined(HAVE_GETTIMEOFDAY)
    gettimeofday(&tv, NULL);
    new_sec = tv.tv_sec;
    new_usec = tv.tv_usec;
#endif

    if(timer->last_sec != 0)
    {
        /* If the delay was greater than 60 seconds, return 10 seconds
         * otherwise we may overflow our ticks counter. */
        if(new_sec >= timer->last_sec + 60)
            ticks = 60 * 1000000;
        else
        {
            ticks = (new_sec - timer->last_sec) * 1000000;
            ticks += new_usec;
            ticks -= timer->last_usec;
        }
    }

    timer->last_sec = new_sec;
    timer->last_usec = new_usec;

    return ticks;
}

