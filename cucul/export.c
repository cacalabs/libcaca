/*
 *  libcucul      Canvas for ultrafast compositing of Unicode letters
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
 *                2006 Jean-Yves Lamoureux <jylam@lnxscene.org>
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
 *  This file contains various export functions
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdlib.h>
#   include <stdio.h>
#   include <string.h>
#endif

#include "cucul.h"
#include "cucul_internals.h"

static void export_ansi(cucul_canvas_t *, cucul_buffer_t *);
static void export_html(cucul_canvas_t *, cucul_buffer_t *);
static void export_html3(cucul_canvas_t *, cucul_buffer_t *);
static void export_irc(cucul_canvas_t *, cucul_buffer_t *);
static void export_ps(cucul_canvas_t *, cucul_buffer_t *);
static void export_svg(cucul_canvas_t *, cucul_buffer_t *);
static void export_tga(cucul_canvas_t *, cucul_buffer_t *);

/** \brief Export a canvas into a foreign format.
 *
 *  This function exports a libcucul canvas into various foreign formats such
 *  as ANSI art, HTML, IRC colours, etc. One should use cucul_get_buffer_data()
 *  and cucul_get_buffer_size() to access the buffer contents. The allocated
 *  data is valid until cucul_free_buffer() is called.
 *
 *  Valid values for \e format are:
 *
 *  \li \e "ansi": export ANSI art (CP437 charset with ANSI colour codes).
 *
 *  \li \e "html": export an HTML page with CSS information.
 *
 *  \li \e "html3": export an HTML table that should be compatible with
 *      most navigators, including textmode ones.
 *
 *  \li \e "irc": export UTF-8 text with mIRC colour codes.
 *
 *  \li \e "ps": export a PostScript document.
 *
 *  \li \e "svg": export an SVG vector image.
 *
 *  \li \e "tga": export a TGA image.
 *
 *  \param cv A libcucul canvas
 *  \param format A string describing the requested output format.
 */
cucul_buffer_t * cucul_create_export(cucul_canvas_t *cv, char const *format)
{
    cucul_buffer_t *ex;

    ex = malloc(sizeof(cucul_buffer_t));
    ex->size = 0;
    ex->data = NULL;

    if(!strcasecmp("ansi", format))
        export_ansi(cv, ex);
    else if(!strcasecmp("html", format))
        export_html(cv, ex);
    else if(!strcasecmp("html3", format))
        export_html3(cv, ex);
    else if(!strcasecmp("irc", format))
        export_irc(cv, ex);
    else if(!strcasecmp("ps", format))
        export_ps(cv, ex);
    else if(!strcasecmp("svg", format))
        export_svg(cv, ex);
    else if(!strcasecmp("tga", format))
        export_tga(cv, ex);

    if(ex->size == 0)
    {
        free(ex);
        return NULL;
    }

    return ex;
}

/** \brief Get available export formats
 *
 *  Return a list of available export formats. The list is a NULL-terminated
 *  array of strings, interleaving a string containing the internal value for
 *  the export format, to be used with \e cucul_create_export(), and a string
 *  containing the natural language description for that export format.
 *
 *  \return An array of strings.
 */
char const * const * cucul_get_export_list(void)
{
    static char const * const list[] =
    {
        "ansi", "ANSI",
        "html", "HTML",
        "html3", "backwards-compatible HTML",
        "irc", "IRC with mIRC colours",
        "ps", "PostScript document",
        "svg", "SVG vector image",
        "tga", "TGA image",
        NULL, NULL
    };

    return list;
}

/*
 * XXX: the following functions are local.
 */

