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
    public class Dither : IDisposable
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
        public Dither(int bpp, Size s, int pitch,
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

#if false
        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern int caca_set_dither_palette(IntPtr d,
                               uint[] r, uint[] g,
                               uint[] b, uint[] a);
#endif
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
}

