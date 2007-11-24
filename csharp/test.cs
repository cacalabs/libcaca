/*
 *  Test      .NET bindings test program
 *  Copyright (c) 2006 Jean-Yves Lamoureux <jylam@lnxscene.org>
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

using Cucul;
using Caca;

class DemoCanvas : CuculCanvas
{
    private DateTime startTime;

    public DemoCanvas()
    {
        startTime = DateTime.Now;
    }

    public void Draw()
    {
        int barCount = 6;
        double t = (DateTime.Now - startTime).TotalMilliseconds;

        Clear();

        setColorAnsi(Libcucul.WHITE, Libcucul.BLACK);
        for(int i = 0; i < barCount; i++)
        {
            double v = ((Math.Sin((t / 500.0)
                          + (i / ((double)barCount))) + 1) / 2) * height;
            int y = (int)v;

            setColorAnsi(i + 9, Libcucul.BLACK);
            /* drawLine is already clipped, we don't care about overflows */
            drawLine(0, y - 2, width, y - 2, '-');
            drawLine(0, y - 1, width, y - 1, '*');
            drawLine(0, y,     width, y,     '#');
            drawLine(0, y + 1, width, y + 1, '*');
            drawLine(0, y + 2, width, y + 2, '-');
        }

        setColorAnsi(Libcucul.WHITE, Libcucul.BLUE);
        putStr(width - 30, height - 2, " -=[ Powered by libcaca ]=- ");
        setColorAnsi(Libcucul.WHITE, Libcucul.BLACK);
    }
}

class DemoDisplay : CacaDisplay
{
    private Event e;
    private DemoCanvas cv;

    public DemoDisplay(DemoCanvas _cv) : base(_cv)
    {
        setDisplayTime(20000); // Refresh every 20 ms
        setDisplayTitle("libcaca .NET Bindings test suite");
        cv = _cv;
        e = new Event();
    }

    public void EventLoop()
    {
        while(getEvent(Event.type.KEY_RELEASE, e, 10) == 0)
        {
            cv.Draw();

            Refresh();
        }
    }
}

class Test
{
    public static void Main()
    {
        Console.WriteLine("libcaca .NET test");
        Console.WriteLine("(c) 2006 Jean-Yves Lamoureux <jylam@lnxscene.org>");

        /* Instanciate a cucul canvas */
        DemoCanvas cv = new DemoCanvas();

        /* We have a proper canvas, let's display it using Caca */
        DemoDisplay dp = new DemoDisplay(cv);

        /* Random number. This is a static method,
           not to be used with previous instance */
        Console.WriteLine("A random number : {0}", Libcucul.Rand(0, 1337));

        dp.EventLoop();

        /* Force deletion of our instances for fun */
        dp.Dispose();
        cv.Dispose();
    }

}
