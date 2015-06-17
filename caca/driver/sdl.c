/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2002-2012 Sam Hocevar <sam@hocevar.net>
 *                2013 Alex Foulon <alxf@lavabit.com>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 *
 */

/*
 *  This file contains a SDL libcaca input and output driver
 */

#include "config.h"

#if defined(USE_SDL)

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined HAVE_LOCALE_H
#   include <locale.h>
#endif

#include "caca.h"
#include "caca_internals.h"

static const int video_bpp = 32;
static const Uint32 video_flags = SDL_HWSURFACE
                                | SDL_HWPALETTE
                                | SDL_ASYNCBLIT
                                | SDL_DOUBLEBUF
                                | SDL_RESIZABLE;

struct driver_private {
    SDL_Surface *screen;
    caca_font_t *font;
    TTF_Font *ttf_font;
    int font_width, font_height;
    int is_active, use_ttf;
    uint32_t attr, ch;

};

static void sdl_write_utf32(caca_display_t *, int, int);

static int sdl_init_graphics(caca_display_t *dp)
{
    int width, height, ttf_size;
    char const *ttf_font_file, *ttf_font_size;
    char const *geometry;
    char const * const *fonts;

    dp->drv.p = malloc(sizeof(struct driver_private));

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "error: %s\n", SDL_GetError());
        return -1;
    }

    SDL_EnableUNICODE(SDL_ENABLE);
    SDL_WM_SetCaption("Caca for SDL", NULL);

    /* ignore unused events */
    SDL_EventState(SDL_ACTIVEEVENT, SDL_IGNORE);
    SDL_EventState(SDL_VIDEOEXPOSE, SDL_IGNORE);
    SDL_EventState(SDL_SYSWMEVENT, SDL_IGNORE);

    width = caca_get_canvas_width(dp->cv);
    height = caca_get_canvas_height(dp->cv);

    dp->drv.p->use_ttf = 0;
    fonts = caca_get_font_list();
    if(fonts[0] == NULL) {
        fprintf(stderr, "error: libcaca was compiled without any fonts\n");
        return -1;
    }
    dp->drv.p->font = caca_load_font(fonts[0], 0);
    if(dp->drv.p->font == NULL) {
        fprintf(stderr, "error: could not load font \"%s\"\n", fonts[0]);
        return -1;
    }

    dp->drv.p->font_width = caca_get_font_width(dp->drv.p->font);
    dp->drv.p->font_height = caca_get_font_height(dp->drv.p->font);

#if defined(HAVE_GETENV)
    ttf_font_file = getenv("CACA_FONT");
    if(ttf_font_file && *ttf_font_file) {
        if(TTF_Init() < 0)
            fprintf(stderr, "error: %s\n", TTF_GetError());

        ttf_font_size = getenv("CACA_FONT_SIZE");
        if(ttf_font_size && *ttf_font_size)
            sscanf(ttf_font_size, "%u", &ttf_size);
        else
            ttf_size = 12;

        /* no need to return error, fallback on compiled font */
        dp->drv.p->ttf_font = TTF_OpenFont(ttf_font_file, ttf_size);
        if(dp->drv.p->ttf_font == NULL) {
            fprintf(stderr, "error: %s\n", TTF_GetError());
        }
        else if(!TTF_FontFaceIsFixedWidth(dp->drv.p->ttf_font)) {
            fprintf(stderr, "error: SDL driver accept only fixed width font\n");
        }
        else {
            dp->drv.p->use_ttf = 1;
            TTF_SizeUTF8(dp->drv.p->ttf_font, "a",
                        &dp->drv.p->font_width, &dp->drv.p->font_height);
        }
    }

    geometry = getenv("CACA_GEOMETRY");
    if(geometry && *geometry)
        sscanf(geometry, "%ux%u", &width, &height);
#endif

#if defined HAVE_LOCALE_H
    setlocale(LC_ALL, "");
#endif

    dp->drv.p->screen = SDL_SetVideoMode(width * dp->drv.p->font_width,
                                         height * dp->drv.p->font_height,
                                         video_bpp, video_flags);
    if(dp->drv.p->screen == NULL) {
        fprintf(stderr, "error: %s\n", SDL_GetError());
        return -1;
    }

    dp->drv.p->is_active = 1;
    caca_add_dirty_rect(dp->cv, 0, 0, dp->cv->width, dp->cv->height);
    dp->resize.allow = 1;
    caca_set_canvas_size(dp->cv, width ? width : 80, height ? height : 32);
    width = caca_get_canvas_width(dp->cv);
    height = caca_get_canvas_height(dp->cv);
	dp->resize.allow = 0;

    return 0;
}

