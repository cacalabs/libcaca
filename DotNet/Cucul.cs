/*
 *  CuculSharp      .NET bindings for libcucul
 *  Copyright (c) 2006 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */



using System;
using System.Runtime.InteropServices;
using System.Security;


namespace libCucul 
  {
    
    public unsafe class Cucul : IDisposable
    {
    /* Fixme */
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]   
      public static extern IntPtr cucul_create_canvas(int w, int h);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32  cucul_get_canvas_width(IntPtr qq);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32  cucul_get_canvas_height(IntPtr qq);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32  cucul_set_canvas_size(IntPtr qq, int w, int h);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32  cucul_rand(int min, int max);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32  cucul_free_canvas(IntPtr qq);
     [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32  cucul_set_color(IntPtr qq, int fg, int bg);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32  cucul_set_truecolor(IntPtr qq, Int32 fg, Int32 bg);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32  cucul_putchar(IntPtr qq, int x, int y, char c);
        [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern Int32  cucul_putstr(IntPtr qq, int x , int y, String c);

    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32  cucul_clear_canvas(IntPtr qq);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32  cucul_blit(IntPtr qq, Int32 x, Int32 y, IntPtr qq1, IntPtr qq2);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 cucul_draw_line(IntPtr qq, Int32 x1, Int32 y1, Int32 x2, Int32 y2, string c);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 cucul_draw_polyline(IntPtr qq, Int32[] x, Int32[] y, Int32 n, IntPtr c);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 cucul_draw_thin_line(IntPtr qq, Int32 x1, Int32 y1, Int32 x2, Int32 y2);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 cucul_draw_thin_polyline(IntPtr qq, Int32[] x, Int32[] y, Int32 n);

    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 cucul_gotoxy(IntPtr qq, Int32 x, Int32 y);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 cucul_get_cursor_x(IntPtr qq);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 cucul_get_cursor_y(IntPtr qq);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 cucul_put_char(IntPtr qq, Int32 x, Int32 y, Int32 c);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 cucul_get_char(IntPtr qq, Int32 x, Int32 y);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 cucul_put_str(IntPtr qq, Int32 x, Int32 y, string c);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 cucul_get_attr(IntPtr qq, Int32 x, Int32 y);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 cucul_set_attr(IntPtr qq, Int32 a);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 cucul_put_attr(IntPtr qq, Int32 x, Int32 y, Int32 a);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 cucul_set_color_ansi(IntPtr qq, Int32 fg, Int32 bg);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 cucul_set_color_argb(IntPtr qq, Int32 fg, Int32 bg);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 cucul_set_canvas_handle(IntPtr qq, Int32 x, Int32 y);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 cucul_get_canvas_handle_x(IntPtr qq);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 cucul_get_canvas_handle_y(IntPtr qq);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 cucul_set_canvas_boundaries(IntPtr qq, Int32 x, Int32 y,
                                 Int32 h,  Int32 w);

    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 cucul_invert(IntPtr qq);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 cucul_flip(IntPtr qq);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 cucul_flop(IntPtr qq);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 cucul_rotate(IntPtr qq);

    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 cucul_attr_to_ansi(Int64 a);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 cucul_attr_to_ansi_fg(Int64 a);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 cucul_attr_to_ansi_bg(Int64 a);



    /* Constants */

    public const Int32 CUCUL_BLACK = 0x00; /**< The colour index for black. */
    public const Int32 CUCUL_BLUE = 0x01; /**< The colour index for blue. */
    public const Int32 CUCUL_GREEN = 0x02; /**< The colour index for green. */
    public const Int32 CUCUL_CYAN = 0x03; /**< The colour index for cyan. */
    public const Int32 CUCUL_RED = 0x04; /**< The colour index for red. */
    public const Int32 CUCUL_MAGENTA = 0x05; /**< The colour index for magenta. */
    public const Int32 CUCUL_BROWN = 0x06; /**< The colour index for brown. */
    public const Int32 CUCUL_LIGHTGRAY = 0x07; /**< The colour index for light gray. */
    public const Int32 CUCUL_DARKGRAY = 0x08; /**< The colour index for dark gray. */
    public const Int32 CUCUL_LIGHTBLUE = 0x09; /**< The colour index for blue. */
    public const Int32 CUCUL_LIGHTGREEN = 0x0a; /**< The colour index for light green. */
    public const Int32 CUCUL_LIGHTCYAN = 0x0b; /**< The colour index for light cyan. */
    public const Int32 CUCUL_LIGHTRED = 0x0c; /**< The colour index for light red. */
    public const Int32 CUCUL_LIGHTMAGENTA = 0x0d; /**< The colour index for light magenta. */
    public const Int32 CUCUL_YELLOW = 0x0e; /**< The colour index for yellow. */
    public const Int32 CUCUL_WHITE = 0x0f; /**< The colour index for white. */
    public const Int32 CUCUL_DEFAULT = 0x10; /**< The output driver's default colour. */
    public const Int32 CUCUL_TRANSPARENT = 0x20; /**< The transparent colour. */

    public const Int32 CUCUL_BOLD = 0x01; /**< The style mask for bold. */
    public const Int32 CUCUL_ITALICS = 0x02; /**< The style mask for italics. */
    public const Int32 CUCUL_UNDERLINE = 0x04; /**< The style mask for underline. */
    public const Int32 CUCUL_BLINK = 0x08; /**< The style mask for blink. */


        IntPtr qq;

        public Cucul()
            {
               qq = cucul_create_canvas(0, 0);
            }
        public void Dispose()
            {
               cucul_free_canvas(qq);
               GC.SuppressFinalize(this);
            }
        public Cucul(Int32 w, Int32 h)
            {
              qq = cucul_create_canvas(w, h);
            }
        public Int32 getWidth()
            {
            return cucul_get_canvas_width(qq);
            }
        public Int32 getHeight()
            {
            return cucul_get_canvas_height(qq);
            }
        public Int32 setSize(Int32 w, Int32 h)
            {
            return cucul_set_canvas_size(qq, w, h);
            }
        public static Int32 Rand(Int32 min, Int32 max)
            {
            return cucul_rand(min, max);
            }
        public Int32 setColor(int fg, int bg)
            {
            return cucul_set_color(qq, fg, bg);
            }
        public Int32  setTruecolor(Int32 fg, Int32 bg)
            {
            return cucul_set_truecolor(qq, fg, bg);
            }
        public Int32  putChar(int x, int y, char c)
            {
            return cucul_putchar(qq, x, y, c);
            }
        public Int32 clearCanvas()
            {
            return cucul_clear_canvas(qq);
            }
        public Int32 Blit(int x, int y, Cucul qq1, Cucul qq2)
            {
            return cucul_blit(qq, x, y, qq1.get_cucul_t(), qq2.get_cucul_t());
            }
        public Int32 drawLine(Int32 x1, Int32 y1, Int32 x2, Int32 y2, string c)
            {
            return cucul_draw_line(qq, x1, y1, x2, y2, c);
            }
        public Int32 putStr(Int32 x, Int32 y, string c)
            {
            return cucul_putstr(qq, x, y, c);
            }
        public Int32 gotoXY(Int32 x, Int32 y)
            {
            return cucul_gotoxy(qq, x, y);
            }
        public Int32 getCursorX()
            {
            return cucul_get_cursor_x(qq);
            }
        public Int32 getCursorY()
            {
            return cucul_get_cursor_y(qq);
            }
        public Int32 getChar(Int32 x, Int32 y)
            {
            return cucul_get_char(qq, x, y);
            }
        public Int32 getAttr(Int32 x, Int32 y)
            {
            return cucul_get_attr(qq, x, y);
            }
        public Int32 setAttr(Int32 a)
            {
            return cucul_set_attr(qq, a);
            }
        public Int32 setAttr(Int32 x, Int32 y, Int32 a)
            {
            return cucul_put_attr(qq, x, y, a);
            }
        public Int32 setColorANSI(Int32 fg, Int32 bg)
            {
            return cucul_set_color_ansi(qq, fg, bg);
            }
        public Int32 setColorARGB(Int32 fg, Int32 bg)
            {
            return cucul_set_color_ansi(qq, fg, bg);
            }
        public Int32 setCanvasHandle(Int32 x, Int32 y)
            {
            return cucul_set_canvas_handle(qq, x, y);
            }
        public Int32 getCanvasHandleX()
            {
            return cucul_get_canvas_handle_x(qq);
            }
        public Int32 getCanvasHandleY()
            {
            return cucul_get_canvas_handle_y(qq);
            }
        public Int32 setCanvasHandleY(Int32 x, Int32 y, Int32 h,  Int32 w)
            {
            return cucul_set_canvas_boundaries(qq, x, y, h, w); 
            }
          

        public Int32 Invert()
            {
            return cucul_invert(qq);
            }
        public Int32 Flip()
            {
            return cucul_flip(qq);
            }
        public Int32 Flop()
            {
            return cucul_flop(qq);
            }
        public Int32 Rotate()
            {
            return cucul_rotate(qq);
            }

        public Int32 AttrToANSI(Int64 a)
            {
            return cucul_attr_to_ansi(a);
            }
        public Int32 AttrToANSIFg(Int64 a)
            {
            return cucul_attr_to_ansi_fg(a);
            }
        public Int32 AttrToANSIBg(Int64 a)
            {
            return cucul_attr_to_ansi_bg(a);
            }










        /* Privates methods, are not meant to be called by user*/
        
        public IntPtr get_cucul_t()
            {
            return qq;
            }
    }
  }

