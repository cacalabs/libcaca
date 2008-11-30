/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
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
 *  This file contains horizontal and vertical flipping routines.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdlib.h>
#endif

#include "caca.h"
#include "caca_internals.h"

static uint32_t flipchar(uint32_t ch);
static uint32_t flopchar(uint32_t ch);
static uint32_t rotatechar(uint32_t ch);
static uint32_t leftchar(uint32_t ch);
static uint32_t rightchar(uint32_t ch);
static void leftpair(uint32_t pair[2]);
static void rightpair(uint32_t pair[2]);

/** \brief Invert a canvas' colours.
 *
 *  Invert a canvas' colours (black becomes white, red becomes cyan, etc.)
 *  without changing the characters in it.
 *
 *  This function never fails.
 *
 *  \param cv The canvas to invert.
 *  \return This function always returns 0.
 */
int caca_invert(caca_canvas_t *cv)
{
    uint32_t *attrs = cv->attrs;
    int i;

    for(i = cv->height * cv->width; i--; )
    {
        *attrs = *attrs ^ 0x000f000f;
        attrs++;
    }

    return 0;
}

/** \brief Flip a canvas horizontally.
 *
 *  Flip a canvas horizontally, choosing characters that look like the
 *  mirrored version wherever possible. Some characters will stay
 *  unchanged by the process, but the operation is guaranteed to be
 *  involutive: performing it again gives back the original canvas.
 *
 *  This function never fails.
 *
 *  \param cv The canvas to flip.
 *  \return This function always returns 0.
 */
int caca_flip(caca_canvas_t *cv)
{
    int y;

    for(y = 0; y < cv->height; y++)
    {
        uint32_t *cleft = cv->chars + y * cv->width;
        uint32_t *cright = cleft + cv->width - 1;
        uint32_t *aleft = cv->attrs + y * cv->width;
        uint32_t *aright = aleft + cv->width - 1;

        while(cleft < cright)
        {
            uint32_t ch;
            uint32_t attr;

            /* Swap attributes */
            attr = *aright;
            *aright-- = *aleft;
            *aleft++ = attr;

            /* Swap characters */
            ch = *cright;
            *cright-- = flipchar(*cleft);
            *cleft++ = flipchar(ch);
        }

        if(cleft == cright)
            *cleft = flipchar(*cleft);

        /* Fix fullwidth characters. Could it be done in one loop? */
        cleft = cv->chars + y * cv->width;
        cright = cleft + cv->width - 1;
        for( ; cleft < cright; cleft++)
        {
            if(cleft[0] == CACA_MAGIC_FULLWIDTH)
            {
                cleft[0] = cleft[1];
                cleft[1] = CACA_MAGIC_FULLWIDTH;
                cleft++;
            }
        }
    }

    return 0;
}

/** \brief Flip a canvas vertically.
 *
 *  Flip a canvas vertically, choosing characters that look like the
 *  mirrored version wherever possible. Some characters will stay
 *  unchanged by the process, but the operation is guaranteed to be
 *  involutive: performing it again gives back the original canvas.
 *
 *  This function never fails.
 *
 *  \param cv The canvas to flop.
 *  \return This function always returns 0.
 */
int caca_flop(caca_canvas_t *cv)
{
    int x;

    for(x = 0; x < cv->width; x++)
    {
        uint32_t *ctop = cv->chars + x;
        uint32_t *cbottom = ctop + cv->width * (cv->height - 1);
        uint32_t *atop = cv->attrs + x;
        uint32_t *abottom = atop + cv->width * (cv->height - 1);

        while(ctop < cbottom)
        {
            uint32_t ch;
            uint32_t attr;

            /* Swap attributes */
            attr = *abottom; *abottom = *atop; *atop = attr;

            /* Swap characters */
            ch = *cbottom; *cbottom = flopchar(*ctop); *ctop = flopchar(ch);

            ctop += cv->width; cbottom -= cv->width;
            atop += cv->width; abottom -= cv->width;
        }

        if(ctop == cbottom)
            *ctop = flopchar(*ctop);
    }

    return 0;
}

/** \brief Rotate a canvas.
 *
 *  Apply a 180-degree transformation to a canvas, choosing characters
 *  that look like the upside-down version wherever possible. Some
 *  characters will stay unchanged by the process, but the operation is
 *  guaranteed to be involutive: performing it again gives back the
 *  original canvas.
 *
 *  This function never fails.
 *
 *  \param cv The canvas to rotate.
 *  \return This function always returns 0.
 */
int caca_rotate_180(caca_canvas_t *cv)
{
    uint32_t *cbegin = cv->chars;
    uint32_t *cend = cbegin + cv->width * cv->height - 1;
    uint32_t *abegin = cv->attrs;
    uint32_t *aend = abegin + cv->width * cv->height - 1;
    int y;

    while(cbegin < cend)
    {
        uint32_t ch;
        uint32_t attr;

        /* Swap attributes */
        attr = *aend; *aend = *abegin; *abegin = attr;

        /* Swap characters */
        ch = *cend; *cend = rotatechar(*cbegin); *cbegin = rotatechar(ch);

        cbegin++; cend--; abegin++; aend--;
    }

    if(cbegin == cend)
        *cbegin = rotatechar(*cbegin);

    /* Fix fullwidth characters. Could it be done in one loop? */
    for(y = 0; y < cv->height; y++)
    {
        cbegin = cv->chars + y * cv->width;
        cend = cbegin + cv->width - 1;
        for( ; cbegin < cend; cbegin++)
        {
            if(cbegin[0] == CACA_MAGIC_FULLWIDTH)
            {
                cbegin[0] = cbegin[1];
                cbegin[1] = CACA_MAGIC_FULLWIDTH;
                cbegin++;
            }
        }
    }

    return 0;
}

