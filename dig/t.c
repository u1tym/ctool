#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>
#include  <math.h>
#include  "t.h"

#define MxOD    ( 19 )
#define URAD    ( 180.0 / 3.14159265359 )

#define RA      ( 6371.2 )
#define RE      ( 6378.16 )
#define RE2     ( RE * RE )
#define RE4     ( RE2 * RE2 )

#define RPRE    ( 1.0 - 1.0 / 298.25 )

#define RP      ( RE * RPRE )
#define RP2     ( RP * RP )
#define RP4     ( RP2 * RP2 )





static double tzero;
static int    kext;
static double ext0;
static double ext1;
static double ext2;

static int    maxod;

static int    kg;
static int    kgc;
static int    kph;

static double gh[ MxOD + 1 ][ MxOD + 1 ];
static double ght[ MxOD + 1 ][ MxOD + 1 ];
static double g[ MxOD + 1 ][ MxOD + 1 ];
double vgh[ MxOD + 1 ][ MxOD + 1 ];
double vght[ MxOD + 1 ][ MxOD + 1 ];

static double blat;
static double blon;
static double rlat;
static double slat;
static double slat2;
static double clat;
static double clat2;
static double r;
static double x;
static double y;
static double z;
static double f;
static double cth;
static double sth;
static double cph;
static double sph;
static double phi;
static double bthe;
static double bthc;

static int kr;
static int kth;

void field( double are, double aflat, double ara, int maxoda );
void tcoef( double agh[ MxOD + 1 ][ MxOD + 1 ],
            double aght[ MxOD + 1 ][ MxOD + 1 ],
            double atzero, int kexta, double aext[ 3 ] );
void tyear( double ayear );
void igrfc( double fido, double fkeido, double hght, double *tf );
void mfldg( double alat, double alon,
            double *ax, double *ay, double *az, double *af );
static void fcalc( double *, double *, double *, double * );

