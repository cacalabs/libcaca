/*
 *  Test      .NET bindings test program
 *  Copyright (c) 2006 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */


using System;
using libCucul;
using libCaca;

class Test {



	public static void Main() {
        Console.WriteLine("libcaca .NET test");
		Console.WriteLine("(c) 2006 Jean-Yves Lamoureux <jylam@lnxscene.org>");
        
        /* Instanciate a cucul canvas */
        Cucul qq = new Cucul();
        
        /* Get size, and change it */
        Console.WriteLine("Old size : {0}x{1}", qq.getWidth(), qq.getHeight());
        qq.setSize(80,50);
        Console.WriteLine("Newsize  : {0}x{1}", qq.getWidth(), qq.getHeight());

        /* Random number. This is a static method, 
           not to be used with previous instance */
        Console.WriteLine("A random number : {0}", Cucul.Rand(0, 1337));


        qq.putChar(0,0, 'J');

        qq.setColor(Cucul.CUCUL_BLUE, Cucul.CUCUL_RED);
        qq.drawLine(10, 15, 45, 27, "#");

        qq.putStr(10, 10, "Hello from .NET");


        Console.WriteLine("Char at 0,0 : {0}", qq.getChar(0,0));

        qq.Flip();

        /* We have a proper canvas, let's display it using Caca */
        Caca kk = new Caca(qq);
        kk.setDisplayTime(2000000); // Refresh every 2 seconds

        kk.setDisplayTitle("libcaca .NET Bindings test suite");

        Event e = new Event();
        int startTime = kk.getDisplayTime();
        while(kk.getEvent(Event.type.KEY_RELEASE, e, 10) == 0)
          {
          kk.Refresh();
          Console.WriteLine("Render time : {0}", kk.getDisplayTime()-startTime);
          }

        /* Force deletion of our instance for fun */
        qq.Dispose();
	}

}