/** \brief Rotate a canvas, 90 degrees counterclockwise.
 *
 *  Apply a 90-degree transformation to a canvas, choosing characters
 *  that look like the rotated version wherever possible. Characters cells
 *  are rotated two-by-two. Some characters will stay unchanged by the
 *  process, some others will be replaced by close equivalents. Fullwidth
 *  characters at odd horizontal coordinates will be lost. The operation is
 *  not guaranteed to be reversible at all.
 *
 *  Note that the width of the canvas is divided by two and becomes the
 *  new height. Height is multiplied by two and becomes the new width. If
 *  the original width is an odd number, the division is rounded up.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EBUSY The canvas is in use by a display driver and cannot be rotated.
 *  - \c ENOMEM Not enough memory to allocate the new canvas size. If this
 *    happens, the previous canvas handle is still valid.
 *
 *  \param cv The canvas to rotate left.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int caca_rotate_left(caca_canvas_t *cv)
{
    uint32_t *newchars, *newattrs;
    int x, y, w2, h2;

    if(cv->refcount)
    {
        seterrno(EBUSY);
        return -1;
    }

    /* Save the current frame shortcuts */
    _caca_save_frame_info(cv);

    w2 = (cv->width + 1) / 2;
    h2 = cv->height;

    newchars = malloc(w2 * h2 * 2 * sizeof(uint32_t));
    if(!newchars)
    {
        seterrno(ENOMEM);
        return -1;
    }

    newattrs = malloc(w2 * h2 * 2 * sizeof(uint32_t));
    if(!newattrs)
    {
        free(newchars);
        seterrno(ENOMEM);
        return -1;
    }

    for(y = 0; y < h2; y++)
    {
        for(x = 0; x < w2; x++)
        {
            uint32_t pair[2], attr1, attr2;

            pair[0] = cv->chars[cv->width * y + x * 2];
            attr1 = cv->attrs[cv->width * y + x * 2];

            if((cv->width & 1) && x == w2 - 1)
            {
                /* Special case: odd column */
                pair[1] = ' ';
                attr2 = attr1;
            }
            else
            {
                pair[1] = cv->chars[cv->width * y + x * 2 + 1];
                attr2 = cv->attrs[cv->width * y + x * 2 + 1];
            }

            /* If one of the characters is a space, we simply ignore
             * its colour attributes. Otherwise the resulting characters
             * may have totally wrong colours. */
            if(pair[0] == ' ')
                attr1 = attr2;
            else if(pair[1] == ' ')
                attr2 = attr1;

            leftpair(pair);

            newchars[(h2 * (w2 - 1 - x) + y) * 2] = pair[0];
            newattrs[(h2 * (w2 - 1 - x) + y) * 2] = attr1;
            newchars[(h2 * (w2 - 1 - x) + y) * 2 + 1] = pair[1];
            newattrs[(h2 * (w2 - 1 - x) + y) * 2 + 1] = attr2;
        }
    }

    free(cv->chars);
    free(cv->attrs);

    /* Swap X and Y information */
    x = cv->frames[cv->frame].x;
    y = cv->frames[cv->frame].y;
    cv->frames[cv->frame].x = y * 2;
    cv->frames[cv->frame].y = (cv->width - 1 - x) / 2;

    x = cv->frames[cv->frame].handlex;
    y = cv->frames[cv->frame].handley;
    cv->frames[cv->frame].handlex = y * 2;
    cv->frames[cv->frame].handley = (cv->width - 1 - x) / 2;

    cv->frames[cv->frame].width = cv->height * 2;
    cv->frames[cv->frame].height = (cv->width + 1) / 2;

    cv->frames[cv->frame].chars = newchars;
    cv->frames[cv->frame].attrs = newattrs;

    /* Reset the current frame shortcuts */
    _caca_load_frame_info(cv);

    return 0;
}