/* Generate ANSI representation of current canvas. */
static void export_ansi(cucul_canvas_t *cv, cucul_buffer_t *ex)
{
    static uint8_t const palette[] =
    {
        0,  4,  2,  6, 1,  5,  3,  7,
        8, 12, 10, 14, 9, 13, 11, 15
    };

    char *cur;
    unsigned int x, y;

    /* 23 bytes assumed for max length per pixel ('\e[5;1;3x;4y;9x;10ym' plus
     * 4 max bytes for a UTF-8 character).
     * Add height*9 to that (zeroes color at the end and jump to next line) */
    ex->size = (cv->height * 9) + (cv->width * cv->height * 23);
    ex->data = malloc(ex->size);

    cur = ex->data;

    for(y = 0; y < cv->height; y++)
    {
        uint32_t *lineattr = cv->attr + y * cv->width;
        uint32_t *linechar = cv->chars + y * cv->width;

        uint8_t prevfg = -1;
        uint8_t prevbg = -1;

        for(x = 0; x < cv->width; x++)
        {
            uint8_t fg = palette[_cucul_argb32_to_ansi4fg(lineattr[x])];
            uint8_t bg = palette[_cucul_argb32_to_ansi4bg(lineattr[x])];
            uint32_t ch = linechar[x];

            if(fg != prevfg || bg != prevbg)
            {
                cur += sprintf(cur, "\033[0;");

                if(fg < 8)
                    if(bg < 8)
                        cur += sprintf(cur, "3%d;4%dm", fg, bg);
                    else
                        cur += sprintf(cur, "5;3%d;4%d;10%dm",
                                            fg, bg - 8, bg - 8);
                else
                    if(bg < 8)
                        cur += sprintf(cur, "1;3%d;4%d;9%dm",
                                            fg - 8, bg, fg - 8);
                    else
                        cur += sprintf(cur, "5;1;3%d;4%d;9%d;10%dm",
                                            fg - 8, bg - 8, fg - 8, bg - 8);
            }

            *cur++ = ch & 0x7f;

            prevfg = fg;
            prevbg = bg;
        }

        cur += sprintf(cur, "\033[0m\r\n");
    }

    /* Crop to really used size */
    ex->size = (uintptr_t)(cur - ex->data);
    ex->data = realloc(ex->data, ex->size);
}

/* Generate HTML representation of current canvas. */
static void export_html(cucul_canvas_t *cv, cucul_buffer_t *ex)
{
    char *cur;
    unsigned int x, y, len;

    /* The HTML header: less than 1000 bytes
     * A line: 7 chars for "<br />\n"
     * A glyph: 47 chars for "<span style="color:#xxx;background-color:#xxx">"
     *          up to 9 chars for "&#xxxxxx;", far less for pure ASCII
     *          7 chars for "</span>" */
    ex->size = 1000 + cv->height * (7 + cv->width * (47 + 9 + 7));
    ex->data = malloc(ex->size);

    cur = ex->data;

    /* HTML header */
    cur += sprintf(cur, "<html><head>\n");
    cur += sprintf(cur, "<title>Generated by libcaca %s</title>\n", VERSION);
    cur += sprintf(cur, "</head><body>\n");

    cur += sprintf(cur, "<div cellpadding='0' cellspacing='0' style='%s'>\n",
                        "font-family: monospace, fixed; font-weight: bold;");

    for(y = 0; y < cv->height; y++)
    {
        uint32_t *lineattr = cv->attr + y * cv->width;
        uint32_t *linechar = cv->chars + y * cv->width;

        for(x = 0; x < cv->width; x += len)
        {
            cur += sprintf(cur, "<span style=\"color:#%.03x;"
                                "background-color:#%.03x\">",
                                _cucul_argb32_to_rgb12fg(lineattr[x]),
                                _cucul_argb32_to_rgb12bg(lineattr[x]));

            for(len = 0;
                x + len < cv->width && lineattr[x + len] == lineattr[x];
                len++)
            {
                if(linechar[x + len] <= 0x00000020)
                    cur += sprintf(cur, "&nbsp;");
                else if(linechar[x + len] < 0x00000080)
                    cur += sprintf(cur, "%c", linechar[x + len]);
                else
                    cur += sprintf(cur, "&#%i;", linechar[x + len]);
            }
            cur += sprintf(cur, "</span>");
        }
        /* New line */
        cur += sprintf(cur, "<br />\n");
    }

    cur += sprintf(cur, "</div></body></html>\n");

    /* Crop to really used size */
    ex->size = strlen(ex->data) + 1;
    ex->data = realloc(ex->data, ex->size);
}

