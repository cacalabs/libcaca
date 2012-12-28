#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# event         event lister for libcaca
# Copyright (c) 2010 Alex Foulon <alxf@lavabit.com>
#
# This file is a Python port of "examples/event.c"
# which is:
# Copyright (c) 2004-2010 Sam Hocevar <sam@hocevar.net>
#               All Rights Reserverd
#
# This library is free software. It comes without any warranty, to
# the extent permitted by applicable law. You can redistribute it
# and/or modify it under the terms of the Do What the Fuck You Want
# to Public License, Version 2, as published by Sam Hocevar. See
# http://www.wtfpl.net/ for more details.
#

import sys

import caca
from caca.canvas import Canvas, CanvasError
from caca.display import Display, DisplayError, Event

def print_event(cv, x, y, event):
    """ Print event name.

        :cv: libcaca canvas
        :x: X coordinate
        :y: Y coordinate
        :event: event to display
    """

    if event.get_type() == caca.EVENT_NONE:
        cv.put_str(x, y, "CACA_EVENT_NONE")
    elif event.get_type() == caca.EVENT_KEY_PRESS:
        character = event.get_key_ch()
        if character > 0x1f and character < 0x80:
            cv.put_str(x, y, "CACA_EVENT_KEY_PRESS 0x%02x (%s)" % (character, chr(character)))
        else:
            cv.put_str(x, y, "CACA_EVENT_KEY_PRESS 0x%02x (%s)" % (character, '?'))
    elif event.get_type() == caca.EVENT_KEY_RELEASE:
        character = event.get_key_ch()
        if character > 0x1f and character < 0x80:
            cv.put_str(x, y, "CACA_EVENT_KEY_RELEASE 0x%02x (%s)" % (character, chr(character)))
        else:
            cv.put_str(x, y, "CACA_EVENT_KEY_RELEASE 0x%02x (%s)" % (character, '?'))
    elif event.get_type() == caca.EVENT_MOUSE_MOTION:
        cv.put_str(x, y, "CACA_EVENT_MOUSE_MOTION %u %u" % (event.get_mouse_x(),
                                                            event.get_mouse_y()))
    elif event.get_type() == caca.EVENT_MOUSE_PRESS:
        cv.put_str(x, y, "CACA_EVENT_MOUSE_PRESS %u" % (event.get_mouse_button()))
    elif event.get_type() == caca.EVENT_MOUSE_RELEASE:
        cv.put_str(x, y, "CACA_EVENT_MOUSE_RELEASE %u" % (event.get_mouse_button()))
    elif event.get_type() == caca.EVENT_RESIZE:
        cv.put_str(x, y, "CACA_EVENT_RESIZE %u %u" % (event.get_resize_width(),
                                                            event.get_resize_height()))
    elif event.get_type() == caca.EVENT_QUIT:
        cv.put_str(x, y, "CACA_EVENT_QUIT")
    else:
        cv.put_str(x, y, "CACA_EVENT_UNKNOWN")

def main():
    """ Main function. """

    events = []
    quit = 0
    quit_string = ["", "q", "qu", "qui", "quit"]

    try:
        cv = Canvas(80, 24)
        dp = Display(cv)
    except (CanvasError, DisplayError) as err:
        sys.stderr.write("%s\n" % err)
        sys.exit(127)

    h = cv.get_height() - 1

    cv.set_color_ansi(caca.COLOR_WHITE, caca.COLOR_BLUE)
    cv.draw_line(0, 0, cv.get_width() - 1, 0, ' ')
    cv.draw_line(0, h, cv.get_width() - 1, h, ' ')
    cv.put_str(0, h, "Type \"quit\" to exit")

    dp.refresh()

    while quit < 4:
        ev = Event()
        if dp.get_event(caca.EVENT_ANY, ev, -1):
            if ev.get_type() == caca.EVENT_KEY_PRESS:
                key = ev.get_key_ch()
                if key == ord('u') and quit == 1:
                    quit += 1
                elif key == ord('i') and quit == 2:
                    quit += 1
                elif key == ord('t') and quit == 3:
                    quit += 1
                elif key == ord('q'):
                    quit = 1
                else:
                    quit = 0

            events.append(ev)

        cv.set_color_ansi(caca.COLOR_LIGHTGRAY, caca.COLOR_BLACK)
        cv.clear()

        #print current event
        cv.set_color_ansi(caca.COLOR_WHITE, caca.COLOR_BLUE)
        cv.draw_line(0, 0, cv.get_width() - 1, 0, ' ')
        if events:
            print_event(cv, 0, 0, events[-1])

        cv.draw_line(0, h, cv.get_width() - 1, h, ' ')
        cv.put_str(0, h, "Type \"quit\" to exit: %s" % quit_string[quit])

        #print previous events
        cv.set_color_ansi(caca.COLOR_WHITE, caca.COLOR_BLACK)
        counts = list(range(0, len(events)-1))
        counts.reverse()
        if len(events) > 1:
            j = 0
            for i in counts:
                if j < h - 1 and events[i].get_type():
                    print_event(cv, 0, ((len(events) - 1) - i), events[i])
                j += 1

        dp.refresh()

if __name__ == "__main__":
    main()

