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


/***************************************************************************//**
*
* 関数名 : 2地点間の方位算出処理
*
* *****************************************************************************
*
* @breif
*   地点１からみた地点２の方位（真方位）を算出する。算出値の単位はラジアンとす
*   る。
*
* @param[in]  tpP1                 - 地点１
* @param[in]  tpP2                 - 地点２
* @param[out] dpOutput             - 方位（真方位）
*
* @retval     RTN_OK               - 正常
* @retval     RTN_PRMERR           - パラメータ不正
*
*******************************************************************************/
int COMN_CA_CalcDirection(STR_LATLON *tpP1, STR_LATLON *tpP2, double *dpOutput)
{
    int     iRetValue;                 /**< 戻り値設定用                      */

    double  dP1_Phi;                   /**< 地点１緯度                        */
    double  dP1_Phi_K;                 /**< 地点１化成緯度                    */
    double  dP1_Lamda;                 /**< 地点１経度                        */
    double  dP2_Phi;                   /**< 地点２緯度                        */
    double  dP2_Phi_K;                 /**< 地点２化成緯度                    */
    double  dP2_Lamda;                 /**< 地点２経度                        */

    double  dF;                        /**< 扁平率                            */
    double  dA;                        /**< 長半径（赤道半径）                */
    double  dB;                        /**< 短半径                            */

    double  dH;                        /**< ２地点の経度の差                  */
    double  dZ;                        /**< 一時計算値                        */
    double  dResult;                   /**< 方角                              */


    /*==========*/
    /* 処理開始 */
    /*==========*/

/*    CA_ALOG_DBGUT1_TRACE( "%.3f/%.3f から見た %.3f/%.3f の方位算出", */
/*                          tpP1->dLat, tpP1->dLon,                    */
/*                          tpP2->dLat, tpP2->dLon );                  */

    /*====================*/
    /* パラメータチェック */
    /*====================*/

    if(tpP1 == (STR_LATLON *)NULL
       || tpP2 == (STR_LATLON *)NULL
       || dpOutput == (double *)NULL)
    {
        iRetValue = RTN_PRMERR;
        goto LABEL_END;
    }


    /*======*/
    /* 算出 */
    /*======*/

    /* 扁平率 */
    dF = DEF_OBLATENESS;

    /* 半径 */
    dA = DEF_EQUATORIAL_RADIUS;
    dB = dA * ( 1 - dF );

    /* 緯度経度のラジアン表記 */
    dP1_Phi   = tpP1->dLat * DEF_PI / 180;
    dP1_Lamda = tpP1->dLon * DEF_PI / 180;

    dP2_Phi   = tpP2->dLat * DEF_PI / 180;
    dP2_Lamda = tpP2->dLon * DEF_PI / 180;

    /* 化成緯度の算出 */
    dP1_Phi_K = atan( dB / dA * tan( dP1_Phi ) );
    dP2_Phi_K = atan( dB / dA * tan( dP2_Phi ) );

    /* 2地点の経度の差 */
    dH        = dP1_Lamda - dP2_Lamda;

    if( CA_ABS( dH ) < 0.000001 )
    {
        if( dP1_Phi >= dP2_Phi )
        {
            *dpOutput = DEF_PI;
        }
        else
        {
            *dpOutput = 0.0;
        }

        iRetValue = RTN_OK;

        goto LABEL_END;
    }

    if( dH < 0 )
    {
        dH += ( 2.0 * DEF_PI );
    }

    dZ = atan( sin( dH ) /
             ( cos( dP1_Phi_K ) * tan( dP2_Phi_K )
             - sin( dP1_Phi_K ) * cos( dH ) ) );

    /*======================*/
    /* 算出結果を方位に変換 */
    /*======================*/

    /* CA_ALOG_DBGUT1_TRACE( "dH = %.6f", dH ); */
    /* CA_ALOG_DBGUT1_TRACE( "dZ = %.6f", dZ ); */

    if( dH > DEF_PI && dZ < 0 )
    {
        dResult = dZ * ( -1.0 );
    }
    else if( dH > DEF_PI && dZ >= 0 )
    {
        dResult = DEF_PI - dZ;
    }
    else if( dH < DEF_PI && dZ < 0 )
    {
        dResult = DEF_PI + ( -1.0 ) * dZ;
    }
    else if( dH < DEF_PI && dZ >= 0 )
    {
        dResult = 2.0 * DEF_PI - dZ;
    }

    *dpOutput = dResult;

    iRetValue = RTN_OK;


    /*==========*/
    /* 終了処理 */
    /*==========*/

LABEL_END:

/*    CA_ALOG_DBGUT1_TRACE( "方位 = %.3f °",                  */
/*                          CALC_RAD_TO_DEG( dResult ) );     */
    return iRetValue;
}