static int sdl_end_graphics(caca_display_t *dp)
{
    if(SDL_WasInit(SDL_INIT_VIDEO) != 0)
        SDL_Quit();

    if(TTF_WasInit() != 0) {
        TTF_CloseFont(dp->drv.p->ttf_font);
        TTF_Quit();
    }

    if(dp->drv.p->font != NULL)
        caca_free_font(dp->drv.p->font);
    free(dp->drv.p);

    return 0;
}

static int sdl_set_display_title(caca_display_t *dp, char const *title)
{
    SDL_WM_SetCaption(title, NULL);
    return 0;
}

static int sdl_get_display_width(caca_display_t const *dp)
{
    return (caca_get_canvas_width(dp->cv) * dp->drv.p->font_width);
}

static int sdl_get_display_height(caca_display_t const *dp)
{
    return (caca_get_canvas_height(dp->cv) * dp->drv.p->font_height);
}

static void sdl_display(caca_display_t *dp)
{
    int x, y, i;
    size_t len;
    void *buffer;
    SDL_RWops *rw;
    SDL_Surface *img;

    if(!dp->drv.p->is_active)
        return;

    if(!dp->drv.p->use_ttf) {
        buffer = caca_export_canvas_to_memory(dp->cv, "tga", &len);
        if(len > 0) {
            rw = SDL_RWFromMem(buffer, len);
            img = IMG_LoadTyped_RW(rw, 1, "TGA");
			SDL_FillRect(dp->drv.p->screen, NULL, 0x000000);
            SDL_BlitSurface(img, NULL, dp->drv.p->screen, NULL);
            SDL_FreeSurface(img);
            if(buffer != NULL)
                free(buffer);
        }
	}
    else {
        for(i = 0; i < caca_get_dirty_rect_count(dp->cv); i++)
        {
            uint32_t const *cvchars, *cvattrs;
            int dx, dy, dw, dh;

            caca_get_dirty_rect(dp->cv, i, &dx, &dy, &dw, &dh);

            cvchars = caca_get_canvas_chars(dp->cv) + dx + dy * dp->cv->width;
            cvattrs = caca_get_canvas_attrs(dp->cv) + dx + dy * dp->cv->width;

            for(y = dy; y < dy + dh; y++)
            {
                for(x = dx; x < dx + dw; x++)
                {
                    dp->drv.p->ch = *cvchars++;
                    dp->drv.p->attr = *cvattrs++;
                    sdl_write_utf32(dp, x, y);
                }
                cvchars += dp->cv->width - dw;
                cvattrs += dp->cv->width - dw;
            }
        }
    }

	SDL_Flip(dp->drv.p->screen);
}

static void sdl_write_utf32(caca_display_t *dp, int x, int y)
{
    int bytes;
    int style = TTF_STYLE_NORMAL;
    char buf[10];
    uint16_t color;
    uint32_t bg;
    SDL_Color fg;
    SDL_Rect pos;
    SDL_Surface *text;

    if(dp->drv.p->ch == CACA_MAGIC_FULLWIDTH)
        return;

    bytes = caca_utf32_to_utf8(buf, dp->drv.p->ch);
    buf[bytes] = '\0';

    if(dp->drv.p->attr & CACA_BOLD)
        style |= TTF_STYLE_BOLD;
    if(dp->drv.p->attr & CACA_ITALICS)
        style |= TTF_STYLE_ITALIC;
    if(dp->drv.p->attr & CACA_UNDERLINE)
        style |= TTF_STYLE_UNDERLINE;

    color = caca_attr_to_rgb12_fg(dp->drv.p->attr);
    fg.r = ((color & 0xf00) >> 8) * 0x11;
    fg.g = ((color & 0xf0) >> 4) * 0x11;
    fg.b = ((color & 0xf) >> 0) * 0x11;

    color = caca_attr_to_rgb12_bg(dp->drv.p->attr);
    bg = ((color & 0xf00) >> 8) * 0x110000;
    bg |= ((color & 0x0f0) >> 4) * 0x001100;
    bg |= ((color & 0x00f) >> 0) * 0x000011;

    pos.x = x * dp->drv.p->font_width;
    pos.y = y * dp->drv.p->font_height;
    pos.w = dp->drv.p->font_width;
    pos.h = dp->drv.p->font_height;

    if(caca_utf32_is_fullwidth(dp->drv.p->ch)) {
        pos.w *= 2;
        pos.h *= 2;
        SDL_FillRect(dp->drv.p->screen, &pos, bg);
    }
    else {
        SDL_FillRect(dp->drv.p->screen, &pos, bg);
    }

    TTF_SetFontStyle(dp->drv.p->ttf_font, style);
    text = TTF_RenderUTF8_Blended(dp->drv.p->ttf_font, buf, fg);
    SDL_BlitSurface(text, NULL, dp->drv.p->screen, &pos);
    SDL_FreeSurface(text);
}

