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
using libCucul;
using libCaca;

class Test {



	public static void Main() {
        int barCount = 6;
        Console.WriteLine("libcaca .NET test");
		Console.WriteLine("(c) 2006 Jean-Yves Lamoureux <jylam@lnxscene.org>");
        
        /* Instanciate a cucul canvas */
        Cucul qq = new Cucul();

 
        /* Random number. This is a static method, 
           not to be used with previous instance */
        Console.WriteLine("A random number : {0}", Cucul.Rand(0, 1337));



        /* We have a proper canvas, let's display it using Caca */
        Caca kk = new Caca(qq);
        kk.setDisplayTime(20000); // Refresh every 20 ms

        kk.setDisplayTitle("libcaca .NET Bindings test suite");

        double v;
        Int32 y = 0;
        Event e = new Event();
        Int32 i;
        
        DateTime startTime = DateTime.Now;
        while(kk.getEvent(Event.type.KEY_RELEASE, e, 10) == 0)
          {
            TimeSpan curTime = DateTime.Now - startTime;
            double t = curTime.TotalMilliseconds;
            qq.setColor(Cucul.CUCUL_WHITE, Cucul.CUCUL_BLACK);
            for(i=0; i<barCount;i++) 
             {
                v = ((Math.Sin((t/500.0)+(i/((double)barCount)))+1)/2)*qq.getHeight();
                y = (Int32) v;

 

                qq.setColor(i+9, Cucul.CUCUL_BLACK);
                /* drawLine is already clipped, we don't care about overflows */
                qq.drawLine(0, y-2, qq.getWidth(), y-2, '-'); 
                qq.drawLine(0, y-1, qq.getWidth(), y-1, '*');
                qq.drawLine(0, y, qq.getWidth(), y, '#');
                qq.drawLine(0, y+1, qq.getWidth(), y+1, '*');
                qq.drawLine(0, y+2, qq.getWidth(), y+2, '-');
             }

             qq.setColor(Cucul.CUCUL_WHITE, Cucul.CUCUL_BLUE);   
             qq.putStr(qq.getWidth() - 30,qq.getHeight() - 2," -=[ Powered by libcaca ]=- ");
             qq.setColor(Cucul.CUCUL_WHITE, Cucul.CUCUL_BLACK);   
 

            kk.Refresh();
            qq.Clear();
            
          }

        /* Force deletion of our instances for fun */
        qq.Dispose();
        kk.Dispose();
	}

}
