/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
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
 *  This file contains the libcaca OpenGL input and output driver
 */

#include "config.h"
#include "common.h"

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
#include "cucul.h"
#include "cucul_internals.h"


/*
 * Global variables
 */

static caca_display_t *gl_d; /* FIXME: we ought to get rid of this */

/*
 * Local functions
 */
static void gl_handle_keyboard(unsigned char, int, int);
static void gl_handle_special_key(int, int, int);
static void gl_handle_reshape(int, int);
static void gl_handle_mouse(int, int, int, int);
static void gl_handle_mouse_motion(int, int);
#ifdef HAVE_GLUTCLOSEFUNC
static void gl_handle_close(void);
#endif
static void _display(void);
void gl_generate_glyph(uint32_t c, uint32_t tid, caca_display_t *dp);
void gl_generate_unicode_glyph(uint32_t c, uint32_t tid, caca_display_t *dp);

struct driver_private
{
    int window;
    unsigned int width, height;
    unsigned int new_width, new_height;
    cucul_font_t *f;
    float font_width, font_height;
    float incx, incy;
    int id[(128 - 32)];
    int id_uni[8]; /* Hack, FIXME */
    unsigned char close;
    unsigned char bit;
    unsigned char mouse_changed, mouse_clicked;
    unsigned int mouse_x, mouse_y;
    unsigned int mouse_button, mouse_state;

    unsigned char key;
    int special_key;

    float sw, sh;

};

