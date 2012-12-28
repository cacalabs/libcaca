/*
 *  libcaca       .NET bindings for libcaca
 *  Copyright (c) 2006 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                2007 Sam Hocevar <sam@@hocevar.net>
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
    }
}