static float rf[7][120] = {
        { -30339., -2123.,  5758.,
           -1654.,  2994., -2006.,  1567.,   130.,  1297., -2036.,
          -403.,1289., 242., 843.,-176., 958., 805., 149., 492.,-280.,
          -392.,   8., 256.,-265.,-223., 357.,  16., 246., 125., -26.,
          -123.,-161.,-107., -51.,  77.,  47.,  60., -14.,   4., 106.,
          -229.,  68.,   3., -32.,  -4., -10.,-112., -13.,  71., -54.,
           -57.,   0., -27.,  12.,  -8., -25.,   9.,  -9.,  23.,  13.,
           -19.,  -2., -17.,  10.,   9.,   3.,  -3., -13., -12.,   5.,
            -4., -17.,   7.,   4.,  -5.,  22.,  12.,  -3.,   6., -16.,
         0.,0.,0.,0.,0.,0.,0.,0.,0.,0., 0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,
         0.,0.,0.,0.,0.,0.,0.,0.,0.,0., 0.,0.,0.,0.,0.,0.,0.,0.,0.,0.},
        { -30186., -2036.,  5735.,
           -1898.,  2997., -2124.,  1551.,   -37.,  1299., -2144.,
          -361.,1296., 249., 805.,-253., 951., 807., 148., 462.,-264.,
          -393.,  37., 235.,-307.,-204., 368.,  39., 275., 142., -20.,
          -147.,-161., -99., -38.,  74.,  46.,  57., -23.,  15., 102.,
          -210.,  88.,  -1., -43.,  -8.,  -9.,-114.,  -4.,  66., -57.,
           -68.,  -7., -24.,   7.,  -4., -22.,  11.,  -9.,  27.,  11.,
           -17.,  -8., -14.,  11.,  13.,   4.,   3., -15., -12.,   2.,
            -4., -19.,   6.,   1.,  -2.,  18.,   9.,  -6.,   1., -19.,
         0.,0.,0.,0.,0.,0.,0.,0.,0.,0., 0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,
         0.,0.,0.,0.,0.,0.,0.,0.,0.,0., 0.,0.,0.,0.,0.,0.,0.,0.,0.,0.},
        { -29988., -1957.,  5606.,
           -1997.,  3028., -2129.,  1662.,  -199.,  1279., -2181.,
          -335.,1251., 271., 833.,-252., 938., 783., 212., 398.,-257.,
          -419.,  53., 199.,-298.,-219., 357.,  46., 261., 149., -74.,
          -150.,-162., -78., -48.,  92.,  49.,  65., -15.,  42.,  93.,
          -192.,  71.,   4., -43.,  14.,  -2.,-108.,  17.,  70., -59.,
           -83.,   2., -28.,  20.,  -5., -13.,  16.,   1.,  18.,  11.,
           -23.,  -2., -10.,  20.,   7.,   7.,   1., -18., -11.,   4.,
            -7., -22.,   4.,   9.,   3.,  16.,   7., -13.,  -1., -15.,
             6.,  11., -21.,   2.,  16., -12.,   9.,   9.,  -5.,  -3.,
            -7.,  -1.,   9.,   7.,  10.,   1.,  -6.,  -5.,   2.,  -3.,
            -4.,   1.,   2.,   1.,  -5.,   2.,  -2.,   5.,   5.,  -4.,
             3.,  -1.,   1.,  -2.,   2.,   4.,   3.,  -1.,   0.,  -6.},
        { -29877., -1903.,  5497.,
           -2073.,  3045., -2191.,  1691.,  -309.,  1300., -2208.,
          -312.,1244., 284., 835.,-296., 937., 780., 233., 363.,-250.,
          -426.,  68., 169.,-298.,-215., 356.,  47., 253., 148., -94.,
          -155.,-161., -75., -48.,  95.,  52.,  65., -16.,  50.,  90.,
          -186.,  69.,   4., -50.,  17.,  -4.,-102.,  20.,  75., -61.,
           -82.,   2., -26.,  24.,  -1.,  -6.,  23.,   4.,  17.,   9.,
           -21.,   0.,  -6.,  21.,   6.,   7.,   0., -21., -11.,   5.,
            -9., -25.,   2.,  11.,   4.,  12.,   4., -16.,  -6., -10.,
             5.,  10., -21.,   1.,  16., -12.,   9.,   9.,  -5.,  -3.,
            -6.,  -1.,   9.,   7.,  10.,   2.,  -6.,  -5.,   2.,  -4.,
            -4.,   1.,   2.,   0.,  -5.,   3.,  -2.,   6.,   5.,  -4.,
             3.,   0.,   1.,  -1.,   2.,   4.,   3.,   0.,   0.,  -6.},
        { -29775., -1851.,  5411.,
           -2136.,  3058., -2278.,  1693.,  -380.,  1315., -2240.,
          -287.,1246., 293., 807.,-348., 939., 782., 248., 324.,-240.,
          -423.,  87., 142.,-299.,-211., 353.,  47., 244., 153.,-111.,
          -154.,-166., -69., -37.,  98.,  61.,  64., -16.,  60.,  83.,
          -178.,  68.,   2., -52.,  17.,   2., -96.,  27.,  77., -64.,
           -81.,   4., -27.,  28.,   1.,   1.,  20.,   6.,  16.,  10.,
           -23.,   0.,  -5.,  22.,   5.,  10.,  -1., -20., -11.,   7.,
           -12., -22.,   4.,  12.,   4.,  11.,   3., -16.,  -6., -11.,
             4.,  10., -21.,   1.,  15., -12.,  10.,   9.,  -6.,  -4.,
            -6.,  -1.,   9.,   7.,   9.,   2.,  -7.,  -6.,   2.,  -4.,
            -4.,   1.,   2.,   0.,  -5.,   3.,  -2.,   6.,   4.,  -4.,
             3.,   0.,   1.,  -1.,   2.,   4.,   3.,   0.,   0.,  -6.},
        { -29682., -1789.,  5318.,
           -2197.,  3074., -2356.,  1685.,  -425.,  1329., -2268.,
          -263.,1249., 302., 769.,-406., 941., 782., 262., 291.,-232.,
          -421.,  98., 116.,-301.,-210., 352.,  44., 237., 157.,-122.,
          -152.,-167., -64., -26.,  99.,  66.,  64., -16.,  65.,  77.,
          -172.,  67.,   2., -57.,  17.,   4., -94.,  28.,  78., -67.,
           -77.,   1., -25.,  29.,   3.,   4.,  22.,   8.,  16.,  10.,
           -23.,  -2.,  -3.,  24.,   4.,  12.,  -1., -20.,  -9.,   7.,
           -14., -21.,   4.,  12.,   5.,  10.,   0., -17.,  -7., -10.,
             4.,   9., -19.,   1.,  15., -12.,  11.,   9.,  -7.,  -4.,
            -7.,  -2.,   9.,   7.,   7.,   0.,  -8.,  -6.,   1.,  -3.,
            -4.,   2.,   2.,   1.,  -5.,   3.,  -2.,   6.,   4.,  -4.,
             3.,   0.,   1.,  -2.,   3.,   3.,   3.,  -1.,   0.,  -6.},
        { -29615., -1728.,  5186.,
           -2267.,  3072., -2478.,  1672.,  -458.,  1341., -2290.,
          -227.,1253., 296., 715.,-492., 935., 787., 272., 251.,-232.,
          -405., 119., 110.,-304.,-217., 351.,  44., 222., 172.,-131.,
          -134.,-169., -40., -12., 107.,  72.,  68., -17.,  74.,  64.,
          -161.,  65.,  -5., -61.,  17.,   1., -91.,  44.,  79., -74.,
           -65.,   0., -24.,  33.,   6.,   9.,  24.,   7.,  15.,   8.,
           -25.,  -2.,  -6.,  25.,   6.,  12.,  -9., -22.,  -8.,   8.,
           -17., -21.,   9.,  15.,   7.,   9.,  -8., -16.,  -7.,  -3.,
             5.,   9., -20.,   3.,  13.,  -8.,  12.,   6.,  -6.,  -9.,
            -8.,  -2.,   9.,   9.,   4.,  -4.,  -8.,  -8.,   5.,  -2.,
            -6.,   1.,   2.,   0.,  -3.,   4.,   0.,   5.,   4.,  -6.,
             1.,  -1.,   2.,  -3.,   4.,   0.,   0.,  -2.,  -1.,  -8.} };

