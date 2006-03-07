/*
 *  libcaca       ASCII-Art library
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/** \file driver_gl.c
 *  \version \$Id$
 *  \author Jean-Yves Lamoureux <jylam@lnxscene.org>
 *  \brief OpenGL driver
 *
 *  This file contains the libcaca OpenGL input and output driver
 */

#include "config.h"

#if defined(USE_GL)

#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>

#include <stdio.h> /* BUFSIZ */
#include <string.h>
#include <stdlib.h>
#if defined(HAVE_UNISTD_H)
#   include <unistd.h>
#endif
#include <stdarg.h>

#include "caca.h"
#include "caca_internals.h"
#include "cucul.h"
#include "cucul_internals.h"

/*
 * Global variables
 */

/* Ok, I just suck. */
static GLbyte const gl_bgpal[][4] =
{
    { 0x00, 0x00, 0x00, 0x7f },
    { 0x00, 0x00, 0x3f, 0x7f },
    { 0x00, 0x3f, 0x00, 0x7f },
    { 0x00, 0x3f, 0x3f, 0x7f },
    { 0x3f, 0x00, 0x00, 0x7f },
    { 0x3f, 0x00, 0x3f, 0x7f },
    { 0x3f, 0x3f, 0x00, 0x7f },
    { 0x3f, 0x3f, 0x3f, 0x7f },
    // + intensity
    { 0x00, 0x00, 0x00, 0x7f },
    { 0x00, 0x00, 0x7f, 0x7f },
    { 0x00, 0x7f, 0x00, 0x7f },
    { 0x00, 0x7f, 0x7f, 0x7f },
    { 0x7f, 0x00, 0x00, 0x7f },
    { 0x7f, 0x00, 0x7f, 0x7f },
    { 0x7f, 0x7f, 0x00, 0x7f },
    { 0x7f, 0x7f, 0x7f, 0x7f }
};

static caca_t *gl_kk; /* FIXME: we ought to get rid of this */

/*
 * Local functions
 */
static void gl_handle_keyboard(unsigned char, int, int);
static void gl_handle_special_key(int, int, int);
static void gl_handle_reshape(int, int);
static void gl_handle_mouse(int, int, int, int);
static void gl_handle_mouse_motion(int, int);

static int gl_init_graphics(caca_t *kk)
{
    char *empty_texture;
    char const *geometry;
    char *argv[2] = { "", NULL };
    unsigned int width = 0, height = 0;
    int argc = 1;
    int i;

    gl_kk = kk;

    geometry = getenv("CACA_GEOMETRY");
    if(geometry && *(geometry))
        sscanf(geometry, "%ux%u", &width, &height);

    if(width && height)
        cucul_set_size(kk->qq, width, height);

    kk->gl.font_width = 9;
    kk->gl.font_height = 15;

    kk->gl.width = kk->qq->width * kk->gl.font_width;
    kk->gl.height = kk->qq->height * kk->gl.font_height;

    kk->gl.resized = 0;
    kk->gl.bit = 0;

    kk->gl.mouse_changed = kk->gl.mouse_clicked = 0;
    kk->gl.mouse_button = kk->gl.mouse_state = 0;

    kk->gl.key = 0;
    kk->gl.special_key = 0;

    kk->gl.sw = 9.0f / 16.0f;
    kk->gl.sh = 15.0f / 16.0f;

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(kk->gl.width, kk->gl.height);
    kk->gl.window = glutCreateWindow("caca for GL");

    gluOrtho2D(0, kk->gl.width, kk->gl.height, 0);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glutKeyboardFunc(gl_handle_keyboard);
    glutSpecialFunc(gl_handle_special_key);
    glutReshapeFunc(gl_handle_reshape);

    glutMouseFunc(gl_handle_mouse);
    glutMotionFunc(gl_handle_mouse_motion);
    glutPassiveMotionFunc(gl_handle_mouse_motion);

    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, kk->gl.width, kk->gl.height, 0);

    glMatrixMode(GL_MODELVIEW);

    glClear(GL_COLOR_BUFFER_BIT);

    empty_texture = malloc(16 * 16 * 4);
    if(empty_texture == NULL)
        return -1;

    memset(empty_texture, 0xff, 16 * 16 * 4);
    glEnable(GL_TEXTURE_2D);

    for(i = 0; i < 94; i++)
    {
        glGenTextures(1, (GLuint*)&kk->gl.id[i]);
        glBindTexture(GL_TEXTURE_2D, kk->gl.id[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8,
                     16, 16, 0, GL_RGB, GL_UNSIGNED_BYTE, empty_texture);
    }

    for(i = 0; i < 94; i++)
    {
        glDisable(GL_TEXTURE_2D);
        glClear(GL_COLOR_BUFFER_BIT);

        glColor3f(1, 1, 1);
        glRasterPos2f(0, 15);
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, i + 32);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, kk->gl.id[i]);
        glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                         0, kk->gl.height - 16, 16, 16, 0);

        glutMainLoopEvent();
        glutPostRedisplay();
    }

    return 0;
}

static int gl_end_graphics(caca_t *kk)
{
    glutDestroyWindow(kk->gl.window);
    return 0;
}

