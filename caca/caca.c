/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2006-2012 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

/*
 *  This file contains the main functions used by \e libcaca applications to
 *  initialise the library, get the screen properties, set the framerate and
 *  so on.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdlib.h>
#   include <string.h>
#   include <stdio.h>
#   if defined(USE_PLUGINS)
#       if defined(HAVE_DLFCN_H)
#           include <dlfcn.h>
#       endif
#   endif
#endif

#include "caca.h"
#include "caca_internals.h"

#if defined(USE_PLUGINS)
#   define gl_install(p) caca_plugin_install(p, "gl")
#   define x11_install(p) caca_plugin_install(p, "x11")
#endif

static int caca_can_resize(caca_display_t *);
static int caca_install_driver(caca_display_t *, char const *);
static int caca_uninstall_driver(caca_display_t *);
static int caca_select_driver(caca_display_t *, char const *);
#if defined(USE_PLUGINS)
static int caca_plugin_install(caca_display_t *, char const *);
#endif

/** \brief Attach a caca graphical context to a caca canvas.
 *
 *  Create a graphical context using device-dependent features (ncurses for
 *  terminals, an X11 window, a DOS command window...) that attaches to a
 *  libcaca canvas. Everything that gets drawn in the libcaca canvas can
 *  then be displayed by the libcaca driver.
 *
 *  If no caca canvas is provided, a new one is created. Its handle can be
 *  retrieved using caca_get_canvas() and it is automatically destroyed when
 *  caca_free_display() is called.
 *
 *  See also caca_create_display_with_driver().
 *
 *  If an error occurs, NULL is returned and \b errno is set accordingly:
 *  - \c ENOMEM Not enough memory.
 *  - \c ENODEV Graphical device could not be initialised.
 *
 *  \param cv The caca canvas or NULL to create a canvas automatically.
 *  \return The caca graphical context or NULL if an error occurred.
 */
caca_display_t * caca_create_display(caca_canvas_t *cv)
{
    return caca_create_display_with_driver(cv, NULL);
}

/** \brief Attach a specific caca graphical context to a caca canvas.
 *
 *  Create a graphical context using device-dependent features (ncurses for
 *  terminals, an X11 window, a DOS command window...) that attaches to a
 *  libcaca canvas. Everything that gets drawn in the libcaca canvas can
 *  then be displayed by the libcaca driver.
 *
 *  If no caca canvas is provided, a new one is created. Its handle can be
 *  retrieved using caca_get_canvas() and it is automatically destroyed when
 *  caca_free_display() is called.
 *
 *  If no driver name is provided, \e libcaca will try to autodetect the best
 *  output driver it can.
 *
 *  See also caca_create_display().
 *
 *  If an error occurs, NULL is returned and \b errno is set accordingly:
 *  - \c ENOMEM Not enough memory.
 *  - \c ENODEV Graphical device could not be initialised.
 *
 *  \param cv The caca canvas or NULL to create a canvas automatically.
 *  \param driver A string describing the desired output driver or NULL to
 *                choose the best driver automatically.
 *  \return The caca graphical context or NULL if an error occurred.
 */
caca_display_t * caca_create_display_with_driver(caca_canvas_t *cv,
                                                 char const *driver)
{
    caca_display_t *dp = malloc(sizeof(caca_display_t));

    if(!dp)
    {
        seterrno(ENOMEM);
        return NULL;
    }

    if((dp->autorelease = (cv == NULL)))
    {
        cv = caca_create_canvas(0, 0);
    }

    dp->cv = cv;

    if(caca_manage_canvas(cv, (int (*)(void *))caca_can_resize, (void *)dp))
    {
        if(dp->autorelease)
            caca_free_canvas(dp->cv);
        free(dp);
        seterrno(EBUSY);
        return NULL;
    }

    if(caca_install_driver(dp, driver))
    {
        caca_unmanage_canvas(cv, (int (*)(void *))caca_can_resize, (void *)dp);
        if(dp->autorelease)
            caca_free_canvas(dp->cv);
        free(dp);
        seterrno(ENODEV);
        return NULL;
    }

    return dp;
}

/** \brief Get available display drivers
 *
 *  Return a list of available display drivers. The list is a NULL-terminated
 *  array of strings, interleaving a string containing the internal value for
 *  the display driver, and a string containing the natural language
 *  description for that driver.
 *
 *  This function never fails.
 *
 *  \return An array of strings.
 */
