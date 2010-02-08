/*
 *  zzuf - general purpose fuzzer
 *  Copyright (c) 2002, 2007 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/*
 *  mygetopt.c: getopt_long reimplementation
 */

#include "config.h"

#include <stdio.h>
#include <string.h>

#include "caca_types.h"

#include "mygetopt.h"

int   myoptind = 1;
char *myoptarg = NULL;

/* XXX: this getopt_long implementation should not be trusted for other
 * applications without any serious peer reviewing. It “just works” with
 * zzuf but may fail miserably in other programs. */
int mygetopt(int argc, char * const _argv[], const char *optstring,
             const struct myoption *longopts, int *longindex)
{
    char **argv = (char **)(uintptr_t)_argv;
    char *flag;
    int i;

    if(myoptind >= argc)
        return -1;

    flag = argv[myoptind];

    if(flag[0] == '-' && flag[1] != '-')
    {
        char *tmp;
        int ret = flag[1];

        if(ret == '\0')
            return -1;

        tmp = strchr(optstring, ret);
        if(!tmp || ret == ':')
            return '?';

        myoptind++;
        if(tmp[1] == ':')
        {
            if(flag[2] != '\0')
                myoptarg = flag + 2;
            else
                myoptarg = argv[myoptind++];
            return ret;
        }

        if(flag[2] != '\0')
        {
            flag[1] = '-';
            myoptind--;
            argv[myoptind]++;
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
                myoptind++;
                myoptarg = flag + 2 + l + 1;
                return longopts[i].val;
            case '\0':
                if(longindex)
                    *longindex = i;
                myoptind++;
                if(longopts[i].has_arg)
                    myoptarg = argv[myoptind++];
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
}