/** \brief Rotate a canvas, 90 degrees counterclockwise.
 *
 *  Apply a 90-degree transformation to a canvas, choosing characters
 *  that look like the rotated version wherever possible. Characters cells
 *  are rotated two-by-two. Some characters will stay unchanged by the
 *  process, some others will be replaced by close equivalents. Fullwidth
 *  characters at odd horizontal coordinates will be lost. The operation is
 *  not guaranteed to be reversible at all.
 *
 *  Note that the width of the canvas is divided by two and becomes the
 *  new height. Height is multiplied by two and becomes the new width. If
 *  the original width is an odd number, the division is rounded up.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EBUSY The canvas is in use by a display driver and cannot be rotated.
 *  - \c ENOMEM Not enough memory to allocate the new canvas size. If this
 *    happens, the previous canvas handle is still valid.
 *
 *  \param cv The canvas to rotate right.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int caca_rotate_right(caca_canvas_t *cv)
{
    uint32_t *newchars, *newattrs;
    int x, y, w2, h2;

    if(cv->refcount)
    {
        seterrno(EBUSY);
        return -1;
    }

    /* Save the current frame shortcuts */
    _caca_save_frame_info(cv);

    w2 = (cv->width + 1) / 2;
    h2 = cv->height;

    newchars = malloc(w2 * h2 * 2 * sizeof(uint32_t));
    if(!newchars)
    {
        seterrno(ENOMEM);
        return -1;
    }

    newattrs = malloc(w2 * h2 * 2 * sizeof(uint32_t));
    if(!newattrs)
    {
        free(newchars);
        seterrno(ENOMEM);
        return -1;
    }

    for(y = 0; y < h2; y++)
    {
        for(x = 0; x < w2; x++)
        {
            uint32_t pair[2], attr1, attr2;

            pair[0] = cv->chars[cv->width * y + x * 2];
            attr1 = cv->attrs[cv->width * y + x * 2];

            if((cv->width & 1) && x == w2 - 1)
            {
                /* Special case: odd column */
                pair[1] = ' ';
                attr2 = attr1;
            }
            else
            {
                pair[1] = cv->chars[cv->width * y + x * 2 + 1];
                attr2 = cv->attrs[cv->width * y + x * 2 + 1];
            }

            /* If one of the characters is a space, we simply ignore
             * its colour attributes. Otherwise the resulting characters
             * may have totally wrong colours. */
            if(pair[0] == ' ')
                attr1 = attr2;
            else if(pair[1] == ' ')
                attr2 = attr1;

            rightpair(pair);

            newchars[(h2 * x + h2 - 1 - y) * 2] = pair[0];
            newattrs[(h2 * x + h2 - 1 - y) * 2] = attr1;
            newchars[(h2 * x + h2 - 1 - y) * 2 + 1] = pair[1];
            newattrs[(h2 * x + h2 - 1 - y) * 2 + 1] = attr2;
        }
    }

    free(cv->chars);
    free(cv->attrs);

    /* Swap X and Y information */
    x = cv->frames[cv->frame].x;
    y = cv->frames[cv->frame].y;
    cv->frames[cv->frame].x = (cv->height - 1 - y) * 2;
    cv->frames[cv->frame].y = x / 2;

    x = cv->frames[cv->frame].handlex;
    y = cv->frames[cv->frame].handley;
    cv->frames[cv->frame].handlex = (cv->height - 1 - y) * 2;
    cv->frames[cv->frame].handley = x / 2;

    cv->frames[cv->frame].width = cv->height * 2;
    cv->frames[cv->frame].height = (cv->width + 1) / 2;

    cv->frames[cv->frame].chars = newchars;
    cv->frames[cv->frame].attrs = newattrs;

    /* Reset the current frame shortcuts */
    _caca_load_frame_info(cv);

    return 0;
}

/** \brief Rotate and stretch a canvas, 90 degrees counterclockwise.
 *
 *  Apply a 90-degree transformation to a canvas, choosing characters
 *  that look like the rotated version wherever possible. Some characters
 *  will stay unchanged by the process, some others will be replaced by
 *  close equivalents. Fullwidth characters will be lost. The operation is
 *  not guaranteed to be reversible at all.
 *
 *  Note that the width and height of the canvas are swapped, causing its
 *  aspect ratio to look stretched.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EBUSY The canvas is in use by a display driver and cannot be rotated.
 *  - \c ENOMEM Not enough memory to allocate the new canvas size. If this
 *    happens, the previous canvas handle is still valid.
 *
 *  \param cv The canvas to rotate left.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int caca_stretch_left(caca_canvas_t *cv)
{
    uint32_t *newchars, *newattrs;
    int x, y;

    if(cv->refcount)
    {
        seterrno(EBUSY);
        return -1;
    }

    /* Save the current frame shortcuts */
    _caca_save_frame_info(cv);

    newchars = malloc(cv->width * cv->height * sizeof(uint32_t));
    if(!newchars)
    {
        seterrno(ENOMEM);
        return -1;
    }

    newattrs = malloc(cv->width * cv->height * sizeof(uint32_t));
    if(!newattrs)
    {
        free(newchars);
        seterrno(ENOMEM);
        return -1;
    }

    for(y = 0; y < cv->height; y++)
    {
        for(x = 0; x < cv->width; x++)
        {
            uint32_t ch, attr;

            ch = cv->chars[cv->width * y + x];
            attr = cv->attrs[cv->width * y + x];

            /* FIXME: do something about fullwidth characters */
            ch = leftchar(ch);

            newchars[cv->height * (cv->width - 1 - x) + y] = ch;
            newattrs[cv->height * (cv->width - 1 - x) + y] = attr;
        }
    }

    free(cv->chars);
    free(cv->attrs);

    /* Swap X and Y information */
    x = cv->frames[cv->frame].x;
    y = cv->frames[cv->frame].y;
    cv->frames[cv->frame].x = y;
    cv->frames[cv->frame].y = cv->width - 1 - x;

    x = cv->frames[cv->frame].handlex;
    y = cv->frames[cv->frame].handley;
    cv->frames[cv->frame].handlex = y;
    cv->frames[cv->frame].handley = cv->width - 1 - x;

    cv->frames[cv->frame].width = cv->height;
    cv->frames[cv->frame].height = cv->width;

    cv->frames[cv->frame].chars = newchars;
    cv->frames[cv->frame].attrs = newattrs;

    /* Reset the current frame shortcuts */
    _caca_load_frame_info(cv);

    return 0;
}

