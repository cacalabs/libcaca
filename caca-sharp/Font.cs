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
    public class Font : IDisposable
    {
        private IntPtr _font;
        private GCHandle _gch;

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern IntPtr caca_load_font(IntPtr data, uint len);
        public Font(string s)
        {
            IntPtr name = Marshal.StringToHGlobalAnsi(s);
            _font = caca_load_font(name, 0);
            Marshal.FreeHGlobal(name);
        }

        public Font(byte[] buf)
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
        public int Render(Canvas cv, uint[,] buf, int pitch)
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
}

