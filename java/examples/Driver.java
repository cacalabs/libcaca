/**
 *  libcaca       Java bindings for libcaca
 *  Copyright (c) 2009 Adrien Grand <jpountz@dinauz.org>
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

import org.zoy.caca.Canvas;
import org.zoy.caca.Color;
import org.zoy.caca.Display;
import org.zoy.caca.Event;
import org.zoy.caca.TimeoutException;

public class Driver {

  public static void main(String[] args) {
    Canvas cv = new Canvas(32, 16);
    Display dp = new Display(cv);
    cv.setColor(Color.Ansi.WHITE, Color.Ansi.BLACK);
    Display.Driver driver;
    int i, cur = 0;
    while(true) {
      cv.put(1, 0, "Available drivers");
      driver = dp.getDriver();
      Display.Driver[] list = Display.getDriverList();
      for (i = 0; i < list.length; i++) {
        if (driver.equals(list[i])) {
          cur = i;
        }
        cv.drawLine(0, 2*i+2, 9999, 2*i+2, ' ');
        cv.put(2, 2*i+2, (cur == i ? "* " : "  ") + list[i].getCode() + " " + list[i].getDescription());
      }
      cv.put(1, 2*i + 2, "Switching driver in 5 seconds");
      dp.refresh();
      try {
        dp.getEvent(Event.Type.KEY_PRESS, 5000000);
        break;
      } catch(TimeoutException e) {
        // Let's continue
      }
      cur++;
      if (list[cur].getCode().equals("raw")) cur++;
      if (cur >= list.length) cur = 0;
      dp.setDriver(list[cur]);
    }
  }

}
