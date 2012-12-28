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
    public class Attr
    {
        private uint _attr;

        public Attr(uint attr)
        {
            _attr = attr;
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern byte caca_attr_to_ansi(uint a);
        public byte toAnsi()
        {
            return caca_attr_to_ansi(_attr);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern byte caca_attr_to_ansi_fg(uint a);
        public byte toAnsiFg()
        {
            return caca_attr_to_ansi_fg(_attr);
        }

        [DllImport("libcaca.dll", CallingConvention=CallingConvention.Cdecl),
         SuppressUnmanagedCodeSecurity]
        private static extern byte caca_attr_to_ansi_bg(uint a);
        public byte toAnsiBg()
        {
            return caca_attr_to_ansi_bg(_attr);
        }
    }
}

