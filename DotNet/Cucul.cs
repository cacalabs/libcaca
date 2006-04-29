/*
 *  CuculSharp      .NET bindings for libcucul
 *  Copyright (c) 2006 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                All Rights Reserved
 *
 *  $Id: $
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
    /*    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern Int32  cucul_putstr(IntPtr qq, int, int, String);*/
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





    /*
      char const *cucul_get_color_name(unsigned int);
      int cucul_putstr(IntPtr *, int, int, char const *);
      int cucul_printf(IntPtr *, int, int, char const *, ...);
    */



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






        /*
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 cucul_draw_polyline(IntPtr qq, Int32[] x, Int32[] y, Int32 n, IntPtr c);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 cucul_draw_thin_line(IntPtr qq, Int32 x1, Int32 y1, Int32 x2, Int32 y2);
    [DllImport("libCucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 cucul_draw_thin_polyline(IntPtr qq, Int32[] x, Int32[] y, Int32 n);
        */



        /* Privates methods, are not meant to be called by user*/
        
        public IntPtr get_cucul_t()
            {
            return qq;
            }
    }
  }

