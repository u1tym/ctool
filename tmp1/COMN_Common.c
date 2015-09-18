#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>
#include  <math.h>

#include  "CA.h"

int COMN_CA_ConvLatlonToXyz( STR_LATLON *tpLatlon,
                             STR_POSXYZ *tpPos )
{
    double f;
    double a;
    double b;
    double e;

    double phi;
    double lambda;

    double n;

    double x;
    double y;
    double z;

    f = DEF_OBLATENESS;
    a = DEF_EQUATORIAL_RADIUS;
    b = ( 1.0 - f ) * a;
    e = sqrt( pow( a, 2 ) - pow( b, 2 ) ) / a;

    phi    = CALC_DEG_TO_RAD( tpLatlon->dLat );
    lambda = CALC_DEG_TO_RAD( tpLatlon->dLon );

    n = a / ( 1 - pow( e, 2 ) * pow( sin( phi ), 2 ) );

    x = n * cos( phi ) * cos( lambda );
    y = n * cos( phi ) * sin( lambda );
    z = n * pow( b, 2 ) / pow( a, 2 ) * sin( phi );

    tpPos->dX = x;
    tpPos->dY = y;
    tpPos->dZ = z;

    return 0;
}

int COMN_CA_ConvXyzToLatlon( STR_POSXYZ *tpPos,
                             STR_LATLON *tpLatlon )
{
    double f;
    double a;
    double b;
    double e;

    double phi;
    double lambda_0;
    double lambda;

    double t;

    int    iRetValue;


    f = DEF_OBLATENESS;
    a = DEF_EQUATORIAL_RADIUS;
    b = ( 1.0 - f ) * a;
    e = sqrt( pow( a, 2 ) - pow( b, 2 ) ) / a;

    if( tpPos->dX == 0.0 && tpPos->dY >= 0.0 )
    {
        lambda = DEF_PI / 2.0;
    }
    else if( tpPos->dX == 0.0 && tpPos->dY < 0.0 )
    {
        lambda = DEF_PI / 2.0 * ( -1.0 );
    }
    else
    {
        lambda_0 = atan( tpPos->dY / tpPos->dX );

        if( tpPos->dX >= 0.0 )
        {
            lambda = lambda_0;
        }
        else
        {
            if( tpPos->dY >= 0.0 )
            {
                lambda = lambda_0 + DEF_PI;
            }
            else
            {
                lambda = lambda_0 - DEF_PI;
            }
        }
    }

    if( tpPos->dX == 0.0 && tpPos->dY == 0.0 )
    {
        if( tpPos->dZ > 0.0 )
        {
            phi = DEF_PI / 2.0;
        }
        else if( tpPos->dZ < 0.0 )
        {
            phi = DEF_PI / 2.0 * ( -1.0 );
        }
        else
        {
            iRetValue = -1;
            goto LABEL_END;
        }
    }
    else
    {
        t = pow( a, 2 ) * tpPos->dZ
          / ( pow( b, 2 ) * sqrt( pow( tpPos->dX, 2 ) + pow( tpPos->dY, 2 ) ) );

        phi = atan( t );
    }


    tpLatlon->dLat = CALC_RAD_TO_DEG( phi );
    tpLatlon->dLon = CALC_RAD_TO_DEG( lambda );

    iRetValue = 0;

LABEL_END:

    return iRetValue;
}

int COMN_CA_CalcPlane( STR_POSXYZ *tpP1,
                       STR_POSXYZ *tpP2,
                       STR_POSXYZ *tpP3,
                       STR_PLANE  *tpPlane )
{
    double a;
    double b;
    double c;
    double d;

    a = ( tpP2->dY - tpP1->dY ) * ( tpP3->dZ - tpP1->dZ )
      - ( tpP3->dY - tpP1->dY ) * ( tpP2->dZ - tpP1->dZ );
    b = ( tpP2->dZ - tpP1->dZ ) * ( tpP3->dX - tpP1->dX )
      - ( tpP3->dZ - tpP1->dZ ) * ( tpP2->dX - tpP1->dX );
    c = ( tpP2->dX - tpP1->dX ) * ( tpP3->dY - tpP1->dY )
      - ( tpP3->dX - tpP1->dX ) * ( tpP2->dY - tpP1->dY );
    d = -1.0 * ( a * tpP1->dX + b * tpP1->dY + c * tpP1->dZ );

    tpPlane->dA = a;
    tpPlane->dB = b;
    tpPlane->dC = c;
    tpPlane->dD = d;

    return 0;
}

