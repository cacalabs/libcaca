/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2002-2012 Sam Hocevar <sam@hocevar.net>
 *                2006 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

/*
 *  This file contains various export functions
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdlib.h>
#   include <stdio.h>
#   include <string.h>
#endif

#include "caca.h"
#include "caca_internals.h"
#include "codec.h"

static inline int sprintu32(char *s, uint32_t x)
{
    s[0] = (uint8_t)(x >> 24);
    s[1] = (uint8_t)(x >> 16) & 0xff;
    s[2] = (uint8_t)(x >>  8) & 0xff;
    s[3] = (uint8_t)(x      ) & 0xff;
    return 4;
}

static inline int sprintu16(char *s, uint16_t x)
{
    s[0] = (uint8_t)(x >>  8) & 0xff;
    s[1] = (uint8_t)(x      ) & 0xff;
    return 2;
}

static void *export_caca(caca_canvas_t const *, size_t *);
static void *export_html(caca_canvas_t const *, size_t *);
static void *export_html3(caca_canvas_t const *, size_t *);
static void *export_bbfr(caca_canvas_t const *, size_t *);
static void *export_ps(caca_canvas_t const *, size_t *);
static void *export_svg(caca_canvas_t const *, size_t *);
static void *export_tga(caca_canvas_t const *, size_t *);
static void *export_troff(caca_canvas_t const *, size_t *);

/** \brief Export a canvas into a foreign format.
 *
 *  This function exports a libcaca canvas into various foreign formats such
 *  as ANSI art, HTML, IRC colours, etc. The returned pointer should be passed
 *  to free() to release the allocated storage when it is no longer needed.
 *
 *  Valid values for \c format are:
 *  - \c "caca": export native libcaca files.
 *  - \c "ansi": export ANSI art (CP437 charset with ANSI colour codes).
 *  - \c "html": export an HTML page with CSS information.
 *  - \c "html3": export an HTML table that should be compatible with
 *    most navigators, including textmode ones.
 *  - \c "irc": export UTF-8 text with mIRC colour codes.
 *  - \c "ps": export a PostScript document.
 *  - \c "svg": export an SVG vector image.
 *  - \c "tga": export a TGA image.
 *  - \c "troff": export a troff source.
 *
 *  If an error occurs, NULL is returned and \b errno is set accordingly:
 *  - \c EINVAL Unsupported format requested.
 *  - \c ENOMEM Not enough memory to allocate output buffer.
 *
 *  \param cv A libcaca canvas
 *  \param format A string describing the requested output format.
 *  \param bytes A pointer to a size_t where the number of allocated bytes
 *         will be written.
 *  \return A pointer to the exported memory area, or NULL in case of error.
 */
void *caca_export_canvas_to_memory(caca_canvas_t const *cv, char const *format,
                                   size_t *bytes)
{
    if(!strcasecmp("caca", format))
        return export_caca(cv, bytes);

    if(!strcasecmp("ansi", format))
        return _export_ansi(cv, bytes);

    if(!strcasecmp("utf8", format))
        return _export_utf8(cv, bytes, 0);

    if(!strcasecmp("utf8cr", format))
        return _export_utf8(cv, bytes, 1);

    if(!strcasecmp("html", format))
        return export_html(cv, bytes);

    if(!strcasecmp("html3", format))
        return export_html3(cv, bytes);

    if(!strcasecmp("bbfr", format))
        return export_bbfr(cv, bytes);

    if(!strcasecmp("irc", format))
        return _export_irc(cv, bytes);

    if(!strcasecmp("ps", format))
        return export_ps(cv, bytes);

    if(!strcasecmp("svg", format))
        return export_svg(cv, bytes);

    if(!strcasecmp("tga", format))
        return export_tga(cv, bytes);

    if(!strcasecmp("troff", format))
        return export_troff(cv, bytes);

    seterrno(EINVAL);
    return NULL;
}

/** \brief Export a canvas portion into a foreign format.
 *
 *  This function exports a portion of a \e libcaca canvas into various
 *  formats. For more information, see caca_export_canvas_to_memory().
 *
 *  If an error occurs, NULL is returned and \b errno is set accordingly:
 *  - \c EINVAL Unsupported format requested or invalid coordinates.
 *  - \c ENOMEM Not enough memory to allocate output buffer.
 *
 *  \param cv A libcaca canvas
 *  \param x The leftmost coordinate of the area to export.
 *  \param y The topmost coordinate of the area to export.
 *  \param w The width of the area to export.
 *  \param h The height of the area to export.
 *  \param format A string describing the requested output format.
 *  \param bytes A pointer to a size_t where the number of allocated bytes
 *         will be written.
 *  \return A pointer to the exported memory area, or NULL in case of error.
 */