static int gl_init_graphics(caca_display_t *dp)
{
    char *empty_texture;
    char const *geometry;
    char *argv[2] = { "", NULL };
    char const * const * fonts;
    unsigned int width = 0, height = 0;
    int argc = 1;
    int i;

    dp->drv.p = malloc(sizeof(struct driver_private));

    gl_d = dp;

#if defined(HAVE_GETENV)
    geometry = getenv("CACA_GEOMETRY");
    if(geometry && *geometry)
        sscanf(geometry, "%ux%u", &width, &height);
#endif

    if(width && height)
        _cucul_set_canvas_size(dp->cv, width, height);


    /* Load a libcucul internal font */
    fonts = cucul_get_font_list();
    if(fonts[0] == NULL)
    {
        fprintf(stderr, "error: libcucul was compiled without any fonts\n");
        return -1;
    }
    dp->drv.p->f = cucul_load_font(fonts[0], 0);
    if(dp->drv.p->f == NULL)
    {
        fprintf(stderr, "error: could not load font \"%s\"\n", fonts[0]);
        return -1;
    }

    dp->drv.p->font_width = cucul_get_font_width(dp->drv.p->f);
    dp->drv.p->font_height = cucul_get_font_height(dp->drv.p->f);

    dp->drv.p->width = dp->cv->width * dp->drv.p->font_width;
    dp->drv.p->height = dp->cv->height * dp->drv.p->font_height;

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

    glutInit(&argc, argv);

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

    empty_texture = malloc(16 * 16 * 4);
    if(empty_texture == NULL)
        return -1;

    memset(empty_texture, 0xff, 16 * 16 * 4);
    glEnable(GL_TEXTURE_2D);

    /* ASCII glyphes textures initialisation */
    for(i = 32; i < 128; i++)
    {
        glGenTextures(1, (GLuint*)&dp->drv.p->id[i - 32]);
        glBindTexture(GL_TEXTURE_2D, dp->drv.p->id[i - 32]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, 4,
                     16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, empty_texture);
    }

    /* Unicode (CP437) glyphes textures initialisation */
    for(i = 0; i < 8; i++)
    {
        glGenTextures(1, (GLuint*)&dp->drv.p->id_uni[i]);
        glBindTexture(GL_TEXTURE_2D, dp->drv.p->id_uni[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, 4,
                     16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, empty_texture);
    }

    /* Get textures for ASCII glyphs */
    for(i = 32; i < 128; i++)
        gl_generate_glyph(i, i-32, dp);


    gl_generate_unicode_glyph(0x00002580, 0, dp);
    gl_generate_unicode_glyph(0x00002584, 1, dp);
    gl_generate_unicode_glyph(0x00002588, 2, dp);
    gl_generate_unicode_glyph(0x0000258c, 3, dp);
    gl_generate_unicode_glyph(0x00002590, 4, dp);
    gl_generate_unicode_glyph(0x00002591, 5, dp);
    gl_generate_unicode_glyph(0x00002592, 6, dp);
    gl_generate_unicode_glyph(0x00002593, 7, dp);

    return 0;
}

static int gl_end_graphics(caca_display_t *dp)
{
    glutDestroyWindow(dp->drv.p->window);
    free(dp->drv.p);
    return 0;
}

static int gl_set_display_title(caca_display_t *dp, char const *title)
{
    glutSetWindowTitle(title);
    return 0;
}

static unsigned int gl_get_display_width(caca_display_t *dp)
{
    return dp->drv.p->width;
}

static unsigned int gl_get_display_height(caca_display_t *dp)
{
    return dp->drv.p->height;
}

static void gl_display(caca_display_t *dp)
{
    unsigned int x, y, line;

    glClear(GL_COLOR_BUFFER_BIT);

    line = 0;
    for(y = 0; y < dp->drv.p->height; y += dp->drv.p->font_height)
    {
        uint32_t *attr = dp->cv->attr + line * dp->cv->width;

        for(x = 0; x < dp->drv.p->width; x += dp->drv.p->font_width)
        {
            uint16_t bg = _cucul_argb32_to_rgb12bg(*attr++);
            glDisable(GL_TEXTURE_2D);
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
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    line = 0;
    for(y = 0; y < dp->drv.p->height; y += dp->drv.p->font_height, line++)
    {
        uint32_t *attr = dp->cv->attr + line * dp->cv->width;
        uint32_t *chars = dp->cv->chars + line * dp->cv->width;

        for(x = 0; x < dp->drv.p->width; x += dp->drv.p->font_width, attr++)
        {
            uint32_t cv = *chars++;
            uint16_t fg;

            if(cv == ' ')
                continue;

            if(cv > 0x00000020 && cv < 0x00000080)
            {
                glBindTexture(GL_TEXTURE_2D, dp->drv.p->id[cv - 32]);
            }
            else switch(cv)
            {
                case 0x00002580: glBindTexture(GL_TEXTURE_2D, dp->drv.p->id_uni[0]); break;
                case 0x00002584: glBindTexture(GL_TEXTURE_2D, dp->drv.p->id_uni[1]); break;
                case 0x00002588: glBindTexture(GL_TEXTURE_2D, dp->drv.p->id_uni[2]); break;
                case 0x0000258c: glBindTexture(GL_TEXTURE_2D, dp->drv.p->id_uni[3]); break;
                case 0x00002590: glBindTexture(GL_TEXTURE_2D, dp->drv.p->id_uni[4]); break;
                case 0x00002591: glBindTexture(GL_TEXTURE_2D, dp->drv.p->id_uni[5]); break;
                case 0x00002592: glBindTexture(GL_TEXTURE_2D, dp->drv.p->id_uni[6]); break;
                case 0x00002593: glBindTexture(GL_TEXTURE_2D, dp->drv.p->id_uni[7]); break;
                default:         glBindTexture(GL_TEXTURE_2D, dp->drv.p->id['?' - 32]); break;
            }

            fg = _cucul_argb32_to_rgb12fg(*attr);
            glColor3b(((fg & 0xf00) >> 8) * 8,
                      ((fg & 0x0f0) >> 4) * 8,
                      (fg & 0x00f) * 8);
            glBegin(GL_QUADS);
            glTexCoord2f(0, dp->drv.p->sh);
            glVertex2f(x, y);
            glTexCoord2f(dp->drv.p->sw, dp->drv.p->sh);
            glVertex2f(x + dp->drv.p->font_width, y);
            glTexCoord2f(dp->drv.p->sw, 0);
            glVertex2f(x + dp->drv.p->font_width,
                       y + dp->drv.p->font_height);
            glTexCoord2f(0, 0);
            glVertex2f(x, y + dp->drv.p->font_height);
            glEnd();
        }
    }
    glBlendFunc(GL_ONE, GL_ZERO);
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

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

static int gl_get_event(caca_display_t *dp, caca_event_t *ev)
{
#ifdef HAVE_GLUTCHECKLOOP
    glutCheckLoop();
#else
    glutMainLoopEvent();
#endif

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
        ev->data.resize.w = dp->cv->width;
        ev->data.resize.h = dp->cv->height;
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


/*
 * Driver initialisation
 */

int gl_install(caca_display_t *dp)
{
#if defined(HAVE_GETENV) && defined(GLUT_XLIB_IMPLEMENTATION)
    if(!getenv("DISPLAY") || !*(getenv("DISPLAY")))
        return -1;
#endif

    dp->drv.driver = CACA_DRIVER_GL;

    dp->drv.init_graphics = gl_init_graphics;
    dp->drv.end_graphics = gl_end_graphics;
    dp->drv.set_display_title = gl_set_display_title;
    dp->drv.get_display_width = gl_get_display_width;
    dp->drv.get_display_height = gl_get_display_height;
    dp->drv.display = gl_display;
    dp->drv.handle_resize = gl_handle_resize;
    dp->drv.get_event = gl_get_event;
    dp->drv.set_mouse = gl_set_mouse;

    return 0;
}



void gl_generate_glyph(uint32_t c, uint32_t tid, caca_display_t *dp) {
    int s,d;
    uint8_t *glyph8 =  calloc(dp->drv.p->font_width*dp->drv.p->font_height, 1);
    uint8_t *glyph32 = calloc(16*16*4, 1);

    cucul_render_glyph(dp->drv.p->f, c, glyph8, dp->drv.p->font_width);

    /* Convert resulting 8bbp glyph to 32bits, 16x16*/
    for(s=0;s<(dp->drv.p->font_height<=16?dp->drv.p->font_height:16);s++) {
        for(d=0;d<dp->drv.p->font_width;d++)
        {
            uint32_t offset = d*4+(15-s)*16*4;
            uint8_t c = glyph8[d+s*(int)dp->drv.p->font_width];
            glyph32[offset] = c;
            glyph32[1+offset] = c;
            glyph32[2+offset] = c;
            glyph32[3+offset] = c;
        }
    }

    glBindTexture(GL_TEXTURE_2D, dp->drv.p->id[tid]);
    glTexImage2D(GL_TEXTURE_2D,
                 0, 4,
                 16,16,
                 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 glyph32);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    free(glyph8);
    free(glyph32);
}

void gl_generate_unicode_glyph(uint32_t c, uint32_t tid, caca_display_t *dp) {
    int s,d;
    uint8_t *glyph8 =  calloc(dp->drv.p->font_width*dp->drv.p->font_height, 1);
    uint8_t *glyph32 = calloc(16*16*4, 1);

    cucul_render_glyph(dp->drv.p->f, c, glyph8, dp->drv.p->font_width);

    /* Convert resulting 8bbp glyph to 32bits, 16x16*/
    for(s=0;s<(dp->drv.p->font_height<=16?dp->drv.p->font_height:16);s++) {
        for(d=0;d<(dp->drv.p->font_width<=16?dp->drv.p->font_width:16);d++)
        {
            uint32_t offset = d*4+(15-s)*16*4;
            uint8_t c = glyph8[d+s*(int)dp->drv.p->font_width];
            glyph32[offset] = c;
            glyph32[1+offset] = c;
            glyph32[2+offset] = c;
            glyph32[3+offset] = c;
        }
    }

    glBindTexture(GL_TEXTURE_2D, dp->drv.p->id_uni[tid]);
    glTexImage2D(GL_TEXTURE_2D,
                 0, 4,
                 16,16,
                 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 glyph32);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    free(glyph8);
    free(glyph32);
}






#endif /* USE_GL */

