#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>
#include  <math.h>

#include  "t.h"

int main( int iArgc, char *cpArgv[] )
{
    double f;
    double x, y, z;
    double d;

    sigrf();

    debug();


    igrfc( 42.0, 140.0, 0.0, &f );


    gcomp( &x, &y, &z );
    fprintf( stdout, "x, y, z is %f, %f, %f\n", x, y, z );
    fflush( stdout );

    d = atan2( y, x );

    fprintf( stdout, "value = %f\n", d * 180.0 / 3.14159265 );
    fflush( stdout );

    return 0;
}