void *caca_export_area_to_memory(caca_canvas_t const *cv, int x, int y, int w,
                                 int h, char const *format, size_t *bytes)
{
    caca_canvas_t *tmp;
    void *ret;

    if(w < 0 || h < 0 || x < 0 || y < 0
        || x + w > cv->width || y + h > cv->height)
    {
        seterrno(EINVAL);
        return NULL;
    }

    /* TODO: we need to spare the blit here by exporting the area we want. */
    tmp = caca_create_canvas(w, h);
    caca_blit(tmp, -x, -y, cv, NULL);

    ret = caca_export_canvas_to_memory(tmp, format, bytes);

    caca_free_canvas(tmp);

    return ret;
}

/** \brief Get available export formats
 *
 *  Return a list of available export formats. The list is a NULL-terminated
 *  array of strings, interleaving a string containing the internal value for
 *  the export format, to be used with caca_export_memory(), and a string
 *  containing the natural language description for that export format.
 *
 *  This function never fails.
 *
 *  \return An array of strings.
 */
char const * const * caca_get_export_list(void)
{
    static char const * const list[] =
    {
        "caca", "native libcaca format",
        "ansi", "ANSI",
        "utf8", "UTF-8 with ANSI escape codes",
        "utf8cr", "UTF-8 with ANSI escape codes and MS-DOS \\r",
        "html", "HTML",
        "html3", "backwards-compatible HTML",
        "bbfr", "BBCode (French)",
        "irc", "IRC with mIRC colours",
        "ps", "PostScript document",
        "svg", "SVG vector image",
        "tga", "TGA image",
        "troff", "troff source",
        NULL, NULL
    };

    return list;
}

/*
 * XXX: the following functions are local.
 */

/* Generate a native libcaca canvas file. */
static void *export_caca(caca_canvas_t const *cv, size_t *bytes)
{
    char *data, *cur;
    int f, n;

    /* 52 bytes for the header:
     *  - 4 bytes for "\xCA\xCA" + "CV"
     *  - 16 bytes for the canvas header
     *  - 32 bytes for the frame info
     * 8 bytes for each character cell */
    *bytes = 20 + (32 + 8 * cv->width * cv->height) * cv->framecount;
    cur = data = malloc(*bytes);

    /* magic */
    cur += sprintf(cur, "%s", "\xCA\xCA" "CV");

    /* canvas_header */
    cur += sprintu32(cur, 16 + 32 * cv->framecount);
    cur += sprintu32(cur, cv->width * cv->height * 8 * cv->framecount);
    cur += sprintu16(cur, 0x0001);
    cur += sprintu32(cur, cv->framecount);
    cur += sprintu16(cur, 0x0000);

    /* frame_info */
    for(f = 0; f < cv->framecount; f++)
    {
        cur += sprintu32(cur, cv->width);
        cur += sprintu32(cur, cv->height);
        cur += sprintu32(cur, 0);
        cur += sprintu32(cur, cv->curattr);
        cur += sprintu32(cur, cv->frames[f].x);
        cur += sprintu32(cur, cv->frames[f].y);
        cur += sprintu32(cur, cv->frames[f].handlex);
        cur += sprintu32(cur, cv->frames[f].handley);
    }

    /* canvas_data */
    for(f = 0; f < cv->framecount; f++)
    {
        uint32_t *attrs = cv->frames[f].attrs;
        uint32_t *chars = cv->frames[f].chars;

        for(n = cv->height * cv->width; n--; )
        {
            cur += sprintu32(cur, *chars++);
            cur += sprintu32(cur, *attrs++);
        }
    }

    return data;
}

