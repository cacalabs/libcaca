/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2009 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/*
 *  This file contains profiling functions.
 */

#ifndef __CACA_PROF_H__
#define __CACA_PROF_H__

#if defined PROF && !defined __KERNEL__
#   define PROFILING_VARS

#   define STAT_IADD(s, n) \
      do \
      { \
        struct caca_stat *ss = s; \
        int ii, nn = n; \
        for (ii = STAT_VALUES - 1; ii > 0; ii--) \
            ss->itable[ii] = ss->itable[ii - 1]; \
        ss->itable[0] = nn; \
        ss->imean = ((int64_t)ss->imean * 15 + nn) / 16; \
      } \
      while(0)

#   define START_PROF(obj, fn)
#   define STOP_PROF(obj, fn)

#else
#   define PROFILING_VARS
#   define STAT_IADD(s) do { } while(0)
#   define START_PROF(obj, fn) do { } while(0)
#   define STOP_PROF(obj, fn) do { } while(0)
#endif

#endif /* __CACA_PROF_H__ */

