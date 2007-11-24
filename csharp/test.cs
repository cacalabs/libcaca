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

class Test {



	public static void Main() {
        int barCount = 6;
        Console.WriteLine("libcaca .NET test");
		Console.WriteLine("(c) 2006 Jean-Yves Lamoureux <jylam@lnxscene.org>");

        /* Instanciate a cucul canvas */
        CuculCanvas cv = new CuculCanvas();


        /* Random number. This is a static method,
           not to be used with previous instance */
        Console.WriteLine("A random number : {0}", Libcucul.Rand(0, 1337));



        /* We have a proper canvas, let's display it using Caca */
        Display dp = new Display(cv);
        dp.setDisplayTime(20000); // Refresh every 20 ms

        dp.setDisplayTitle("libcaca .NET Bindings test suite");

        double v;
        Int32 y = 0;
        Event e = new Event();
        int i;

        DateTime startTime = DateTime.Now;
        while(dp.getEvent(Event.type.KEY_RELEASE, e, 10) == 0)
          {
            TimeSpan curTime = DateTime.Now - startTime;
            double t = curTime.TotalMilliseconds;
            cv.setColorAnsi(Libcucul.WHITE, Libcucul.BLACK);
            for(i=0; i<barCount;i++)
             {
                v = ((Math.Sin((t/500.0)+(i/((double)barCount)))+1)/2)*cv.height;
                y = (Int32) v;



                cv.setColorAnsi(i+9, Libcucul.BLACK);
                /* drawLine is already clipped, we don't care about overflows */
                cv.drawLine(0, y-2, cv.width, y-2, '-');
                cv.drawLine(0, y-1, cv.width, y-1, '*');
                cv.drawLine(0, y, cv.width, y, '#');
                cv.drawLine(0, y+1, cv.width, y+1, '*');
                cv.drawLine(0, y+2, cv.width, y+2, '-');
             }

             cv.setColorAnsi(Libcucul.WHITE, Libcucul.BLUE);
             cv.putStr(cv.width - 30,cv.height - 2," -=[ Powered by libcaca ]=- ");
             cv.setColorAnsi(Libcucul.WHITE, Libcucul.BLACK);


            dp.Refresh();
            cv.Clear();

          }

        /* Force deletion of our instances for fun */
        dp.Dispose();
        cv.Dispose();
	}

}