/* Export an HTML3 document. This function is way bigger than export_html(),
 * but permits viewing in old browsers (or limited ones such as links). It
 * will not work under gecko (mozilla rendering engine) unless you set a
 * correct header. */
static void export_html3(cucul_canvas_t *cv, cucul_buffer_t *ex)
{
    char *cur;
    unsigned int x, y, len;

    /* The HTML table markup: less than 1000 bytes
     * A line: 10 chars for "<tr></tr>\n"
     * A glyph: 40 chars for "<td bgcolor=#xxxxxx><font color=#xxxxxx>"
     *          up to 9 chars for "&#xxxxxx;", far less for pure ASCII
     *          12 chars for "</font></td>" */
    ex->size = 1000 + cv->height * (10 + cv->width * (40 + 9 + 12));
    ex->data = malloc(ex->size);

    cur = ex->data;

    /* Table */
    cur += sprintf(cur, "<table cols='%d' cellpadding='0' cellspacing='0'>\n",
                        cv->height);

    for(y = 0; y < cv->height; y++)
    {
        uint32_t *lineattr = cv->attr + y * cv->width;
        uint32_t *linechar = cv->chars + y * cv->width;

        cur += sprintf(cur, "<tr>");

        for(x = 0; x < cv->width; x += len)
        {
            unsigned int i;

            /* Use colspan option to factor cells with same attributes
             * (see below) */
            len = 1;
            while(x + len < cv->width && lineattr[x + len] == lineattr[x])
                len++;

            cur += sprintf(cur, "<td bgcolor=#%.06x",
                           _cucul_argb32_to_rgb24bg(lineattr[x]));

            if(len > 1)
                cur += sprintf(cur, " colspan=%d", len);

            cur += sprintf(cur, "><font color=#%.06x>",
                           _cucul_argb32_to_rgb24fg(lineattr[x]));

            for(i = 0; i < len; i++)
            {
                if(linechar[x + i] <= 0x00000020)
                    cur += sprintf(cur, "&nbsp;");
                else if(linechar[x + i] < 0x00000080)
                    cur += sprintf(cur, "%c", linechar[x + i]);
                else
                    cur += sprintf(cur, "&#%i;", linechar[x + i]);
            }

            cur += sprintf(cur, "</font></td>");
        }
        cur += sprintf(cur, "</tr>\n");
    }

    /* Footer */
    cur += sprintf(cur, "</table>\n");

    /* Crop to really used size */
    ex->size = (uintptr_t)(cur - ex->data);
    ex->data = realloc(ex->data, ex->size);
}