static float sv[7][80] = {
        {  15.3,  8.7, -2.3,-24.4,  0.3,-11.8, -1.6,-16.7,  0.2,-10.8,
            4.2,  0.7,  0.7, -3.8, -7.7, -0.7,  0.2, -0.1, -3.0,  1.6,
           -0.1,  2.9, -2.1, -4.2,  1.9,  1.1,  2.3,  2.9,  1.7,  0.6,
           -2.4,  0.0,  0.8,  1.3, -0.3, -0.1, -0.3, -0.9,  1.1, -0.4,
            1.9,  2.0, -0.4, -1.1, -0.4,  0.1, -0.2,  0.9, -0.5, -0.3,
           -1.1, -0.7,  0.3, -0.5,  0.4,  0.3,  0.2,  0.0,  0.4, -0.2,
            0.2, -0.6,  0.3,  0.1,  0.4,  0.1,  0.6, -0.2,  0.0, -0.3,
            0.0, -0.2, -0.1, -0.3,  0.3, -0.4, -0.3, -0.3, -0.5, -0.3},
        {  25.6, 10.0,-10.2,-24.9,  0.7, -3.0,  4.3,-18.9, -3.8,-10.4,
            6.9, -4.1,  2.5, -4.2, -5.0, -0.2, -2.0,  5.0, -3.9,  0.8,
           -2.1,  1.7, -3.1, -1.0,  0.3, -0.7,  1.2,  1.1,  2.3, -1.6,
           -2.0, -0.5,  1.3,  1.0,  1.1,  0.2,  0.5, -0.5,  2.0, -0.1,
            2.8, -0.2,  0.0, -1.3,  0.9,  0.7, -0.1,  1.7,  0.0,  0.0,
           -1.4,  0.0, -0.1,  0.6,  0.3,  0.9,  0.3,  0.3, -0.7,  0.3,
            0.1, -0.5,  0.8,  0.2,  0.3, -0.2,  0.0, -0.4,  0.2, -0.2,
           -0.4, -0.3, -0.3,  0.4,  0.6, -0.3, -0.3, -0.6, -0.1,  0.3},
        {  22.4, 11.3,-15.9,-18.3,  3.2,-12.7,  7.0,-25.2,  0.0, -6.5,
            0.2, -0.7,  2.7,  1.0, -7.9, -1.4, -1.4,  4.6, -8.2,  1.6,
           -1.8,  2.9, -5.0,  0.4,  1.5,  0.4,  1.8, -0.8, -0.4, -3.3,
            0.0,  0.2,  1.3,  1.4,  2.1,  0.4,  0.0, -0.5,  3.4, -1.4,
            0.8,  0.0,  0.8, -1.6,  0.3,  0.5, -0.1,  0.0, -1.0, -0.8,
           -0.4,  0.4,  0.4,  0.5,  0.2,  1.6,  1.4,  0.1, -0.5,  0.1,
           -0.1,  0.0,  1.1,  0.8, -0.2, -0.1, -0.3, -0.7,  0.3,  0.0,
           -0.8, -0.8, -0.2,  0.2,  0.7,  0.2, -0.3, -1.1,  1.2,  0.8},
        {  23.2, 10.0,-24.5,-13.7,  3.4,-11.5,  7.0,-20.2,  5.1, -4.6,
            5.3, -0.6,  2.3,  0.1,-10.8,  0.1, -0.6,  3.8, -7.8,  2.2,
           -1.4,  2.5, -6.8,  0.9,  1.3,  0.1,  0.1, -1.5, -0.2, -3.2,
           -0.1,  0.1,  0.6, -0.1,  0.0,  1.4, -0.3, -0.4,  1.7, -1.1,
            0.6, -0.8,  0.0, -2.3,  0.9, -0.5,  1.2, -0.1,  0.2, -0.6,
            0.2, -0.5,  1.0,  0.8,  1.1,  1.0,  1.9,  0.4,  0.3, -0.5,
            0.2, -0.1,  0.9,  0.7,  0.0,  0.1,  0.3, -1.0,  0.4,  0.1,
           -0.3, -0.8, -0.3,  0.2,  0.1, -0.8, -0.5, -0.1, -0.8,  1.3},
        {  18.0, 10.6,-16.1,-12.9,  2.4,-15.8,  0.0,-13.8,  3.3, -6.7,
            4.4,  0.1,  1.6, -5.9,-10.6,  0.5,  0.6,  2.6, -7.0,  1.8,
            0.5,  3.1, -5.5, -1.4,  0.6, -0.1, -0.1, -1.6,  0.5, -3.1,
            0.4, -0.1,  1.7,  2.3,  0.4,  1.3, -0.2,  0.2,  1.8, -1.3,
            1.3,  0.0, -0.2, -0.9,  0.1,  0.5,  1.2,  1.2,  0.6, -0.5,
            0.6, -0.3,  0.2,  0.6,  0.8,  1.6, -0.5,  0.2, -0.2,  0.2,
            0.0,  0.3,  0.0,  0.2, -0.7,  0.5, -0.2, -0.2,  0.1,  0.3,
           -1.1,  0.3,  0.0,  0.4, -0.1, -0.5, -0.5, -0.3, -0.6,  0.6},
        {  17.6, 13.0,-18.3,-13.2,  3.7,-15.0, -0.8, -8.8,  1.5, -6.4,
            4.1, -0.2,  2.2, -8.1,-12.1,  0.8,  0.9,  1.8, -6.9,  1.2,
            0.5,  2.7, -4.6, -1.0,  0.8,  0.1,  0.2, -1.5,  1.2, -2.0,
            0.3, -0.1,  1.8,  2.3,  0.9,  0.5, -0.4,  0.3,  0.6, -1.6,
            1.9, -0.2, -0.2, -0.9, -0.2,  1.0,  0.0,  2.2, -0.2, -0.8,
            0.8, -0.6,  0.2,  0.6,  0.6,  1.2, -0.4,  0.1,  0.0,  0.2,
           -0.3, -0.6,  0.0,  0.3, -0.2,  0.4,  0.1, -0.2,  0.4,  0.2,
           -1.1,  0.7,  0.3,  0.0,  0.2, -1.2, -0.9, -0.7, -0.3, -0.6},
        {  14.6, 10.7,-22.5,-12.4,  1.1,-20.6, -1.1, -9.6,  0.7, -5.4,
            6.0,  0.9, -0.1, -7.7,-14.2, -1.3,  1.6,  2.1, -7.3,  1.3,
            2.9,  5.0, -3.2,  0.3,  0.0, -0.7, -0.1, -2.1,  0.6, -2.8,
            1.7, -0.8,  1.9,  2.5,  0.1,  1.0, -0.4, -0.2,  0.9, -1.4,
            2.0,  0.0, -0.6, -0.8, -0.3,  0.0,  1.2,  0.9, -0.4, -0.4,
            1.1, -0.3,  0.0,  1.1,  0.3,  1.1, -0.1, -0.2, -0.6,  0.6,
           -0.7, -0.9,  0.2, -0.3,  0.2,  0.1, -0.3,  0.0,  0.4,  0.0,
           -1.0,  0.3,  0.3,  0.6, -0.5, -0.4, -0.7,  0.3, -0.4,  0.7} };


