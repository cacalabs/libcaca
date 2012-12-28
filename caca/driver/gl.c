/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2002-2012 Sam Hocevar <sam@hocevar.net>
 *                2006 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                2007 Ben Wiley Sittler <bsittler@gmail.com>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

/*
 *  This file contains the libcaca OpenGL input and output driver
 */

#include "config.h"

#if defined(USE_GL)

#ifdef HAVE_OPENGL_GL_H
#   include <OpenGL/gl.h>
#   include <GLUT/glut.h>
#else
#   include <GL/gl.h>
#   include <GL/glut.h>
#   include <GL/freeglut_ext.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "caca.h"
#include "caca_internals.h"

/*
 * Global variables
 */

static int glut_init;
static caca_display_t *gl_d; /* FIXME: we ought to get rid of this */

/*
 * Local functions
 */
static int gl_get_event_inner(caca_display_t *, caca_privevent_t *);
static void gl_handle_keyboard(unsigned char, int, int);
static void gl_handle_special_key(int, int, int);
static void gl_handle_reshape(int, int);
static void gl_handle_mouse(int, int, int, int);
static void gl_handle_mouse_motion(int, int);
#ifdef HAVE_GLUTCLOSEFUNC
static void gl_handle_close(void);
#endif
static void _display(void);
static void gl_compute_font(caca_display_t *);

struct driver_private
{
    int window;
    int width, height;
    int new_width, new_height;
    caca_font_t *f;
    float font_width, font_height;
    float incx, incy;
    uint32_t const *blocks;
    int *txid;
    uint8_t close;
    uint8_t bit;
    uint8_t mouse_changed, mouse_clicked;
    int mouse_x, mouse_y;
    int mouse_button, mouse_state;

    uint8_t key;
    int special_key;

    float sw, sh;
};

static int gl_init_graphics(caca_display_t *dp)
{
    char const *geometry;
    char *argv[2] = { "", NULL };
    char const * const * fonts;
    int width = caca_get_canvas_width(dp->cv);
    int height = caca_get_canvas_height(dp->cv);
    int argc = 1;

    dp->drv.p = malloc(sizeof(struct driver_private));

    gl_d = dp;

#if defined(HAVE_GETENV)
    geometry = getenv("CACA_GEOMETRY");
    if(geometry && *geometry)
        sscanf(geometry, "%ux%u", &width, &height);
#endif

    dp->resize.allow = 1;
    caca_set_canvas_size(dp->cv, width ? width : 80, height ? height : 32);
    dp->resize.allow = 0;

    /* Load a libcaca internal font */
    fonts = caca_get_font_list();
    if(fonts[0] == NULL)
    {
        fprintf(stderr, "error: libcaca was compiled without any fonts\n");
        return -1;
    }
    dp->drv.p->f = caca_load_font(fonts[0], 0);
    if(dp->drv.p->f == NULL)
    {
        fprintf(stderr, "error: could not load font \"%s\"\n", fonts[0]);
        return -1;
    }

    dp->drv.p->font_width = caca_get_font_width(dp->drv.p->f);
    dp->drv.p->font_height = caca_get_font_height(dp->drv.p->f);

    dp->drv.p->width = caca_get_canvas_width(dp->cv) * dp->drv.p->font_width;
    dp->drv.p->height = caca_get_canvas_height(dp->cv) * dp->drv.p->font_height;

#ifdef HAVE_GLUTCLOSEFUNC
    dp->drv.p->close = 0;
#endif
    dp->drv.p->bit = 0;

    dp->drv.p->mouse_changed = dp->drv.p->mouse_clicked = 0;
    dp->drv.p->mouse_button = dp->drv.p->mouse_state = 0;

    dp->drv.p->key = 0;
    dp->drv.p->special_key = 0;

    dp->drv.p->sw = ((float)dp->drv.p->font_width) / 16.0f;
    dp->drv.p->sh = ((float)dp->drv.p->font_height) / 16.0f;

    if(!glut_init)
    {
        glut_init = 1;
        glutInit(&argc, argv);
    }

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(dp->drv.p->width, dp->drv.p->height);
    dp->drv.p->window = glutCreateWindow("caca for GL");

    gluOrtho2D(0, dp->drv.p->width, dp->drv.p->height, 0);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glutKeyboardFunc(gl_handle_keyboard);
    glutSpecialFunc(gl_handle_special_key);
    glutReshapeFunc(gl_handle_reshape);
    glutDisplayFunc(_display);

#ifdef HAVE_GLUTCLOSEFUNC
    glutCloseFunc(gl_handle_close);
#endif

    glutMouseFunc(gl_handle_mouse);
    glutMotionFunc(gl_handle_mouse_motion);
    glutPassiveMotionFunc(gl_handle_mouse_motion);

    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, dp->drv.p->width, dp->drv.p->height, 0);

    glMatrixMode(GL_MODELVIEW);

    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_TEXTURE_2D);

    gl_compute_font(dp);

    return 0;
}