char const * const * caca_get_display_driver_list(void)
{
    static char const * const list[] =
    {
#if defined(USE_COCOA)
        "cocoa", "Mac OS X Cocoa",
#endif
#if defined(USE_WIN32)
        "win32", "Windows console",
#endif
#if defined(USE_CONIO)
        "conio", "MS-DOS conio",
#endif
#if defined(USE_X11)
        "x11", "X11 graphical window",
#endif
#if defined(USE_GL)
        "gl", "OpenGL window",
#endif
#if defined(USE_SLANG)
        "slang", "S-Lang console library",
#endif
#if defined(USE_NCURSES)
        "ncurses", "ncurses console library",
#endif
#if defined(USE_VGA)
        "vga", "direct VGA memory",
#endif
#if !defined(__KERNEL__)
        "raw", "raw libcaca output",
        "null", "null driver",
#endif
        NULL, NULL
    };

    return list;
}

/** \brief Return a caca graphical context's current output driver.
 *
 *  Return the given display's current output driver.
 *
 *  This function never fails.
 *
 *  \param dp The caca display.
 *  \return A static string.
 */
char const * caca_get_display_driver(caca_display_t *dp)
{
    return dp->drv.driver;
}

/** \brief Set the output driver.
 *
 *  Dynamically change the given display's output driver.
 *
 *  FIXME: decide what to do in case of failure
 *
 *  \param dp The caca display.
 *  \param driver A string describing the desired output driver or NULL to
 *                choose the best driver automatically.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int caca_set_display_driver(caca_display_t *dp, char const *driver)
{
    caca_uninstall_driver(dp);
    if(caca_install_driver(dp, driver))
    {
        seterrno(ENODEV);
        return -1;
    }

    return 0;
}

/** \brief Detach a caca graphical context from a caca backend context.
 *
 *  Detach a graphical context from its caca backend and destroy it. The
 *  libcaca canvas continues to exist and other graphical contexts can be
 *  attached to it afterwards.
 *
 *  If the caca canvas was automatically created by caca_create_display(),
 *  it is automatically destroyed and any handle to it becomes invalid.
 *
 *  This function never fails.
 *
 *  \param dp The libcaca graphical context.
 *  \return This function always returns 0.
 */
int caca_free_display(caca_display_t *dp)
{
    caca_uninstall_driver(dp);
    caca_unmanage_canvas(dp->cv, (int (*)(void *))caca_can_resize, (void *)dp);
    if(dp->autorelease)
        caca_free_canvas(dp->cv);
    free(dp);

    return 0;
}

/** \brief Get the canvas attached to a caca graphical context.
 *
 *  Return a handle on the \e caca_canvas_t object that was either attached
 *  or created by caca_create_display().
 *
 *  This function never fails.
 *
 *  \param dp The libcaca graphical context.
 *  \return The libcaca canvas.
 */
caca_canvas_t * caca_get_canvas(caca_display_t *dp)
{
    return dp->cv;
}

/** \brief Return the \e libcaca version.
 *
 *  Return a read-only string with the \e libcaca version information.
 *
 *  This function never fails.
 *
 *  \return The \e libcaca version information.
 */
char const * caca_get_version(void)
{
    return PACKAGE_VERSION;
}

/*
 * XXX: The following functions are local.
 */

static int caca_can_resize(caca_display_t *dp)
{
    return dp->resize.allow;
}

static int caca_install_driver(caca_display_t *dp, char const *driver)
{
#if defined(USE_PLUGINS)
    dp->plugin = NULL;
#endif

    if(caca_select_driver(dp, driver))
    {
#if defined(USE_PLUGINS)
        if(dp->plugin)
            dlclose(dp->plugin);
#endif
        return -1;
    }

    if(dp->drv.init_graphics(dp))
    {
#if defined(USE_PLUGINS)
        if(dp->plugin)
            dlclose(dp->plugin);
#endif
        return -1;
    }

    /* Graphics stuff */
    dp->delay = 0;
    dp->rendertime = 0;

    /* Events stuff */
#if defined(USE_SLANG) || defined(USE_NCURSES)
    dp->events.key_timer.last_sec = 0;
    dp->events.key_timer.last_usec = 0;
    dp->events.last_key_ticks = 0;
    dp->events.autorepeat_ticks = 0;
    dp->events.last_key_event.type = CACA_EVENT_NONE;
#endif
#if defined(USE_SLANG) || defined(USE_NCURSES) || defined(USE_CONIO) || defined(USE_GL)
    dp->events.queue = 0;
#endif

    dp->timer.last_sec = 0;
    dp->timer.last_usec = 0;
#if defined PROF
    _caca_init_stat(&dp->display_stat, "dp[%p] disp_sys time", dp);
    _caca_init_stat(&dp->wait_stat, "dp[%p] disp_wait time", dp);
    _caca_init_stat(&dp->ev_sys_stat, "dp[%p] ev_sys time", dp);
    _caca_init_stat(&dp->ev_wait_stat, "dp[%p] ev_wait time", dp);
#endif
    dp->lastticks = 0;

    /* Mouse position */
    dp->mouse.x = caca_get_canvas_width(dp->cv) / 2;
    dp->mouse.y = caca_get_canvas_height(dp->cv) / 2;

    /* Resize events */
    dp->resize.resized = 0;
    dp->resize.allow = 0;

    return 0;
}