int COMN_CA_CalcCrossLine( STR_PLANE *tpF1,
                           STR_PLANE *tpF2,
                           STR_LINE  *tpLine )
{
    STR_POSXYZ tPos;
    STR_VECTOR tVec;
    int        iRetValue;


    tVec.dX = tpF1->dB * tpF2->dC - tpF1->dC * tpF2->dB;
    tVec.dY = tpF1->dC * tpF2->dA - tpF1->dA * tpF2->dC;
    tVec.dZ = tpF1->dA * tpF2->dB - tpF1->dB * tpF2->dA;

    if( tVec.dZ != 0 )
    {
        tPos.dX = ( -1.0 * tpF1->dD * tpF2->dB + tpF2->dD * tpF1->dB ) / tVec.dZ;
        tPos.dY = ( -1.0 * tpF1->dD * tpF2->dA + tpF2->dD * tpF1->dA ) / tVec.dZ;
        tPos.dZ = 0.0;
    }
    else if( tVec.dY != 0 )
    {
        tPos.dX = ( -1.0 * tpF1->dD * tpF2->dC + tpF2->dD * tpF1->dC ) / ( -1.0 * tVec.dY );
        tPos.dY = 0.0;
        tPos.dZ = ( -1.0 * tpF1->dD * tpF2->dA + tpF2->dD * tpF1->dA ) / tVec.dY;
    }
    else if( tVec.dX != 0 )
    {
        tPos.dX = 0.0;
        tPos.dY = ( -1.0 * tpF1->dD * tpF2->dC + tpF2->dD * tpF1->dC ) / tVec.dX;
        tPos.dZ = ( -1.0 * tpF1->dD * tpF2->dB + tpF2->dD * tpF1->dB ) / ( -1.0 * tVec.dX );
    }
    else
    {
        iRetValue = -1;
        goto LABEL_END;
    }

    tpLine->tPos.dX = tPos.dX;
    tpLine->tPos.dY = tPos.dY;
    tpLine->tPos.dZ = tPos.dZ;

    tpLine->tVec.dX = tVec.dX;
    tpLine->tVec.dY = tVec.dY;
    tpLine->tVec.dZ = tVec.dZ;


    iRetValue = 0;

LABEL_END:
    return iRetValue;

}

int COMN_CA_CalcCrossPoint( double     dA,
                            double     dB,
                            STR_LINE   *tpLine,
                            STR_POSXYZ *tpPos1,
                            STR_POSXYZ *tpPos2 )
{
    double k1;
    double k2;
    double k3;

    double r;
    double t1;
    double t2;

    int    iRetValue;


    k1 = pow( dB, 2 ) * pow( tpLine->tVec.dX, 2 )
       + pow( dB, 2 ) * tpLine->tVec.dY
       + pow( dA, 2 ) * pow( tpLine->tVec.dZ, 2 );
    k2 = 2.0 * ( pow( dB, 2 ) * tpLine->tVec.dX * tpLine->tPos.dX
               + pow( dB, 2 ) * tpLine->tVec.dY * tpLine->tPos.dY
               + pow( dA, 2 ) * tpLine->tVec.dZ * tpLine->tPos.dZ );
    k3 = pow( dB, 2 ) * pow( tpLine->tPos.dX, 2 )
       + pow( dB, 2 ) * pow( tpLine->tPos.dY, 2 )
       + pow( dA, 2 ) * pow( tpLine->tPos.dZ, 2 )
       - pow( dA, 2 ) * pow( dB, 2 );

    r  = pow( k2, 2 ) - 4.0 * k1 * k3;

    if( r <= 0.0 )
    {
        iRetValue = -1;
        goto LABEL_END;
    }

    t1 = ( -1.0 * k2 + sqrt( r ) ) / ( 2.0 * k1 );
    t2 = ( -1.0 * k2 - sqrt( r ) ) / ( 2.0 * k1 );

    tpPos1->dX = tpLine->tPos.dX + t1 * tpLine->tVec.dX;
    tpPos1->dY = tpLine->tPos.dY + t1 * tpLine->tVec.dY;
    tpPos1->dZ = tpLine->tPos.dZ + t1 * tpLine->tVec.dZ;

    tpPos2->dX = tpLine->tPos.dX + t2 * tpLine->tVec.dX;
    tpPos2->dY = tpLine->tPos.dY + t2 * tpLine->tVec.dY;
    tpPos2->dZ = tpLine->tPos.dZ + t2 * tpLine->tVec.dZ;

    iRetValue = 0;

LABEL_END:
    return iRetValue;
}