/* Generate HTML representation of current canvas. */
static void *export_html(caca_canvas_t const *cv, size_t *bytes)
{
    char *data, *cur;
    int x, y, len;

    /* The HTML header: less than 1000 bytes
     * A line: 7 chars for "<br />\n"
     * A glyph: 47 chars for "<span style="color:#xxx;background-color:#xxx">"
     *          83 chars for ";font-weight..."
     *          up to 10 chars for "&#xxxxxxx;", far less for pure ASCII
     *          7 chars for "</span>" */
    *bytes = 1000 + cv->height * (7 + cv->width * (47 + 83 + 10 + 7));
    cur = data = malloc(*bytes);

    /* HTML header */

    cur += sprintf(cur, "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n");
    cur += sprintf(cur, "   \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n");
    cur += sprintf(cur, "<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\"en\" xml:lang=\"en\">");
    cur += sprintf(cur, "<head>\n");
    cur += sprintf(cur, "<title>Generated by libcaca %s</title>\n",
                        caca_get_version());
    cur += sprintf(cur, "</head><body>\n");

    cur += sprintf(cur, "<div style=\"%s\">\n",
                        "font-family: monospace, fixed; font-weight: bold;");

    for(y = 0; y < cv->height; y++)
    {
        uint32_t *lineattr = cv->attrs + y * cv->width;
        uint32_t *linechar = cv->chars + y * cv->width;

        for(x = 0; x < cv->width; x += len)
        {
            cur += sprintf(cur, "<span style=\"");
            if(caca_attr_to_ansi_fg(lineattr[x]) != CACA_DEFAULT)
                cur += sprintf(cur, ";color:#%.03x",
                               caca_attr_to_rgb12_fg(lineattr[x]));
            if(caca_attr_to_ansi_bg(lineattr[x]) < 0x10)
                cur += sprintf(cur, ";background-color:#%.03x",
                               caca_attr_to_rgb12_bg(lineattr[x]));
            if(lineattr[x] & CACA_BOLD)
                cur += sprintf(cur, ";font-weight:bold");
            if(lineattr[x] & CACA_ITALICS)
                cur += sprintf(cur, ";font-style:italic");
            if(lineattr[x] & CACA_UNDERLINE)
                cur += sprintf(cur, ";text-decoration:underline");
            if(lineattr[x] & CACA_BLINK)
                cur += sprintf(cur, ";text-decoration:blink");
            cur += sprintf(cur, "\">");

            for(len = 0;
                x + len < cv->width && lineattr[x + len] == lineattr[x];
                len++)
            {
                if(linechar[x + len] == CACA_MAGIC_FULLWIDTH)
                    ;
                else if((linechar[x + len] <= 0x00000020)
                        ||
                        ((linechar[x + len] >= 0x0000007f)
                         &&
                         (linechar[x + len] <= 0x000000a0)))
                {
                    /* Control characters and space converted to
                     * U+00A0 NO-BREAK SPACE, a.k.a. "&nbsp;" in HTML,
                     * but we use the equivalent numeric character
                     * reference &#160; so this will work in plain
                     * XHTML with no DTD too. */
                    cur += sprintf(cur, "&#160;");
                }
                else if(linechar[x + len] == '&')
                    cur += sprintf(cur, "&amp;");
                else if(linechar[x + len] == '<')
                    cur += sprintf(cur, "&lt;");
                else if(linechar[x + len] == '>')
                    cur += sprintf(cur, "&gt;");
                else if(linechar[x + len] == '\"')
                    cur += sprintf(cur, "&quot;");
                else if(linechar[x + len] == '\'')
                    cur += sprintf(cur, "&#39;");
                else if(linechar[x + len] < 0x00000080)
                    cur += sprintf(cur, "%c", (uint8_t)linechar[x + len]);
                else if((linechar[x + len] <= 0x0010fffd)
                        &&
                        ((linechar[x + len] & 0x0000fffe) != 0x0000fffe)
                        &&
                        ((linechar[x + len] < 0x0000d800)
                         ||
                         (linechar[x + len] > 0x0000dfff)))
                    cur += sprintf(cur, "&#%i;", (unsigned int)linechar[x + len]);
                else
                    /* non-character codepoints become U+FFFD
                     * REPLACEMENT CHARACTER */
                    cur += sprintf(cur, "&#%i;", (unsigned int)0x0000fffd);
            }
            cur += sprintf(cur, "</span>");
        }
        /* New line */
        cur += sprintf(cur, "<br />\n");
    }

    cur += sprintf(cur, "</div></body></html>\n");

    /* Crop to really used size */
    debug("html export: alloc %lu bytes, realloc %lu",
          (unsigned long int)*bytes, (unsigned long int)(cur - data));
    *bytes = (uintptr_t)(cur - data);
    data = realloc(data, *bytes);

    return data;
}

/* Export an HTML3 document. This function is way bigger than export_html(),
 * but permits viewing in old browsers (or limited ones such as links). It
 * will not work under gecko (mozilla rendering engine) unless you set a
 * correct header. */
