/*
 *  Test          .NET bindings test program
 *  Copyright (c) 2006 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                2007 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */


using System;
using System.Drawing;
using System.Runtime.InteropServices;

using Caca;

class DemoCanvas : Canvas
{
    private uint[,] image;

    private DateTime startTime;
    private Dither d;
    private Canvas scroll;

    public DemoCanvas()
    {
        startTime = DateTime.Now;

        string message = " --- POWERED BY LIBCACA --- OLDSCHOOL TEXT EFFECTS ARE 100% PURE WIN";

        scroll = new Canvas(new Size(message.Length, 1));
        scroll.setColorAnsi(AnsiColor.WHITE, AnsiColor.TRANSPARENT);
        scroll.putStr(new Point(0, 0), message);

        Caca.Font f = new Caca.Font(Caca.Font.getList()[1]);
        int w = f.Size.Width * message.Length;
        int h = f.Size.Height;
        image = new uint[w, h];
        d = new Dither(32, new Size(w, h), w * 4,
                            0xff00, 0xff0000, 0xff000000, 0xff);
        f.Render(scroll, image, image.GetLength(0) * 4);
    }

    public void Draw()
    {
        int barCount = 6;
        double t = (DateTime.Now - startTime).TotalMilliseconds;

        Clear();

        setColorAnsi(AnsiColor.WHITE, AnsiColor.BLACK);
        for(int i = 0; i < barCount; i++)
        {
            double v = ((Math.Sin((t / 500.0)
                          + (i / ((double)barCount))) + 1) / 2) * Size.Height;
            Point p1 = new Point(0, (int)v);
            Point p2 = new Point(Size.Width - 1, (int)v);

            setColorAnsi((uint)(i + 9), AnsiColor.BLACK);
            /* drawLine is already clipped, we don't care about overflows */
            drawLine(p1 + new Size(0, -2), p2 + new Size(0, -2), '-');
            drawLine(p1 + new Size(0, -1), p2 + new Size(0, -1), '*');
            drawLine(p1,                   p2,                   '#');
            drawLine(p1 + new Size(0,  1), p2 + new Size(0,  1), '*');
            drawLine(p1 + new Size(0,  2), p2 + new Size(0,  2), '-');
        }

        int w = Size.Width;
        int h = Size.Height;
        int x = (int)(t / 10) % (12 * w);
        int y = (int)(h * (2.0 + Math.Sin(t / 200.0)) / 4);
        ditherBitmap(new Rectangle(- x, h / 2 - y, w * 12, y * 2), d, image);
        ditherBitmap(new Rectangle(12 * w - x, h / 2 - y, w * 12, y * 2), d, image);

        setColorAnsi(AnsiColor.WHITE, AnsiColor.BLUE);
        putStr(new Point(-30, -2) + Size, " -=[ Powered by libcaca ]=- ");
        setColorAnsi(AnsiColor.WHITE, AnsiColor.BLACK);
    }
}

class DemoDisplay : Display
{
    private DemoCanvas cv;

    public DemoDisplay(DemoCanvas _cv) : base(_cv)
    {
        Title = "libcaca .NET Bindings test suite";
        DisplayTime = 20000; // Refresh every 20 ms
        cv = _cv;
    }

    public void EventLoop()
    {
        Event ev;

        while((ev = getEvent(EventType.KEY_RELEASE, 10)).Type == 0)
        {
            cv.Draw();

            Refresh();
        }

        if(ev.KeyCh > 0x20 && ev.KeyCh < 0x7f)
            Console.WriteLine("Key pressed: {0}", ev.KeyUtf8);
        else
            Console.WriteLine("Key pressed: 0x{0:x}", ev.KeyCh);
    }
}

class Test
{
    public static void Main()
    {
        Console.WriteLine("libcaca {0} .NET test", Libcaca.getVersion());
        Console.WriteLine("(c) 2006 Jean-Yves Lamoureux <jylam@lnxscene.org>");

        /* Instanciate a caca canvas */
        DemoCanvas cv = new DemoCanvas();

        /* We have a proper canvas, let's display it using Caca */
        DemoDisplay dp = new DemoDisplay(cv);

        /* Random number. This is a static method,
           not to be used with previous instance */
        Console.WriteLine("A random number: {0}", Libcaca.Rand(0, 1337));

        dp.EventLoop();
    }
}

