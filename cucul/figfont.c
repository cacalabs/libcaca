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

/*
 *  FIXME: this file needs huge cleanup to be usable
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#   include <stdlib.h>
#   include <string.h>
#endif

#include "cucul.h"
#include "cucul_internals.h"

struct cucul_figfont
{
    int term_width;
    int x, y, w, h, lines;

    enum { H_DEFAULT, H_KERN, H_SMUSH, H_NONE, H_OVERLAP } hmode;
    int hsmushrule;
    uint32_t hardblank;
    int height, baseline, max_length;
    int old_layout;
    int print_direction, full_layout, codetag_count;
    int glyphs;
    cucul_canvas_t *fontcv, *charcv;
    int *left, *right; /* Unused yet */
    uint32_t *lookup;
};

static uint32_t hsmush(uint32_t ch1, uint32_t ch2, int rule);
static cucul_figfont_t * open_figfont(char const *);
static int free_figfont(cucul_figfont_t *);

int cucul_canvas_set_figfont(cucul_canvas_t *cv, char const *path)
{
    cucul_figfont_t *ff = NULL;

    if(path)
    {
        ff = open_figfont(path);
        if(!ff)
            return -1;
    }

    if(cv->ff)
    {
        cucul_free_canvas(cv->ff->charcv);
        free(cv->ff->left);
        free(cv->ff->right);
        free_figfont(cv->ff);
    }

    cv->ff = ff;

    if(!path)
        return 0;

    /* from TOIlet’s main.c */
    ff->term_width = 80;
    ff->hmode = H_DEFAULT;

    /* from TOIlet’s render.c */
    ff->x = ff->y = 0;
    ff->w = ff->h = 0;
    ff->lines = 0;
    cucul_set_canvas_size(cv, 0, 0); /* XXX */

    /* from TOIlet’s figlet.c */
    if(ff->full_layout & 0x3f)
        ff->hsmushrule = ff->full_layout & 0x3f;
    else if(ff->old_layout > 0)
        ff->hsmushrule = ff->old_layout;

    switch(ff->hmode)
    {
    case H_DEFAULT:
        if(ff->old_layout == -1)
            ff->hmode = H_NONE;
        else if(ff->old_layout == 0 && (ff->full_layout & 0xc0) == 0x40)
            ff->hmode = H_KERN;
        else if((ff->old_layout & 0x3f) && (ff->full_layout & 0x3f)
                 && (ff->full_layout & 0x80))
        {
            ff->hmode = H_SMUSH;
            ff->hsmushrule = ff->full_layout & 0x3f;
        }
        else if(ff->old_layout == 0 && (ff->full_layout & 0xbf) == 0x80)
        {
            ff->hmode = H_SMUSH;
            ff->hsmushrule = 0x3f;
        }
        else
            ff->hmode = H_OVERLAP;
        break;
    default:
        break;
    }

    ff->charcv = cucul_create_canvas(ff->max_length - 2, ff->height);

    ff->left = malloc(ff->height * sizeof(int));
    ff->right = malloc(ff->height * sizeof(int));

    cv->ff = ff;

    return 0;
}

int cucul_put_figchar(cucul_canvas_t *cv, uint32_t ch)
{
    cucul_figfont_t *ff = cv->ff;
    int c, w, h, x, y, overlap, extra, xleft, xright;

    switch(ch)
    {
        case (uint32_t)'\r':
            return 0;
        case (uint32_t)'\n':
            ff->x = 0;
            ff->y += ff->height;
            return 0;
        /* FIXME: handle '\t' */
    }

    /* Look whether our glyph is available */
    for(c = 0; c < ff->glyphs; c++)
        if(ff->lookup[c * 2] == ch)
            break;

    if(c == ff->glyphs)
        return 0;

    w = ff->lookup[c * 2 + 1];
    h = ff->height;

    cucul_set_canvas_handle(ff->fontcv, 0, c * ff->height);
    cucul_blit(ff->charcv, 0, 0, ff->fontcv, NULL);

    /* Check whether we reached the end of the screen */
    if(ff->x && ff->x + w > ff->term_width)
    {
        ff->x = 0;
        ff->y += h;
    }

    /* Compute how much the next character will overlap */
    switch(ff->hmode)
    {
    case H_SMUSH:
    case H_KERN:
    case H_OVERLAP:
        extra = (ff->hmode == H_OVERLAP);
        overlap = w;
        for(y = 0; y < h; y++)
        {
            /* Compute how much spaces we can eat from the new glyph */
            for(xright = 0; xright < overlap; xright++)
                if(cucul_get_char(ff->charcv, xright, y) != ' ')
                    break;

            /* Compute how much spaces we can eat from the previous glyph */
            for(xleft = 0; xright + xleft < overlap && xleft < ff->x; xleft++)
                if(cucul_get_char(cv, ff->x - 1 - xleft, ff->y + y) != ' ')
                    break;

            /* Handle overlapping */
            if(ff->hmode == H_OVERLAP && xleft < ff->x)
                xleft++;

            /* Handle smushing */
            if(ff->hmode == H_SMUSH)
            {
                if(xleft < ff->x &&
                    hsmush(cucul_get_char(cv, ff->x - 1 - xleft, ff->y + y),
                          cucul_get_char(ff->charcv, xright, y),
                          ff->hsmushrule))
                    xleft++;
            }

            if(xleft + xright < overlap)
                overlap = xleft + xright;
        }
        break;
    case H_NONE:
        overlap = 0;
        break;
    default:
        return -1;
    }

    /* Check whether the current canvas is large enough */
    if(ff->x + w - overlap > ff->w)
        ff->w = ff->x + w - overlap < ff->term_width
              ? ff->x + w - overlap : ff->term_width;

    if(ff->y + h > ff->h)
        ff->h = ff->y + h;

#if 0 /* deactivated for libcaca insertion */
    if(attr)
        cucul_set_attr(cv, attr);
#endif
    cucul_set_canvas_size(cv, ff->w, ff->h);

    /* Render our char (FIXME: create a rect-aware cucul_blit_canvas?) */
    for(y = 0; y < h; y++)
        for(x = 0; x < w; x++)
    {
        uint32_t ch1, ch2;
        //uint32_t tmpat = cucul_get_attr(ff->fontcv, x, y + c * ff->height);
        ch2 = cucul_get_char(ff->charcv, x, y);
        if(ch2 == ' ')
            continue;
        ch1 = cucul_get_char(cv, ff->x + x - overlap, ff->y + y);
        /* FIXME: this could be changed to cucul_put_attr() when the
         * function is fixed in libcucul */
        //cucul_set_attr(cv, tmpat);
        if(ch1 == ' ' || ff->hmode != H_SMUSH)
            cucul_put_char(cv, ff->x + x - overlap, ff->y + y, ch2);
        else
            cucul_put_char(cv, ff->x + x - overlap, ff->y + y,
                           hsmush(ch1, ch2, ff->hsmushrule));
        //cucul_put_attr(cv, ff->x + x, ff->y + y, tmpat);
    }

    /* Advance cursor */
    ff->x += w - overlap;

    return 0;
}