static int sdl_get_event(caca_display_t *dp, caca_privevent_t *ev)
{
    SDL_Event event;

    while(SDL_PollEvent(&event))
    {
        size_t bytes;

        if(event.type == SDL_QUIT) {
            ev->type = CACA_EVENT_QUIT;
            return 1;
        }

        if(event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            if(event.type == SDL_KEYDOWN)
                ev->type = CACA_EVENT_KEY_PRESS;
            else if(event.type == SDL_KEYUP)
                ev->type = CACA_EVENT_KEY_RELEASE;

            switch(event.key.keysym.sym)
            {
                case SDLK_BACKSPACE:
                    ev->data.key.ch = CACA_KEY_BACKSPACE; break;
                case SDLK_DELETE:
                    ev->data.key.ch = CACA_KEY_DELETE; break;
                case SDLK_LEFT:
                    ev->data.key.ch = CACA_KEY_LEFT; break;
                case SDLK_RIGHT:
                    ev->data.key.ch = CACA_KEY_RIGHT; break;
                case SDLK_UP:
                    ev->data.key.ch = CACA_KEY_UP; break;
                case SDLK_DOWN:
                    ev->data.key.ch = CACA_KEY_DOWN; break;
                case SDLK_END:
                    ev->data.key.ch = CACA_KEY_END; break;
                case SDLK_ESCAPE:
                    ev->data.key.ch = CACA_KEY_ESCAPE; break;
                case SDLK_HOME:
                    ev->data.key.ch = CACA_KEY_HOME; break;
                case SDLK_INSERT:
                    ev->data.key.ch = CACA_KEY_INSERT; break;
                case SDLK_PAGEDOWN:
                    ev->data.key.ch = CACA_KEY_PAGEDOWN; break;
                case SDLK_PAGEUP:
                    ev->data.key.ch = CACA_KEY_PAGEUP; break;
                case SDLK_PAUSE:
                    ev->data.key.ch = CACA_KEY_PAUSE; break;
                case SDLK_RETURN:
                    ev->data.key.ch = CACA_KEY_RETURN; break;
                case SDLK_TAB:
                    ev->data.key.ch = CACA_KEY_TAB; break;

                case SDLK_F1:  ev->data.key.ch = CACA_KEY_F1; break;
                case SDLK_F2:  ev->data.key.ch = CACA_KEY_F2; break;
                case SDLK_F3:  ev->data.key.ch = CACA_KEY_F3; break;
                case SDLK_F4:  ev->data.key.ch = CACA_KEY_F4; break;
                case SDLK_F5:  ev->data.key.ch = CACA_KEY_F5; break;
                case SDLK_F6:  ev->data.key.ch = CACA_KEY_F6; break;
                case SDLK_F7:  ev->data.key.ch = CACA_KEY_F7; break;
                case SDLK_F8:  ev->data.key.ch = CACA_KEY_F8; break;
                case SDLK_F9:  ev->data.key.ch = CACA_KEY_F9; break;
                case SDLK_F10: ev->data.key.ch = CACA_KEY_F10; break;
                case SDLK_F11: ev->data.key.ch = CACA_KEY_F11; break;
                case SDLK_F12: ev->data.key.ch = CACA_KEY_F12; break;

                case SDLK_KP0:
                case SDLK_0:   ev->data.key.ch = '0'; break;
                case SDLK_KP1:
                case SDLK_1:   ev->data.key.ch = '1'; break;
                case SDLK_KP2:
                case SDLK_2:   ev->data.key.ch = '2'; break;
                case SDLK_KP3:
                case SDLK_3:   ev->data.key.ch = '3'; break;
                case SDLK_KP4:
                case SDLK_4:   ev->data.key.ch = '4'; break;
                case SDLK_KP5:
                case SDLK_5:   ev->data.key.ch = '5'; break;
                case SDLK_KP6:
                case SDLK_6:   ev->data.key.ch = '6'; break;
                case SDLK_KP7:
                case SDLK_7:   ev->data.key.ch = '7'; break;
                case SDLK_KP8:
                case SDLK_8:   ev->data.key.ch = '8'; break;
                case SDLK_KP9:
                case SDLK_9:   ev->data.key.ch = '9'; break;

                default:
                    ev->data.key.ch = event.key.keysym.unicode; break;
                    break;
            }

            ev->data.key.utf32 = (uint32_t) event.key.keysym.sym;
            bytes = caca_utf32_to_utf8(ev->data.key.utf8, ev->data.key.utf32);
            if(bytes) {
                ev->data.key.utf8[bytes] = '\0';
            }
            else {
                ev->data.key.utf8[0] = ev->data.key.ch;
                ev->data.key.utf8[1] = '\0';
            }

            return 1;
        }

        if(event.type == SDL_VIDEORESIZE) {
            ev->type = CACA_EVENT_RESIZE;
            ev->data.resize.w = event.resize.w / dp->drv.p->font_width;
            ev->data.resize.h = event.resize.h / dp->drv.p->font_height;

            dp->resize.w = ev->data.resize.w;
            dp->resize.h = ev->data.resize.h;
            dp->resize.resized = 1;

            return 1;
        }

        if(event.type == SDL_MOUSEMOTION) {
            ev->type = CACA_EVENT_MOUSE_MOTION;
            ev->data.mouse.x = event.motion.x / dp->drv.p->font_width;
            ev->data.mouse.y = event.motion.y / dp->drv.p->font_height;
            return 1;
        }

        if(event.type == SDL_MOUSEBUTTONDOWN
                || event.type == SDL_MOUSEBUTTONUP) {
            if(event.type == SDL_MOUSEBUTTONDOWN)
                ev->type = CACA_EVENT_MOUSE_PRESS;
            else if(event.type == SDL_MOUSEBUTTONUP)
                ev->type = CACA_EVENT_MOUSE_RELEASE;

            if(event.button.button == SDL_BUTTON_LEFT
                    || event.button.button == SDL_BUTTON_MIDDLE
                    || event.button.button == SDL_BUTTON_RIGHT) {
                ev->data.mouse.button = event.button.button;
                return 1;
            }
            else {
                break;
            }
        }
    }

    ev->type = CACA_EVENT_NONE;
    return 0;
}

