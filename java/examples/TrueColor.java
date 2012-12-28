/**
 *  libcaca       Java bindings for libcaca
 *  Copyright (c) 2009 Adrien Grand <jpountz@dinauz.org>
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

import org.zoy.caca.Canvas;
import org.zoy.caca.Color;
import org.zoy.caca.Display;
import org.zoy.caca.Event;

public class TrueColor {

  public static void main(String[] args) {
    Canvas cv = new Canvas(32, 16);
    Display dp = new Display(cv);
    for (int y = 0; y < 16; y++) {
      for (int x = 0; x < 16; x++) {
        int bgcolor = 0xff00 | (y << 4) | x;
        int fgcolor = 0xf000 | ((15 - y) << 4) | ((15 - x) << 8);
        cv.setColor(new Color.Argb(bgcolor), new Color.Argb(fgcolor));
        cv.put(x*2, y, "CA");
      }
    }
    cv.setColor(Color.Ansi.WHITE, Color.Ansi.LIGHTBLUE);
    cv.put(2, 1, "truecolor libcaca");
    dp.refresh();
    dp.getEvent(Event.Type.KEY_PRESS, -1);
  }

}