static void *export_html3(caca_canvas_t const *cv, size_t *bytes)
{
    char *data, *cur;
    int x, y, len;
    int has_multi_cell_row = 0;
    unsigned char *cell_boundary_bitmap;

    /* Table */
    cell_boundary_bitmap = (unsigned char *) malloc((cv->width + 7) / 8);
    if(cell_boundary_bitmap)
        memset((void *) cell_boundary_bitmap, 0, (cv->width + 7) / 8);
    for(y = 0; y < cv->height; y++)
    {
        uint32_t *lineattr = cv->attrs + y * cv->width;
        uint32_t *linechar = cv->chars + y * cv->width;

        for(x = 1; x < cv->width; x++)
            if((! (cell_boundary_bitmap
                   ?
                   (cell_boundary_bitmap[x / 8] & (1 << (x % 8)))
                   :
                   has_multi_cell_row))
               &&
               (((linechar[x - 1] == CACA_MAGIC_FULLWIDTH)
                 &&
                 (! caca_utf32_is_fullwidth(linechar[x])))
                ||
                (caca_attr_to_ansi_bg(lineattr[x - 1])
                 !=
                 caca_attr_to_ansi_bg(lineattr[x]))
                ||
                ((caca_attr_to_ansi_bg(lineattr[x]) < 0x10)
                 ?
                 (_caca_attr_to_rgb24bg(lineattr[x - 1])
                  !=
                  _caca_attr_to_rgb24bg(lineattr[x]))
                 :
                 0)))
            {
                has_multi_cell_row = 1;
                if(cell_boundary_bitmap)
                    cell_boundary_bitmap[x / 8] |= 1 << (x % 8);
            }
    }

    /* The HTML table markup: less than 1000 bytes
     * A line: 10 chars for "<tr></tr>\n"
     * A glyph: up to 48 chars for "<td bgcolor=\"#xxxxxx\"><tt><font color=\"#xxxxxx\">"
     *          up to 36 chars for "<b><i><u><blink></blink></u></i></b>"
     *          up to 10 chars for "&#xxxxxxx;" (far less for pure ASCII)
     *          17 chars for "</font></tt></td>" */
    *bytes = 1000 + cv->height * (10 + cv->width * (48 + 36 + 10 + 17));
    cur = data = malloc(*bytes);

    cur += sprintf(cur, "<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" summary=\"[libcaca canvas export]\">\n");

    for(y = 0; y < cv->height; y++)
    {
        uint32_t *lineattr = cv->attrs + y * cv->width;
        uint32_t *linechar = cv->chars + y * cv->width;

        cur += sprintf(cur, "<tr>");

        for(x = 0; x < cv->width; x += len)
        {
            int i, needfont = 0;
            int nonblank = 0;

            /* Use colspan option to factor cells with same attributes
             * (see below) */
            len = 1;
            while((x + len < cv->width)
                  &&
                  ((y
                    &&
                    (linechar[x + len] > 0x00000020)
                    &&
                    ((linechar[x + len] < 0x0000007f)
                     ||
                     (linechar[x + len] > 0x000000a0)))
                   ||
                   (! (cell_boundary_bitmap
                       ?
                       (cell_boundary_bitmap[(x + len) / 8] & (1 << ((x + len) % 8)))
                       :
                       has_multi_cell_row))
                   ||
                   (linechar[x + len] == CACA_MAGIC_FULLWIDTH)
                   ||
                   (cv->height == 1))
                  &&
                  ((linechar[x + len - 1] != CACA_MAGIC_FULLWIDTH)
                   ||
                   caca_utf32_is_fullwidth(linechar[x + len]))
                  &&
                  (caca_attr_to_ansi_bg(lineattr[x + len])
                   ==
                   caca_attr_to_ansi_bg(lineattr[x]))
                  &&
                  ((caca_attr_to_ansi_bg(lineattr[x]) < 0x10)
                   ?
                   (_caca_attr_to_rgb24bg(lineattr[x + len])
                    ==
                    _caca_attr_to_rgb24bg(lineattr[x]))
                   :
                   1))
                len++;

            for(i = 0; i < len; i++)
                if(! ((linechar[x + i] <= 0x00000020)
                      ||
                      ((linechar[x + i] >= 0x0000007f)
                       &&
                       (linechar[x + i] <= 0x000000a0))))
                    nonblank = 1;

            cur += sprintf(cur, "<td");

            if(caca_attr_to_ansi_bg(lineattr[x]) < 0x10)
                cur += sprintf(cur, " bgcolor=\"#%.06lx\"", (unsigned long int)
                               _caca_attr_to_rgb24bg(lineattr[x]));

            if(has_multi_cell_row && (len > 1))
            {
                int colspan;

                colspan = len;
                if(cell_boundary_bitmap)
                    for(i = 0; i < len; i ++)
                        if(i
                           &&
                           ! (cell_boundary_bitmap[(x + i) / 8]
                              &
                              (1 << ((x + i) % 8))))
                            colspan --;
                if(colspan > 1)
                    cur += sprintf(cur, " colspan=\"%d\"", colspan);
            }

            cur += sprintf(cur, ">");

            cur += sprintf(cur, "<tt>");

            for(i = 0; i < len; i++)
            {
                if(nonblank
                   &&
                   ((! i)
                    ||
                    (lineattr[x + i] != lineattr[x + i - 1])))
                {
                    needfont = (caca_attr_to_ansi_fg(lineattr[x + i])
                                !=
                                CACA_DEFAULT);

                    if(needfont)
                        cur += sprintf(cur, "<font color=\"#%.06lx\">",
                                       (unsigned long int)
                                       _caca_attr_to_rgb24fg(lineattr[x + i]));

                    if(lineattr[x + i] & CACA_BOLD)
                        cur += sprintf(cur, "<b>");
                    if(lineattr[x + i] & CACA_ITALICS)
                        cur += sprintf(cur, "<i>");
                    if(lineattr[x + i] & CACA_UNDERLINE)
                        cur += sprintf(cur, "<u>");
                    if(lineattr[x + i] & CACA_BLINK)
                        cur += sprintf(cur, "<blink>");
                }

                if(linechar[x + i] == CACA_MAGIC_FULLWIDTH)
                    ;
                else if((linechar[x + i] <= 0x00000020)
                        ||
                        ((linechar[x + i] >= 0x0000007f)
                         &&
                         (linechar[x + i] <= 0x000000a0)))
                {
                    /* Control characters and space converted to
                     * U+00A0 NO-BREAK SPACE, a.k.a. "&nbsp;" in HTML,
                     * but we use the equivalent numeric character
                     * reference &#160; so this will work in plain
                     * XHTML with no DTD too. */
                    cur += sprintf(cur, "&#160;");
                }
                else if(linechar[x + i] == '&')
                    cur += sprintf(cur, "&amp;");
                else if(linechar[x + i] == '<')
                    cur += sprintf(cur, "&lt;");
                else if(linechar[x + i] == '>')
                    cur += sprintf(cur, "&gt;");
                else if(linechar[x + i] == '\"')
                    cur += sprintf(cur, "&quot;");
                else if(linechar[x + i] == '\'')
                    cur += sprintf(cur, "&#39;");
                else if(linechar[x + i] < 0x00000080)
                    cur += sprintf(cur, "%c", (uint8_t)linechar[x + i]);
                else if((linechar[x + i] <= 0x0010fffd)
                        &&
                        ((linechar[x + i] & 0x0000fffe) != 0x0000fffe)
                        &&
                        ((linechar[x + i] < 0x0000d800)
                         ||
                         (linechar[x + i] > 0x0000dfff)))
                    cur += sprintf(cur, "&#%i;", (unsigned int)linechar[x + i]);
                else
                    /* non-character codepoints become U+FFFD
                     * REPLACEMENT CHARACTER */
                    cur += sprintf(cur, "&#%i;", (unsigned int)0x0000fffd);

                if (nonblank
                    &&
                    (((i + 1) == len)
                     ||
                     (lineattr[x + i + 1] != lineattr[x + i])))
                {
                    if(lineattr[x + i] & CACA_BLINK)
                        cur += sprintf(cur, "</blink>");
                    if(lineattr[x + i] & CACA_UNDERLINE)
                        cur += sprintf(cur, "</u>");
                    if(lineattr[x + i] & CACA_ITALICS)
                        cur += sprintf(cur, "</i>");
                    if(lineattr[x + i] & CACA_BOLD)
                        cur += sprintf(cur, "</b>");

                    if(needfont)
                        cur += sprintf(cur, "</font>");
                }
            }

            cur += sprintf(cur, "</tt>");
            cur += sprintf(cur, "</td>");
        }
        cur += sprintf(cur, "</tr>\n");
    }

    /* Footer */
    cur += sprintf(cur, "</table>\n");

    /* Free working memory */
    if (cell_boundary_bitmap)
        free((void *) cell_boundary_bitmap);

    /* Crop to really used size */
    debug("html3 export: alloc %lu bytes, realloc %lu",
          (unsigned long int)*bytes, (unsigned long int)(cur - data));
    *bytes = (uintptr_t)(cur - data);
    data = realloc(data, *bytes);

    return data;
}

