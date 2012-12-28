/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2002-2012 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

/*
 *  getopt.c: getopt_long reimplementation
 */

#include "config.h"

#if !defined __KERNEL__
#   include <stdio.h>
#   include <string.h>
#   if defined HAVE_GETOPT_H && defined HAVE_GETOPT_LONG
#       include <getopt.h>
#   endif
#endif

#include "caca.h"
#include "caca_internals.h"

int   caca_optind = 1;
char *caca_optarg = NULL;

int caca_getopt(int argc, char * const _argv[], char const *optstring,
                struct caca_option const *longopts, int *longindex)
{
#if defined HAVE_GETOPT_LONG
    int ret;
    optind = caca_optind;
    optarg = caca_optarg;
    ret = getopt_long(argc, _argv, optstring,
                      (struct option const *)longopts, longindex);
    caca_optind = optind;
    caca_optarg = optarg;
    return ret;

#else
    /* XXX: this getopt_long implementation should not be trusted for other
     * applications without any serious peer reviewing. It “just works” with
     * zzuf and a few libcaca programs but may fail miserably in other
     * programs. */
    char **argv = (char **)(uintptr_t)_argv;
    char *flag;
    int i;

    if(caca_optind >= argc)
        return -1;

    flag = argv[caca_optind];

    if(flag[0] == '-' && flag[1] != '-')
    {
        char *tmp;
        int ret = flag[1];

        if(ret == '\0')
            return -1;

        tmp = strchr(optstring, ret);
        if(!tmp || ret == ':')
            return '?';

        caca_optind++;
        if(tmp[1] == ':')
        {
            if(flag[2] != '\0')
                caca_optarg = flag + 2;
            else
                caca_optarg = argv[caca_optind++];
            return ret;
        }

        if(flag[2] != '\0')
        {
            flag[1] = '-';
            caca_optind--;
            argv[caca_optind]++;
        }

        return ret;
    }

    if(flag[0] == '-' && flag[1] == '-')
    {
        if(flag[2] == '\0')
            return -1;

        for(i = 0; longopts[i].name; i++)
        {
            size_t l = strlen(longopts[i].name);

            if(strncmp(flag + 2, longopts[i].name, l))
                continue;

            switch(flag[2 + l])
            {
            case '=':
                if(!longopts[i].has_arg)
                    goto bad_opt;
                if(longindex)
                    *longindex = i;
                caca_optind++;
                caca_optarg = flag + 2 + l + 1;
                return longopts[i].val;
            case '\0':
                if(longindex)
                    *longindex = i;
                caca_optind++;
                if(longopts[i].has_arg)
                    caca_optarg = argv[caca_optind++];
                return longopts[i].val;
            default:
                break;
            }
        }
    bad_opt:
        fprintf(stderr, "%s: unrecognized option `%s'\n", argv[0], flag);
        return '?';
    }

    return -1;
#endif
}

