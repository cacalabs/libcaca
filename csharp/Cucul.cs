/*
 *  libcucul      .NET bindings for libcucul
 *  Copyright (c) 2006 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                2007 Sam Hocevar <sam@zoy.org>
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

using System;
using System.Runtime.InteropServices;
using System.Security;

namespace Cucul
{
    /* Static libcucul stuff that does not fit in any object */
    public static class Libcucul
    {
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_rand(int min, int max);

        public static int Rand(int min, int max)
        {
            return cucul_rand(min, max);
        }

        public const int BLACK = 0x00,
                         BLUE = 0x01,
                         GREEN = 0x02,
                         CYAN = 0x03,
                         RED = 0x04,
                         MAGENTA = 0x05,
                         BROWN = 0x06,
                         LIGHTGRAY = 0x07,
                         DARKGRAY = 0x08,
                         LIGHTBLUE = 0x09,
                         LIGHTGREEN = 0x0a,
                         LIGHTCYAN = 0x0b,
                         LIGHTRED = 0x0c,
                         LIGHTMAGENTA = 0x0d,
                         YELLOW = 0x0e,
                         WHITE = 0x0f,
                         DEFAULT = 0x10,
                         TRANSPARENT = 0x20;

        public const int BOLD = 0x01,
                         ITALICS = 0x02,
                         UNDERLINE = 0x04,
                         BLINK = 0x08;
    }

    public class CuculCanvas : IDisposable
    {
        public readonly IntPtr _cv;

        /* libcucul basic functions */

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern IntPtr cucul_create_canvas(int w, int h);
        public CuculCanvas()
        {
            _cv = cucul_create_canvas(0, 0);
        }

        public CuculCanvas(int w, int h)
        {
            _cv = cucul_create_canvas(w, h);
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_free_canvas(IntPtr cv);
        public void Dispose()
        {
           cucul_free_canvas(_cv);
           GC.SuppressFinalize(this);
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_set_canvas_size(IntPtr cv,
                                                        int w, int h);
        public void setSize(int w, int h)
        {
            cucul_set_canvas_size(_cv, w, h);
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_get_canvas_width(IntPtr cv);
        public int width
        {
            get { return cucul_get_canvas_width(_cv); }
            set { cucul_set_canvas_size(_cv, value,
                                        cucul_get_canvas_height(_cv)); }
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_get_canvas_height(IntPtr cv);
        public int height
        {
            get { return cucul_get_canvas_height(_cv); }
            set { cucul_set_canvas_size(_cv, cucul_get_canvas_width(_cv),
                                        value); }
        }

        /* canvas drawing */

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_gotoxy(IntPtr cv, int x, int y);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_get_cursor_x(IntPtr cv);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_get_cursor_y(IntPtr cv);
        public int cursorX
        {
            get { return cucul_get_cursor_x(_cv); }
            set { cucul_gotoxy(_cv, value, cucul_get_cursor_y(_cv)); }
        }

        public int cursorY
        {
            get { return cucul_get_cursor_y(_cv); }
            set { cucul_gotoxy(_cv, cucul_get_cursor_x(_cv), value); }
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_put_char(IntPtr cv,
                                                 int x, int y, int c);
        public int putChar(int x, int y, int c)
        {
            return cucul_put_char(_cv, x, y, c);
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_get_char(IntPtr cv, int x, int y);
        public int getChar(int x, int y)
        {
            return cucul_get_char(_cv, x, y);
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_put_str(IntPtr cv,
                                                int x, int y, string c);
        public int putStr(int x, int y, string c)
        {
            return cucul_put_str(_cv, x, y, c);
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_get_attr(IntPtr cv, int x, int y);
        public int getAttr(int x, int y)
        {
            return cucul_get_attr(_cv, x, y);
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_set_attr(IntPtr cv, int a);
        public int setAttr(int a)
        {
            return cucul_set_attr(_cv, a);
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_put_attr(IntPtr cv,
                                                 int x, int y, int a);
        public int putAttr(int x, int y, int a)
        {
            return cucul_put_attr(_cv, x, y, a);
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_set_color_ansi(IntPtr cv,
                                                       byte fg, byte bg);
        public int setColorAnsi(int fg, int bg)
        {
            return cucul_set_color_ansi(_cv, (byte)fg, (byte)bg);
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_set_color_argb(IntPtr cv,
                                                       int fg, int bg);
        public int setColorArgb(int fg, int bg)
        {
            return cucul_set_color_argb(_cv, fg, bg);
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_clear_canvas(IntPtr cv);
        public int Clear()
        {
            return cucul_clear_canvas(_cv);
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_set_canvas_handle(IntPtr cv,
                                                          int x, int y);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_get_canvas_handle_x(IntPtr cv);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_get_canvas_handle_y(IntPtr cv);
        public int handleX
        {
            get { return cucul_get_canvas_handle_x(_cv); }
            set { cucul_set_canvas_handle(_cv, value,
                                          cucul_get_canvas_handle_y(_cv)); }
        }

        public int handleY
        {
            get { return cucul_get_canvas_handle_y(_cv); }
            set { cucul_set_canvas_handle(_cv, cucul_get_canvas_handle_x(_cv),
                                          value); }
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_blit(IntPtr cv, int x, int y,
                                             IntPtr cv1, IntPtr cv2);
        public int Blit(int x, int y, CuculCanvas canvas)
        {
            return cucul_blit(_cv, x, y, canvas._cv, IntPtr.Zero);
        }

        public int Blit(int x, int y, CuculCanvas cv, CuculCanvas mask)
        {
            return cucul_blit(_cv, x, y, cv._cv, mask._cv);
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_set_canvas_boundaries(IntPtr cv,
                                                              int x, int y,
                                                              int h,  int w);
        public int setBoundaries(int x, int y, int h, int w)
        {
            return cucul_set_canvas_boundaries(_cv, x, y, h, w);
        }

        /* canvas transformation */

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_invert(IntPtr cv);
        public int Invert()
        {
            return cucul_invert(_cv);
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_flip(IntPtr cv);
        public int Flip()
        {
            return cucul_flip(_cv);
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_flop(IntPtr cv);
        public int Flop()
        {
            return cucul_flop(_cv);
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_rotate_180(IntPtr cv);
        public int Rotate180()
        {
            return cucul_rotate_180(_cv);
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_rotate_left(IntPtr cv);
        public int RotateLeft()
        {
            return cucul_rotate_left(_cv);
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_rotate_right(IntPtr cv);
        public int RotateRight()
        {
            return cucul_rotate_right(_cv);
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_stretch_left(IntPtr cv);
        public int StretchLeft()
        {
            return cucul_stretch_left(_cv);
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_stretch_right(IntPtr cv);
        public int StretchRight()
        {
            return cucul_stretch_right(_cv);
        }

        /* primitives drawing */
        /* FIXME: highly incomplete */

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_draw_line(IntPtr cv, int x1, int y1, int x2, int y2, int c);
        public int drawLine(int x1, int y1, int x2, int y2, int c)
        {
            return cucul_draw_line(_cv, x1, y1, x2, y2, c);
        }
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_draw_polyline(IntPtr cv, int[] x, int[] y, int n, IntPtr c);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_draw_thin_line(IntPtr cv, int x1, int y1, int x2, int y2);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_draw_thin_polyline(IntPtr cv, int[] x, int[] y, int n);

        /* frame handling */

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_get_frame_count(IntPtr cv);
        public int getFrameCount()
        {
            return cucul_get_frame_count(_cv);
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_set_frame(IntPtr cv, int f);
        public int setFrame(int f)
        {
            return cucul_set_frame(_cv, f);
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern string cucul_get_frame_name(IntPtr cv);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_set_frame_name(IntPtr cv, string n);
        public string frameName
        {
            get { return cucul_get_frame_name(_cv); }
            set { cucul_set_frame_name(_cv, value); }
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_create_frame(IntPtr cv, int f);
        public int createFrame(int f)
        {
            return cucul_create_frame(_cv, f);
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_free_frame(IntPtr cv, int f);
        public int freeFrame(int f)
        {
            return cucul_free_frame(_cv, f);
        }

        /* bitmap dithering */

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_dither_bitmap(IntPtr c, int  x, int y,
                                                      int w, int h,
                                                      IntPtr d, IntPtr data);
        public int ditherBitmap(int x, int y, int w, int h, CuculDither d,
                                object data)
        {
            GCHandle gch = GCHandle.Alloc(data, GCHandleType.Pinned);
            int ret = cucul_dither_bitmap(_cv, x, y, w, h, d._dither,
                                          gch.AddrOfPinnedObject());
            gch.Free();
            return ret;
        }
    }

    public class CuculAttr
    {
        private int _attr;

        public CuculAttr(int attr)
        {
            attr = _attr;
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern byte cucul_attr_to_ansi(Int32 a);
        public byte toAnsi()
        {
            return cucul_attr_to_ansi(_attr);
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern byte cucul_attr_to_ansi_fg(Int32 a);
        public byte toAnsiFg()
        {
            return cucul_attr_to_ansi_fg(_attr);
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern byte cucul_attr_to_ansi_bg(Int32 a);
        public byte toAnsiBg()
        {
            return cucul_attr_to_ansi_bg(_attr);
        }
    }

    public class CuculDither : IDisposable
    {
        public readonly IntPtr _dither;

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern IntPtr cucul_create_dither(int bpp, int w,
                                                         int h, int pitch,
                                                         ulong rmask,
                                                         ulong gmask,
                                                         ulong bmask,
                                                         ulong amask);
        public CuculDither(int bpp, int w,int h, int pitch,
                           uint rmask, uint gmask, uint bmask, uint amask)
        {
            _dither = cucul_create_dither(bpp, w, h, pitch,
                                          rmask, gmask, bmask, amask);
        }

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_free_dither(IntPtr d);
        public void Dispose()
        {
            cucul_free_dither(_dither);
            GC.SuppressFinalize(this);
        }

        /* TODO: fix this shit */

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_set_dither_palette(IntPtr d,
                               int[] r, int[] g,
                               int[] b, int[] a);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_set_dither_brightness(IntPtr d, float b);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_set_dither_gamma(IntPtr d, float g);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_set_dither_contrast(IntPtr d, float c);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_set_dither_invert(IntPtr d, int i);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_set_dither_antialias(IntPtr d, string s);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern string[] cucul_get_dither_antialias_list(IntPtr d);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_set_dither_color(IntPtr d, string s);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern string[] cucul_get_dither_color_list(IntPtr d);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_set_dither_charset(IntPtr d, string s);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern string[] cucul_get_dither_charset_list(IntPtr d);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int cucul_set_dither_mode(IntPtr d, string s);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern string[] cucul_get_dither_mode_list(IntPtr d);


        public int setBrightness(float b)
        {
            return cucul_set_dither_brightness(_dither, b);
        }

        public int setGamma(float g)
        {
            return cucul_set_dither_gamma(_dither, g);
        }

        public int setContrast(float c)
        {
            return cucul_set_dither_contrast(_dither, c);
        }

        public int setInvert(int i)
        {
            return cucul_set_dither_invert(_dither, i);
        }

        public int setAntialias(string s)
        {
            return cucul_set_dither_antialias(_dither, s);
        }

        public int setColor(string s)
        {
            return cucul_set_dither_color(_dither, s);
        }

        public int setCharset(string s)
        {
            return cucul_set_dither_charset(_dither, s);
        }

        public int setMode(string s)
        {
            return cucul_set_dither_mode(_dither, s);
        }

        /* <FIXME> */
        public string[] getAntialiasList()
        {
            return cucul_get_dither_antialias_list(_dither);
        }

        public string[] getColorList()
        {
            return cucul_get_dither_color_list(_dither);
        }

        public string[] getCharsetList()
        {
            return cucul_get_dither_charset_list(_dither);
        }

        public string[] getModeList()
        {
            return cucul_get_dither_mode_list(_dither);
        }

        /* </FIXME> */
    }
}