/** \brief Rotate and stretch a canvas, 90 degrees clockwise.
 *
 *  Apply a 270-degree transformation to a canvas, choosing characters
 *  that look like the rotated version wherever possible. Some characters
 *  will stay unchanged by the process, some others will be replaced by
 *  close equivalents. Fullwidth characters will be lost. The operation is
 *  not guaranteed to be reversible at all.
 *
 *  Note that the width and height of the canvas are swapped, causing its
 *  aspect ratio to look stretched.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EBUSY The canvas is in use by a display driver and cannot be rotated.
 *  - \c ENOMEM Not enough memory to allocate the new canvas size. If this
 *    happens, the previous canvas handle is still valid.
 *
 *  \param cv The canvas to rotate right.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int caca_stretch_right(caca_canvas_t *cv)
{
    uint32_t *newchars, *newattrs;
    int x, y;

    if(cv->refcount)
    {
        seterrno(EBUSY);
        return -1;
    }

    /* Save the current frame shortcuts */
    _caca_save_frame_info(cv);

    newchars = malloc(cv->width * cv->height * sizeof(uint32_t));
    if(!newchars)
    {
        seterrno(ENOMEM);
        return -1;
    }

    newattrs = malloc(cv->width * cv->height * sizeof(uint32_t));
    if(!newattrs)
    {
        free(newchars);
        seterrno(ENOMEM);
        return -1;
    }

    for(y = 0; y < cv->height; y++)
    {
        for(x = 0; x < cv->width; x++)
        {
            uint32_t ch, attr;

            ch = cv->chars[cv->width * y + x];
            attr = cv->attrs[cv->width * y + x];

            /* FIXME: do something about fullwidth characters */
            ch = rightchar(ch);

            newchars[cv->height * x + cv->height - 1 - y] = ch;
            newattrs[cv->height * x + cv->height - 1 - y] = attr;
        }
    }

    free(cv->chars);
    free(cv->attrs);

    /* Swap X and Y information */
    x = cv->frames[cv->frame].x;
    y = cv->frames[cv->frame].y;
    cv->frames[cv->frame].x = cv->height - 1 - y;
    cv->frames[cv->frame].y = x;

    x = cv->frames[cv->frame].handlex;
    y = cv->frames[cv->frame].handley;
    cv->frames[cv->frame].handlex = cv->height - 1 - y;
    cv->frames[cv->frame].handley = x;

    cv->frames[cv->frame].width = cv->height;
    cv->frames[cv->frame].height = cv->width;

    cv->frames[cv->frame].chars = newchars;
    cv->frames[cv->frame].attrs = newattrs;

    /* Reset the current frame shortcuts */
    _caca_load_frame_info(cv);

    return 0;
}

/* FIXME: as the lookup tables grow bigger, use a log(n) lookup instead
 * of linear lookup. */
static uint32_t flipchar(uint32_t ch)
{
    int i;

    static uint32_t const noflip[] =
    {
         /* ASCII */
         ' ', '"', '#', '\'', '-', '.', '*', '+', ':', '=', '0', '8',
         'A', 'H', 'I', 'M', 'O', 'T', 'U', 'V', 'W', 'X', 'Y', '^',
         '_', 'i', 'o', 'v', 'w', 'x', '|',
         /* CP437 and box drawing */
         0x2591, 0x2592, 0x2593, 0x2588, 0x2584, 0x2580, /* ░ ▒ ▓ █ ▄ ▀ */
         0x2500, 0x2501, 0x2503, 0x2503, 0x253c, 0x254b, /* ─ ━ │ ┃ ┼ ╋ */
         0x252c, 0x2534, 0x2533, 0x253b, 0x2566, 0x2569, /* ┬ ┴ ┳ ┻ ╦ ╩ */
         0x2550, 0x2551, 0x256c, /* ═ ║ ╬ */
         0x2575, 0x2577, 0x2579, 0x257b, /* ╵ ╷ ╹ ╻ */
         0
    };

    static uint32_t const pairs[] =
    {
         /* ASCII */
         '(', ')',
         '/', '\\',
         '<', '>',
         '[', ']',
         'b', 'd',
         'p', 'q',
         '{', '}',
         /* ASCII-Unicode */
         ';', 0x204f, /* ; ⁏ */
         '`', 0x00b4, /* ` ´ */
         ',', 0x02ce, /* , ˎ */
         '1', 0x07c1, /* 1 ߁ */
         'C', 0x03fd, /* C Ͻ */
         'E', 0x018e, /* E Ǝ */
         'L', 0x2143, /* L ⅃ */
         'N', 0x0418, /* N И */
         'R', 0x042f, /* R Я */
         'S', 0x01a7, /* S Ƨ */
         'c', 0x0254, /* c ɔ */
         'e', 0x0258, /* e ɘ */
         /* CP437 */
         0x258c, 0x2590, /* ▌ ▐ */
         0x2596, 0x2597, /* ▖ ▗ */
         0x2598, 0x259d, /* ▘ ▝ */
         0x2599, 0x259f, /* ▙ ▟ */
         0x259a, 0x259e, /* ▚ ▞ */
         0x259b, 0x259c, /* ▛ ▜ */
         0x25ba, 0x25c4, /* ► ◄ */
         0x2192, 0x2190, /* → ← */
         0x2310, 0xac,   /* ⌐ ¬ */
         /* Box drawing */
         0x250c, 0x2510, /* ┌ ┐ */
         0x2514, 0x2518, /* └ ┘ */
         0x251c, 0x2524, /* ├ ┤ */
         0x250f, 0x2513, /* ┏ ┓ */
         0x2517, 0x251b, /* ┗ ┛ */
         0x2523, 0x252b, /* ┣ ┫ */
         0x2552, 0x2555, /* ╒ ╕ */
         0x2558, 0x255b, /* ╘ ╛ */
         0x2553, 0x2556, /* ╓ ╖ */
         0x2559, 0x255c, /* ╙ ╜ */
         0x2554, 0x2557, /* ╔ ╗ */
         0x255a, 0x255d, /* ╚ ╝ */
         0x255e, 0x2561, /* ╞ ╡ */
         0x255f, 0x2562, /* ╟ ╢ */
         0x2560, 0x2563, /* ╠ ╣ */
         0x2574, 0x2576, /* ╴ ╶ */
         0x2578, 0x257a, /* ╸ ╺ */
         0
    };

    for(i = 0; noflip[i]; i++)
        if(ch == noflip[i])
            return ch;

    for(i = 0; pairs[i]; i++)
        if(ch == pairs[i])
            return pairs[i ^ 1];

    return ch;
}