static int gl_end_graphics(caca_display_t *dp)
{
    glutHideWindow();
    glutDestroyWindow(dp->drv.p->window);
    caca_free_font(dp->drv.p->f);
    free(dp->drv.p->txid);
    free(dp->drv.p);
    return 0;
}

static int gl_set_display_title(caca_display_t *dp, char const *title)
{
    glutSetWindowTitle(title);
    return 0;
}

static int gl_get_display_width(caca_display_t const *dp)
{
    return dp->drv.p->width;
}

static int gl_get_display_height(caca_display_t const *dp)
{
    return dp->drv.p->height;
}

static void gl_display(caca_display_t *dp)
{
    uint32_t const *cvchars = caca_get_canvas_chars(dp->cv);
    uint32_t const *cvattrs = caca_get_canvas_attrs(dp->cv);
    int width = caca_get_canvas_width(dp->cv);
    int x, y, line;

    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    line = 0;
    for(y = 0; y < dp->drv.p->height; y += dp->drv.p->font_height)
    {
        uint32_t const *attrs = cvattrs + line * width;

        /* FIXME: optimise using stride */
        for(x = 0; x < dp->drv.p->width; x += dp->drv.p->font_width)
        {
            uint16_t bg = caca_attr_to_rgb12_bg(*attrs++);

            glColor4b(((bg & 0xf00) >> 8) * 8,
                      ((bg & 0x0f0) >> 4) * 8,
                      (bg & 0x00f) * 8,
                      0xff);
            glBegin(GL_QUADS);
            glVertex2f(x, y);
            glVertex2f(x + dp->drv.p->font_width, y);
            glVertex2f(x + dp->drv.p->font_width,
                       y + dp->drv.p->font_height);
            glVertex2f(x, y + dp->drv.p->font_height);
            glEnd();
        }

        line++;
    }

    /* 2nd pass, avoids changing render state too much */
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    line = 0;
    for(y = 0; y < dp->drv.p->height; y += dp->drv.p->font_height, line++)
    {
        uint32_t const *attrs = cvattrs + line * width;
        uint32_t const *chars = cvchars + line * width;

        for(x = 0; x < dp->drv.p->width; x += dp->drv.p->font_width, attrs++)
        {
            uint32_t ch = *chars++;
            uint16_t fg;
            int i, b, fullwidth;

            fullwidth = caca_utf32_is_fullwidth(ch);

            for(b = 0, i = 0; dp->drv.p->blocks[i + 1]; i += 2)
            {
                if(ch < (uint32_t)dp->drv.p->blocks[i])
                     break;

                if(ch >= (uint32_t)dp->drv.p->blocks[i + 1])
                {
                    b += (uint32_t)(dp->drv.p->blocks[i + 1]
                                     - dp->drv.p->blocks[i]);
                    continue;
                }

                glBindTexture(GL_TEXTURE_2D,
                              dp->drv.p->txid[b + ch
                                        - (uint32_t)dp->drv.p->blocks[i]]);

                fg = caca_attr_to_rgb12_fg(*attrs);
                glColor3b(((fg & 0xf00) >> 8) * 8,
                          ((fg & 0x0f0) >> 4) * 8,
                          (fg & 0x00f) * 8);
                /* FIXME: handle fullwidth glyphs here */
                glBegin(GL_QUADS);
                glTexCoord2f(0, dp->drv.p->sh);
                glVertex2f(x, y);
                glTexCoord2f(dp->drv.p->sw, dp->drv.p->sh);
                glVertex2f(x + dp->drv.p->font_width * (fullwidth ? 2 : 1), y);
                glTexCoord2f(dp->drv.p->sw, 0);
                glVertex2f(x + dp->drv.p->font_width * (fullwidth ? 2 : 1),
                           y + dp->drv.p->font_height);
                glTexCoord2f(0, 0);
                glVertex2f(x, y + dp->drv.p->font_height);
                glEnd();
            }

            if(fullwidth)
            {
                chars++; attrs++; x += dp->drv.p->font_width;
            }
        }
    }

#ifdef HAVE_GLUTCHECKLOOP
    glutCheckLoop();
#else
    glutMainLoopEvent();
#endif
    glutSwapBuffers();
    glutPostRedisplay();
}