/* Export a text file with IRC colours */
static void export_irc(cucul_canvas_t *cv, cucul_buffer_t *ex)
{
    static uint8_t const palette[] =
    {
        1, 2, 3, 10, 5, 6, 7, 15, /* Dark */
        14, 12, 9, 11, 4, 13, 8, 0, /* Light */
    };

    char *cur;
    unsigned int x, y;

    /* 11 bytes assumed for max length per pixel. Worst case scenario:
     * ^Cxx,yy   6 bytes
     * ^B^B      2 bytes
     * ch        1 byte
     * \r\n      2 bytes
     * In real life, the average bytes per pixel value will be around 5.
     */

    ex->size = 2 + (cv->width * cv->height * 11);
    ex->data = malloc(ex->size);

    cur = ex->data;

    for(y = 0; y < cv->height; y++)
    {
        uint32_t *lineattr = cv->attr + y * cv->width;
        uint32_t *linechar = cv->chars + y * cv->width;

        uint8_t prevfg = -1;
        uint8_t prevbg = -1;

        for(x = 0; x < cv->width; x++)
        {
            uint8_t fg = palette[_cucul_argb32_to_ansi4fg(lineattr[x])];
            uint8_t bg = palette[_cucul_argb32_to_ansi4bg(lineattr[x])];
            uint32_t ch = linechar[x];

            if(bg == prevbg)
            {
                if(fg == prevfg)
                    ; /* Same fg/bg, do nothing */
                else if(ch == (uint32_t)' ')
                    fg = prevfg; /* Hackety hack */
                else
                {
                    cur += sprintf(cur, "\x03%d", fg);
                    if(ch >= (uint32_t)'0' && ch <= (uint32_t)'9')
                        cur += sprintf(cur, "\x02\x02");
                }
            }
            else
            {
                if(fg == prevfg)
                    cur += sprintf(cur, "\x03,%d", bg);
                else
                    cur += sprintf(cur, "\x03%d,%d", fg, bg);

                if(ch >= (uint32_t)'0' && ch <= (uint32_t)'9')
                    cur += sprintf(cur, "\x02\x02");
            }
            *cur++ = ch & 0x7f;
            prevfg = fg;
            prevbg = bg;
        }
        *cur++ = '\r';
        *cur++ = '\n';
    }

    /* Crop to really used size */
    ex->size = (uintptr_t)(cur - ex->data);
    ex->data = realloc(ex->data, ex->size);
}

/* Export a PostScript document. */
static void export_ps(cucul_canvas_t *cv, cucul_buffer_t *ex)
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

    char *cur;
    unsigned int x, y;

    /* 200 is arbitrary but should be ok */
    ex->size = strlen(ps_header) + (cv->width * cv->height * 200);
    ex->data = malloc(ex->size);

    cur = ex->data;

    /* Header */
    cur += sprintf(cur, "%s", ps_header);

    /* Background, drawn using csquare macro defined in header */
    for(y = cv->height; y--; )
    {
        uint32_t *lineattr = cv->attr + y * cv->width;

        for(x = 0; x < cv->width; x++)
        {
            uint8_t argb[8];
            _cucul_argb32_to_argb4(*lineattr++, argb);
            cur += sprintf(cur, "1 0 translate\n %f %f %f csquare\n",
                           (float)argb[1] * (1.0 / 0xf),
                           (float)argb[2] * (1.0 / 0xf),
                           (float)argb[3] * (1.0 / 0xf));
        }

        /* Return to beginning of the line, and jump to the next one */
        cur += sprintf(cur, "-%d 1 translate\n", cv->width);
    }

    cur += sprintf(cur, "grestore\n"); /* Restore transformation matrix */

    for(y = cv->height; y--; )
    {
        uint32_t *lineattr = cv->attr + (cv->height - y - 1) * cv->width;
        uint32_t *linechar = cv->chars + (cv->height - y - 1) * cv->width;

        for(x = 0; x < cv->width; x++)
        {
            uint8_t argb[8];
            uint32_t ch = *linechar++;

            _cucul_argb32_to_argb4(*lineattr++, argb);

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
                    cur += sprintf(cur, "(\\%c) show\n", ch);
                    break;
                default:
                    cur += sprintf(cur, "(%c) show\n", ch);
                    break;
            }
        }
    }

    cur += sprintf(cur, "showpage\n");

    /* Crop to really used size */
    ex->size = (uintptr_t)(cur - ex->data);
    ex->data = realloc(ex->data, ex->size);
}

