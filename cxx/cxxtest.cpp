/*
 *  cpptest       libcaca++ rendering test
 *  Copyright (c) 2006-2007 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                2009-2012 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

#include <iostream>
#include <cstring>

#include <caca++.h>

using namespace std;


static char const pigstring[] =
    "                                   \n"
    "                             _     \n"
    "    _._ _..._ .-',     _.._(`))    \n"
    "   '-. `     '  /-._.-'    ',/     \n"
    "      )         \\            '.    \n"
    "     / _    _    |             \\   \n"
    "    |  a    a    /              |  \n"
    "    \\   .-.                     ;  \n"
    "     '-('' ).-'       ,'       ;   \n"
    "        '-;           |      .'    \n"
    "           \\           \\    /      \n"
    "           | 7  .__  _.-\\   \\      \n"
    "           | |  |  ``/  /`  /      \n"
    "      jgs /,_|  |   /,_/   /       \n"
    "             /,_/      '`-'        \n";

int main(int argc, char *argv[])
{
    Canvas *cv, *pig;
    Caca  *dp;

    int x = 0, y = 0, ix = 1, iy = 1;

    try {
        cv = new Canvas();
    }
    catch (int e) {
        cerr << "Error while creating canvas (" << e << ")" << endl;
        return -1;
    }

    try {
        dp = new Caca(cv);
    }
    catch(int e) {
        cerr << "Error while attaching canvas to display (" << e << ")" << endl;
        return -1;
    }

    try {
        // Import buffer into a canvas
        pig = new Canvas();
        pig->setColorANSI(CACA_LIGHTMAGENTA, CACA_TRANSPARENT);
        pig->importFromMemory(pigstring, strlen(pigstring), "text");
    }
    catch(int e) {
        cerr << "Error while importing image (" << e << ")" << endl;
        return -1;
    }

    dp->setDisplayTime(20000);

    while(!dp->getEvent(Event::CACA_EVENT_KEY_PRESS, NULL, 0))
    {

        /* In case of resize ...*/
        if((x + pig->getWidth())-1 >= cv->getWidth() || x < 0 )
            x = 0;
        if((y + pig->getHeight())-1 >= cv->getHeight() || y < 0 )
            y = 0;

        cv->Clear();

        /* Draw pig */
        cv->Blit(x, y, pig, NULL);

        /* printf works */
        cv->setColorANSI(CACA_LIGHTBLUE, CACA_BLACK);
        cv->Printf(cv->getWidth() / 2 - 10, cv->getHeight() / 2,
                   "Powered by libcaca %s", dp->getVersion());

        /* Blit */
        dp->Display();

        x += ix;
        y += iy;

        if(x + pig->getWidth() >= cv->getWidth() || x < 0 )
            ix = -ix;
        if(y + pig->getHeight() >= cv->getHeight() || y < 0 )
            iy = -iy;

    }

    delete dp;
    delete pig;
    delete cv;

    return 0;
}