static int caca_uninstall_driver(caca_display_t *dp)
{
    dp->drv.end_graphics(dp);
#if defined(USE_PLUGINS)
    if(dp->plugin)
        dlclose(dp->plugin);
#endif
#if defined PROF
    _caca_fini_stat(&dp->display_stat);
    _caca_fini_stat(&dp->wait_stat);
    _caca_fini_stat(&dp->ev_wait_stat);
    _caca_fini_stat(&dp->ev_sys_stat);
#endif

    return 0;
}

static int caca_select_driver(caca_display_t *dp, char const *driver)
{
    char const *var = driver;
#if defined(HAVE_GETENV)
    if(!var)
        var = getenv("CACA_DRIVER");
#endif

#if defined(HAVE_STRCASECMP)
    /* If the environment variable was set, use it */
    if(var && *var)
    {
#if defined(USE_COCOA)
        if(!strcasecmp(var, "cocoa")) return cocoa_install(dp);
#endif
#if defined(USE_WIN32)
        if(!strcasecmp(var, "win32")) return win32_install(dp);
#endif
#if defined(USE_CONIO)
        if(!strcasecmp(var, "conio")) return conio_install(dp);
#endif
#if defined(USE_X11)
        if(!strcasecmp(var, "x11")) return x11_install(dp);
#endif
#if defined(USE_GL)
        if(!strcasecmp(var, "gl")) return gl_install(dp);
#endif
#if !defined(__KERNEL__)
        if(!strcasecmp(var, "raw")) return raw_install(dp);
#endif
#if defined(USE_SLANG)
        if(!strcasecmp(var, "slang")) return slang_install(dp);
#endif
#if defined(USE_NCURSES)
        if(!strcasecmp(var, "ncurses")) return ncurses_install(dp);
#endif
#if defined(USE_VGA)
        if(!strcasecmp(var, "vga")) return vga_install(dp);
#endif
#if !defined(__KERNEL__)
        if(!strcasecmp(var, "null")) return null_install(dp);
#endif
        return -1;
    }
#endif

#if defined(USE_COCOA)
    if(cocoa_install(dp) == 0) return 0;
#endif
#if defined(USE_WIN32)
    if(win32_install(dp) == 0) return 0;
#endif
#if defined(USE_CONIO)
    if(conio_install(dp) == 0) return 0;
#endif
#if defined(USE_VGA)
    if(vga_install(dp) == 0) return 0;
#endif
#if defined(USE_X11)
    if(x11_install(dp) == 0) return 0;
#endif
#if defined(USE_GL)
    if(gl_install(dp) == 0) return 0;
#endif
    /* ncurses has a higher priority than slang because it has better colour
     * support across terminal types, despite being slightly slower. */
#if defined(USE_NCURSES)
    if(ncurses_install(dp) == 0) return 0;
#endif
#if defined(USE_SLANG)
    if(slang_install(dp) == 0) return 0;
#endif
    /* Of course we don't try "raw" or "null" if the user did not
     * specifically ask for them. */

    return -1;
}

#if defined(USE_PLUGINS)
static int caca_plugin_install(caca_display_t *dp, char const *driver)
{
    char buf[512];
    int (*sym) (caca_display_t *);

    sprintf(buf, "%s/lib%s_plugin.so", PLUGINDIR, driver);
    dp->plugin = dlopen(buf, RTLD_NOW);
    if(!dp->plugin)
    {
        sprintf(buf, "lib%s_plugin.so", driver);
        dp->plugin = dlopen(buf, RTLD_NOW);
        if(!dp->plugin)
            return -1;
    }

    sprintf(buf, "%s_install", driver);
    sym = dlsym(dp->plugin, buf);
    if(!sym)
    {
        dlclose(dp->plugin);
        return -1;
    }

    return sym(dp);
}
#endif

/*
 * XXX: The following functions are aliases.
 */

char const * cucul_get_version(void) CACA_ALIAS(caca_get_version);