static uint32_t flopchar(uint32_t ch)
{
    int i;

    static uint32_t const noflop[] =
    {
         /* ASCII */
         ' ', '(', ')', '*', '+', '-', '0', '3', '8', ':', '<', '=',
         '>', 'B', 'C', 'D', 'E', 'H', 'I', 'K', 'O', 'X', '[', ']',
         'c', 'o', '{', '|', '}',
         /* CP437 and box drawing */
         0x2591, 0x2592, 0x2593, 0x2588, 0x258c, 0x2590, /* ░ ▒ ▓ █ ▌ ▐ */
         0x2500, 0x2501, 0x2503, 0x2503, 0x253c, 0x254b, /* ─ ━ │ ┃ ┼ ╋ */
         0x251c, 0x2524, 0x2523, 0x252b, 0x2560, 0x2563, /* ├ ┤ ┣ ┫ ╠ ╣ */
         0x2550, 0x2551, 0x256c, /* ═ ║ ╬ */
         0x2574, 0x2576, 0x2578, 0x257a, /* ╴ ╶ ╸ ╺ */
         0
    };

    static uint32_t const pairs[] =
    {
         /* ASCII */
         '/', '\\',
         'M', 'W',
         ',', '`',
         'b', 'p',
         'd', 'q',
         'p', 'q',
         'f', 't',
         '.', '\'',
         /* ASCII-Unicode */
         '_', 0x203e, /* _ ‾ */
         '!', 0x00a1, /* ! ¡ */
         'L', 0x0413, /* L Г */
         'N', 0x0418, /* N И */
         'P', 0x042c, /* P Ь */
         'R', 0x0281, /* R ʁ */
         'S', 0x01a7, /* S Ƨ */
         'U', 0x0548, /* U Ո */
         'V', 0x039b, /* V Λ */
         'Y', 0x2144, /* Y ⅄ */
         'h', 0x03bc, /* h μ */
         'i', 0x1d09, /* i ᴉ */
         'j', 0x1e37, /* j ḷ */
         'l', 0x0237, /* l ȷ */
         'v', 0x028c, /* v ʌ */
         'w', 0x028d, /* w ʍ */
         'y', 0x03bb, /* y λ */
         /* Not perfect, but better than nothing */
         '"', 0x201e, /* " „ */
         'm', 0x026f, /* m ɯ */
         'n', 'u',
         /* CP437 */
         0x2584, 0x2580, /* ▄ ▀ */
         0x2596, 0x2598, /* ▖ ▘ */
         0x2597, 0x259d, /* ▗ ▝ */
         0x2599, 0x259b, /* ▙ ▛ */
         0x259f, 0x259c, /* ▟ ▜ */
         0x259a, 0x259e, /* ▚ ▞ */
         /* Box drawing */
         0x250c, 0x2514, /* ┌ └ */
         0x2510, 0x2518, /* ┐ ┘ */
         0x252c, 0x2534, /* ┬ ┴ */
         0x250f, 0x2517, /* ┏ ┗ */
         0x2513, 0x251b, /* ┓ ┛ */
         0x2533, 0x253b, /* ┳ ┻ */
         0x2554, 0x255a, /* ╔ ╚ */
         0x2557, 0x255d, /* ╗ ╝ */
         0x2566, 0x2569, /* ╦ ╩ */
         0x2552, 0x2558, /* ╒ ╘ */
         0x2555, 0x255b, /* ╕ ╛ */
         0x2564, 0x2567, /* ╤ ╧ */
         0x2553, 0x2559, /* ╓ ╙ */
         0x2556, 0x255c, /* ╖ ╜ */
         0x2565, 0x2568, /* ╥ ╨ */
         0x2575, 0x2577, /* ╵ ╷ */
         0x2579, 0x257b, /* ╹ ╻ */
         0
    };

    for(i = 0; noflop[i]; i++)
        if(ch == noflop[i])
            return ch;

    for(i = 0; pairs[i]; i++)
        if(ch == pairs[i])
            return pairs[i ^ 1];

    return ch;
}

