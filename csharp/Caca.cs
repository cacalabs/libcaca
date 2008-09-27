/*
 *  libcaca       .NET bindings for libcaca
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
using System.Drawing;

namespace Caca
{
    /* Static libcaca stuff that does not fit in any object */
    public static class Libcaca
    {
        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_rand(int min, int max);
        public static int Rand(int min, int max)
        {
            return caca_rand(min, max);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern IntPtr caca_get_version();
        public static string getVersion()
        {
            return Marshal.PtrToStringAnsi(caca_get_version());
        }

        public const uint BLACK = 0x00,
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

        public const uint BOLD = 0x01,
                          ITALICS = 0x02,
                          UNDERLINE = 0x04,
                          BLINK = 0x08;
    }

    public enum CacaEventType
    {
        NONE =          0x0000,

        KEY_PRESS =     0x0001,
        KEY_RELEASE =   0x0002,
        MOUSE_PRESS =   0x0004,
        MOUSE_RELEASE = 0x0008,
        MOUSE_MOTION =  0x0010,
        RESIZE =        0x0020,
        QUIT =          0x0040,

        ANY =           0xffff,
    }

    public enum CacaEventKey
    {
        UNKNOWN = 0x00,

        CTRL_A =    0x01,
        CTRL_B =    0x02,
        CTRL_C =    0x03,
        CTRL_D =    0x04,
        CTRL_E =    0x05,
        CTRL_F =    0x06,
        CTRL_G =    0x07,
        BACKSPACE = 0x08,
        TAB =       0x09,
        CTRL_J =    0x0a,
        CTRL_K =    0x0b,
        CTRL_L =    0x0c,
        RETURN =    0x0d,
        CTRL_N =    0x0e,
        CTRL_O =    0x0f,
        CTRL_P =    0x10,
        CTRL_Q =    0x11,
        CTRL_R =    0x12,
        PAUSE =     0x13,
        CTRL_T =    0x14,
        CTRL_U =    0x15,
        CTRL_V =    0x16,
        CTRL_W =    0x17,
        CTRL_X =    0x18,
        CTRL_Y =    0x19,
        CTRL_Z =    0x1a,
        ESCAPE =    0x1b,
        DELETE =    0x7f,

        UP =    0x111,
        DOWN =  0x112,
        LEFT =  0x113,
        RIGHT = 0x114,

        INSERT =   0x115,
        HOME =     0x116,
        END =      0x117,
        PAGEUP =   0x118,
        PAGEDOWN = 0x119,

        F1 =  0x11a,
        F2 =  0x11b,
        F3 =  0x11c,
        F4 =  0x11d,
        F5 =  0x11e,
        F6 =  0x11f,
        F7 =  0x120,
        F8 =  0x121,
        F9 =  0x122,
        F10 = 0x123,
        F11 = 0x124,
        F12 = 0x125,
        F13 = 0x126,
        F14 = 0x127,
        F15 = 0x128,
    }

    public class CacaCanvas : IDisposable
    {
        public readonly IntPtr _c_cv;

        /* libcaca basic functions */

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern IntPtr caca_create_canvas(int w, int h);
        public CacaCanvas()
        {
            _c_cv = caca_create_canvas(0, 0);
        }

        public CacaCanvas(Size s)
        {
            _c_cv = caca_create_canvas(s.Width, s.Height);
        }

        public CacaCanvas(int w, int h)
        {
            _c_cv = caca_create_canvas(h, w);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_free_canvas(IntPtr cv);
        public void Dispose()
        {
           /* FIXME: don't destroy ourselves if we're attached */
           caca_free_canvas(_c_cv);
           GC.SuppressFinalize(this);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_set_canvas_size(IntPtr cv,
                                                        int w, int h);
        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_canvas_width(IntPtr cv);
        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_canvas_height(IntPtr cv);
        public Size Size
        {
            get { return new Size(caca_get_canvas_width(_c_cv),
                                  caca_get_canvas_height(_c_cv)); }
            set { caca_set_canvas_size(_c_cv, value.Width, value.Height); }
        }

        public Rectangle Rectangle
        {
            get { return new Rectangle(0, 0, caca_get_canvas_width(_c_cv),
                                             caca_get_canvas_height(_c_cv)); }
            set { caca_set_canvas_size(_c_cv, value.Width, value.Height); }
        }

        /* canvas drawing */

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_gotoxy(IntPtr cv, int x, int y);
        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_cursor_x(IntPtr cv);
        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_cursor_y(IntPtr cv);
        public Point Cursor
        {
            get { return new Point(caca_get_cursor_x(_c_cv),
                                   caca_get_cursor_y(_c_cv)); }
            set { caca_gotoxy(_c_cv, value.X, value.Y); }
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_put_char(IntPtr cv,
                                                 int x, int y, uint c);
        public int putChar(Point p, uint c)
        {
            return caca_put_char(_c_cv, p.X, p.Y, c);
        }

        public int putChar(int x, int y, uint c)
        {
            return caca_put_char(_c_cv, x, y, c);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern uint caca_get_char(IntPtr cv, int x, int y);
        public uint getChar(Point p)
        {
            return caca_get_char(_c_cv, p.X, p.Y);
        }

        public uint getChar(int x, int y)
        {
            return caca_get_char(_c_cv, x, y);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_put_str(IntPtr cv,
                                                int x, int y, string c);
        public int putStr(Point p, string c)
        {
            return caca_put_str(_c_cv, p.X, p.Y, c);
        }

        public int putStr(int x, int y, string c)
        {
            return caca_put_str(_c_cv, x, y, c);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_attr(IntPtr cv, int x, int y);
        public int getAttr(Point p)
        {
            return caca_get_attr(_c_cv, p.X, p.Y);
        }

        public int getAttr(int x, int y)
        {
            return caca_get_attr(_c_cv, x, y);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_set_attr(IntPtr cv, uint a);
        public int setAttr(uint a)
        {
            return caca_set_attr(_c_cv, a);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_put_attr(IntPtr cv,
                                                 int x, int y, uint a);
        public int putAttr(Point p, uint a)
        {
            return caca_put_attr(_c_cv, p.X, p.Y, a);
        }

        public int putAttr(int x, int y, uint a)
        {
            return caca_put_attr(_c_cv, x, y, a);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_set_color_ansi(IntPtr cv,
                                                       byte fg, byte bg);
        public int setColorAnsi(uint fg, uint bg)
        {
            return caca_set_color_ansi(_c_cv, (byte)fg, (byte)bg);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_set_color_argb(IntPtr cv,
                                                       uint fg, uint bg);
        public int setColorArgb(uint fg, uint bg)
        {
            return caca_set_color_argb(_c_cv, fg, bg);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_clear_canvas(IntPtr cv);
        public int Clear()
        {
            return caca_clear_canvas(_c_cv);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_set_canvas_handle(IntPtr cv,
                                                          int x, int y);
        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_canvas_handle_x(IntPtr cv);
        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_canvas_handle_y(IntPtr cv);
        public Point Handle
        {
            get { return new Point(caca_get_canvas_handle_x(_c_cv),
                                   caca_get_canvas_handle_y(_c_cv)); }
            set { caca_set_canvas_handle(_c_cv, value.X, value.Y); }
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_blit(IntPtr cv, int x, int y,
                                             IntPtr cv1, IntPtr cv2);
        public int Blit(Point p, CacaCanvas canvas)
        {
            return caca_blit(_c_cv, p.X, p.Y, canvas._c_cv, IntPtr.Zero);
        }

        public int Blit(Point p, CacaCanvas cv, CacaCanvas mask)
        {
            return caca_blit(_c_cv, p.X, p.Y, cv._c_cv, mask._c_cv);
        }

        public int Blit(int x, int y, CacaCanvas canvas)
        {
            return caca_blit(_c_cv, x, y, canvas._c_cv, IntPtr.Zero);
        }

        public int Blit(int x, int y, CacaCanvas cv, CacaCanvas mask)
        {
            return caca_blit(_c_cv, x, y, cv._c_cv, mask._c_cv);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_set_canvas_boundaries(IntPtr cv,
                                                              int x, int y,
                                                              int h, int w);
        public int setBoundaries(Rectangle r)
        {
            return caca_set_canvas_boundaries(_c_cv, r.X, r.Y,
                                               r.Width, r.Height);
        }

        public int setBoundaries(int x, int y, int w, int h)
        {
            return caca_set_canvas_boundaries(_c_cv, x, y, w, h);
        }

        /* canvas transformation */

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_invert(IntPtr cv);
        public int Invert()
        {
            return caca_invert(_c_cv);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_flip(IntPtr cv);
        public int Flip()
        {
            return caca_flip(_c_cv);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_flop(IntPtr cv);
        public int Flop()
        {
            return caca_flop(_c_cv);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_rotate_180(IntPtr cv);
        public int Rotate180()
        {
            return caca_rotate_180(_c_cv);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_rotate_left(IntPtr cv);
        public int RotateLeft()
        {
            return caca_rotate_left(_c_cv);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_rotate_right(IntPtr cv);
        public int RotateRight()
        {
            return caca_rotate_right(_c_cv);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_stretch_left(IntPtr cv);
        public int StretchLeft()
        {
            return caca_stretch_left(_c_cv);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_stretch_right(IntPtr cv);
        public int StretchRight()
        {
            return caca_stretch_right(_c_cv);
        }

        /* primitives drawing */

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_draw_line(IntPtr cv, int x1, int y1,
                                                  int x2, int y2, uint c);
        public int drawLine(Point p1, Point p2, uint c)
        {
            return caca_draw_line(_c_cv, p1.X, p1.Y, p2.X, p2.Y, c);
        }

        public int drawLine(int x1, int y1, int x2, int y2, uint c)
        {
            return caca_draw_line(_c_cv, x1, y1, x2, y2, c);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_draw_polyline(IntPtr cv, int[] x,
                                                      int[] y, int n, uint c);
        public int drawPolyline(Point[] lp, uint c)
        {
            int[] lx = new int[lp.Length];
            int[] ly = new int[lp.Length];
            for(int i = 0; i < lp.Length; i++)
            {
                lx[i] = lp[i].X;
                ly[i] = lp[i].Y;
            }
            return caca_draw_polyline(_c_cv, lx, ly, lp.Length - 1, c);
        }

        public int drawPolyline(int[] lx, int[] ly, uint c)
        {
            if(lx.Length != ly.Length)
                return -1;

            return caca_draw_polyline(_c_cv, lx, ly, lx.Length - 1, c);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_draw_thin_line(IntPtr cv, int x1,
                                                       int y1, int x2, int y2);
        public int drawThinLine(Point p1, Point p2)
        {
            return caca_draw_thin_line(_c_cv, p1.X, p1.Y, p2.X, p2.Y);
        }

        public int drawThinLine(int x1, int y1, int x2, int y2)
        {
            return caca_draw_thin_line(_c_cv, x1, y1, x2, y2);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_draw_thin_polyline(IntPtr cv, int[] x,
                                                           int[] y, int n);
        public int drawThinPolyline(Point[] lp)
        {
            int[] lx = new int[lp.Length];
            int[] ly = new int[lp.Length];
            for(int i = 0; i < lp.Length; i++)
            {
                lx[i] = lp[i].X;
                ly[i] = lp[i].Y;
            }
            return caca_draw_thin_polyline(_c_cv, lx, ly, lp.Length - 1);
        }

        public int drawThinPolyline(int[] lx, int[] ly)
        {
            if(lx.Length != ly.Length)
                return -1;

            return caca_draw_thin_polyline(_c_cv, lx, ly, lx.Length - 1);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_draw_circle(IntPtr cv, int x, int y,
                                                    int r, uint c);
        public int drawCircle(Point p, int r, uint c)
        {
            return caca_draw_circle(_c_cv, p.X, p.Y, r, c);
        }

        public int drawCircle(int x, int y, int r, uint c)
        {
            return caca_draw_circle(_c_cv, x, y, r, c);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_draw_ellipse(IntPtr cv, int x, int y,
                                                     int a, int b, uint c);
        public int drawEllipse(Point p, int a, int b, uint c)
        {
            return caca_draw_ellipse(_c_cv, p.X, p.Y, a, b, c);
        }

        public int drawEllipse(int x, int y, int a, int b, uint c)
        {
            return caca_draw_ellipse(_c_cv, x, y, a, b, c);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_draw_thin_ellipse(IntPtr cv,
                                                          int x, int y,
                                                          int a, int b);
        public int drawThinEllipse(Point p, int a, int b)
        {
            return caca_draw_thin_ellipse(_c_cv, p.X, p.Y, a, b);
        }

        public int drawThinEllipse(int x, int y, int a, int b)
        {
            return caca_draw_thin_ellipse(_c_cv, x, y, a, b);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_fill_ellipse(IntPtr cv, int x, int y,
                                                     int a, int b, uint c);
        public int fillEllipse(Point p, int a, int b, uint c)
        {
            return caca_fill_ellipse(_c_cv, p.X, p.Y, a, b, c);
        }

        public int fillEllipse(int x, int y, int a, int b, uint c)
        {
            return caca_fill_ellipse(_c_cv, x, y, a, b, c);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_draw_box(IntPtr cv, int x, int y,
                                                 int w, int h, uint c);
        public int drawBox(Rectangle r, uint c)
        {
            return caca_draw_box(_c_cv, r.X, r.Y, r.Width, r.Height, c);
        }

        public int drawBox(int x, int y, int w, int h, uint c)
        {
            return caca_draw_box(_c_cv, x, y, w, h, c);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_draw_thin_box(IntPtr cv, int x, int y,
                                                      int w, int h);
        public int drawThinBox(Rectangle r)
        {
            return caca_draw_thin_box(_c_cv, r.X, r.Y, r.Width, r.Height);
        }

        public int drawThinBox(int x, int y, int w, int h)
        {
            return caca_draw_thin_box(_c_cv, x, y, w, h);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_draw_cp437_box(IntPtr cv, int x, int y,
                                                       int w, int h);
        public int drawCp437Box(Rectangle r)
        {
            return caca_draw_cp437_box(_c_cv, r.X, r.Y, r.Width, r.Height);
        }

        public int drawCp437Box(int x, int y, int w, int h)
        {
            return caca_draw_cp437_box(_c_cv, x, y, w, h);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_fill_box(IntPtr cv, int x, int y,
                                                 int w, int h, uint c);
        public int fillBox(Rectangle r, uint c)
        {
            return caca_fill_box(_c_cv, r.X, r.Y, r.Width, r.Height, c);
        }

        public int fillBox(int x, int y, int w, int h, uint c)
        {
            return caca_fill_box(_c_cv, x, y, w, h, c);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_draw_triangle(IntPtr cv, int x1,
                                                      int y1, int x2, int y2,
                                                      int x3, int y3, uint c);
        public int drawTriangle(Point p1, Point p2, Point p3, uint c)
        {
            return caca_draw_triangle(_c_cv, p1.X, p1.Y, p2.X, p2.Y,
                                       p3.X, p3.Y, c);
        }

        public int drawTriangle(int x1, int y1, int x2, int y2,
                                int x3, int y3, uint c)
        {
            return caca_draw_triangle(_c_cv, x1, y1, x2, y2, x3, y3, c);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_draw_thin_triangle(IntPtr cv,
                                                           int x1, int y1,
                                                           int x2, int y2,
                                                           int x3, int y3);
        public int drawThinTriangle(Point p1, Point p2, Point p3)
        {
            return caca_draw_thin_triangle(_c_cv, p1.X, p1.Y, p2.X, p2.Y,
                                            p3.X, p3.Y);
        }

        public int drawThinTriangle(int x1, int y1, int x2, int y2,
                                    int x3, int y3)
        {
            return caca_draw_thin_triangle(_c_cv, x1, y1, x2, y2, x3, y3);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_fill_triangle(IntPtr cv, int x1,
                                                      int y1, int x2, int y2,
                                                      int x3, int y3, uint c);
        public int fillTriangle(Point p1, Point p2, Point p3, uint c)
        {
            return caca_fill_triangle(_c_cv, p1.X, p1.Y, p2.X, p2.Y,
                                       p3.X, p3.Y, c);
        }

        public int fillTriangle(int x1, int y1, int x2, int y2,
                                int x3, int y3, uint c)
        {
            return caca_fill_triangle(_c_cv, x1, y1, x2, y2, x3, y3, c);
        }

        /* frame handling */

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_frame_count(IntPtr cv);
        public int getFrameCount()
        {
            return caca_get_frame_count(_c_cv);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_set_frame(IntPtr cv, int f);
        public int setFrame(int f)
        {
            return caca_set_frame(_c_cv, f);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern string caca_get_frame_name(IntPtr cv);
        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_set_frame_name(IntPtr cv, string n);
        public string FrameName
        {
            get { return caca_get_frame_name(_c_cv); }
            set { caca_set_frame_name(_c_cv, value); }
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_create_frame(IntPtr cv, int f);
        public int createFrame(int f)
        {
            return caca_create_frame(_c_cv, f);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_free_frame(IntPtr cv, int f);
        public int freeFrame(int f)
        {
            return caca_free_frame(_c_cv, f);
        }

        /* bitmap dithering */

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_dither_bitmap(IntPtr c, int  x, int y,
                                                      int w, int h,
                                                      IntPtr d, IntPtr data);
        public int ditherBitmap(Rectangle r, CacaDither d, object data)
        {
            GCHandle gch = GCHandle.Alloc(data, GCHandleType.Pinned);
            int ret = caca_dither_bitmap(_c_cv, r.X, r.Y, r.Width, r.Height,
                                          d._dither, gch.AddrOfPinnedObject());
            gch.Free();
            return ret;
        }

        public int ditherBitmap(int x, int y, int w, int h,
                                CacaDither d, object data)
        {
            GCHandle gch = GCHandle.Alloc(data, GCHandleType.Pinned);
            int ret = caca_dither_bitmap(_c_cv, x, y, w, h, d._dither,
                                          gch.AddrOfPinnedObject());
            gch.Free();
            return ret;
        }
    }

    public class CacaAttr
    {
        private uint _attr;

        public CacaAttr(uint attr)
        {
            _attr = attr;
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern byte caca_attr_to_ansi(uint a);
        public byte toAnsi()
        {
            return caca_attr_to_ansi(_attr);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern byte caca_attr_to_ansi_fg(uint a);
        public byte toAnsiFg()
        {
            return caca_attr_to_ansi_fg(_attr);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern byte caca_attr_to_ansi_bg(uint a);
        public byte toAnsiBg()
        {
            return caca_attr_to_ansi_bg(_attr);
        }
    }

    public class CacaDither : IDisposable
    {
        public readonly IntPtr _dither;

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern IntPtr caca_create_dither(int bpp, int w,
                                                         int h, int pitch,
                                                         uint rmask,
                                                         uint gmask,
                                                         uint bmask,
                                                         uint amask);
        public CacaDither(int bpp, Size s, int pitch,
                           uint rmask, uint gmask, uint bmask, uint amask)
        {
            _dither = caca_create_dither(bpp, s.Width, s.Height, pitch,
                                          rmask, gmask, bmask, amask);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_free_dither(IntPtr d);
        public void Dispose()
        {
            caca_free_dither(_dither);
            GC.SuppressFinalize(this);
        }

        /* TODO: fix this shit */

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_set_dither_palette(IntPtr d,
                               uint[] r, uint[] g,
                               uint[] b, uint[] a);
        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_set_dither_brightness(IntPtr d, float b);
        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_set_dither_gamma(IntPtr d, float g);
        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_set_dither_contrast(IntPtr d, float c);
        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_set_dither_invert(IntPtr d, int i);
        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_set_dither_antialias(IntPtr d, string s);
        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern string[] caca_get_dither_antialias_list(IntPtr d);
        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_set_dither_color(IntPtr d, string s);
        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern string[] caca_get_dither_color_list(IntPtr d);
        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_set_dither_charset(IntPtr d, string s);
        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern string[] caca_get_dither_charset_list(IntPtr d);
        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_set_dither_mode(IntPtr d, string s);
        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern string[] caca_get_dither_mode_list(IntPtr d);


        public int setBrightness(float b)
        {
            return caca_set_dither_brightness(_dither, b);
        }

        public int setGamma(float g)
        {
            return caca_set_dither_gamma(_dither, g);
        }

        public int setContrast(float c)
        {
            return caca_set_dither_contrast(_dither, c);
        }

        public int setInvert(int i)
        {
            return caca_set_dither_invert(_dither, i);
        }

        public int setAntialias(string s)
        {
            return caca_set_dither_antialias(_dither, s);
        }

        public int setColor(string s)
        {
            return caca_set_dither_color(_dither, s);
        }

        public int setCharset(string s)
        {
            return caca_set_dither_charset(_dither, s);
        }

        public int setMode(string s)
        {
            return caca_set_dither_mode(_dither, s);
        }

        /* <FIXME> */
        public string[] getAntialiasList()
        {
            return caca_get_dither_antialias_list(_dither);
        }

        public string[] getColorList()
        {
            return caca_get_dither_color_list(_dither);
        }

        public string[] getCharsetList()
        {
            return caca_get_dither_charset_list(_dither);
        }

        public string[] getModeList()
        {
            return caca_get_dither_mode_list(_dither);
        }

        /* </FIXME> */
    }

    public class CacaFont : IDisposable
    {
        private IntPtr _font;
        private GCHandle _gch;

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern IntPtr caca_load_font(IntPtr data, uint len);
        public CacaFont(string s)
        {
            IntPtr name = Marshal.StringToHGlobalAnsi(s);
            _font = caca_load_font(name, 0);
            Marshal.FreeHGlobal(name);
        }

        public CacaFont(byte[] buf)
        {
            GCHandle _gch = GCHandle.Alloc(buf, GCHandleType.Pinned);
            _font = caca_load_font(_gch.AddrOfPinnedObject(),
                                    (uint)buf.Length);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_free_font(IntPtr d);
        public void Dispose()
        {
            caca_free_font(_font);
            _gch.Free();
            GC.SuppressFinalize(this);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern IntPtr caca_get_font_list();
        public static string[] getList()
        {
            IntPtr l = caca_get_font_list();

            int size;
            for(size = 0; true; size++)
                if(Marshal.ReadIntPtr(l, IntPtr.Size * size) == IntPtr.Zero)
                    break;

            string[] ret = new string[size];
            for(int i = 0; i < size; i++)
            {
                IntPtr s = Marshal.ReadIntPtr(l, IntPtr.Size * i);
                ret[i] = Marshal.PtrToStringAnsi(s);
            }

            return ret;
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_font_width(IntPtr font);
        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_font_height(IntPtr font);
        public Size Size
        {
            get { return new Size(caca_get_font_width(_font),
                                  caca_get_font_height(_font)); }
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern IntPtr caca_get_font_blocks(IntPtr font);
        public int[,] getBlocks()
        {
            IntPtr l = caca_get_font_blocks(_font);

            int size;
            for(size = 1; true; size += 2)
                if(Marshal.ReadIntPtr(l, IntPtr.Size * size) == IntPtr.Zero)
                    break;

            int[,] ret = new int[size,2];
            for(int i = 0; i < size; i++)
            {
                ret[i,0] = (int)Marshal.ReadIntPtr(l, IntPtr.Size * i * 2);
                ret[i,1] = (int)Marshal.ReadIntPtr(l, IntPtr.Size * i * 2 + 1);
            }

            return ret;
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_render_canvas(IntPtr cv, IntPtr f,
                                                      IntPtr buf, int w, int h,
                                                      int pitch);
        public int Render(CacaCanvas cv, uint[,] buf, int pitch)
        {
            GCHandle gch = GCHandle.Alloc(buf, GCHandleType.Pinned);
            int ret = caca_render_canvas(cv._c_cv, _font,
                                          gch.AddrOfPinnedObject(),
                                          buf.GetLength(0), buf.GetLength(1),
                                          pitch);
            gch.Free();
            return ret;
        }
    }

    public class CacaEvent : IDisposable
    {
        public IntPtr cevent;
        private IntPtr _utf8;

        public CacaEvent()
        {
            cevent = Marshal.AllocHGlobal(32);
            _utf8 = Marshal.AllocHGlobal(8);
        }

        public void Dispose()
        {
            Marshal.FreeHGlobal(cevent);
            Marshal.FreeHGlobal(_utf8);
            GC.SuppressFinalize(this);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_event_type(IntPtr ev);
        public CacaEventType Type
        {
            get { return (CacaEventType)caca_get_event_type(cevent); }
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_event_key_ch(IntPtr ev);
        public int KeyCh
        {
            get { return caca_get_event_key_ch(cevent); }
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern uint caca_get_event_key_utf32(IntPtr ev);
        public uint KeyUtf32
        {
            get { return caca_get_event_key_utf32(cevent); }
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_event_key_utf8(IntPtr ev,
                                                          IntPtr _utf8);
        public string KeyUtf8
        {
            get
            {
                caca_get_event_key_utf8(cevent, _utf8);
                return Marshal.PtrToStringAnsi(_utf8);
            }
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_event_mouse_button(IntPtr ev);
        public int MouseButton
        {
            get { return caca_get_event_mouse_button(cevent); }
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_event_mouse_x(IntPtr ev);
        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_event_mouse_y(IntPtr ev);
        public Point MousePos
        {
            get { return new Point(caca_get_event_mouse_x(cevent),
                                   caca_get_event_mouse_y(cevent)); }
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_event_resize_width(IntPtr ev);
        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_event_resize_height(IntPtr ev);
        public Size ResizeSize
        {
            get { return new Size(caca_get_event_resize_width(cevent),
                                  caca_get_event_resize_height(cevent)); }
        }
    }

    public class CacaDisplay : IDisposable
    {
        private CacaCanvas _cv;
        public CacaCanvas Canvas { get { return _cv; } }

        private IntPtr _c_cv;
        private IntPtr _c_dp;

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern IntPtr caca_create_display(IntPtr cv);
        public CacaDisplay(CacaCanvas cv)
        {
            _cv = cv;
            _c_cv = _cv._c_cv;
            _c_dp = caca_create_display(_c_cv);
        }

        public CacaDisplay()
        {
            /* XXX: we do not call caca_create_display() with a NULL
             * argument because it's then impossible to create a CacaCanvas
             * and I don't want to add a weird constructor */
            _cv = new CacaCanvas();
            _c_cv = _cv._c_cv;
            _c_dp = caca_create_display(_c_cv);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_free_display(IntPtr dp);
        public void Dispose()
        {
            caca_free_display(_c_dp);
            GC.SuppressFinalize(this);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_refresh_display(IntPtr dp);
        public void Refresh()
        {
            caca_refresh_display(_c_dp);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_set_display_time(IntPtr dp, int d);
        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_display_time(IntPtr dp);
        public int DisplayTime
        {
            get { return caca_get_display_time(_c_dp); }
            set { caca_set_display_time(_c_dp, value); }
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_event(IntPtr dp, uint t,
                                                 IntPtr cevent,
                                                 int timeout);
        public CacaEvent getEvent(CacaEventType t, int timeout)
        {
            CacaEvent e = new CacaEvent();
            caca_get_event(_c_dp, (uint)t, e.cevent, timeout);
            return e;
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_display_width(IntPtr dp);
        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_display_height(IntPtr dp);
        public Size Size
        {
            get { return new Size(caca_get_display_width(_c_dp),
                                  caca_get_display_height(_c_dp)); }
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_set_display_title(IntPtr dp, string t);
        public string Title
        {
            set { caca_set_display_title(_c_dp, value); }
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_set_mouse(IntPtr k, bool status);
        public bool Mouse
        {
            set { caca_set_mouse(_c_dp, value); }
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_mouse_x(IntPtr k);
        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_mouse_y(IntPtr k);
        public Point MousePos
        {
            get { return new Point(caca_get_mouse_x(_c_dp),
                                   caca_get_mouse_y(_c_dp)); }
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_set_cursor(IntPtr k, bool status);
        public bool Cursor
        {
            set { caca_set_cursor(_c_dp, value); }
        }
    }
}

