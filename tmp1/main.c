#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>

#include  <stdarg.h>

#define   CA_EXTERN

#include  "CA.h"

void tst1( void );
int  tst2( void );

int main( int argc, char *argv[] )
{

#if 1

    /* tst1(); */
    /* tst2(); */
    tst3();
#else
    int          iRet;
    STR_LATLON   tP1;
    STR_LATLON   tP2;
    STR_5DEGREES tDeg;


    tP1.dLat = 0.0;
    tP1.dLon = 0.0;

    tP2.dLat = 30.0;
    tP2.dLon = 10.0;

    memset( &tDeg, 0x00, sizeof( tDeg ) );

    iRet = COMN_Get5Degrees_NS( &tP1, &tP2, &tDeg );
#endif

    return 0;
}

#define TST_CALL( M, X, ... )    iRet = X( __VA_ARGS__ );                             \
                                 if( iRet != 0 )                                      \
                                 {                                                    \
                                     CA_ALOG_ERROR( "%s 処理異常 Ret=%d", M, iRet );  \
                                     goto LABEL_END;                                  \
                                 }

void tst1( void )
{
    int        iRet;
    int        iRetValue;

    STR_LATLON ll_1_1;
    STR_LATLON ll_1_2;
    STR_LATLON ll_2_1;
    STR_LATLON ll_2_2;
    STR_LATLON ll_2_3;

    STR_POSXYZ p_0;
    STR_POSXYZ p_1_1;
    STR_POSXYZ p_1_2;
    STR_POSXYZ p_2_1;
    STR_POSXYZ p_2_2;
    STR_POSXYZ p_2_3;

    STR_PLANE  f1;
    STR_PLANE  f2;

    STR_LINE   line;

    STR_POSXYZ c1, c2;

    STR_LATLON cl1;
    STR_LATLON cl2;

    ll_1_1.dLat =  0.0;
    ll_1_1.dLon =  0.0;

    ll_1_2.dLat = 30.0;
    ll_1_2.dLon = 30.0;


    p_0.dX = 0.0;
    p_0.dY = 0.0;
    p_0.dZ = 0.0;

    TST_CALL( "XYZ変換処理", COMN_CA_ConvLatlonToXyz, &ll_1_1, &p_1_1 );

    TST_CALL( "XYZ変換処理", COMN_CA_ConvLatlonToXyz, &ll_1_2, &p_1_2 );

    TST_CALL( "平面生成処理", COMN_CA_CalcPlane, &p_0, &p_1_1, &p_1_2, &f1 );

    CA_ALOG_DBGUT1_TRACE( "f1 a=%f", f1.dA );
    CA_ALOG_DBGUT1_TRACE( "   b=%f", f1.dB );
    CA_ALOG_DBGUT1_TRACE( "   c=%f", f1.dC );
    CA_ALOG_DBGUT1_TRACE( "   d=%f", f1.dD );


    ll_2_1.dLat = 25.0;
    ll_2_1.dLon =  0.0;

    ll_2_2.dLat = 25.0;
    ll_2_2.dLon = 90.0;

    ll_2_3.dLat = 25.0;
    ll_2_3.dLon = -90.0;

    TST_CALL( "XYZ変換処理", COMN_CA_ConvLatlonToXyz, &ll_2_1, &p_2_1 );

    TST_CALL( "XYZ変換処理", COMN_CA_ConvLatlonToXyz, &ll_2_2, &p_2_2 );

    TST_CALL( "XYZ変換処理", COMN_CA_ConvLatlonToXyz, &ll_2_3, &p_2_3 );

    TST_CALL( "平面生成処理", COMN_CA_CalcPlane, &p_2_1, &p_2_2, &p_2_3, &f2 );

    CA_ALOG_DBGUT1_TRACE( "f2 a=%f", f2.dA );
    CA_ALOG_DBGUT1_TRACE( "   b=%f", f2.dB );
    CA_ALOG_DBGUT1_TRACE( "   c=%f", f2.dC );
    CA_ALOG_DBGUT1_TRACE( "   d=%f", f2.dD );


    TST_CALL( "交線算出処理", COMN_CA_CalcCrossLine, &f1, &f2, &line );


    TST_CALL( "交点算出処理", COMN_CA_CalcCrossPoint, DEF_A, DEF_B, &line, &c1, &c2 );

    CA_ALOG_DBGUT1_TRACE( "交点 1  x=%f", c1.dX );
    CA_ALOG_DBGUT1_TRACE( "        y=%f", c1.dY );
    CA_ALOG_DBGUT1_TRACE( "        z=%f", c1.dZ );

    CA_ALOG_DBGUT1_TRACE( "交点 2  x=%f", c2.dX );
    CA_ALOG_DBGUT1_TRACE( "        y=%f", c2.dY );
    CA_ALOG_DBGUT1_TRACE( "        z=%f", c2.dZ );



    TST_CALL( "緯度経度算出処理", COMN_CA_ConvXyzToLatlon, &c1, &cl1 );

    TST_CALL( "緯度経度算出処理", COMN_CA_ConvXyzToLatlon, &c2, &cl2 );

    CA_ALOG_DBGUT1_TRACE( "c1 緯度=%f", cl1.dLat );
    CA_ALOG_DBGUT1_TRACE( "   経度=%f", cl1.dLon );
    CA_ALOG_DBGUT1_TRACE( "c2 緯度=%f", cl2.dLat );
    CA_ALOG_DBGUT1_TRACE( "   経度=%f", cl2.dLon );

LABEL_END:

    return;
}

