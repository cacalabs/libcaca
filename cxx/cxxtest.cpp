/*
 *  cpptest       libcaca++ rendering test
 *  Copyright (c) 2006 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This program is free software. It commes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#include "config.h"

#include <iostream>

#include <cucul++.h>
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
    Cucul *qq, *pig;
    Caca  *kk;
    Event ev;

    int x = 0, y = 0, ix = 1, iy = 1;

    try {
        qq = new Cucul();
    }
    catch (int e) {
        cerr << "Error while initializing cucul (" << e << ")" << endl;
        return -1;
    }

    try {
        kk = new Caca(qq);
    }
    catch(int e) {
        cerr << "Error while attaching cucul to caca (" << e << ")" << endl;
        return -1;
    }

    try {
        // Import buffer into a canvas
        pig = new Cucul();
        pig->setColorANSI(CUCUL_LIGHTMAGENTA, CUCUL_TRANSPARENT);
        pig->importMemory(pigstring, strlen(pigstring), "text");
    }
    catch(int e) {
        cerr << "Error while importing image (" << e << ")" << endl;
        return -1;
    }

    kk->setDisplayTime(20000);

    while(!kk->getEvent(ev.CACA_EVENT_KEY_PRESS, &ev, 0)) {


        /* In case of resize ...*/
        if((x + pig->getWidth())-1 >= qq->getWidth() || x < 0 )
            x = 0;
        if((y + pig->getHeight())-1 >= qq->getHeight() || y < 0 )
            y = 0;




        qq->Clear();

        /* Draw pig */
        qq->Blit(x, y, pig, NULL);

        /* printf works */
        qq->setColorANSI(CUCUL_LIGHTBLUE, CUCUL_BLACK);
        qq->Printf(qq->getWidth() / 2 - 10, qq->getHeight() / 2,
                   "Powered by libcaca %s", VERSION);

        /* Blit */
        kk->Display();

        x += ix;
        y += iy;

        if(x + pig->getWidth() >= qq->getWidth() || x < 0 )
            ix = -ix;
        if(y + pig->getHeight() >= qq->getHeight() || y < 0 )
            iy = -iy;

    }

    delete kk;
    delete qq;

    return 0;
}
