#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>
#include  <math.h>
#include  "t.h"

#define MxOD    ( 19 )
#define URAD    ( 180.0 / 3.14159265359 )

static double tzero;
static int    kext;
static double ra;
static double re;
static double re2;
static double re4;
static double rp;
static double rp2;
static double rp4;
static double rpre;
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
static double vgh[ MxOD + 1 ][ MxOD + 1 ];
static double vght[ MxOD + 1 ][ MxOD + 1 ];
static double p[ MxOD + 2 ][ MxOD + 1 ];
static double rar[ MxOD + 1 ];
static double csp[ MxOD + 1 ];
static double snp[ MxOD + 1 ];

static double blat;
static double blon;
static double bhi;
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
void mfldg( double alat, double alon, double ahi,
            double *ax, double *ay, double *az, double *af );
static void fcalc( void );

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
{   int    maxod, i, n, m, l;
    double tzero, dmy[ 3 ];

    l     =  6;
    maxod = 10;
    tzero = 2000.0;

    for( i = 0, n = 1; n <= maxod; ++n)
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

        ++i;

        for( m = 1; m <= n; ++m )
        {
            vgh[ m ][ n ] = rf[ l ][ i ];

            vght[ m ][ n ] = 0.0;
            if( n <= 8 )
            {
                vght[ m ][ n ] = sv[ l ][ i ];
            }

            ++i;

            vgh[ n ][ m - 1 ] = rf[ l ][ i ];

            vght[ n ][ m - 1 ] = 0.0;
            if( n <= 8 )
            {
                vght[ n ][ m - 1 ] = sv[ l ][ i ];
            }

            ++i;
        }
    }

    field( 6378.16, 298.25, 6371.2, maxod );
    tcoef( vgh, vght, tzero, 0, dmy );
    tyear( 2010.0 );

    return;
}

void field( double are, double aflat, double ara, int maxoda )
{
    ra    = ara;
    maxod = maxoda;
    rpre  = 1.0 - 1.0 / aflat;
    re    = are;
    re2   = re * re;
    re4   = re2 * re2;
    rp    = re * rpre;
    rp2   = rp * rp;
    rp4   = rp2 * rp2;
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
    mfldg( fido, fkeido, hght / 1000.0, &fx, &fy, &fz, tf );

    return;
}

void mfldg( double alat, double alon, double ahi,
            double *ax, double *ay, double *az, double *af )
{
    double hi, rm2, rm, rrm;

    if( ( kg != 1 ) || ( blat != alat ) || ( bhi != ahi ) )
    {
        kg    = 1;
        kr    = 1;
        kth   = 1;
        blat  = alat;
        bhi   = ahi;
        rlat  = alat / URAD;
        hi    = ahi;
        slat  = sin( rlat );
        slat2 = slat * slat;
        clat2 = 1.0 - slat2;
        rm2   = re2 * clat2 + rp2 * slat2;
        rm    = sqrt( rm2 );
        rrm   = ( re4 * clat2 + rp4 * slat2 ) / rm2;
        r     = sqrt( rrm + 2.0 * hi * rm + hi * hi );
        cth   = slat * ( hi + rp2 / rm ) / r;
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

    fcalc();

    *ax = x;
    *ay = y;
    *az = z;
    *af = f;

    return;
}

/**
 * This is an internal function
 */
static void fcalc( void )
{
    double t, pn1m, tx, ty, tz; int n, m;

    if( kr != 0 )
    {
        kr       = 0;
        t        = ra / r;
        rar[ 0 ] = t * t;

        for( n = 0; n < maxod; ++n )
        {
            rar[ n + 1 ] = rar[ n ] * t;
        }
    }

    if( kth != 0 )
    {
        kth = 0;
        p[ 0 ][ 0 ] = 1.0;
        p[ 1 ][ 0 ] = 0.0;
        p[ 0 ][ 1 ] = cth;
        p[ 1 ][ 1 ] = sth;
        p[ 2 ][ 0 ] = -sth;
        p[ 2 ][ 1 ] = cth;

        for( n = 1; n < maxod; ++n)
        {
            p[ 0 ][ n + 1 ] = ( p[ 0 ][ n ] * cth * ( n + n + 1 ) - p[ 0 ][ n - 1 ] * n )
                              / ( n + 1 );
            p[ n + 2 ][ 0 ] = ( p[ 0 ][ n + 1 ] * cth - p[ 0 ][ n ] ) * ( n + 1 ) / sth;

            for( m = 0; m <= n; ++m)
            {
                pn1m = p[ m ][ n + 1 ];
                p[ m + 1 ][ n + 1 ] = ( p[ m ][ n ] * ( n + m + 1 )
                                      - pn1m * cth * ( n - m + 1 ) ) / sth;
                p[ n + 2 ][ m + 1 ] = pn1m * ( n + m + 2 ) * ( n - m + 1 )
                                      - p[ m + 1 ][ n + 1 ] * cth * ( m + 1 ) / sth;
            }
        }
    }

    if( kph != 0 )
    {
        kph = 0;
        csp[ 0 ] = 1.0;
        snp[ 0 ] = 0.0;

        for( m = 0; m < maxod; ++m)
        {
            csp[ m + 1 ] = csp[ m ] * cph - snp[ m ] * sph;
            snp[ m + 1 ] = snp[ m ] * cph + csp[ m ] * sph;
        }
    }

    x = 0.0;
    y = 0.0;
    z = 0.0;

    for( n = 0; n < maxod; ++n)
    {
        tx = g[ 0 ][ n + 1 ] * p[ n + 2 ][ 0 ];
        ty = 0.0;
        tz = g[ 0 ][ n + 1 ] * p[ 0 ][ n + 1 ];

        for( m = 0; m <= n; ++m )
        {
            tx += ( g[ m + 1 ][ n + 1 ] * csp[ m + 1 ]
                  + g[ n + 1 ][ m ] * snp[ m + 1 ] ) * p[ n + 2 ][ m + 1 ];
            ty += ( g[ m + 1 ][ n + 1 ] * snp[ m + 1 ]
                  - g[ n + 1 ][ m ] * csp[ m + 1 ] ) * p[ m + 1 ][ n + 1] * ( m + 1 );
            tz += ( g[ m + 1 ][ n + 1 ] * csp[ m + 1 ]
                  + g[ n + 1 ][ m ] * snp[ m + 1 ] ) * p[ m + 1][ n + 1 ];
        }

        x += rar[ n + 1 ] * tx;
        y += rar[ n + 1 ] * ty;
        z -= rar[ n + 1 ] * tz *( n + 2 );
    }

    y /= sth;

    if( kext != 0 )
    {
        t  = ext1 * cph + ext2 * sph;
        x -= ( ext0 * cth + t * sth );
        y += ( ext1 * sph - ext2 * cph );
        z += ( ext0 * sth - t * cth );
    }

    f = sqrt( x * x + y * y + z * z );
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
            rep2  = re2 - rp2;
            tlat  = slat / clat;
            tlat2 = tlat * tlat;
            rlat  = atan( tlat );

            do{
                rlatp  = rlat;
                rmc2   = re2 + rp2 * tlat2;
                rmc    = sqrt( rmc2 );
                rmc3   = rmc2 * rmc;
                ffp    = rp2 * rep2 * tlat / rmc3 + rc / tlat2;
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

    cga  = cth * slat + sth * clat;
    sga  = cth * clat - sth * slat;
    xg   = x * cga - z * sga;
    zg   = x * sga + z * cga;
    *axg = xg;
    *ayg = y;
    *azg = zg;

    return;
}