static uint32_t rotatechar(uint32_t ch)
{
    int i;

    static uint32_t const norotate[] =
    {
         /* ASCII */
         ' ', '*', '+', '-', '/', '0', '8', ':', '=', 'H', 'I', 'N',
         'O', 'S', 'X', 'Z', '\\', 'o', 's', 'x', 'z', '|',
         /* Unicode */
         0x2591, 0x2592, 0x2593, 0x2588, 0x259a, 0x259e, /* ░ ▒ ▓ █ ▚ ▞ */
         0x2500, 0x2501, 0x2503, 0x2503, 0x253c, 0x254b, /* ─ ━ │ ┃ ┼ ╋ */
         0x2550, 0x2551, 0x256c, /* ═ ║ ╬ */
         0
    };

    static uint32_t const pairs[] =
    {
         /* ASCII */
         '(', ')',
         '<', '>',
         '[', ']',
         '{', '}',
         '.', '\'',
         '6', '9',
         'M', 'W',
         'b', 'q',
         'd', 'p',
         'n', 'u',
         /* ASCII-Unicode */
         '_', 0x203e, /* _ ‾ */
         ',', 0x00b4, /* , ´ */
         '`', 0x02ce, /* ` ˎ */
         '&', 0x214b, /* & ⅋ */
         '!', 0x00a1, /* ! ¡ */
         '?', 0x00bf, /* ? ¿ */
         'C', 0x03fd, /* C Ͻ */
         'E', 0x018e, /* E Ǝ */
         'F', 0x2132, /* F Ⅎ */
         'G', 0x2141, /* G ⅁ */
         'L', 0x2142, /* L ⅂ */
         'U', 0x0548, /* U Ո */
         'V', 0x039b, /* V Λ */
         'Y', 0x2144, /* Y ⅄ */
         'a', 0x0250, /* a ɐ */
         'c', 0x0254, /* c ɔ */
         'e', 0x01dd, /* e ǝ */
         'f', 0x025f, /* f ɟ */
         'g', 0x1d77, /* g ᵷ */
         'h', 0x0265, /* h ɥ */
         'i', 0x1d09, /* i ᴉ */
         'j', 0x1e37, /* j ḷ */
         'k', 0x029e, /* k ʞ */
         'l', 0x0237, /* l ȷ */
         'm', 0x026f, /* m ɯ */
         'r', 0x0279, /* r ɹ */
         't', 0x0287, /* t ʇ */
         'v', 0x028c, /* v ʌ */
         'w', 0x028d, /* w ʍ */
         'y', 0x028e, /* y ʎ */
         /* Unicode-ASCII to match third-party software */
         0x0183, 'g', /* ƃ g */
         0x0259, 'e', /* ə e */
         0x027e, 'j', /* ɾ j */
         0x02d9, '.', /* ˙ . */
         0x05df, 'l', /* ן l */
         /* Not perfect, but better than nothing */
         '"', 0x201e, /* " „ */
         /* Misc Unicode */
         0x00e6, 0x1d02, /* æ ᴂ */
         0x0153, 0x1d14, /* œ ᴔ */
         /* CP437 */
         0x258c, 0x2590, /* ▌ ▐ */
         0x2584, 0x2580, /* ▄ ▀ */
         0x2596, 0x259d, /* ▖ ▝ */
         0x2597, 0x2598, /* ▗ ▘ */
         0x2599, 0x259c, /* ▙ ▜ */
         0x259f, 0x259b, /* ▟ ▛ */
         /* Box drawing */
         0x250c, 0x2518, /* ┌ ┘ */
         0x2510, 0x2514, /* ┐ └ */
         0x251c, 0x2524, /* ├ ┤ */
         0x252c, 0x2534, /* ┬ ┴ */
         0x250f, 0x251b, /* ┏ ┛ */
         0x2513, 0x2517, /* ┓ ┗ */
         0x2523, 0x252b, /* ┣ ┫ */
         0x2533, 0x253b, /* ┳ ┻ */
         0x2554, 0x255d, /* ╔ ╝ */
         0x2557, 0x255a, /* ╗ ╚ */
         0x2560, 0x2563, /* ╠ ╣ */
         0x2566, 0x2569, /* ╦ ╩ */
         0x2552, 0x255b, /* ╒ ╛ */
         0x2555, 0x2558, /* ╕ ╘ */
         0x255e, 0x2561, /* ╞ ╡ */
         0x2564, 0x2567, /* ╤ ╧ */
         0x2553, 0x255c, /* ╓ ╜ */
         0x2556, 0x2559, /* ╖ ╙ */
         0x255f, 0x2562, /* ╟ ╢ */
         0x2565, 0x2568, /* ╥ ╨ */
         0x2574, 0x2576, /* ╴ ╶ */
         0x2575, 0x2577, /* ╵ ╷ */
         0x2578, 0x257a, /* ╸ ╺ */
         0x2579, 0x257b, /* ╹ ╻ */
         0
    };

    for(i = 0; norotate[i]; i++)
        if(ch == norotate[i])
            return ch;

    for(i = 0; pairs[i]; i++)
        if(ch == pairs[i])
            return pairs[i ^ 1];

    return ch;
}

static uint32_t const leftright2[] =
{
    /* ASCII */
    '/', '\\',
    '|', '-',
    '|', '_', /* This is all right because there was already a '|' before */
    /* ASCII-Unicode */
    '|', 0x203e, /* | ‾ */
    /* Misc Unicode */
    0x2571, 0x2572, /* ╱ ╲ */
    /* Box drawing */
    0x2500, 0x2502, /* ─ │ */
    0x2501, 0x2503, /* ━ ┃ */
    0x2550, 0x2551, /* ═ ║ */
    0, 0
};