int COMN_CA_CalcDistance( STR_LATLON *tpP1,
                          STR_LATLON *tpP2,
                          double     *dpOutput )
{
    double f;
    double a;
    double b;

    double phi_1, lambda_1, phi_d_1;
    double phi_2, lambda_2, phi_d_2;
    double delta_lambda;

    double r;
    double C, D, P, Q;

    double result;


    f = DEF_OBLATENESS;
    a = DEF_EQUATORIAL_RADIUS;
    b = ( 1.0 - f ) * a;

    phi_1    = CALC_DEG_TO_RAD( tpP1->dLat );
    lambda_1 = CALC_DEG_TO_RAD( tpP1->dLon );

    phi_2    = CALC_DEG_TO_RAD( tpP2->dLat );
    lambda_2 = CALC_DEG_TO_RAD( tpP2->dLon );

    phi_d_1  = atan( b / a * tan( phi_1 ) );
    phi_d_2  = atan( b / a * tan( phi_2 ) );

    delta_lambda = lambda_2 - lambda_1;

    r = acos( sin( phi_d_1 ) * sin( phi_d_2 )
              + cos( phi_d_1 ) * cos( phi_d_2 ) * cos( delta_lambda ) );

    C = pow( sin( phi_d_1 ) + sin( phi_d_2 ), 2 );
    D = pow( sin( phi_d_1 ) - sin( phi_d_2 ), 2 );
    P = ( a - b ) * ( r - sin( r ) ) / ( 4.0 * ( 1.0 + cos( r ) ) );
    Q = ( a - b ) * ( r + sin( r ) ) / ( 4.0 * ( 1.0 - cos( r ) ) );

    result = a * r - C * P - D * Q;

    *dpOutput = result;

    return 0;
}

typedef struct tag_STR_STOCK_LATLON_REC
{
    double     dDistance;
    STR_LATLON tLatLon;
} STR_STOCK_LATLON_REC;

typedef struct tag_STR_STOCK_LATLON_TBL
{
    int                  iMax;
    STR_STOCK_LATLON_REC tRec[ 128 ];
} STR_STOCK_LATLON_TBL;