int COMN_CA_CalcPotision(STR_LATLON *tpP,
                         double dDir, double dDist, STR_LATLON *tpOutput)
{
    int     iRetValue;                 /**< 戻り値設定用                      */
    int     iCnt;                      /**< カウンタ                          */

    double  dF;                        /**< 扁平率                            */
    double  dA;                        /**< 長半径（赤道半径）                */
    double  dB;                        /**< 短半径                            */

    double  dPhi1;                     /**< 地点緯度                          */
    double  dLambda1;                  /**< 地点経度                          */

    double  dS;                        /**< 指定距離                          */
    double  dSinAlpha12;               /**< 指定方位角                        */
    double  dCosAlpha12;               /**< 指定方位角                        */

    double  dSigma1;                   /**< 計算用変数                        */
    double  dTanSigma1;                /**< 計算用変数                        */
    double  dCosU1;                    /**< 計算用変数                        */
    double  dSinAlpha_2;               /**< 計算用変数                        */
    double  dCA;                       /**< 計算用変数                        */
    double  dCB;                       /**< 計算用変数                        */
    double  dSigma;                    /**< 計算用変数                        */
    double  dDeltaSigma;               /**< 計算用変数                        */
    double  d2SigmaM;                  /**< 計算用変数                        */
    double  dSinSigma;                 /**< 計算用変数                        */
    double  dSinSigma_2;               /**< 計算用変数                        */
    double  dCosSigma;                 /**< 計算用変数                        */
    double  dCos2SigmaM;               /**< 計算用変数                        */
    double  dCos2SigmaM_2;             /**< 計算用変数                        */
    double  dTmp1;                     /**< 計算用変数                        */
    double  dTmp2;                     /**< 計算用変数                        */
    double  dTmp3;                     /**< 計算用変数                        */
    double  dTmp4;                     /**< 計算用変数                        */
    double  dTmp5;                     /**< 計算用変数                        */
    double  dNumerator;                /**< 計算用変数                        */
    double  dDenominator;              /**< 計算用変数                        */
    double  dU1;                       /**< 計算用変数                        */
    double  dSinU1;                    /**< 計算用変数                        */
    double  dTanPhi2;                  /**< 計算用変数                        */
    double  dPhi2;                     /**< 計算用変数                        */
    double  dTanLambda;                /**< 計算用変数                        */
    double  dC;                        /**< 計算用変数                        */
    double  dCosAlpha_2;               /**< 計算用変数                        */
    double  dOmega;                    /**< 計算用変数                        */
    double  dLambda;                   /**< 計算用変数                        */
    double  dLambda2;                  /**< 計算用変数                        */
    double  dTanU1;                    /**< 計算用変数                        */
    double  dSinAlpha;                 /**< 計算用変数                        */
    double  dU2;                       /**< 計算用変数                        */


    /*==========*/
    /* 処理開始 */
    /*==========*/


    /*====================*/
    /* パラメータチェック */
    /*====================*/

    if(tpP == (STR_LATLON *)NULL
       || dDist <= 0.0
       || tpOutput == (STR_LATLON *)NULL)
    {
        COMN_ALOG_ENTER("tpP      = 0x%x", tpP);
        COMN_ALOG_ENTER("dDist    = %f", dDist);
        COMN_ALOG_ENTER("tpOutput = 0x%x", tpOutput);

        iRetValue = RTN_PRMERR;
        goto LABEL_END;
    }


    /*==============*/
    /* 規定値の算出 */
    /*==============*/

    /* 扁平率 */
    dF = DEF_OBLATENESS;

    /* 半径 */
    dA = DEF_EQUATORIAL_RADIUS;
    dB = dA * ( 1 - dF );


    /* 緯度経度のラジアン表記 */
    dPhi1    = tpP->dLat * DEF_PI / 180.0;
    dLambda1 = tpP->dLon * DEF_PI / 180.0;

    /* 指定方位角 */
    dSinAlpha12 = sin( dDir );
    dCosAlpha12 = cos( dDir );

    /* 距離 */
    dS = dDist;


    /*======*/
    /* 算出 */
    /*======*/

    /* tan U1 = ( 1 - F ) tan φ1 */

    dTanU1 = ( 1.0 - dF ) * tan( dPhi1 );

    /* CA_ALOG_DBGUT1_TRACE( "tanU1 = %.3f", dTanU1 ); */

    /* tan σ1 = tan U1 / cos α12 */
    /* σ1     = atan( tan σ1 )   */

    dTanSigma1 = dTanU1 / dCosAlpha12;
    dSigma1 = atan( dTanSigma1 );

    /* sin alpha */
    dSinAlpha   = dCosU1 * dSinAlpha12;
    dSinAlpha_2 = pow( dSinAlpha, 2 );

    /* u^2 */
    dU2 = ( 1 - pow( dSinAlpha, 2 ) ) * ( pow( dA, 2 ) - pow( dB, 2 ) ) / pow( dB, 2 );

    /* CA_ALOG_DBGUT1_TRACE( "u^2 = %.3f", dU2 ); */

    /* A, B */
    dCA = 1.0 + ( dU2 / 16384.0 ) * ( 4096.0 + dU2 * ( -768.0 + dU2 * ( 320.0 - 175.0 * dU2 ) ) );
    dCB = ( dU2 / 1024.0 ) * ( 256.0 + dU2 * ( -128.0 + dU2 * ( 74.0 - 47.0 * dU2 ) ) );

    /* CA_ALOG_DBGUT1_TRACE( "A = %.3f", dCA ); */
    /* CA_ALOG_DBGUT1_TRACE( "B = %.3f", dCB ); */

    /* sigma */
    dSigma = dS / ( dB * dCA );

    /* CA_ALOG_DBGUT1_TRACE( "sigma = %.8f", dSigma ); */

    /* delta_sigma が十分小さくなるまで繰り返す */
    for( iCnt = 0; ; ++iCnt )
    {
        /* 2σm = 2σ1 + σ */
        d2SigmaM = 2.0 * dSigma1 + dSigma;

        dSinSigma     = sin( dSigma );
        dSinSigma_2   = pow( sin( dSigma ), 2 );
        dCosSigma     = cos( dSigma );
        dCos2SigmaM   = cos( d2SigmaM );
        dCos2SigmaM_2 = pow( dCos2SigmaM, 2 );

        dTmp1         = -1.0 + 2.0 * dCos2SigmaM_2;
        dTmp2         = -3.0 + 4.0 * dSinSigma_2;
        dTmp3         = -3.0 + 4.0 * dCos2SigmaM_2;

        dDeltaSigma   = dCB
                        * dSinSigma
                        * ( dCos2SigmaM
                            + dCB / 4.0
                            * ( dCosSigma * dTmp1
                                - dCB / 6.0
                                * dCos2SigmaM * dTmp2 * dTmp3 ) );

        dSigma        = dS / ( dB * dCA ) + dDeltaSigma;

        /* CA_ALOG_DBGUT1_TRACE( "sigma     = %.8f", dSigma ); */
        /* CA_ALOG_DBGUT1_TRACE( "d sigma   = %.8f", dDeltaSigma ); */
        /* CA_ALOG_DBGUT1_TRACE( "2 sigma m = %.8f", d2SigmaM ); */

        if( -0.001 < dDeltaSigma && dDeltaSigma < 0.001 )
        {
            break;
        }
    }

    dU1 = atan( dTanU1 );

    /* sin U1 */
    dSinU1 = sin( dU1 );
    dCosU1 = cos( dU1 );

    /* sin sigma, cos sigme */
    dSinSigma = sin( dSigma );
    dCosSigma = cos( dSigma );

    dNumerator   = dSinU1 * dCosSigma + dCosU1 * dSinSigma * dCosAlpha12;
    dTmp4        = dSinU1 * dSinSigma - dCosU1 * dCosSigma * dCosAlpha12;
    dTmp5        = pow( dTmp4, 2 );
    dDenominator = ( 1.0 - dF ) * sqrt( dSinAlpha_2 + dTmp5 );

    /* phi2 */
    dTanPhi2 = dNumerator / dDenominator;
    dPhi2    = atan( dTanPhi2 );

    /* CA_ALOG_DBGUT1_TRACE( "sin U1   = %.6f", dSinU1 ); */
    /* CA_ALOG_DBGUT1_TRACE( "cos U1   = %.6f", dCosU1 ); */

    /* CA_ALOG_DBGUT1_TRACE( "tan phi2 = %.6f", dTanPhi2 ); */
    /* CA_ALOG_DBGUT1_TRACE( "phi2     = .6f", dPhi2 ); */

    dTanLambda = ( dSinSigma * dSinAlpha12 )
               / ( dCosU1 * dCosSigma - dSinU1 * dSinSigma * dCosAlpha12 );
    dLambda    = atan( dTanLambda );
    dC         = ( dF / 16.0 ) * dCosAlpha_2 * ( 4.0 + dF * ( 4.0 - 3.0 * dCosAlpha_2 ) );
    dOmega     = dLambda
               - ( 1.0 - dC )
               * dF * dSinAlpha
               * ( dSigma + dC * dSinSigma
               * ( dCos2SigmaM + dC * dCosSigma * ( -1.0 + 2.0 * dCos2SigmaM_2 ) ) );
    dLambda2   = dLambda1 + dLambda;

    /* 緯度経度のラジアン表記 */
    tpOutput->dLat = dPhi2    * 180.0 / DEF_PI;
    tpOutput->dLon = dLambda2 * 180.0 / DEF_PI;


    iRetValue = RTN_OK;


    /*==========*/
    /* 処理終了 */
    /*==========*/

LABEL_END:
    /* COMN_ALOG_EXIT("Ret=%d", iRetValue); */
    return iRetValue;
}