static uint32_t const leftright4[] =
{
    /* ASCII */
    '<', 'v', '>', '^',
    ',', '.', '\'', '`',
    /* ASCII / Unicode */
    '(', 0x203f, ')', 0x2040,       /* ( ‿ ) ⁀ */
    /* Misc Unicode */
    0x256d, 0x2570, 0x256f, 0x256e, /* ╭ ╰ ╯ ╮ */
    /* CP437 */
    0x258c, 0x2584, 0x2590, 0x2580, /* ▌ ▄ ▐ ▀ */
    0x2596, 0x2597, 0x259d, 0x2598, /* ▖ ▗ ▝ ▘ */
    0x2599, 0x259f, 0x259c, 0x259b, /* ▙ ▟ ▜ ▛ */
    /* Box drawing */
    0x250c, 0x2514, 0x2518, 0x2510, /* ┌ └ ┘ ┐ */
    0x250f, 0x2517, 0x251b, 0x2513, /* ┏ ┗ ┛ ┓ */
    0x251c, 0x2534, 0x2524, 0x252c, /* ├ ┴ ┤ ┬ */
    0x2523, 0x253b, 0x252b, 0x2533, /* ┣ ┻ ┫ ┳ */
    0x2552, 0x2559, 0x255b, 0x2556, /* ╒ ╙ ╛ ╖ */
    0x2553, 0x2558, 0x255c, 0x2555, /* ╓ ╘ ╜ ╕ */
    0x2554, 0x255a, 0x255d, 0x2557, /* ╔ ╚ ╝ ╗ */
    0x255e, 0x2568, 0x2561, 0x2565, /* ╞ ╨ ╡ ╥ */
    0x255f, 0x2567, 0x2562, 0x2564, /* ╟ ╧ ╢ ╤ */
    0x2560, 0x2569, 0x2563, 0x2566, /* ╠ ╩ ╣ ╦ */
    0x2574, 0x2577, 0x2576, 0x2575, /* ╴ ╷ ╶ ╵ */
    0x2578, 0x257b, 0x257a, 0x2579, /* ╸ ╻ ╺ ╹ */
    0, 0, 0, 0
};

static uint32_t leftchar(uint32_t ch)
{
    int i;

    for(i = 0; leftright2[i]; i++)
        if(ch == leftright2[i])
            return leftright2[(i & ~1) | ((i + 1) & 1)];

    for(i = 0; leftright4[i]; i++)
        if(ch == leftright4[i])
            return leftright4[(i & ~3) | ((i + 1) & 3)];

    return ch;
}

static uint32_t rightchar(uint32_t ch)
{
    int i;

    for(i = 0; leftright2[i]; i++)
        if(ch == leftright2[i])
            return leftright2[(i & ~1) | ((i - 1) & 1)];

    for(i = 0; leftright4[i]; i++)
        if(ch == leftright4[i])
            return leftright4[(i & ~3) | ((i - 1) & 3)];

    return ch;
}

static uint32_t const leftright2x2[] =
{
    /* ASCII / Unicode */
    '-', '-', 0x4e28, CACA_MAGIC_FULLWIDTH, /* -- 丨 */
    '|', '|', 0x2f06, CACA_MAGIC_FULLWIDTH, /* || ⼆ */
    /* Unicode */
    0x2584, 0x2580, 0x2580, 0x2584, /* ▄▀ ▀▄ */
    0, 0, 0, 0
};