static void *export_bbfr(caca_canvas_t const *cv, size_t *bytes)
{
    char *data, *cur;
    int x, y, len;

    /* The font markup: less than 100 bytes
     * A line: 1 char for "\n"
     * A glyph: 22 chars for "[f=#xxxxxx][c=#xxxxxx]"
     *          up to 21 chars for "[g][i][s][/s][/i][/g]"
     *          up to 6 chars for the UTF-8 glyph
     *          8 chars for "[/c][/f]" */
    *bytes = 100 + cv->height * (1 + cv->width * (22 + 21 + 6 + 8));
    cur = data = malloc(*bytes);

    /* Table */
    cur += sprintf(cur, "[font=Courier New]");

    for(y = 0; y < cv->height; y++)
    {
        uint32_t *lineattr = cv->attrs + y * cv->width;
        uint32_t *linechar = cv->chars + y * cv->width;

        for(x = 0; x < cv->width; x += len)
        {
            int i, needback, needfront;

            /* Use colspan option to factor cells with same attributes
             * (see below) */
            len = 1;
            if(linechar[x] == ' ')
                while(x + len < cv->width && lineattr[x + len] == lineattr[x]
                        && linechar[x] == ' ')
                    len++;
            else
                while(x + len < cv->width && lineattr[x + len] == lineattr[x]
                        && linechar[x] != ' ')
                    len++;

            needback = caca_attr_to_ansi_bg(lineattr[x]) < 0x10;
            needfront = caca_attr_to_ansi_fg(lineattr[x]) < 0x10;

            if(needback)
                cur += sprintf(cur, "[f=#%.06lx]", (unsigned long int)
                               _caca_attr_to_rgb24bg(lineattr[x]));

            if(linechar[x] == ' ')
                cur += sprintf(cur, "[c=#%.06lx]", (unsigned long int)
                               _caca_attr_to_rgb24bg(lineattr[x]));
            else if(needfront)
                cur += sprintf(cur, "[c=#%.06lx]", (unsigned long int)
                               _caca_attr_to_rgb24fg(lineattr[x]));

            if(lineattr[x] & CACA_BOLD)
                cur += sprintf(cur, "[g]");
            if(lineattr[x] & CACA_ITALICS)
                cur += sprintf(cur, "[i]");
            if(lineattr[x] & CACA_UNDERLINE)
                cur += sprintf(cur, "[s]");
            if(lineattr[x] & CACA_BLINK)
                ; /* FIXME */

            for(i = 0; i < len; i++)
            {
                if(linechar[x + i] == CACA_MAGIC_FULLWIDTH)
                    ;
                else if(linechar[x + i] == ' ')
                    *cur++ = '_';
                else
                    cur += caca_utf32_to_utf8(cur, linechar[x + i]);
            }

            if(lineattr[x] & CACA_BLINK)
                ; /* FIXME */
            if(lineattr[x] & CACA_UNDERLINE)
                cur += sprintf(cur, "[/s]");
            if(lineattr[x] & CACA_ITALICS)
                cur += sprintf(cur, "[/i]");
            if(lineattr[x] & CACA_BOLD)
                cur += sprintf(cur, "[/g]");

            if(linechar[x] == ' ' || needfront)
                cur += sprintf(cur, "[/c]");
            if(needback)
                cur += sprintf(cur, "[/f]");
        }
        cur += sprintf(cur, "\n");
    }

    /* Footer */
    cur += sprintf(cur, "[/font]\n");

    /* Crop to really used size */
    debug("bbfr export: alloc %lu bytes, realloc %lu",
          (unsigned long int)*bytes, (unsigned long int)(cur - data));
    *bytes = (uintptr_t)(cur - data);
    data = realloc(data, *bytes);

    return data;
}

