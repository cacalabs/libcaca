/*
 *  libcaca       .NET bindings for libcaca
 *  Copyright (c) 2006 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                2007 Sam Hocevar <sam@hocevar.net>
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
    public class Display : IDisposable
    {
        private Canvas _cv;
        public Canvas Canvas { get { return _cv; } }

        private IntPtr _c_cv;
        private IntPtr _c_dp;

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern IntPtr caca_create_display(IntPtr cv);
        public Display(Canvas cv)
        {
            _cv = cv;
            _c_cv = _cv._c_cv;
            _c_dp = caca_create_display(_c_cv);
        }

        public Display()
        {
            /* XXX: we do not call caca_create_display() with a NULL
             * argument because it's then impossible to create a Canvas
             * and I don't want to add a weird constructor */
            _cv = new Canvas();
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
        public Event getEvent(EventType t, int timeout)
        {
            Event e = new Event();
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

