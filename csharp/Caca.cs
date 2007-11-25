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

using Cucul;

namespace Caca
{
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

    public class CacaEvent : IDisposable
    {
        public IntPtr cevent;
        private IntPtr utf8;

        public CacaEvent()
        {
            cevent = Marshal.AllocHGlobal(32);
            utf8 = Marshal.AllocHGlobal(8);
        }

        public void Dispose()
        {
            Marshal.FreeHGlobal(cevent);
            Marshal.FreeHGlobal(utf8);
            GC.SuppressFinalize(this);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_event_type(IntPtr ev);
        public CacaEventType type
        {
            get { return (CacaEventType)caca_get_event_type(cevent); }
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_event_key_ch(IntPtr ev);
        public int keyCh
        {
            get { return caca_get_event_key_ch(cevent); }
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_event_key_utf32(IntPtr ev);
        public int keyUtf32
        {
            get { return caca_get_event_key_utf32(cevent); }
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_event_key_utf8(IntPtr ev,
                                                          IntPtr utf8);
        public string keyUtf8
        {
            get
            {
                caca_get_event_key_utf8(cevent, utf8);
                return Marshal.PtrToStringUni(utf8);
            }
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_event_mouse_button(IntPtr ev);
        public int mouseButton
        {
            get { return caca_get_event_mouse_button(cevent); }
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_event_mouse_x(IntPtr ev);
        public int mouseX
        {
            get { return caca_get_event_mouse_x(cevent); }
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_event_mouse_y(IntPtr ev);
        public int mouseY
        {
            get { return caca_get_event_mouse_y(cevent); }
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_event_resize_width(IntPtr ev);
        public int resizeWidth
        {
            get { return caca_get_event_resize_width(cevent); }
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_event_resize_height(IntPtr ev);
        public int resizeHeight
        {
            get { return caca_get_event_resize_height(cevent); }
        }
    }

    public class CacaDisplay : IDisposable
    {
        private IntPtr _cv;
        private IntPtr _dp;

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern IntPtr caca_create_display(IntPtr cv);
        public CacaDisplay(CuculCanvas cv)
        {
            _cv = cv._cv;
            _dp = caca_create_display(_cv);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern void caca_free_display(IntPtr dp);
        public void Dispose()
        {
            caca_free_display(_dp);
            GC.SuppressFinalize(this);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern void caca_refresh_display(IntPtr dp);
        public void Refresh()
        {
            caca_refresh_display(_dp);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern void caca_set_display_time(IntPtr dp, int d);
        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_display_time(IntPtr dp);
        public int displayTime
        {
            get { return caca_get_display_time(_dp); }
            set { caca_set_display_time(_dp, value); }
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_event(IntPtr dp, int t,
                                                 IntPtr cevent,
                                                 int timeout);
        public CacaEvent getEvent(CacaEventType t, int timeout)
        {
            CacaEvent e = new CacaEvent();
            caca_get_event(_dp, (int)t, e.cevent, timeout);
            return e;
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_display_width(IntPtr dp);
        public int width
        {
            get { return caca_get_display_width(_dp); }
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_display_height(IntPtr dp);
        public int height
        {
            get { return caca_get_display_height(_dp); }
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_set_display_title(IntPtr dp, string t);
        public string title
        {
            set { caca_set_display_title(_dp, value); }
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern void caca_set_mouse(IntPtr k, bool status);
        public bool mouse
        {
            set { caca_set_mouse(_dp, value); }
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_mouse_x(IntPtr k);
        public int mouseX
        {
            get { return caca_get_mouse_x(_dp); }
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_get_mouse_y(IntPtr k);
        public int mouseY
        {
            get { return caca_get_mouse_y(_dp); }
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern void caca_set_cursor(IntPtr k, bool status);
        public bool cursor
        {
            set { caca_set_cursor(_dp, value); }
        }
    }
}
