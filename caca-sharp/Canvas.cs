/*
 *  libcaca       .NET bindings for libcaca
 *  Copyright (c) 2006 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                2007-2009 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

using System;
using System.Runtime.InteropServices;
using System.Security;
using System.Drawing;

namespace Caca
{
    public enum AnsiColor
    {
        BLACK = 0x00,
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
        TRANSPARENT = 0x20,
    }

    public enum AnsiStyle
    {
        BOLD = 0x01,
        ITALICS = 0x02,
        UNDERLINE = 0x04,
        BLINK = 0x08,
    }

    public class Canvas : IDisposable
    {
        public readonly IntPtr _c_cv;

        /* libcaca basic functions */

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern IntPtr caca_create_canvas(int w, int h);
        public Canvas()
        {
            _c_cv = caca_create_canvas(0, 0);
        }

        public Canvas(Size s)
        {
            _c_cv = caca_create_canvas(s.Width, s.Height);
        }

        public Canvas(int w, int h)
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
        private static extern int caca_wherex(IntPtr cv);
        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_wherey(IntPtr cv);
        public Point Cursor
        {
            get { return new Point(caca_wherex(_c_cv), caca_wherey(_c_cv)); }
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
        public int setColorAnsi(AnsiColor fg, AnsiColor bg)
        {
            return caca_set_color_ansi(_c_cv, (byte)fg, (byte)bg);
        }

        public int setColorAnsi(uint fg, AnsiColor bg)
        {
            return caca_set_color_ansi(_c_cv, (byte)fg, (byte)bg);
        }

        public int setColorAnsi(AnsiColor fg, uint bg)
        {
            return caca_set_color_ansi(_c_cv, (byte)fg, (byte)bg);
        }

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
        public int Blit(Point p, Canvas canvas)
        {
            return caca_blit(_c_cv, p.X, p.Y, canvas._c_cv, IntPtr.Zero);
        }

        public int Blit(Point p, Canvas cv, Canvas mask)
        {
            return caca_blit(_c_cv, p.X, p.Y, cv._c_cv, mask._c_cv);
        }

        public int Blit(int x, int y, Canvas canvas)
        {
            return caca_blit(_c_cv, x, y, canvas._c_cv, IntPtr.Zero);
        }

        public int Blit(int x, int y, Canvas cv, Canvas mask)
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
        public int ditherBitmap(Rectangle r, Dither d, object data)
        {
            GCHandle gch = GCHandle.Alloc(data, GCHandleType.Pinned);
            int ret = caca_dither_bitmap(_c_cv, r.X, r.Y, r.Width, r.Height,
                                          d._dither, gch.AddrOfPinnedObject());
            gch.Free();
            return ret;
        }

        public int ditherBitmap(int x, int y, int w, int h,
                                Dither d, object data)
        {
            GCHandle gch = GCHandle.Alloc(data, GCHandleType.Pinned);
            int ret = caca_dither_bitmap(_c_cv, x, y, w, h, d._dither,
                                          gch.AddrOfPinnedObject());
            gch.Free();
            return ret;
        }
    }
}

