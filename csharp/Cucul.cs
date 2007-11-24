/*
 *  libcucul   .NET bindings for libcucul
 *  Copyright (c) 2006 Jean-Yves Lamoureux <jylam@lnxscene.org>
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
    public static class Libcucul
    {
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
        public static extern int cucul_rand(int min, int max);

        public static int Rand(int min, int max)
        {
            return cucul_rand(min, max);
        }

        /* Constants */
        public const int BLACK = 0x00;
        public const int BLUE = 0x01;
        public const int GREEN = 0x02;
        public const int CYAN = 0x03;
        public const int RED = 0x04;
        public const int MAGENTA = 0x05;
        public const int BROWN = 0x06;
        public const int LIGHTGRAY = 0x07;
        public const int DARKGRAY = 0x08;
        public const int LIGHTBLUE = 0x09;
        public const int LIGHTGREEN = 0x0a;
        public const int LIGHTCYAN = 0x0b;
        public const int LIGHTRED = 0x0c;
        public const int LIGHTMAGENTA = 0x0d;
        public const int YELLOW = 0x0e;
        public const int WHITE = 0x0f;
        public const int DEFAULT = 0x10;
        public const int TRANSPARENT = 0x20;

        public const int BOLD = 0x01;
        public const int ITALICS = 0x02;
        public const int UNDERLINE = 0x04;
        public const int BLINK = 0x08;
    }

    public unsafe class CuculCanvas : IDisposable
    {
        /* Fixme */
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]   
          public static extern IntPtr cucul_create_canvas(int w, int h);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int  cucul_get_canvas_width(IntPtr cv);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int  cucul_get_canvas_height(IntPtr cv);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int  cucul_set_canvas_size(IntPtr cv, int w, int h);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int  cucul_free_canvas(IntPtr cv);
         [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int  cucul_set_color(IntPtr cv, int fg, int bg);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int  cucul_set_truecolor(IntPtr cv, int fg, int bg);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int  cucul_putchar(IntPtr cv, int x, int y, char c);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int  cucul_putstr(IntPtr cv, int x , int y, String c);

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int  cucul_clear_canvas(IntPtr cv);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int  cucul_blit(IntPtr cv, int x, int y, IntPtr cv1, IntPtr cv2);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int cucul_draw_line(IntPtr cv, int x1, int y1, int x2, int y2, int c);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int cucul_draw_polyline(IntPtr cv, int[] x, int[] y, int n, IntPtr c);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int cucul_draw_thin_line(IntPtr cv, int x1, int y1, int x2, int y2);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int cucul_draw_thin_polyline(IntPtr cv, int[] x, int[] y, int n);
    
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int cucul_gotoxy(IntPtr cv, int x, int y);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int cucul_get_cursor_x(IntPtr cv);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int cucul_get_cursor_y(IntPtr cv);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int cucul_put_char(IntPtr cv, int x, int y, int c);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int cucul_get_char(IntPtr cv, int x, int y);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int cucul_put_str(IntPtr cv, int x, int y, string c);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int cucul_get_attr(IntPtr cv, int x, int y);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int cucul_set_attr(IntPtr cv, int a);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int cucul_put_attr(IntPtr cv, int x, int y, int a);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int cucul_set_color_ansi(IntPtr cv, int fg, int bg);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int cucul_set_color_argb(IntPtr cv, int fg, int bg);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int cucul_set_canvas_handle(IntPtr cv, int x, int y);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int cucul_get_canvas_handle_x(IntPtr cv);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int cucul_get_canvas_handle_y(IntPtr cv);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int cucul_set_canvas_boundaries(IntPtr cv, int x, int y,
                                     int h,  int w);

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int cucul_invert(IntPtr cv);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int cucul_flip(IntPtr cv);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int cucul_flop(IntPtr cv);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int cucul_rotate(IntPtr cv);

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int cucul_attr_to_ansi(Int64 a);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int cucul_attr_to_ansi_fg(Int64 a);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
          public static extern int cucul_attr_to_ansi_bg(Int64 a);

        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
           public static extern int cucul_get_frame_count(IntPtr cv);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
           public static extern int cucul_set_frame(IntPtr cv, int f);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
           public static extern string cucul_get_frame_name(IntPtr cv);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
           public static extern int cucul_set_frame_name(IntPtr cv, string n);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
           public static extern int cucul_create_frame(IntPtr cv, int f);
        [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
           public static extern int cucul_free_frame(IntPtr cv, int f);

        IntPtr cv;

        public CuculCanvas()
        {
           cv = cucul_create_canvas(0, 0);
        }

        public void Dispose()
        {
           cucul_free_canvas(cv);
           GC.SuppressFinalize(this);
        }

        public CuculCanvas(int w, int h)
        {
            cv = cucul_create_canvas(w, h);
        }

        public int getWidth()
        {
            return cucul_get_canvas_width(cv);
        }

        public int getHeight()
        {
            return cucul_get_canvas_height(cv);
        }

        public int setSize(int w, int h)
        {
            return cucul_set_canvas_size(cv, w, h);
        }

        public int setColor(int fg, int bg)
        {
            return cucul_set_color(cv, fg, bg);
        }

        public int  setTruecolor(int fg, int bg)
        {
            return cucul_set_truecolor(cv, fg, bg);
        }

        public int  putChar(int x, int y, char c)
        {
            return cucul_putchar(cv, x, y, c);
        }

        public int Clear()
        {
            return cucul_clear_canvas(cv);
        }

        public int Blit(int x, int y, CuculCanvas cv1, CuculCanvas cv2)
        {
            return cucul_blit(cv, x, y, cv1.get_cucul_t(), cv2.get_cucul_t());
        }

        public int drawLine(int x1, int y1, int x2, int y2, int c)
        {
            return cucul_draw_line(cv, x1, y1, x2, y2, c);
        }

        public int putStr(int x, int y, string c)
        {
            return cucul_putstr(cv, x, y, c);
        }

        public int gotoXY(int x, int y)
        {
            return cucul_gotoxy(cv, x, y);
        }

        public int getCursorX()
        {
            return cucul_get_cursor_x(cv);
        }

        public int getCursorY()
        {
            return cucul_get_cursor_y(cv);
        }

        public int getChar(int x, int y)
        {
            return cucul_get_char(cv, x, y);
        }

        public int getAttr(int x, int y)
        {
            return cucul_get_attr(cv, x, y);
        }

        public int setAttr(int a)
        {
            return cucul_set_attr(cv, a);
        }

        public int setAttr(int x, int y, int a)
        {
            return cucul_put_attr(cv, x, y, a);
        }

        public int setColorANSI(int fg, int bg)
        {
            return cucul_set_color_ansi(cv, fg, bg);
        }

        public int setColorARGB(int fg, int bg)
        {
            return cucul_set_color_ansi(cv, fg, bg);
        }

        public int setCanvasHandle(int x, int y)
        {
            return cucul_set_canvas_handle(cv, x, y);
        }

        public int getCanvasHandleX()
        {
            return cucul_get_canvas_handle_x(cv);
        }

        public int getCanvasHandleY()
        {
            return cucul_get_canvas_handle_y(cv);
        }

        public int setCanvasHandleY(int x, int y, int h,  int w)
        {
            return cucul_set_canvas_boundaries(cv, x, y, h, w); 
        }

          

        public int Invert()
        {
            return cucul_invert(cv);
        }

        public int Flip()
        {
            return cucul_flip(cv);
        }

        public int Flop()
        {
            return cucul_flop(cv);
        }

        public int Rotate()
        {
            return cucul_rotate(cv);
        }


        public int AttrToANSI(Int64 a)
        {
            return cucul_attr_to_ansi(a);
        }

        public int AttrToANSIFg(Int64 a)
        {
            return cucul_attr_to_ansi_fg(a);
        }

        public int AttrToANSIBg(Int64 a)
        {
            return cucul_attr_to_ansi_bg(a);
        }





        public int getFrameCount()
        {
            return cucul_get_frame_count(cv);
        }

        public int setFrame(int f)
        {
            return cucul_set_frame(cv, f);
        }

        public string getFrameName()
        {
            return cucul_get_frame_name(cv);
        }

        public int setFrameName(string n)
        {
            return cucul_set_frame_name(cv, n);
        }

        public int createFrame(int f)
        {
            return cucul_create_frame(cv, f);
        }

        public int freeFrame(int f)
        {
            return cucul_free_frame(cv, f);
        }



        /* Privates methods, are not meant to be called by user*/
        
        public IntPtr get_cucul_t()
        {
            return cv;
        }

    }




    public unsafe class CuculDither : IDisposable
    {
    [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]   
      public static extern IntPtr cucul_create_dither(int bpp, int w,
                                                      int h, int pitch,
                                                      Int64 rmask,
                                                      Int64 gmask,
                                                      Int64 bmask,
                                                      Int64 amask);


    [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]  
      public static extern int cucul_set_dither_palette(IntPtr d,
                             int[] r, int[] g,
                             int[] b, int[] a);
    [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]  
      public static extern int cucul_set_dither_brightness(IntPtr d, float b);
    [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]  
      public static extern int cucul_set_dither_gamma(IntPtr d, float g);
    [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]  
      public static extern int cucul_set_dither_contrast(IntPtr d, float c);
    [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]  
      public static extern int cucul_set_dither_invert(IntPtr d, int i);
    [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]  
      public static extern int cucul_set_dither_antialias(IntPtr d, string s);
    [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]  
      public static extern string[] cucul_get_dither_antialias_list(IntPtr d);
    [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]  
      public static extern int cucul_set_dither_color(IntPtr d, string s);
    [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]  
      public static extern string[] cucul_get_dither_color_list(IntPtr d);
    [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]  
      public static extern int cucul_set_dither_charset(IntPtr d, string s);
    [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]  
      public static extern string[] cucul_get_dither_charset_list(IntPtr d);
    [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]  
      public static extern int cucul_set_dither_mode(IntPtr d, string s);
    [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]  
      public static extern string[] cucul_get_dither_mode_list(IntPtr d);
    [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]  
      public static extern int cucul_free_dither(IntPtr d);

  /* FIXME  [DllImport("libcucul.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]  
      int cucul_dither_bitmap(Canvas c,int  x, int y, int w , int y,
                         IntPtr d2, void *);*/



        IntPtr dither;

        public CuculDither(int bpp, int w,int h, int pitch,
                           Int64 rmask, Int64 gmask,Int64 bmask, Int64 amask)
        {
            dither = cucul_create_dither(bpp, w, h, pitch, rmask, gmask, bmask, amask);
        }

        public void Dispose()
        {
            cucul_free_dither(dither);
            GC.SuppressFinalize(this);
        }

        public int setBrightness(float b)
        {
            return cucul_set_dither_brightness(dither, b);
        }

        public int setGamma(float g)
        {
            return cucul_set_dither_gamma(dither, g);
        }

        public int setContrast(float c)
        {
            return cucul_set_dither_contrast(dither, c);
        }

        public int setInvert(int i)
        {
            return cucul_set_dither_invert(dither, i);
        }

        public int setAntialias(string s)
        {
            return cucul_set_dither_antialias(dither, s);
        }

        public int setColor(string s)
        {
            return cucul_set_dither_color(dither, s);
        }

        public int setCharset(string s)
        {
            return cucul_set_dither_charset(dither, s);
        }

        public int setMode(string s)
        {
            return cucul_set_dither_mode(dither, s);
        }

        /* <FIXME> */
        public string[] getAntialiasList() 
        {
            return cucul_get_dither_antialias_list(dither);
        }

        public string[] getColorList() 
        {
            return cucul_get_dither_color_list(dither);
        }

        public string[] getCharsetList() 
        {
            return cucul_get_dither_charset_list(dither);
        }

        public string[] getModeList() 
        {
            return cucul_get_dither_mode_list(dither);
        }

        /* </FIXME> */
    }

}