/* Export a PostScript document. */
static void *export_ps(caca_canvas_t const *cv, size_t *bytes)
{
    static char const *ps_header =
        "%!\n"
        "%% libcaca PDF export\n"
        "%%LanguageLevel: 2\n"
        "%%Pages: 1\n"
        "%%DocumentData: Clean7Bit\n"
        "/csquare {\n"
        "  newpath\n"
        "  0 0 moveto\n"
        "  0 1 rlineto\n"
        "  1 0 rlineto\n"
        "  0 -1 rlineto\n"
        "  closepath\n"
        "  setrgbcolor\n"
        "  fill\n"
        "} def\n"
        "/S {\n"
        "  Show\n"
        "} bind def\n"
        "/Courier-Bold findfont\n"
        "8 scalefont\n"
        "setfont\n"
        "gsave\n"
        "6 10 scale\n";

    char *data, *cur;
    int x, y;

    /* 200 is arbitrary but should be ok */
    *bytes = strlen(ps_header) + 100 + cv->height * (32 + cv->width * 200);
    cur = data = malloc(*bytes);

    /* Header */
    cur += sprintf(cur, "%s", ps_header);
    cur += sprintf(cur, "0 %d translate\n", cv->height);

    /* Background, drawn using csquare macro defined in header */
    for(y = cv->height; y--; )
    {
        uint32_t *lineattr = cv->attrs + y * cv->width;

        for(x = 0; x < cv->width; x++)
        {
            uint8_t argb[8];
            caca_attr_to_argb64(*lineattr++, argb);
            cur += sprintf(cur, "1 0 translate\n %f %f %f csquare\n",
                           (float)argb[1] * (1.0 / 0xf),
                           (float)argb[2] * (1.0 / 0xf),
                           (float)argb[3] * (1.0 / 0xf));
        }

        /* Return to beginning of the line, and jump to the next one */
        cur += sprintf(cur, "-%d 1 translate\n", cv->width);
    }

    cur += sprintf(cur, "grestore\n"); /* Restore transformation matrix */
    cur += sprintf(cur, "0 %d translate\n", cv->height*10);

    for(y = cv->height; y--; )
    {
        uint32_t *lineattr = cv->attrs + (cv->height - y - 1) * cv->width;
        uint32_t *linechar = cv->chars + (cv->height - y - 1) * cv->width;

        for(x = 0; x < cv->width; x++)
        {
            uint8_t argb[8];
            uint32_t ch = *linechar++;

            caca_attr_to_argb64(*lineattr++, argb);

            cur += sprintf(cur, "newpath\n");
            cur += sprintf(cur, "%d %d moveto\n", (x + 1) * 6, y * 10 + 2);
            cur += sprintf(cur, "%f %f %f setrgbcolor\n",
                           (float)argb[5] * (1.0 / 0xf),
                           (float)argb[6] * (1.0 / 0xf),
                           (float)argb[7] * (1.0 / 0xf));

            if(ch < 0x00000020)
                cur += sprintf(cur, "(?) show\n");
            else if(ch >= 0x00000080)
                cur += sprintf(cur, "(?) show\n");
            else switch((uint8_t)(ch & 0x7f))
            {
                case '\\':
                case '(':
                case ')':
                    cur += sprintf(cur, "(\\%c) show\n", (uint8_t)ch);
                    break;
                default:
                    cur += sprintf(cur, "(%c) show\n", (uint8_t)ch);
                    break;
            }
        }
    }

    cur += sprintf(cur, "showpage\n");

    /* Crop to really used size */
    debug("PS export: alloc %lu bytes, realloc %lu",
          (unsigned long int)*bytes, (unsigned long int)(cur - data));
    *bytes = (uintptr_t)(cur - data);
    data = realloc(data, *bytes);

    return data;
}

