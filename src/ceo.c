
#include "common.h"

void ceo_alert( void )
{
    char key;
    int  end = 0;

    while( !end )
    {
        clear_graphics();

        if( get_key() == '\t' )
        {
            end = 1;
        }

        fprintf( stderr, "foo\n" );

        refresh_graphics();

        usleep( 40000 );
    }
}