int COMN_Get5Degrees_NS( STR_LATLON   *tpSt,
                         STR_LATLON   *tpEd,
                         STR_5DEGREES *tp5Degrees )
{
    int        iRet;
    int        iRetValue;

    STR_POSXYZ p0;
    STR_POSXYZ p1;
    STR_POSXYZ p2;
    STR_POSXYZ q1;
    STR_POSXYZ q2;
    STR_POSXYZ q3;
    STR_POSXYZ cross1;
    STR_POSXYZ cross2;

    STR_LATLON latlon1;
    STR_LATLON latlon2;

    STR_PLANE  f0;
    STR_PLANE  ft;
    STR_LINE   l;

    double     dD;
    double     dD_1S;
    double     dD_1E;
    double     dD_2S;
    double     dD_2E;

    int        lat;

    STR_LATLON tTmp;

    STR_STOCK_LATLON_TBL tTbl;


    memset( &tTbl, 0x00, sizeof( tTbl ) );


    p0.dX = 0;
    p0.dY = 0;
    p0.dZ = 0;

    iRet = COMN_CA_ConvLatlonToXyz( tpSt, &p1 );
    if( iRet != 0 )
    {
        CA_ALOG_ERROR( "XYZ変換異常" );
        iRetValue = -1;
        goto LABEL_END;
    }

    iRet = COMN_CA_ConvLatlonToXyz( tpEd, &p2 );
    if( iRet != 0 )
    {
        CA_ALOG_ERROR( "XYZ変換異常" );
        iRetValue = -1;
        goto LABEL_END;
    }

    iRet = COMN_CA_CalcPlane( &p0, &p1, &p2, &f0 );
    if( iRet != 0 )
    {
        CA_ALOG_ERROR( "PLANE生成処理異常" );
        iRetValue = -1;
        goto LABEL_END;
    }

    iRet = COMN_CA_CalcDistance( tpSt, tpEd, &dD );
    if( iRet != 0 )
    {
        CA_ALOG_ERROR( "距離算出処理異常" );
        iRetValue = -1;
        goto LABEL_END;
    }

    lat = -85;

    for( ;; )
    {
        tTmp.dLat = ( double )lat;
        tTmp.dLon = 0.0;
        iRet = COMN_CA_ConvLatlonToXyz( &tTmp, &q1 );
        if( iRet != 0 )
        {
            CA_ALOG_ERROR( "XYZ変換異常" );
            iRetValue = -1;
            goto LABEL_END;
        }

        tTmp.dLat = lat;
        tTmp.dLon = 90.0;
        iRet = COMN_CA_ConvLatlonToXyz( &tTmp, &q2 );
        if( iRet != 0 )
        {
            CA_ALOG_ERROR( "XYZ変換異常" );
            iRetValue = -1;
            goto LABEL_END;
        }

        tTmp.dLat = lat;
        tTmp.dLon = -90.0;
        iRet = COMN_CA_ConvLatlonToXyz( &tTmp, &q3 );
        if( iRet != 0 )
        {
            CA_ALOG_ERROR( "XYZ変換異常" );
            iRetValue = -1;
            goto LABEL_END;
        }

        iRet = COMN_CA_CalcPlane( &q1, &q2, &q3, &ft );
        if( iRet != 0 )
        {
            CA_ALOG_ERROR( "PLANE生成処理異常" );
            iRetValue = -1;
            goto LABEL_END;
        }

        iRet = COMN_CA_CalcCrossLine( &f0, &ft, &l );
        if( iRet != 0 )
        {
            CA_ALOG_ERROR( "CROSS-LINE生成処理異常" );
            iRetValue = -1;
            goto LABEL_END;
        }

        iRet = COMN_CA_CalcCrossPoint( DEF_A, DEF_B, &l, &cross1, &cross2 );
        if( iRet != 0 )
        {
            CA_ALOG_ERROR( "CROSS-POINT生成処理異常" );
            iRetValue = -1;
            goto LABEL_END;
        }

        iRet = COMN_CA_ConvXyzToLatlon( &cross1, &latlon1 );
        if( iRet != 0 )
        {
            CA_ALOG_ERROR( "LATLON変換処理異常" );
            iRetValue = -1;
            goto LABEL_END;
        }

        iRet = COMN_CA_ConvXyzToLatlon( &cross2, &latlon2 );
        if( iRet != 0 )
        {
            CA_ALOG_ERROR( "LATLON変換処理異常" );
            iRetValue = -1;
            goto LABEL_END;
        }


        iRet = COMN_CA_CalcDistance( &latlon1, tpSt, &dD_1S );
        if( iRet != 0 )
        {
            CA_ALOG_ERROR( "距離算出処理異常" );
            iRetValue = -1;
            goto LABEL_END;
        }

        iRet = COMN_CA_CalcDistance( &latlon1, tpEd, &dD_1E );
        if( iRet != 0 )
        {
            CA_ALOG_ERROR( "距離算出処理異常" );
            iRetValue = -1;
            goto LABEL_END;
        }

        iRet = COMN_CA_CalcDistance( &latlon2, tpSt, &dD_2S );
        if( iRet != 0 )
        {
            CA_ALOG_ERROR( "距離算出処理異常" );
            iRetValue = -1;
            goto LABEL_END;
        }

        iRet = COMN_CA_CalcDistance( &latlon2, tpEd, &dD_2E );
        if( iRet != 0 )
        {
            CA_ALOG_ERROR( "距離算出処理異常" );
            iRetValue = -1;
            goto LABEL_END;
        }


        if( dD_1S < dD && dD_1E < dD )
        {
            /* 追加 */

            tTbl.tRec[ tTbl.iMax ].dDistance = dD_1S;
            tTbl.tRec[ tTbl.iMax ].tLatLon.dLat = latlon1.dLat;
            tTbl.tRec[ tTbl.iMax ].tLatLon.dLon = latlon1.dLon;

            COMN_ALOG_DBGUT1_TRACE( "add lat=%f", latlon1.dLat );
            COMN_ALOG_DBGUT1_TRACE( "    lon=%f", latlon1.dLon );
            COMN_ALOG_DBGUT1_TRACE( "    dis=%f", dD_1S );

            ++( tTbl.iMax );
        }

        if( dD_2S < dD && dD_2E < dD )
        {
            /* 追加 */

            tTbl.tRec[ tTbl.iMax ].dDistance = dD_2S;
            tTbl.tRec[ tTbl.iMax ].tLatLon.dLat = latlon2.dLat;
            tTbl.tRec[ tTbl.iMax ].tLatLon.dLon = latlon2.dLon;

            COMN_ALOG_DBGUT1_TRACE( "add lat=%f", latlon2.dLat );
            COMN_ALOG_DBGUT1_TRACE( "    lon=%f", latlon2.dLon );
            COMN_ALOG_DBGUT1_TRACE( "    dis=%f", dD_2S );

            ++( tTbl.iMax );
        }

        lat += 5;

        if( lat >= 90 )
        {
            break;
        }
    }

    iRetValue = 0;

LABEL_END:
    return iRetValue;
}

