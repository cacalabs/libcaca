/*
 *  libcucul      Canvas for ultrafast compositing of Unicode letters
 *  Copyright (c) 2006-2007 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/*
 *  This file contains FIGlet and TOIlet font handling functions.
 */

#include "config.h"
#include "common.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#   include <stdlib.h>
#   include <string.h>
#endif

#include "cucul.h"
#include "cucul_internals.h"

struct cucul_figfont
{
    /* Used by the FIGlet driver */
    unsigned long int hardblank;
    unsigned int height, baseline, max_length;
    int old_layout;
    unsigned int print_direction, full_layout, codetag_count;
    unsigned int glyphs;
    cucul_canvas_t *fontcv;
    unsigned int *lookup;
};

int cucul_canvas_set_figfont(cucul_canvas_t *cv, char const *path)
{
    cucul_figfont_t *ff = NULL;

    if(path)
    {
        ff = _cucul_open_figfont(path);
        if(!ff)
            return -1;
    }

    if(cv->ff)
        _cucul_free_figfont(cv->ff);

    cv->ff = ff;

    return 0;
}

#define STD_GLYPHS (127 - 32)
#define EXT_GLYPHS (STD_GLYPHS + 7)

cucul_figfont_t * _cucul_open_figfont(char const *path)
{
    char altpath[2048];
    char buf[2048];
    char hardblank[10];
    cucul_figfont_t *ff;
    char *data = NULL;
    cucul_file_t *f;
    unsigned int i, j, size, comment_lines;

    ff = malloc(sizeof(cucul_figfont_t));
    if(!ff)
    {
        seterrno(ENOMEM);
        return NULL;
    }

    /* Open font: if not found, try .tlf, then .flf */
    f = _cucul_file_open(path, "r");
    if(!f)
    {
        snprintf(altpath, 2047, "%s.tlf", path);
        altpath[2047] = '\0';
        f = _cucul_file_open(altpath, "r");
    }
    if(!f)
    {
        snprintf(altpath, 2047, "%s.flf", path);
        altpath[2047] = '\0';
        f = _cucul_file_open(altpath, "r");
    }
    if(!f)
    {
        free(ff);
        seterrno(ENOENT);
        return NULL;
    }

    /* Read header */
    ff->print_direction = 0;
    ff->full_layout = 0;
    ff->codetag_count = 0;
    _cucul_file_gets(buf, 2048, f);
    if(sscanf(buf, "%*[ft]lf2a%6s %u %u %u %i %u %u %u %u\n", hardblank,
              &ff->height, &ff->baseline, &ff->max_length,
              &ff->old_layout, &comment_lines, &ff->print_direction,
              &ff->full_layout, &ff->codetag_count) < 6)
    {
        debug("figfont error: `%s' has invalid header: %s", path, buf);
        _cucul_file_close(f);
        free(ff);
        seterrno(EINVAL);
        return NULL;
    }

    if(ff->old_layout < -1 || ff->old_layout > 63 || ff->full_layout > 32767
        || ((ff->full_layout & 0x80) && (ff->full_layout & 0x3f) == 0
            && ff->old_layout))
    {
        debug("figfont error: `%s' has invalid layout %i/%u",
                path, ff->old_layout, ff->full_layout);
        _cucul_file_close(f);
        free(ff);
        seterrno(EINVAL);
        return NULL;
    }

    ff->hardblank = cucul_utf8_to_utf32(hardblank, NULL);

    /* Skip comment lines */
    for(i = 0; i < comment_lines; i++)
        _cucul_file_gets(buf, 2048, f);

    /* Read mandatory characters (32-127, 196, 214, 220, 228, 246, 252, 223)
     * then read additional characters. */
    ff->glyphs = 0;
    ff->lookup = NULL;

    for(i = 0, size = 0; !_cucul_file_eof(f); ff->glyphs++)
    {
        if((ff->glyphs % 2048) == 0)
            ff->lookup = realloc(ff->lookup,
                                   (ff->glyphs + 2048) * 2 * sizeof(int));

        if(ff->glyphs < STD_GLYPHS)
        {
            ff->lookup[ff->glyphs * 2] = 32 + ff->glyphs;
        }
        else if(ff->glyphs < EXT_GLYPHS)
        {
            static int const tab[7] = { 196, 214, 220, 228, 246, 252, 223 };
            ff->lookup[ff->glyphs * 2] = tab[ff->glyphs - STD_GLYPHS];
        }
        else
        {
            if(_cucul_file_gets(buf, 2048, f) == NULL)
                break;

            /* Ignore blank lines, as in jacky.flf */
            if(buf[0] == '\n' || buf[0] == '\r')
                continue;

            /* Ignore negative indices for now, as in ivrit.flf */
            if(buf[0] == '-')
            {
                for(j = 0; j < ff->height; j++)
                    _cucul_file_gets(buf, 2048, f);
                continue;
            }

            if(!buf[0] || buf[0] < '0' || buf[0] > '9')
            {
                debug("figfont error: glyph #%u in `%s'", ff->glyphs, path);
                free(data);
                free(ff->lookup);
                free(ff);
                seterrno(EINVAL);
                return NULL;
            }

            if(buf[1] == 'x')
                sscanf(buf, "%x", &ff->lookup[ff->glyphs * 2]);
            else
                sscanf(buf, "%u", &ff->lookup[ff->glyphs * 2]);
        }

        ff->lookup[ff->glyphs * 2 + 1] = 0;

        for(j = 0; j < ff->height; j++)
        {
            if(i + 2048 >= size)
                data = realloc(data, size += 2048);

            _cucul_file_gets(data + i, 2048, f);
            i = (uintptr_t)strchr(data + i, 0) - (uintptr_t)data;
        }
    }

    _cucul_file_close(f);

    if(ff->glyphs < EXT_GLYPHS)
    {
        debug("figfont error: only %u glyphs in `%s', expected at least %u",
                        ff->glyphs, path, EXT_GLYPHS);
        free(data);
        free(ff->lookup);
        free(ff);
        seterrno(EINVAL);
        return NULL;
    }

    /* Import buffer into canvas */
    ff->fontcv = cucul_create_canvas(0, 0);
    cucul_import_memory(ff->fontcv, data, i, "utf8");
    free(data);

    /* Remove EOL characters. For now we ignore hardblanks, don’t do any
     * smushing, nor any kind of error checking. */
    for(j = 0; j < ff->height * ff->glyphs; j++)
    {
        unsigned long int ch, oldch = 0;

        for(i = ff->max_length; i--;)
        {
            ch = cucul_get_char(ff->fontcv, i, j);

            /* Replace hardblanks with U+00A0 NO-BREAK SPACE */
            if(ch == ff->hardblank)
                cucul_put_char(ff->fontcv, i, j, ch = 0xa0);

            if(oldch && ch != oldch)
            {
                if(!ff->lookup[j / ff->height * 2 + 1])
                    ff->lookup[j / ff->height * 2 + 1] = i + 1;
            }
            else if(oldch && ch == oldch)
                cucul_put_char(ff->fontcv, i, j, ' ');
            else if(ch != ' ')
            {
                oldch = ch;
                cucul_put_char(ff->fontcv, i, j, ' ');
            }
        }
    }

    return ff;
}

int _cucul_free_figfont(cucul_figfont_t *ff)
{
    cucul_free_canvas(ff->fontcv);
    free(ff->lookup);
    free(ff);

    return 0;
}