/* Export an SVG vector image */
static void export_svg(cucul_canvas_t *cv, cucul_buffer_t *ex)
{
    static char const svg_header[] =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<svg width=\"%d\" height=\"%d\" viewBox=\"0 0 %d %d\""
        " xmlns=\"http://www.w3.org/2000/svg\""
        " xmlns:xlink=\"http://www.w3.org/1999/xlink\""
        " xml:space=\"preserve\" version=\"1.1\"  baseProfile=\"full\">\n";

    char *cur;
    unsigned int x, y;

    /* 200 is arbitrary but should be ok */
    ex->size = strlen(svg_header) + (cv->width * cv->height * 200);
    ex->data = malloc(ex->size);

    cur = ex->data;

    /* Header */
    cur += sprintf(cur, svg_header, cv->width * 6, cv->height * 10,
                                    cv->width * 6, cv->height * 10);

    cur += sprintf(cur, " <g id=\"mainlayer\" font-size=\"12\">\n");

    /* Background */
    for(y = 0; y < cv->height; y++)
    {
        uint32_t *lineattr = cv->attr + y * cv->width;

        for(x = 0; x < cv->width; x++)
        {
            cur += sprintf(cur, "<rect style=\"fill:#%.03x\" x=\"%d\" y=\"%d\""
                                " width=\"6\" height=\"10\"/>\n",
                                _cucul_argb32_to_rgb12bg(*lineattr++),
                                x * 6, y * 10);
        }
    }

    /* Text */
    for(y = 0; y < cv->height; y++)
    {
        uint32_t *lineattr = cv->attr + y * cv->width;
        uint32_t *linechar = cv->chars + y * cv->width;

        for(x = 0; x < cv->width; x++)
        {
            uint32_t ch = *linechar++;

            cur += sprintf(cur, "<text style=\"fill:#%.03x\" "
                                "x=\"%d\" y=\"%d\">",
                                _cucul_argb32_to_rgb12fg(*lineattr++),
                                x * 6, (y * 10) + 10);
            if(ch < 0x00000020)
                cur += sprintf(cur, "?");
            else if(ch > 0x0000007f)
            {
                static const uint8_t mark[7] =
                {
                    0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC
                };

                char buf[10], *parser;
                int bytes = (ch < 0x800) ? 2 : (ch < 0x10000) ? 3 : 4;

                buf[bytes] = '\0';
                parser = buf + bytes;

                switch(bytes)
                {
                    case 4: *--parser = (ch | 0x80) & 0xbf; ch >>= 6;
                    case 3: *--parser = (ch | 0x80) & 0xbf; ch >>= 6;
                    case 2: *--parser = (ch | 0x80) & 0xbf; ch >>= 6;
                }
                *--parser = ch | mark[bytes];

                cur += sprintf(cur, "%s", buf);
            }
            else switch((uint8_t)ch)
            {
                case '>': cur += sprintf(cur, "&gt;"); break;
                case '<': cur += sprintf(cur, "&lt;"); break;
                case '&': cur += sprintf(cur, "&amp;"); break;
                default: cur += sprintf(cur, "%c", ch); break;
            }
            cur += sprintf(cur, "</text>\n");
        }
    }

    cur += sprintf(cur, " </g>\n");
    cur += sprintf(cur, "</svg>\n");

    /* Crop to really used size */
    ex->size = (uintptr_t)(cur - ex->data);
    ex->data = realloc(ex->data, ex->size);
}

/* Export a TGA image */
static void export_tga(cucul_canvas_t *cv, cucul_buffer_t *ex)
{
    char const * const * fontlist;
    char * cur;
    cucul_font_t *f;
    unsigned int i, w, h;

    fontlist = cucul_get_font_list();
    if(!fontlist[0])
        return;

    f = cucul_load_font(fontlist[0], 0);

    w = cucul_get_width(cv) * cucul_get_font_width(f);
    h = cucul_get_height(cv) * cucul_get_font_height(f);

    ex->size = w * h * 4 + 18; /* 32 bpp + 18 bytes for the header */
    ex->data = malloc(ex->size);

    cur = ex->data;

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
    cucul_render_canvas(cv, f, cur, w, h, 4 * w);

    /* Swap bytes. What a waste of time. */
    for(i = 0; i < w * h * 4; i += 4)
    {
        char w;
        w = cur[i]; cur[i] = cur[i + 3]; cur[i + 3] = w;
        w = cur[i + 1]; cur[i + 1] = cur[i + 2]; cur[i + 2] = w;
    }

    cucul_free_font(f);
}