static void gl_handle_resize(caca_display_t *dp)
{
    dp->drv.p->width = dp->drv.p->new_width;
    dp->drv.p->height = dp->drv.p->new_height;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glViewport(0, 0, dp->drv.p->width, dp->drv.p->height);
    gluOrtho2D(0, dp->drv.p->width, dp->drv.p->height, 0);
    glMatrixMode(GL_MODELVIEW);
}

static int gl_get_event(caca_display_t *dp, caca_privevent_t *ev)
{
    int ret = gl_get_event_inner(dp, ev);

    if (ret)
        return ret;

#ifdef HAVE_GLUTCHECKLOOP
    glutCheckLoop();
#else
    glutMainLoopEvent();
#endif

    return gl_get_event_inner(dp, ev);
}

static int gl_get_event_inner(caca_display_t *dp, caca_privevent_t *ev)
{
#ifdef HAVE_GLUTCLOSEFUNC
    if(dp->drv.p->close)
    {
        dp->drv.p->close = 0;
        ev->type = CACA_EVENT_QUIT;
        return 1;
    }
#endif

    if(dp->resize.resized)
    {
        ev->type = CACA_EVENT_RESIZE;
        ev->data.resize.w = caca_get_canvas_width(dp->cv);
        ev->data.resize.h = caca_get_canvas_height(dp->cv);
        return 1;
    }

    if(dp->drv.p->mouse_changed)
    {
        ev->type = CACA_EVENT_MOUSE_MOTION;
        ev->data.mouse.x = dp->mouse.x;
        ev->data.mouse.y = dp->mouse.y;
        dp->drv.p->mouse_changed = 0;

        if(dp->drv.p->mouse_clicked)
        {
            _push_event(dp, ev);
            ev->type = CACA_EVENT_MOUSE_PRESS;
            ev->data.mouse.button = dp->drv.p->mouse_button;
            dp->drv.p->mouse_clicked = 0;
        }

        return 1;
    }

    if(dp->drv.p->key != 0)
    {
        ev->type = CACA_EVENT_KEY_PRESS;
        ev->data.key.ch = dp->drv.p->key;
        ev->data.key.utf32 = (uint32_t)dp->drv.p->key;
        ev->data.key.utf8[0] = dp->drv.p->key;
        ev->data.key.utf8[1] = '\0';
        dp->drv.p->key = 0;
        return 1;
    }

    if(dp->drv.p->special_key != 0)
    {
        switch(dp->drv.p->special_key)
        {
            case GLUT_KEY_F1 : ev->data.key.ch = CACA_KEY_F1; break;
            case GLUT_KEY_F2 : ev->data.key.ch = CACA_KEY_F2; break;
            case GLUT_KEY_F3 : ev->data.key.ch = CACA_KEY_F3; break;
            case GLUT_KEY_F4 : ev->data.key.ch = CACA_KEY_F4; break;
            case GLUT_KEY_F5 : ev->data.key.ch = CACA_KEY_F5; break;
            case GLUT_KEY_F6 : ev->data.key.ch = CACA_KEY_F6; break;
            case GLUT_KEY_F7 : ev->data.key.ch = CACA_KEY_F7; break;
            case GLUT_KEY_F8 : ev->data.key.ch = CACA_KEY_F8; break;
            case GLUT_KEY_F9 : ev->data.key.ch = CACA_KEY_F9; break;
            case GLUT_KEY_F10: ev->data.key.ch = CACA_KEY_F10; break;
            case GLUT_KEY_F11: ev->data.key.ch = CACA_KEY_F11; break;
            case GLUT_KEY_F12: ev->data.key.ch = CACA_KEY_F12; break;
            case GLUT_KEY_LEFT : ev->data.key.ch = CACA_KEY_LEFT; break;
            case GLUT_KEY_RIGHT: ev->data.key.ch = CACA_KEY_RIGHT; break;
            case GLUT_KEY_UP   : ev->data.key.ch = CACA_KEY_UP; break;
            case GLUT_KEY_DOWN : ev->data.key.ch = CACA_KEY_DOWN; break;
            case GLUT_KEY_PAGE_UP : ev->data.key.ch = CACA_KEY_PAGEUP; break;
            case GLUT_KEY_PAGE_DOWN  : ev->data.key.ch = CACA_KEY_PAGEDOWN;
                break;
            case GLUT_KEY_HOME : ev->data.key.ch = CACA_KEY_HOME; break;
            case GLUT_KEY_END : ev->data.key.ch = CACA_KEY_END; break;
            case GLUT_KEY_INSERT : ev->data.key.ch = CACA_KEY_INSERT; break;

            default: ev->type = CACA_EVENT_NONE; return 0;
        }

        ev->type = CACA_EVENT_KEY_PRESS;
        ev->data.key.utf32 = 0;
        ev->data.key.utf8[0] = '\0';

        dp->drv.p->special_key = 0;
        return 1;
    }

    ev->type = CACA_EVENT_NONE;
    return 0;
}