int tst2( void )
{
    int  iRet;
    int  iRetValue;

    int  ido;
    int  keido;

    STR_LATLON ll, ll2;
    STR_POSXYZ p;

    for( ido = -85; ido <= 85; ido += 5 )
    {
    for( keido = -180; keido <= 180; keido += 5 )
    {
        ll.dLat = ( double )ido;
        ll.dLon = ( double )keido;

        TST_CALL( "XYZ変換処理",      COMN_CA_ConvLatlonToXyz, &ll, &p );

        TST_CALL( "緯度経度算出処理", COMN_CA_ConvXyzToLatlon, &p, &ll2 );

#if 1
        CA_ALOG_DBGUT1_TRACE( "[%f / %f ] -> [ %f / %f ]",
                              ll.dLat, ll.dLon,
                              ll2.dLat, ll2.dLon );
#else
        CA_ALOG_DBGUT1_TRACE( "[%f / %f ] -> ( %f, %f, %f ) -> [ %f / %f ]",
                              ll.dLat, ll.dLon,
                              p.dX, p.dY, p.dZ,
                              ll2.dLat, ll2.dLon );
#endif
    }
    }

LABEL_END:

    return iRetValue;
}

int tst3( void )
{
    int        iRet;
    int        iRetValue;
    double     dD;
    STR_LATLON l1, l2;

    l1.dLat = 0.0;
    l1.dLon = 0.0;
    l2.dLat = 0.0;
    l2.dLon = 0.0 + 0.0 / 60.0 + 1.0 / 3600.0;

    TST_CALL( "距離算出", COMN_CA_CalcDistance, &l1, &l2, &dD );
    CA_ALOG_DBGUT1_TRACE( "距離 = %f", dD );

    l1.dLat = 89.0 + 59.0 / 60.0 + 59.0 / 3600.0;
    l1.dLon =  0.0;
    l2.dLat = 89.0 + 59.0 / 60.0 + 59.0 / 3600.0;
    l2.dLon =  0.0 +  0.0 / 60.0 +  1.0 / 3600.0;

    TST_CALL( "距離算出", COMN_CA_CalcDistance, &l1, &l2, &dD );
    CA_ALOG_DBGUT1_TRACE( "距離 = %f", dD );

    iRetValue = 0;

LABEL_END:
    return iRetValue;
}