static int gl_set_window_title(caca_t *kk, char const *title)
{
    glutSetWindowTitle(title);
    return 0;
}

static unsigned int gl_get_window_width(caca_t *kk)
{
    return kk->gl.width;
}

static unsigned int gl_get_window_height(caca_t *kk)
{
    return kk->gl.height;
}

static void gl_display(caca_t *kk)
{
    unsigned int x, y, line;

    glClear(GL_COLOR_BUFFER_BIT);

    line = 0;
    for(y = 0; y < kk->gl.height; y += kk->gl.font_height)
    {
        uint8_t *attr = kk->qq->attr + line * kk->qq->width;

        for(x = 0; x < kk->gl.width; x += kk->gl.font_width)
        {
            glDisable(GL_TEXTURE_2D);
            glColor4bv(gl_bgpal[attr[0] >> 4]);
            glBegin(GL_QUADS);
                glVertex2f(x, y);
                glVertex2f(x + kk->gl.font_width, y);
                glVertex2f(x + kk->gl.font_width, y + kk->gl.font_height);
                glVertex2f(x, y + kk->gl.font_height);
            glEnd();

            attr++;
        }

        line++;
    }

    /* 2nd pass, avoids changing render state too much */
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_ONE, GL_ONE);

    line = 0;
    for(y = 0; y < kk->gl.height; y += kk->gl.font_height)
    {
        uint8_t *attr = kk->qq->attr + line * kk->qq->width;
        uint32_t *chars = kk->qq->chars + line * kk->qq->width;

        for(x = 0; x < kk->gl.width; x += kk->gl.font_width)
        {
            if(*chars != (uint32_t)' ')
            {
                char ch = *chars & 0x7f;

                /* FIXME: check ch bounds */
                glBindTexture(GL_TEXTURE_2D, kk->gl.id[ch - 32]);
                glColor4bv(gl_bgpal[attr[0] & 0xf]);
                glBegin(GL_QUADS);
                    glTexCoord2f(0, kk->gl.sh);
                    glVertex2f(x, y);
                    glTexCoord2f(kk->gl.sw, kk->gl.sh);
                    glVertex2f(x + kk->gl.font_width, y);
                    glTexCoord2f(kk->gl.sw, 0);
                    glVertex2f(x + kk->gl.font_width, y + kk->gl.font_height);
                    glTexCoord2f(0, 0);
                    glVertex2f(x, y + kk->gl.font_height);
                glEnd();
            }

            attr++;
            chars++;
        }
        line++;
    }
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

    glutMainLoopEvent();
    glutSwapBuffers();
    glutPostRedisplay();
}

static void gl_handle_resize(caca_t *kk, unsigned int *new_width,
                                         unsigned int *new_height)
{
    kk->gl.width = kk->gl.new_width;
    kk->gl.height = kk->gl.new_height;

    *new_width = kk->gl.width / kk->gl.font_width;
    *new_height = (kk->gl.height / kk->gl.font_height) + 1;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glViewport(0, 0, kk->gl.width, kk->gl.height);
    gluOrtho2D(0, kk->gl.width, kk->gl.height, 0);
    glMatrixMode(GL_MODELVIEW);
}

/*
 * XXX: following functions are local
 */

static void gl_handle_keyboard(unsigned char key, int x, int y)
{
    caca_t *kk = gl_kk;

    kk->gl.key = key;
}

static void gl_handle_special_key(int key, int x, int y)
{
    caca_t *kk = gl_kk;

    kk->gl.special_key = key;
}

static void gl_handle_reshape(int w, int h)
{
    caca_t *kk = gl_kk;

    if(kk->gl.bit) /* Do not handle reshaping at the first time */
    {
        kk->gl.new_width = w;
        kk->gl.new_height = h;

        kk->gl.resized = 1;
    }
    else
        kk->gl.bit = 1;
}

static void gl_handle_mouse(int button, int state, int x, int y)
{
    caca_t *kk = gl_kk;

    kk->gl.mouse_clicked = 1;
    kk->gl.mouse_button = button;
    kk->gl.mouse_state = state;
    kk->gl.mouse_x = x / kk->gl.font_width;
    kk->gl.mouse_y = y / kk->gl.font_height;
    kk->gl.mouse_changed = 1;
}

static void gl_handle_mouse_motion(int x, int y)
{
    caca_t *kk = gl_kk;

    kk->gl.mouse_x = x / kk->gl.font_width;
    kk->gl.mouse_y = y / kk->gl.font_height;
    kk->gl.mouse_changed = 1;
}

/*
 * Driver initialisation
 */

void gl_init_driver(caca_t *kk)
{
    kk->driver.driver = CACA_DRIVER_GL;

    kk->driver.init_graphics = gl_init_graphics;
    kk->driver.end_graphics = gl_end_graphics;
    kk->driver.set_window_title = gl_set_window_title;
    kk->driver.get_window_width = gl_get_window_width;
    kk->driver.get_window_height = gl_get_window_height;
    kk->driver.display = gl_display;
    kk->driver.handle_resize = gl_handle_resize;
}

#endif /* USE_GL */