int COMN_CA_CalcLation( STR_LATLON *tpLatLonSt1,
                        STR_LATLON *tpLatLonEd1,
                        STR_LATLON *tpLatLonSt2,
                        STR_LATLON *tpLatLonEd2,
                        STR_LATLON *tpLatLon,
                        int        *ipSts )
{
    double theta1, theta2, thetaT;
    double omega1, omega2, omegaT;

    int    iRet;
    int    iRet1;
    int    iRet2;

    STR_LATLON Q1;
    STR_LATLON Q2;
    STR_LATLON Q_Dash;

    int        iCnt;
    double     dist;
    double     dir;
    double     dir_Dash;


    ( void )COMN_CA_CalcDirection( tpLatLonSt1, tpLatLonSt2, &theta1 );

    ( void )COMN_CA_CalcDirection( tpLatLonSt1, tpLatLonEd2, &theta2 );

    ( void )COMN_CA_CalcDirection( tpLatLonSt1, tpLatLonEd1, &thetaT );

    ( void )COMN_CA_CalcDirection( tpLatLonSt2, tpLatLonSt1, &omega1 );

    ( void )COMN_CA_CalcDirection( tpLatLonSt2, tpLatLonEd1, &omega2 );

    ( void )COMN_CA_CalcDirection( tpLatLonSt2, tpLatLonEd2, &omegaT );

    CA_ALOG_DBGUT1_TRACE( "θ1=%.3f θ2=%.3f θT=%.3f", theta1, theta2, thetaT );
    CA_ALOG_DBGUT1_TRACE( "ω1=%.3f ω2=%.3f ωT=%.3f", omega1, omega2, omegaT );

    CA_ALOG_DBGUT1_TRACE( "t1=%.3f t2=%.3f tT=%.3f",
                          CALC_RAD_TO_DEG( theta1 ),                                                                                   
                          CALC_RAD_TO_DEG( theta2 ),                                                                                   
                          CALC_RAD_TO_DEG( thetaT ) );
    CA_ALOG_DBGUT1_TRACE( "o1=%.3f o2=%.3f oT=%.3f",
                          CALC_RAD_TO_DEG( omega1 ),                                                                                   
                          CALC_RAD_TO_DEG( omega2 ),                                                                                   
                          CALC_RAD_TO_DEG( omegaT ) );
                                                                                   
    iRet1 = ASUB( theta1, theta2, thetaT );
    iRet2 = ASUB( omega1, omega2, omegaT );

    CA_ALOG_DBGUT1_TRACE( "t 判定結果=%d", iRet1 );
    CA_ALOG_DBGUT1_TRACE( "o 判定結果=%d", iRet2 );

    if( iRet1 != 0 || iRet2 != 0 )
    {
        CA_ALOG_DBGUT1_TRACE( "交点なし" );

        *ipSts = 1;
        return 0;
    }

    Q1 = *tpLatLonSt2;
    Q2 = *tpLatLonEd2;

    for( iCnt = 0; iCnt < 100; ++iCnt )
    {

        iRet = COMN_CA_CalcDistance( &Q1, &Q2, &dist );
        if( iRet != 0 )
        {
            CA_ALOG_ERROR( "処理異常" );
            return 1;
        }

        iRet = COMN_CA_CalcDirection( &Q1, &Q2, &dir );
        if( iRet != 0 )
        {
            CA_ALOG_ERROR( "処理異常" );
            return 1;
        }

        /* CA_ALOG_DBGUT1_TRACE( "Q1 = %.3f/%.3f", Q1.dLat, Q1.dLon ); */
        /* CA_ALOG_DBGUT1_TRACE( "Q2 = %.3f/%.3f", Q2.dLat, Q2.dLon ); */
        /* CA_ALOG_DBGUT1_TRACE( "Dis(Q1,Q2) = %.3f", dist ); */
        /* CA_ALOG_DBGUT1_TRACE( "Dir(Q1,Q2) = %.3f", CALC_RAD_TO_DEG( dir ) ); */

        ( void )COMN_CA_CalcPotision( &Q1, dir, dist / 2.0, &Q_Dash );

        /* CA_ALOG_DBGUT1_TRACE( "CP = %.3f/%.3f", Q_Dash.dLat, Q_Dash.dLon ); */


        ( void )COMN_CA_CalcDirection( tpLatLonSt1, &Q_Dash, &dir_Dash );

        CA_ALOG_DBGUT1_TRACE( "%d Q' = %.6f/%.6f", iCnt, Q_Dash.dLat, Q_Dash.dLon );

        if( CA_ABS( thetaT - dir_Dash ) < 0.00000001 )
        {
            *tpLatLon = Q_Dash;
            *ipSts = 0;
            break;
        }


        iRet1 = ASUB( theta1, thetaT, dir_Dash );
        iRet2 = ASUB( thetaT, theta2, dir_Dash );

        if( iRet1 == 0 )
        {
            /* CA_ALOG_DBGUT1_TRACE( "-> Q1 = Q'" ); */
            Q1 = Q_Dash;
        }
        else if( iRet2 == 0 )
        {
            /* CA_ALOG_DBGUT1_TRACE( "-> Q2 = Q'" ); */
            Q2 = Q_Dash;
        }
        else
        {
            CA_ALOG_ERROR( "処理異常" );
            return 1;
        }

    }

    return 0;
}