void sigrf( void )
{   int    maxod_tmp, i, n, m, l;
    double tzero_tmp, dmy[ 3 ];

    l     =  6;
    maxod_tmp = 10;
    tzero_tmp = 2000.0;

    for( i = 0, n = 1; n <= maxod_tmp; ++n)
    {
        vgh[ 0 ][ n ] = rf[ l ][ i ];

        if( n <= 8 )
        {
            vght[ 0 ][ n ] = sv[ l ][ i ];
        }
        else
        {
            vght[ 0 ][ n ] = 0.0 ;
        }

/* fprintf( stdout, "vght[ 0 ][ %d ] = %f\n", n, vght[ 0 ][ n ] ); */
/* fflush( stdout ); */

        ++i;

        for( m = 1; m <= n; ++m )
        {
            vgh[ m ][ n ] = rf[ l ][ i ];

            vght[ m ][ n ] = 0.0;
            if( n <= 8 )
            {
                vght[ m ][ n ] = sv[ l ][ i ];
            }

/* fprintf( stdout, "vght[ %d ][ %d ] = %f\n", m, n, vght[ m ][ n ] ); */
/* fflush( stdout ); */

            ++i;

            vgh[ n ][ m - 1 ] = rf[ l ][ i ];

            vght[ n ][ m - 1 ] = 0.0;
            if( n <= 8 )
            {
                vght[ n ][ m - 1 ] = sv[ l ][ i ];
            }

/* fprintf( stdout, "vght[ %d ][ %d ] = %f\n", n, ( m - 1 ), vght[ n ][ m - 1 ] ); */
/* fflush( stdout ); */

            ++i;
        }
    }

    field( 6378.16, 298.25, 6371.2, maxod_tmp );
    tcoef( vgh, vght, tzero_tmp, 0, dmy );
    tyear( 2010.0 );

    return;
}