/* Export an SVG vector image */
static void *export_svg(caca_canvas_t const *cv, size_t *bytes)
{
    static char const svg_header[] =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<svg width=\"%d\" height=\"%d\" viewBox=\"0 0 %d %d\""
        " xmlns=\"http://www.w3.org/2000/svg\""
        " xmlns:xlink=\"http://www.w3.org/1999/xlink\""
        " xml:space=\"preserve\" version=\"1.1\"  baseProfile=\"full\">\n";

    char *data, *cur;
    int x, y;

    /* 200 is arbitrary but should be ok */
    *bytes = strlen(svg_header) + 128 + cv->width * cv->height * 200;
    cur = data = malloc(*bytes);

    /* Header */
    cur += sprintf(cur, svg_header, cv->width * 6, cv->height * 10,
                                    cv->width * 6, cv->height * 10);

    cur += sprintf(cur, " <g id=\"mainlayer\" font-size=\"10\""
                        " style=\"font-family: monospace\">\n");

    /* Background */
    for(y = 0; y < cv->height; y++)
    {
        uint32_t *lineattr = cv->attrs + y * cv->width;

        for(x = 0; x < cv->width; x++)
        {
            cur += sprintf(cur, "<rect style=\"fill:#%.03x\" x=\"%d\" y=\"%d\""
                                " width=\"6\" height=\"10\"/>\n",
                                caca_attr_to_rgb12_bg(*lineattr++),
                                x * 6, y * 10);
        }
    }

    /* Text */
    for(y = 0; y < cv->height; y++)
    {
        uint32_t *lineattr = cv->attrs + y * cv->width;
        uint32_t *linechar = cv->chars + y * cv->width;

        for(x = 0; x < cv->width; x++)
        {
            uint32_t ch = *linechar++;

            if(ch == ' ' || ch == CACA_MAGIC_FULLWIDTH)
            {
                lineattr++;
                continue;
            }

            cur += sprintf(cur, "<text style=\"fill:#%.03x\" "
                                "x=\"%d\" y=\"%d\">",
                                caca_attr_to_rgb12_fg(*lineattr++),
                                x * 6, (y * 10) + 8);

            if(ch < 0x00000020)
                *cur++ = '?';
            else if(ch > 0x0000007f)
                cur += caca_utf32_to_utf8(cur, ch);
            else switch((uint8_t)ch)
            {
                case '>': cur += sprintf(cur, "&gt;"); break;
                case '<': cur += sprintf(cur, "&lt;"); break;
                case '&': cur += sprintf(cur, "&amp;"); break;
                default: *cur++ = (uint8_t)ch; break;
            }
            cur += sprintf(cur, "</text>\n");
        }
    }

    cur += sprintf(cur, " </g>\n");
    cur += sprintf(cur, "</svg>\n");

    /* Crop to really used size */
    debug("SVG export: alloc %lu bytes, realloc %lu",
          (unsigned long int)*bytes, (unsigned long int)(cur - data));
    *bytes = (uintptr_t)(cur - data);
    data = realloc(data, *bytes);

    return data;
}