static void gl_set_mouse(caca_display_t *dp, int flag)
{
    if(flag)
        glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
    else
        glutSetCursor(GLUT_CURSOR_NONE);
}

/*
 * XXX: following functions are local
 */

static void gl_handle_keyboard(unsigned char key, int x, int y)
{
    caca_display_t *dp = gl_d;

    dp->drv.p->key = key;
}

static void gl_handle_special_key(int key, int x, int y)
{
    caca_display_t *dp = gl_d;

    dp->drv.p->special_key = key;
}

static void gl_handle_reshape(int w, int h)
{
    caca_display_t *dp = gl_d;

    if(dp->drv.p->bit) /* Do not handle reshaping at the first time */
    {
        dp->drv.p->new_width = w;
        dp->drv.p->new_height = h;

        dp->resize.w = w / dp->drv.p->font_width;
        dp->resize.h = (h / dp->drv.p->font_height) + 1;

        dp->resize.resized = 1;
    }
    else
        dp->drv.p->bit = 1;
}

static void gl_handle_mouse(int button, int state, int x, int y)
{
    caca_display_t *dp = gl_d;

    dp->drv.p->mouse_clicked = 1;
    dp->drv.p->mouse_button = button;
    dp->drv.p->mouse_state = state;
    dp->drv.p->mouse_x = x / dp->drv.p->font_width;
    dp->drv.p->mouse_y = y / dp->drv.p->font_height;
    dp->mouse.x = dp->drv.p->mouse_x;
    dp->mouse.y = dp->drv.p->mouse_y;
    dp->drv.p->mouse_changed = 1;
}

static void gl_handle_mouse_motion(int x, int y)
{
    caca_display_t *dp = gl_d;
    dp->drv.p->mouse_x = x / dp->drv.p->font_width;
    dp->drv.p->mouse_y = y / dp->drv.p->font_height;
    dp->mouse.x = dp->drv.p->mouse_x;
    dp->mouse.y = dp->drv.p->mouse_y;
    dp->drv.p->mouse_changed = 1;
}

#ifdef HAVE_GLUTCLOSEFUNC
static void gl_handle_close(void)
{
    caca_display_t *dp = gl_d;
    dp->drv.p->close = 1;
}
#endif

static void _display(void)
{
    caca_display_t *dp = gl_d;
    gl_display(dp);
}