void field( double are, double aflat, double ara, int maxoda )
{
    maxod = maxoda;
 
    kg    = 2;
    kgc   = 0;
    kph   = 1;

    return;
}

void tcoef( double agh[ MxOD + 1 ][ MxOD + 1 ],
            double aght[ MxOD + 1 ][ MxOD + 1 ],
            double atzero, int kexta, double aext[ 3 ] )
{
    int    nn, mm, n;
    double fac;

    tzero = atzero;
    kext  = kexta;
    gh[ 0 ][ 0 ]  = 0.0;
    ght[ 0 ][ 0 ] = 0.0;

    for( nn = 1; nn <= maxod; ++nn)
    {
        gh[ 0 ][ nn ]  = agh[ 0 ][ nn ];
        ght[ 0 ][ nn ] = aght[ 0 ][ nn ];
        fac            = sqrt( 2.0 );

        for( mm =1; mm <= nn; ++mm)
        {
            fac /= sqrt( ( double )( ( nn + mm ) * ( nn - mm + 1 ) ) );

            gh[ mm ][ nn ]      = agh[ mm ][ nn ] * fac;
            gh[ nn ][ mm - 1 ]  = agh[ nn ][ mm - 1 ] * fac;
            ght[ mm ][ nn ]     = aght[ mm ][ nn ] * fac;
            ght[ nn ][ mm - 1 ] = aght[ nn ][ mm - 1 ] * fac;
        }
    }

    if( kext == 0)
    {
        ext0 = 0.0;
        ext1 = 0.0;
        ext2 = 0.0;
    }
    else
    {
        ext0 = aext[ 0 ];
        ext1 = aext[ 1 ];
        ext2 = aext[ 2 ];
    }

    return;
}

void tyear( double ayear )
{
    double dyear;
    int    nn, mm;

    dyear = ayear - tzero;

    for( nn = 0; nn <= maxod; ++nn)
    {
        for( mm = 0; mm <= maxod; ++mm )
        {
            g[ mm ][ nn ] = gh[ mm ][ nn ] + ght[ mm ][ nn ] * dyear;
        }
    }

    return;
}

