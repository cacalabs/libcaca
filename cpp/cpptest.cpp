#include <iostream>

#include <cucul++.h>
#include <caca++.h>

using namespace std;


static char const *pig[]= {
    "                             _    ",
    "    _._ _..._ .-',     _.._(`))   ",
    "   '-. `     '  /-._.-'    ',/    ",
    "      )         \            '.   ",
    "     / _    _    |             \\  ",
    "    |  a    a    /              | ",
    "    \   .-.                     ; " ,
    "     '-('' ).-'       ,'       ;  ",
    "        '-;           |      .'   ",
    "           \\           \\    /     ",
    "           | 7  .__  _.-\\   \\     ",
    "           | |  |  ``/  /`  /     ",
    "      jgs /,_|  |   /,_/   /      ",
    "             /,_/      '`-'       ",
 NULL
};

int main(int argc, char *argv[])
{
    Cucul *qq;
    Caca  *kk;
    Caca::Event ev;

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

    qq->set_color(CUCUL_COLOR_LIGHTMAGENTA, CUCUL_COLOR_BLACK);

    for(int i = 0; pig[i]; i++)
        qq->putstr(0, i, (char*)pig[i]);


    kk->display();
    kk->get_event(CACA_EVENT_KEY_PRESS, &ev, -1);


    delete kk;
    delete qq;

    return 0;
}