static void gl_compute_font(caca_display_t *dp)
{
    caca_canvas_t *cv;
    uint32_t *image;
    int i, b, w, h, x, y;

    /* Count how many glyphs this font has */
    dp->drv.p->blocks = caca_get_font_blocks(dp->drv.p->f);

    for(b = 0, i = 0; dp->drv.p->blocks[i + 1]; i += 2)
        b += (int)(dp->drv.p->blocks[i + 1] - dp->drv.p->blocks[i]);

    /* Allocate a libcaca canvas and print all the glyphs on it */
    cv = caca_create_canvas(2, b);
    caca_set_color_ansi(cv, CACA_WHITE, CACA_BLACK);

    for(b = 0, i = 0; dp->drv.p->blocks[i + 1]; i += 2)
    {
        int j, n = (int)(dp->drv.p->blocks[i + 1] - dp->drv.p->blocks[i]);

        for(j = 0; j < n; j++)
            caca_put_char(cv, 0, b + j, dp->drv.p->blocks[i] + j);

        b += n;
    }

    /* Draw the caca canvas onto an image buffer */
    image = malloc(b * dp->drv.p->font_height *
                   2 * dp->drv.p->font_width * sizeof(uint32_t));
    caca_render_canvas(cv, dp->drv.p->f, image, 2 * dp->drv.p->font_width,
                        b * dp->drv.p->font_height, 8 * dp->drv.p->font_width);
    caca_free_canvas(cv);

    /* Convert all glyphs in the image buffer to GL textures */
    dp->drv.p->txid = malloc(b * sizeof(int));

    w = dp->drv.p->font_width <= 16 ? dp->drv.p->font_width : 16;
    h = dp->drv.p->font_height <= 16 ? dp->drv.p->font_height : 16;

    for(b = 0, i = 0; dp->drv.p->blocks[i + 1]; i += 2)
    {
        int j, n = (int)(dp->drv.p->blocks[i + 1] - dp->drv.p->blocks[i]);

        for(j = 0; j < n; j++)
        {
            uint8_t tmp[16 * 8 * 16];
            uint32_t *glyph = image + (int)((b + j) * dp->drv.p->font_width * 2
                                            * dp->drv.p->font_height);
            int fullwidth =
                    caca_utf32_is_fullwidth(dp->drv.p->blocks[i] + j);

            memset(tmp, 0, 16 * 8 * 16);

            for(y = 0; y < h; y++)
            {
                for(x = 0; x < w * (fullwidth ? 2 : 1); x++)
                {
                    int offset = x + (15 - y) * (fullwidth ? 32 : 16);
                    uint8_t c = glyph[x + y * 2 * (int)dp->drv.p->font_width]
                                 >> 8;
                    tmp[offset * 4] = c;
                    tmp[offset * 4 + 1] = c;
                    tmp[offset * 4 + 2] = c;
                    tmp[offset * 4 + 3] = c;
                }
            }

            glGenTextures(1, (GLuint*)&dp->drv.p->txid[b + j]);
            glBindTexture(GL_TEXTURE_2D, dp->drv.p->txid[b + j]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, 4, (fullwidth ? 32 : 16), 16, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, tmp);
        }

        b += n;
    }

    free(image);
}

/*
 * Driver initialisation
 */

int gl_install(caca_display_t *dp)
{
#if defined(HAVE_GETENV) && defined(GLUT_XLIB_IMPLEMENTATION)
    if(!getenv("DISPLAY") || !*(getenv("DISPLAY")))
        return -1;
#endif

    dp->drv.id = CACA_DRIVER_GL;
    dp->drv.driver = "gl";

    dp->drv.init_graphics = gl_init_graphics;
    dp->drv.end_graphics = gl_end_graphics;
    dp->drv.set_display_title = gl_set_display_title;
    dp->drv.get_display_width = gl_get_display_width;
    dp->drv.get_display_height = gl_get_display_height;
    dp->drv.display = gl_display;
    dp->drv.handle_resize = gl_handle_resize;
    dp->drv.get_event = gl_get_event;
    dp->drv.set_mouse = gl_set_mouse;
    dp->drv.set_cursor = NULL;

    return 0;
}

#endif /* USE_GL */