void igrfc( double fido, double fkeido, double hght, double *tf )
{
    double fx, fy, fz;
    mfldg( fido, fkeido, &fx, &fy, &fz, tf );

    fprintf( stdout, "[igrfc] x=%f, y=%f, z=%f, f=%f\n", fx, fy, fz, *tf );
    fflush( stdout );

    return;
}

void mfldg( double alat, double alon,
            double *ax, double *ay, double *az, double *af )
{
    double rm2, rm, rrm;

    if( ( kg != 1 ) || ( blat != alat ) )
    {
        kg    = 1;
        kr    = 1;
        kth   = 1;

        blat  = alat;
        rlat  = alat / URAD;
        slat  = sin( rlat );
        slat2 = slat * slat;
        clat2 = 1.0 - slat2;
        rm2   = RE2 * clat2 + RP2 * slat2;
        rm    = sqrt( rm2 );
        rrm   = ( RE4 * clat2 + RP4 * slat2 ) / rm2;
        r     = sqrt( rrm );
        cth   = slat * ( RP2 / rm ) / r;
        sth   = sqrt( 1.0 - cth * cth );
    }

    if( blon != alon )
    {
        kph = 1;
    }

    if( kph != 0 )
    {
        blon = alon;
        phi  = alon / URAD;
        cph  = cos( phi );
        sph  = sin( phi );
    }

    fcalc( ax, ay, az, af );

/*    *ax = x; */
/*    *ay = y; */
/*    *az = z; */
/*    *af = f; */
    x = *ax;
    y = *ay;
    z = *az;
    f = *af;

    return;
}

/**
 * This is an internal function
 */
