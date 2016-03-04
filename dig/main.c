#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>
#include  <math.h>

int main( int iArgc, char *cpArgv[] )
{
    double f;
    double x, y, z;
    double d;

    sigrf();

    igrfc( 42.0, 140.0, 0.0, &f );

    gcomp( &x, &y, &z );

    d = atan2( y, x );

    fprintf( stdout, "value = %f\n", d );
    fflush( stdout );

    return 0;
}