static uint32_t const leftright2x4[] =
{
    /* ASCII */
    ':', ' ', '.', '.', ' ', ':', '\'', '\'',
    /* ASCII / Unicode */
    ' ', '`', 0x00b4, ' ', 0x02ce, ' ', ' ', ',',      /*  ` ´  ˎ   , */
    ' ', '`', '\'',   ' ', '.',    ' ', ' ', ',',      /* fallback ASCII */
    '`', ' ', ',', ' ', ' ', 0x00b4, ' ', 0x02ce,      /*  ` ,   ˎ  ´ */
    '`', ' ', ',', ' ', ' ', '.',    ' ', '\'',        /* fallback ASCII */
    '/', ' ', '-', 0x02ce, ' ', '/', '`', '-',         /* /  -ˎ  / `- */
    '/', ' ', '-', '.',    ' ', '/', '\'', '-',        /* fallback ASCII */
    '\\', ' ', ',', '-', ' ', '\\', '-', 0x00b4,       /* \  ,-  \ -´ */
    '\\', ' ', '.', '-', ' ', '\\', '-', '\'',         /* fallback ASCII */
    '\\', ' ', '_', ',', ' ', '\\', 0x00b4, 0x203e,    /* \  _,  \ ´‾ */
    '\\', '_', '_', '/', 0x203e, '\\', '/', 0x203e,    /* \_ _/ ‾\ /‾ */
    '_', '\\', 0x203e, '/', '\\', 0x203e, '/', '_',    /* _\ ‾/ \‾ /_ */
    '|', ' ', '_', '_', ' ', '|', 0x203e, 0x203e,      /* |  __  | ‾‾ */
    '_', '|', 0x203e, '|', '|', 0x203e, '|', '_',      /* _| ‾| |‾ |_ */
    '|', '_', '_', '|', 0x203e, '|', '|', 0x203e,      /* |_ _| ‾| |‾ */
    '_', ' ', ' ', 0x2577, ' ', 0x203e, 0x2575, ' ',   /* _   ╷  ‾ ╵  */
    ' ', '_', ' ', 0x2575, 0x203e, ' ', 0x2577, ' ',   /*  _  ╵ ‾  ╷  */
    '.', '_', '.', 0x2575, 0x203e, '\'', 0x2577, '\'', /* ._ .╵ ‾' ╷' */
    '(', '_', 0x203f, '|', 0x203e, ')', '|', 0x2040,   /* (_ ‿| ‾) |⁀ */
    '(', 0x203e, '|', 0x203f, '_', ')', 0x2040, '|',   /* (‾ |‿ _) ⁀| */
    '\\', '/', 0xff1e, CACA_MAGIC_FULLWIDTH,
            '/', '\\', 0xff1c, CACA_MAGIC_FULLWIDTH,  /* \/ ＞ /\ ＜ */
    ')', ' ', 0xfe35, CACA_MAGIC_FULLWIDTH,
            ' ', '(', 0xfe36, CACA_MAGIC_FULLWIDTH,   /* )  ︵  ( ︶ */
    '}', ' ', 0xfe37, CACA_MAGIC_FULLWIDTH,
            ' ', '{', 0xfe38, CACA_MAGIC_FULLWIDTH,   /* }  ︷  { ︸ */
    /* Not perfect, but better than nothing */
    '(', ' ', 0x02ce, ',', ' ', ')', 0x00b4, '`',      /* (  ˎ,  ) ´` */
    ' ', 'v', '>', ' ', 0x028c, ' ', ' ', '<',         /*  v >  ʌ   < */
    ' ', 'V', '>', ' ', 0x039b, ' ', ' ', '<',         /*  V >  Λ   < */
    'v', ' ', '>', ' ', ' ', 0x028c, ' ', '<',         /* v  >   ʌ  < */
    'V', ' ', '>', ' ', ' ', 0x039b, ' ', '<',         /* V  >   Λ  < */
    '\\', '|', 0xff1e, CACA_MAGIC_FULLWIDTH,
            '|', '\\', 0xff1c, CACA_MAGIC_FULLWIDTH,  /* \| ＞ |\ ＜ */
    '|', '/', 0xff1e, CACA_MAGIC_FULLWIDTH,
            '/', '|', 0xff1c, CACA_MAGIC_FULLWIDTH,   /* |/ ＞ /| ＜ */
    /* Unicode */
    0x2584, ' ', ' ', 0x2584, ' ', 0x2580, 0x2580, ' ',       /* ▄   ▄  ▀ ▀  */
    0x2588, ' ', 0x2584, 0x2584, ' ', 0x2588, 0x2580, 0x2580, /* █  ▄▄  █ ▀▀ */
    0x2588, 0x2584, 0x2584, 0x2588,
            0x2580, 0x2588, 0x2588, 0x2580,                   /* █▄ ▄█ ▀█ █▀ */
    /* TODO: Braille */
    /* Not perfect, but better than nothing */
    0x2591, ' ', 0x28e4, 0x28e4, ' ', 0x2591, 0x281b, 0x281b, /* ░  ⣤⣤  ░ ⠛⠛ */
    0x2592, ' ', 0x28f6, 0x28f6, ' ', 0x2592, 0x283f, 0x283f, /* ▒  ⣶⣶  ▒ ⠿⠿ */
    0, 0, 0, 0, 0, 0, 0, 0
};

static void leftpair(uint32_t pair[2])
{
    int i;

    for(i = 0; leftright2x2[i]; i += 2)
        if(pair[0] == leftright2x2[i] && pair[1] == leftright2x2[i + 1])
        {
            pair[0] = leftright2x2[(i & ~3) | ((i + 2) & 3)];
            pair[1] = leftright2x2[((i & ~3) | ((i + 2) & 3)) + 1];
            return;
        }

    for(i = 0; leftright2x4[i]; i += 2)
        if(pair[0] == leftright2x4[i] && pair[1] == leftright2x4[i + 1])
        {
            pair[0] = leftright2x4[(i & ~7) | ((i + 2) & 7)];
            pair[1] = leftright2x4[((i & ~7) | ((i + 2) & 7)) + 1];
            return;
        }
}

static void rightpair(uint32_t pair[2])
{
    int i;

    for(i = 0; leftright2x2[i]; i += 2)
        if(pair[0] == leftright2x2[i] && pair[1] == leftright2x2[i + 1])
        {
            pair[0] = leftright2x2[(i & ~3) | ((i - 2) & 3)];
            pair[1] = leftright2x2[((i & ~3) | ((i - 2) & 3)) + 1];
            return;
        }

    for(i = 0; leftright2x4[i]; i += 2)
        if(pair[0] == leftright2x4[i] && pair[1] == leftright2x4[i + 1])
        {
            pair[0] = leftright2x4[(i & ~7) | ((i - 2) & 7)];
            pair[1] = leftright2x4[((i & ~7) | ((i - 2) & 7)) + 1];
            return;
        }
}

/*
 * XXX: The following functions are aliases.
 */

int cucul_invert(cucul_canvas_t *) CACA_ALIAS(caca_invert);
int cucul_flip(cucul_canvas_t *) CACA_ALIAS(caca_flip);
int cucul_flop(cucul_canvas_t *) CACA_ALIAS(caca_flop);
int cucul_rotate_180(cucul_canvas_t *) CACA_ALIAS(caca_rotate_180);
int cucul_rotate_left(cucul_canvas_t *) CACA_ALIAS(caca_rotate_left);
int cucul_rotate_right(cucul_canvas_t *) CACA_ALIAS(caca_rotate_right);
int cucul_stretch_left(cucul_canvas_t *) CACA_ALIAS(caca_stretch_left);
int cucul_stretch_right(cucul_canvas_t *) CACA_ALIAS(caca_stretch_right);

