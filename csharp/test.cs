/*
 *  Test          .NET bindings test program
 *  Copyright (c) 2006 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                2007 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */


using System;
using System.Drawing;
using System.Runtime.InteropServices;

using Cucul;
using Caca;

class DemoCanvas : CuculCanvas
{
    private uint[,] image;

    private DateTime startTime;
    private CuculDither d;
    private CuculFont f;

    public DemoCanvas()
    {
        startTime = DateTime.Now;

        image = new uint[16,16];
        d = new CuculDither(32, new Size(16, 16), 16 * 4,
                            0xff0000, 0xff00, 0xff, 0x0);
        f = new CuculFont(CuculFont.getList()[0]);
        f.getBlocks();
    }

    public void Draw()
    {
        int barCount = 6;
        double t = (DateTime.Now - startTime).TotalMilliseconds;

        Clear();

        double cos = Math.Cos(t / 500.0);
        double sin = Math.Sin(t / 500.0);

        for(int y = 0; y < 16; y++)
            for(int x = 0; x < 16; x++)
            {
                double xt = (double)(x - 8);
                double yt = (double)(y - 8);
                int x2 = (int)(xt * cos + yt * sin + 8.0);
                int y2 = (int)(xt * sin - yt * cos + 8.0);
                if(x2 < 0) x2 = 0;
                if(y2 < 0) y2 = 0;

                image[x,y] = (uint)((x2 + y2) << 16)
                              | (uint)(x2 << 8)
                              | (uint)(y2);
            }
        ditherBitmap(Rectangle, d, image);

        setColorAnsi(Libcucul.WHITE, Libcucul.BLACK);
        for(int i = 0; i < barCount; i++)
        {
            double v = ((Math.Sin((t / 500.0)
                          + (i / ((double)barCount))) + 1) / 2) * Size.Height;
            Point p1 = new Point(0, (int)v);
            Point p2 = new Point(Size.Width - 1, (int)v);

            setColorAnsi(i + 9, Libcucul.BLACK);
            /* drawLine is already clipped, we don't care about overflows */
            drawLine(p1 + new Size(0, -2), p2 + new Size(0, -2), '-');
            drawLine(p1 + new Size(0, -1), p2 + new Size(0, -1), '*');
            drawLine(p1,                   p2,                   '#');
            drawLine(p1 + new Size(0,  1), p2 + new Size(0,  1), '*');
            drawLine(p1 + new Size(0,  2), p2 + new Size(0,  2), '-');
        }

        setColorAnsi(Libcucul.WHITE, Libcucul.BLUE);
        putStr(new Point(-30, -2) + Size, " -=[ Powered by libcaca ]=- ");
        setColorAnsi(Libcucul.WHITE, Libcucul.BLACK);
    }
}

class DemoDisplay : CacaDisplay
{
    private DemoCanvas cv;

    public DemoDisplay(DemoCanvas _cv) : base(_cv)
    {
        displayTime = 20000; // Refresh every 20 ms
        title = "libcaca .NET Bindings test suite";
        cv = _cv;
    }

    public void EventLoop()
    {
        CacaEvent ev;

        while((ev = getEvent(CacaEventType.KEY_RELEASE, 10)).type == 0)
        {
            cv.Draw();

            Refresh();
        }

        if(ev.keyCh > 0x20 && ev.keyCh < 0x7f)
            Console.WriteLine("Key pressed: {0}", ev.keyUtf8);
        else
            Console.WriteLine("Key pressed: 0x{0:x}", ev.keyCh);
    }
}

class Test
{
    public static void Main()
    {
        Console.WriteLine("libcaca {0} .NET test", Libcaca.getVersion());
        Console.WriteLine("(c) 2006 Jean-Yves Lamoureux <jylam@lnxscene.org>");

        /* Instanciate a cucul canvas */
        DemoCanvas cv = new DemoCanvas();

        /* We have a proper canvas, let's display it using Caca */
        DemoDisplay dp = new DemoDisplay(cv);

        /* Random number. This is a static method,
           not to be used with previous instance */
        Console.WriteLine("A random number: {0}", Libcucul.Rand(0, 1337));

        dp.EventLoop();
    }
}

