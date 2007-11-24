/*
 *  CacaSharp      .NET bindings for libcaca
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

using libCucul;


namespace libCaca
  {

  enum Keys
  { 
    CACA_KEY_UNKNOWN = 0x00, /**< Unknown key. */

    /* The following keys have ASCII equivalents */
    CACA_KEY_BACKSPACE = 0x08, /**< The backspace key. */
    CACA_KEY_TAB =       0x09, /**< The tabulation key. */
    CACA_KEY_RETURN =    0x0d, /**< The return key. */
    CACA_KEY_PAUSE =     0x13, /**< The pause key. */
    CACA_KEY_ESCAPE =    0x1b, /**< The escape key. */
    CACA_KEY_DELETE =    0x7f, /**< The delete key. */

    /* The following keys do not have ASCII equivalents but have been
     * chosen to match the SDL equivalents */
    CACA_KEY_UP =    0x111, /**< The up arrow key. */
    CACA_KEY_DOWN =  0x112, /**< The down arrow key. */
    CACA_KEY_LEFT =  0x113, /**< The left arrow key. */
    CACA_KEY_RIGHT = 0x114, /**< The right arrow key. */

    CACA_KEY_INSERT =   0x115, /**< The insert key. */
    CACA_KEY_HOME =     0x116, /**< The home key. */
    CACA_KEY_END =      0x117, /**< The end key. */
    CACA_KEY_PAGEUP =   0x118, /**< The page up key. */
    CACA_KEY_PAGEDOWN = 0x119, /**< The page down key. */

    CACA_KEY_F1 =  0x11a, /**< The F1 key. */
    CACA_KEY_F2 =  0x11b, /**< The F2 key. */
    CACA_KEY_F3 =  0x11c, /**< The F3 key. */
    CACA_KEY_F4 =  0x11d, /**< The F4 key. */
    CACA_KEY_F5 =  0x11e, /**< The F5 key. */
    CACA_KEY_F6 =  0x11f, /**< The F6 key. */
    CACA_KEY_F7 =  0x120, /**< The F7 key. */
    CACA_KEY_F8 =  0x121, /**< The F8 key. */
    CACA_KEY_F9 =  0x122, /**< The F9 key. */
    CACA_KEY_F10 = 0x123, /**< The F10 key. */
    CACA_KEY_F11 = 0x124, /**< The F11 key. */
    CACA_KEY_F12 = 0x125, /**< The F12 key. */
    CACA_KEY_F13 = 0x126, /**< The F13 key. */
    CACA_KEY_F14 = 0x127, /**< The F14 key. */
    CACA_KEY_F15 = 0x128  /**< The F15 key. */
  }  
  public unsafe class Event
    {
    public enum type
      {
        NONE =          0x0000, /**< No event. */
        
        KEY_PRESS =     0x0001, /**< A key was pressed. */
        KEY_RELEASE =   0x0002, /**< A key was released. */
        MOUSE_PRESS =   0x0004, /**< A mouse button was pressed. */
        MOUSE_RELEASE = 0x0008, /**< A mouse button was released. */
        MOUSE_MOTION =  0x0010, /**< The mouse was moved. */
        RESIZE =        0x0020, /**< The window was resized. */
        QUIT =          0x0040, /**< The user requested to quit. */
        
        ANY =           0xffff  /**< Bitmask for any event. */
      };
    
    }


  public unsafe class Caca : IDisposable
    {
    [DllImport("libCaca.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern IntPtr caca_create_display(IntPtr qq);
    [DllImport("libCaca.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern void caca_free_display(IntPtr kk);
    [DllImport("libCaca.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern void caca_refresh_display(IntPtr kk);
    [DllImport("libCaca.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern void caca_set_display_time(IntPtr kk, Int32 d);
    [DllImport("libCaca.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 caca_get_display_time(IntPtr kk);
    [DllImport("libCaca.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 caca_get_display_width(IntPtr kk);
    [DllImport("libCaca.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 caca_get_display_height(IntPtr kk);
    [DllImport("libCaca.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 caca_set_display_title(IntPtr kk, string t);
    [DllImport("libCaca.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 caca_get_event(IntPtr k, Event.type t, Event e, Int32 timeout);
    [DllImport("libCaca.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 caca_get_mouse_x(IntPtr k);
    [DllImport("libCaca.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern Int32 caca_get_mouse_y(IntPtr k);
    [DllImport("libCaca.dll", CallingConvention=CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
      public static extern void caca_set_mouse(IntPtr k, bool status);





    IntPtr qq;
    IntPtr kk;

    public Caca(Cucul qqt)
        {
        qq = qqt.get_cucul_t();
        kk = caca_create_display(qq);
        }
    public void Dispose()
        {
        caca_free_display(kk);
        GC.SuppressFinalize(this);
        }
    public void Refresh()
        {
        caca_refresh_display(kk);
        }
    public void setDisplayTime(Int32 d)
        {
        caca_set_display_time(kk, d);
        }
    public Int32 getDisplayTime()
        {
        return caca_get_display_time(kk);
        }
    public Int32 getDisplayWidth()
        {
        return caca_get_display_width(kk);
        }
    public Int32 getDisplayHeight()
        {
        return caca_get_display_height(kk);
        }
    public Int32 setDisplayTitle(string t)
        {
        return caca_set_display_title(kk, t);
        }
    public Int32 getEvent(Event.type t, Event e, Int32 timeout)
        {
        return caca_get_event(kk, t, e, timeout);
        }
    public Int32 getMouseX()
        {
        return caca_get_mouse_x(kk);
        }
    public Int32 getMouseY()
        {
        return caca_get_mouse_y(kk);
        }
    public void caca_set_mouse(bool status)
        {
        caca_set_mouse(kk, status);
        }







    public IntPtr get_caca_t()
        {
        return kk;
        }


    }
  }
