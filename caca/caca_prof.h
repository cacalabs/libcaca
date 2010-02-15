/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2009-2010 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/*
 *  This file contains profiling functions.
 */

#ifndef __CACA_PROF_H__
#define __CACA_PROF_H__

#if defined PROF && !defined __KERNEL__
#   define PROFILING_VARS

#   define STAT_IADD(_s, _n) \
      do \
      { \
        struct caca_stat *s = _s; \
        int i, n = _n; \
        for (i = STAT_VALUES - 1; i > 0; i--) \
            s->itable[i] = s->itable[i - 1]; \
        s->itable[0] = n; \
        s->imean = (s->imean * 63 + (int64_t)n * 64 + 32) / 64; \
      } \
      while(0)

#   define START_PROF(obj, fn)
#   define STOP_PROF(obj, fn)

#else
#   define PROFILING_VARS
#   define STAT_IADD(_s, _n) do { } while(0)
#   define START_PROF(obj, fn) do { } while(0)
#   define STOP_PROF(obj, fn) do { } while(0)
#endif

#endif /* __CACA_PROF_H__ */