static void sdl_handle_resize(caca_display_t *dp)
{
    dp->resize.allow = 1;
    caca_set_canvas_size(dp->cv, dp->resize.w, dp->resize.h);
    dp->resize.allow = 0;

    dp->drv.p->screen = SDL_SetVideoMode(sdl_get_display_width(dp),
                                         sdl_get_display_height(dp),
                                         video_bpp, video_flags);
}

static void sdl_set_mouse(caca_display_t *dp, int flags)
{
    if(!flags)
        SDL_ShowCursor(SDL_DISABLE);
    else
        SDL_ShowCursor(SDL_ENABLE);
}

/*
 * Driver initialisation
 */

int sdl_install(caca_display_t *dp)
{
#if defined HAVE_GETENV
    if(!getenv("DISPLAY") || !*(getenv("DISPLAY")))
        return -1;
#endif
    
    dp->drv.id = CACA_DRIVER_SDL;
    dp->drv.driver = "sdl";

    dp->drv.init_graphics = sdl_init_graphics;
    dp->drv.end_graphics = sdl_end_graphics;
    dp->drv.set_display_title = sdl_set_display_title;
    dp->drv.get_display_width = sdl_get_display_width;
    dp->drv.get_display_height = sdl_get_display_height;
    dp->drv.display = sdl_display;
    dp->drv.handle_resize = sdl_handle_resize;
    dp->drv.get_event = sdl_get_event;
    dp->drv.set_mouse = sdl_set_mouse;
    dp->drv.set_cursor = NULL;

    return 0;
}

#endif /* USE_SDL */