static void fcalc( double *dpX, double *dpY, double *dpZ, double *dpF )
{
    double dT;
    double dPn1m;
    double dTx;
    double dTy;
    double dTz;
    int    iCnt, iInd;
    double dP[ MxOD + 2 ][ MxOD + 1 ];
    double dCsp[ MxOD + 1 ];
    double dSnp[ MxOD + 1 ];
    double dRar[ MxOD + 1 ];


    memset( dP, 0x00, sizeof( dP ) );
    memset( dCsp, 0x00, sizeof( dCsp ) );
    memset( dSnp, 0x00, sizeof( dSnp ) );
    memset( dRar, 0x00, sizeof( dRar ) );


    /* このとき、krは1 */
    dT        = RA / r;
    dRar[ 0 ] = dT * dT;

    for( iCnt = 0; iCnt < maxod; ++iCnt )
    {
        dRar[ iCnt + 1 ] = dRar[ iCnt ] * dT;
    }

    /* このとき、kthは1 */
    kth = 0;
    dP[ 0 ][ 0 ] = 1.0;
    dP[ 1 ][ 0 ] = 0.0;
    dP[ 0 ][ 1 ] = cth;
    dP[ 1 ][ 1 ] = sth;
    dP[ 2 ][ 0 ] = -sth;
    dP[ 2 ][ 1 ] = cth;

    for( iCnt = 1; iCnt < maxod; ++iCnt)
    {
        dP[ 0 ][ iCnt + 1 ] = ( dP[ 0 ][ iCnt ] * cth * ( iCnt + iCnt + 1 ) - dP[ 0 ][ iCnt - 1 ] * iCnt )
                          / ( iCnt + 1 );
        dP[ iCnt + 2 ][ 0 ] = ( dP[ 0 ][ iCnt + 1 ] * cth - dP[ 0 ][ iCnt ] ) * ( iCnt + 1 ) / sth;

        for( iInd = 0; iInd <= iCnt; ++iInd)
        {
            dPn1m = dP[ iInd ][ iCnt + 1 ];
            dP[ iInd + 1 ][ iCnt + 1 ] = ( dP[ iInd ][ iCnt ] * ( iCnt + iInd + 1 )
                                  - dPn1m * cth * ( iCnt - iInd + 1 ) ) / sth;
            dP[ iCnt + 2 ][ iInd + 1 ] = dPn1m * ( iCnt + iInd + 2 ) * ( iCnt - iInd + 1 )
                                  - dP[ iInd + 1 ][ iCnt + 1 ] * cth * ( iInd + 1 ) / sth;
        }
    }


    kph = 0;
    dCsp[ 0 ] = 1.0;
    dSnp[ 0 ] = 0.0;

    for( iInd = 0; iInd < maxod; ++iInd)
    {
        dCsp[ iInd + 1 ] = dCsp[ iInd ] * cph - dSnp[ iInd ] * sph;
        dSnp[ iInd + 1 ] = dSnp[ iInd ] * cph + dCsp[ iInd ] * sph;
    }



    *dpX = 0.0;
    *dpY = 0.0;
    *dpZ = 0.0;

    for( iCnt = 0; iCnt < maxod; ++iCnt)
    {
        dTx = g[ 0 ][ iCnt + 1 ] * dP[ iCnt + 2 ][ 0 ];
        dTy = 0.0;
        dTz = g[ 0 ][ iCnt + 1 ] * dP[ 0 ][ iCnt + 1 ];

        for( iInd = 0; iInd <= iCnt; ++iInd )
        {
            dTx += ( g[ iInd + 1 ][ iCnt + 1 ] * dCsp[ iInd + 1 ]
                  + g[ iCnt + 1 ][ iInd ] * dSnp[ iInd + 1 ] ) * dP[ iCnt + 2 ][ iInd + 1 ];
            dTy += ( g[ iInd + 1 ][ iCnt + 1 ] * dSnp[ iInd + 1 ]
                  - g[ iCnt + 1 ][ iInd ] * dCsp[ iInd + 1 ] ) * dP[ iInd + 1 ][ iCnt + 1] * ( iInd + 1 );
            dTz += ( g[ iInd + 1 ][ iCnt + 1 ] * dCsp[ iInd + 1 ]
                  + g[ iCnt + 1 ][ iInd ] * dSnp[ iInd + 1 ] ) * dP[ iInd + 1][ iCnt + 1 ];
        }

        *dpX += dRar[ iCnt + 1 ] * dTx;
        *dpY += dRar[ iCnt + 1 ] * dTy;
        *dpZ -= dRar[ iCnt + 1 ] * dTz *( iCnt + 2 );
    }

    *dpY /= sth;

    if( kext != 0 )
    {
        dT  = ext1 * cph + ext2 * sph;
        *dpX -= ( ext0 * cth + dT * sth );
        *dpY += ( ext1 * sph - ext2 * cph );
        *dpZ += ( ext0 * sth - dT * cth );
    }

    *dpF = sqrt( *dpX * *dpX + *dpY * *dpY + *dpZ * *dpZ );

    return;
}

void gcomp( double *axg, double *ayg, double *azg )
{
    double bthc, rc, rs, rep2, tlat, tlat2, rlatp, rmc, rmc2, rmc3, ffp, ff;
    double cga, sga, xg, zg;

    if( kg == 1 )
    {
        kgc  = 0;
        clat = sqrt( clat2 );
    }
#if 0
    else if( ( kgc == 0 ) || ( bthc != bthe ) )
    {
        kgc  = 1;
        bthc = bthe;
        slat = cth;
        clat = sth;

        if( clat > 1.5e-9 )
        {
            rc    = r * cth;
            rs    = r * sth;
            rep2  = RE2 - RP2;
            tlat  = slat / clat;
            tlat2 = tlat * tlat;
            rlat  = atan( tlat );

            do{
                rlatp  = rlat;
                rmc2   = RE2 + RP2 * tlat2;
                rmc    = sqrt( rmc2 );
                rmc3   = rmc2 * rmc;
                ffp    = RP2 * rep2 * tlat / rmc3 + rc / tlat2;
                ff     = rep2 / rmc + rc / tlat - rs;
                tlat  += ff / ffp;
                tlat2  = tlat * tlat;
                rlat   = atan( tlat );

            } while( fabs( rlat - rlatp ) > 1.5e-9 );

            clat2 = 1.0 / ( 1.0 + tlat2 );
            clat  = sqrt( clat2 );
            slat  = tlat * clat;
        }
    }
#endif

    cga  = cth * slat + sth * clat;
    sga  = cth * clat - sth * slat;
    xg   = x * cga - z * sga;
    zg   = x * sga + z * cga;
    *axg = xg;
    *ayg = y;
    *azg = zg;

    return;
}

int debug( void )
{
    return 0;
}