static int flush_figlet(cucul_canvas_t *cv)
{
    cucul_figfont_t *ff = cv->ff;
    int x, y;

    //ff->torender = cv;
    //cucul_set_canvas_size(ff->torender, ff->w, ff->h);
    cucul_set_canvas_size(cv, ff->w, ff->h);

    /* FIXME: do this somewhere else, or record hardblank positions */
    for(y = 0; y < ff->h; y++)
        for(x = 0; x < ff->w; x++)
            if(cucul_get_char(cv, x, y) == 0xa0)
            {
                uint32_t attr = cucul_get_attr(cv, x, y);
                cucul_put_char(cv, x, y, ' ');
                cucul_put_attr(cv, x, y, attr);
            }

    ff->x = ff->y = 0;
    ff->w = ff->h = 0;

    //cv = cucul_create_canvas(1, 1); /* XXX */

    /* from render.c */
    ff->lines += cucul_get_canvas_height(cv);

    return 0;
}

#define STD_GLYPHS (127 - 32)
#define EXT_GLYPHS (STD_GLYPHS + 7)

cucul_figfont_t * open_figfont(char const *path)
{
    char altpath[2048];
    char buf[2048];
    char hardblank[10];
    cucul_figfont_t *ff;
    char *data = NULL;
    cucul_file_t *f;
    int i, j, size, comment_lines;

    ff = malloc(sizeof(cucul_figfont_t));
    if(!ff)
    {
        seterrno(ENOMEM);
        return NULL;
    }

    /* Open font: if not found, try .tlf, then .flf */
    f = _cucul_file_open(path, "r");
#if !defined __KERNEL__ && defined HAVE_SNPRINTF

#if (! defined(snprintf)) && ( defined(_WIN32) || defined(WIN32) ) && (! defined(__CYGWIN__))
#define snprintf _snprintf
#endif

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
#endif
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
        uint32_t ch, oldch = 0;

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

int free_figfont(cucul_figfont_t *ff)
{
    cucul_free_canvas(ff->fontcv);
    free(ff->lookup);
    free(ff);

    return 0;
}

static uint32_t hsmush(uint32_t ch1, uint32_t ch2, int rule)
{
    /* Rule 1 */
    if((rule & 0x01) && ch1 == ch2 && ch1 != 0xa0)
        return ch2;

    if(ch1 < 0x80 && ch2 < 0x80)
    {
        char const charlist[] = "|/\\[]{}()<>";
        char *tmp1, *tmp2;

        /* Rule 2 */
        if(rule & 0x02)
        {
            if(ch1 == '_' && strchr(charlist, ch2))
                return ch2;

            if(ch2 == '_' && strchr(charlist, ch1))
                return ch1;
        }

        /* Rule 3 */
        if((rule & 0x04) &&
           (tmp1 = strchr(charlist, ch1)) && (tmp2 = strchr(charlist, ch2)))
        {
            int cl1 = (tmp1 + 1 - charlist) / 2;
            int cl2 = (tmp2 + 1 - charlist) / 2;

            if(cl1 < cl2)
                return ch2;
            if(cl1 > cl2)
                return ch1;
        }

        /* Rule 4 */
        if(rule & 0x08)
        {
            uint16_t s = ch1 + ch2;
            uint16_t p = ch1 * ch2;

            if(p == 15375 /* '{' * '}' */
                || p == 8463 /* '[' * ']' */
                || (p == 1640 && s == 81)) /* '(' *|+ ')' */
                return '|';
        }

        /* Rule 5 */
        if(rule & 0x10)
        {
            switch((ch1 << 8) | ch2)
            {
                case 0x2f5c: return '|'; /* /\ */
                case 0x5c2f: return 'Y'; /* \/ */
                case 0x3e3c: return 'X'; /* >< */
            }
        }

        /* Rule 6 */
        if((rule & 0x20) && ch1 == ch2 && ch1 == 0xa0)
            return 0xa0;
    }

    return 0;
}