/* Export a TGA image */
static void *export_tga(caca_canvas_t const *cv, size_t *bytes)
{
    char const * const *fontlist;
    char *data, *cur;
    caca_font_t *f;
    int i, w, h;

    fontlist = caca_get_font_list();
    if(!fontlist[0])
    {
        seterrno(EINVAL);
        return NULL;
    }

    f = caca_load_font(fontlist[0], 0);

    w = caca_get_canvas_width(cv) * caca_get_font_width(f);
    h = caca_get_canvas_height(cv) * caca_get_font_height(f);

    *bytes = w * h * 4 + 18; /* 32 bpp + 18 bytes for the header */
    cur = data = malloc(*bytes);

    /* ID Length */
    cur += sprintf(cur, "%c", 0);
    /* Color Map Type: no colormap */
    cur += sprintf(cur, "%c", 0);
    /* Image Type: uncompressed truecolor */
    cur += sprintf(cur, "%c", 2);
    /* Color Map Specification: no color map */
    memset(cur, 0, 5); cur += 5;

    /* Image Specification */
    cur += sprintf(cur, "%c%c", 0, 0); /* X Origin */
    cur += sprintf(cur, "%c%c", 0, 0); /* Y Origin */
    cur += sprintf(cur, "%c%c", w & 0xff, w >> 8); /* Width */
    cur += sprintf(cur, "%c%c", h & 0xff, h >> 8); /* Height */
    cur += sprintf(cur, "%c", 32); /* Pixel Depth */
    cur += sprintf(cur, "%c", 40); /* Image Descriptor */

    /* Image ID: no ID */
    /* Color Map Data: no colormap */

    /* Image Data */
    caca_render_canvas(cv, f, cur, w, h, 4 * w);

    /* Swap bytes. What a waste of time. */
    for(i = 0; i < w * h * 4; i += 4)
    {
        char c;
        c = cur[i]; cur[i] = cur[i + 3]; cur[i + 3] = c;
        c = cur[i + 1]; cur[i + 1] = cur[i + 2]; cur[i + 2] = c;
    }

    caca_free_font(f);

    return data;
}

/* Generate troff representation of current canvas. */
static void *export_troff(caca_canvas_t const *cv, size_t *bytes)
{
    char *data, *cur;
    int x, y;

    uint32_t prevfg = 0;
    uint32_t prevbg = 0;
    int started = 0;

    /* Each char is at most
     *  2x\mM (2x10)
     *  + \fB + \fI + \fR (9)
     *  + 4 bytes = 33
     * Each line has a \n (1) and maybe 0xc2 0xa0 (2)
     * Header has .nf\n (3)
     */
    *bytes = 3 + cv->height * 3 + (cv->width * cv->height * 33);
    cur = data = malloc(*bytes);

    cur += sprintf(cur, ".nf\n");

    prevfg = 0;
    prevbg = 0;
    started = 0;

    for(y = 0; y < cv->height; y++)
    {
        uint32_t *lineattr = cv->attrs + y * cv->width;
        uint32_t *linechar = cv->chars + y * cv->width;

        for(x = 0; x < cv->width; x++)
        {
            static char const * ansi2troff[16] =
            {
                /* Dark */
                "black", "blue", "green", "cyan",
                "red", "magenta", "yellow", "white",
                /* Bright */
                "black", "blue", "green", "cyan",
                "red", "magenta", "yellow", "white",
            };
            uint8_t fg = caca_attr_to_ansi_fg(lineattr[x]);
            uint8_t bg = caca_attr_to_ansi_bg(lineattr[x]);
            uint32_t ch = linechar[x];

            if(fg != prevfg || !started)
                cur += sprintf(cur, "\\m[%s]", ansi2troff[fg]);
            if(bg != prevbg || !started)
                cur += sprintf(cur, "\\M[%s]", ansi2troff[bg]);
            if(lineattr[x] & CACA_BOLD)
                cur += sprintf(cur, "\\fB");
            if(lineattr[x] & CACA_ITALICS)
                cur += sprintf(cur, "\\fI");

            if(ch == '\\')
                cur += sprintf(cur, "\\\\");
            else if(ch == ' ')
            {
                /* Use unbreakable space at line ends, else spaces are dropped */
                if(x == 0 || x == cv->width-1)
                    cur += sprintf(cur, "%c%c", 0xc2, 0xa0);
                else
                    cur += caca_utf32_to_utf8(cur, ch);
            }
            else
                cur += caca_utf32_to_utf8(cur, ch);

            if(lineattr[x] & (CACA_BOLD|CACA_ITALICS))
                cur += sprintf(cur, "\\fR");

            prevfg = fg;
            prevbg = bg;
            started = 1;
        }
        cur += sprintf(cur, "\n");
    }
    /* Crop to really used size */
    debug("troff export: alloc %lu bytes, realloc %lu",
          (unsigned long int)*bytes, (unsigned long int)(cur - data));
    *bytes = (uintptr_t)(cur - data);
    data = realloc(data, *bytes);

    return data;
}

/*
 * XXX: The following functions are aliases.
 */

void *cucul_export_memory(cucul_canvas_t const *, char const *,
                          size_t *) CACA_ALIAS(caca_export_canvas_to_memory);
void *caca_export_memory(caca_canvas_t const *, char const *,
                         size_t *) CACA_ALIAS(caca_export_canvas_to_memory);
char const * const * cucul_get_export_list(void)
         CACA_ALIAS(caca_get_export_list);

