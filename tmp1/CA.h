#include  "COMN_extern.h"

#ifndef _CA_H
#define _CA_H

#ifdef CA_EXTERN
       int gs_iLogOutput = 1;
#else
extern int gs_iLogOutput;
#endif


#define  RTN_OK       (  0 )
#define  RTN_PRMERR   ( -2 )

/** 円周率 */
#define DEF_PI                      ( 3.1415926535898 )

/** 赤道半径 */
#if 0
#define DEF_EQUATORIAL_RADIUS       ( 10000.0 )
#else
#define DEF_EQUATORIAL_RADIUS       ( 6378137.0 )
#endif

/** 扁平率 */
#if 0
#define DEF_OBLATENESS              ( 0.0 )
#else
#define DEF_OBLATENESS              ( 1.0 / 298.257223563 )
#endif

/** 扁平率の簡易表記 */
#define DEF_F                       DEF_OBLATENESS

/** 赤道半径の簡易表記 */
#define DEF_A                       DEF_EQUATORIAL_RADIUS

/** 極半径の簡易表記 */
#define DEF_B                       ( DEF_A * ( 1.0 - DEF_F ) )

/** 度表記からラジアン表記の変換 */
#define CALC_DEG_TO_RAD( D )        ( ( D ) * DEF_PI / 180.0 )

/** ラジアン表記から度表記への変換 */
#define CALC_RAD_TO_DEG( R )        ( ( R ) * 180.0 / DEF_PI )

/** NMからmの変換 */
#define CALC_NM_TO_M( N )           ( ( N ) * 1852.0 )

/** mからNMへの変換 */
#define CALC_M_TO_NM( M )           ( ( M ) / 1852.0 )


#define CA_SET_LOGOFF()             gs_iLogOutput = 0
#define CA_SET_LOGON()              gs_iLogOutput = 1
#define CA_ALOG_ENTER( fmt, ... )         if( gs_iLogOutput == 1 )                    \
                                          {                                           \
                                              COMN_ALOG_ENTER( fmt, ## __VA_ARGS__ ); \
                                          }

#define CA_ALOG_EXIT( fmt, ... )          if( gs_iLogOutput == 1 )                    \
                                          {                                           \
                                              COMN_ALOG_EXIT( fmt, ## __VA_ARGS__ );  \
                                          }

#define CA_ALOG_DBGUT1_TRACE( fmt, ... )  if( gs_iLogOutput == 1 )                            \
                                          {                                                   \
                                              COMN_ALOG_DBGUT1_TRACE( fmt, ## __VA_ARGS__ );  \
                                          }

#define CA_ALOG_ERROR                     COMN_ALOG_ERROR
#define CA_ALOG_PARAM_ERROR               COMN_ALOG_PARAM_ERROR
#define CA_ALOG_LOGIC_ERROR               COMN_ALOG_LOGIC_ERROR


#define  CA_ABS( X )      ( ( X ) >= 0.0 ? ( X ) : ( X ) * ( -1.0 ) )
#define  DMIN( X, Y )     ( ( X ) <= ( Y ) ? ( X ) : ( Y ) )
#define  DMAX( X, Y )     ( ( X ) >= ( Y ) ? ( X ) : ( Y ) )

/* ASUB( P1, P2, PT )  PTがP1～P2の間の場合には、0、間ではない場合には1を返す */
#define  ASUB( P1, P2, PT )    ( {                                             \
                               double _psi1, _psi2, _psiT;                     \
                               double _phi1, _phi2, _phiT;                     \
                               int    _res;                                    \
                                                                               \
                               /* 小さい方を _psi1 とする */                   \
                               _psi1 = DMIN( P1, P2 );                         \
                               _psi2 = DMAX( P1, P2 );                         \
                                                                               \
                               /* 角度幅をπ以内とし、_phi1、_phi2とする。  */ \
                               if( _psi2 - _psi1 > DEF_PI )                    \
                               {                                               \
                                   _phi1 = _psi2 - 2.0 * DEF_PI;               \
                                   _phi2 = _psi1;                              \
                               }                                               \
                               else                                            \
                               {                                               \
                                   _phi1 = _psi1;                              \
                                   _phi2 = _psi2;                              \
                               }                                               \
                                                                               \
                               /* 評価値の調整 */                              \
                               _phiT = PT;                                     \
                               if( PT < _phi1 )                                \
                               {                                               \
                                   _phiT = PT + 2.0 * DEF_PI;                  \
                               }                                               \
                               else if( PT > _phi2 )                           \
                               {                                               \
                                   _phiT = PT - 2.0 * DEF_PI;                  \
                               }                                               \
                                                                               \
                               /* 判定 */                                      \
                               _res = 1;                                       \
                               if( _phi1 <= _phiT && _phiT <= _phi2 )          \
                               {                                               \
                                   _res = 0;                                   \
                               }                                               \
                                                                               \
                               /* 判定結果の返却 */                            \
                               _res;                                           \
                               })

/**
* 度分秒から度の変換
* @param[in] H  度
* @param[in] M  分（単位は 1/60度）
* @param[in] S  秒（単位は 1/60/1000度）
*/
#define  CALC_DEGREE( H, M, S )    ( ( (double)(H) )                     \
                                     + ( ( (double)(M) ) / 60.0 )        \
                                     + ( ( (double)(S) )/1000.0/60.0 ) )


typedef struct tag_STR_LATLON{  /**< 緯度経度構造体 */
    double dLat;                    /**< 緯度              */
    double dLon;                    /**< 経度              */
}STR_LATLON;

/*
 * XYZ座標構造体
 *
 */
typedef struct tag_STR_POSXYZ{  /**< XYZ座標構造体 */
    double dX;                         /**< X座標              */
    double dY;                         /**< Y座標              */
    double dZ;                         /**< Z座標              */
}STR_POSXYZ;

typedef STR_POSXYZ STR_VECTOR;

typedef struct tag_STR_PLANE
{
    double dA;
    double dB;
    double dC;
    double dD;
} STR_PLANE;

typedef struct tag_STR_LINE
{
    STR_POSXYZ  tPos;
    STR_VECTOR  tVec;
} STR_LINE;

#define COMN_5DEGREES_MAX      ( 36 )

typedef struct tag_STR_5DEGREES                  /** ５度点取得結果構造体     */
{
    int     iCnt;                                /** 取得５度点数             */
    STR_LATLON t5degrees_latlon[COMN_5DEGREES_MAX];
                                                 /** 取得５度点緯度経度構造体 */
} STR_5DEGREES;

int COMN_CA_ConvLatlonToXyz( STR_LATLON *, STR_POSXYZ * );
int COMN_CA_ConvXyzToLatlon( STR_POSXYZ *, STR_LATLON * );
int COMN_CA_CalcPlane( STR_POSXYZ *, STR_POSXYZ *, STR_POSXYZ *, STR_PLANE  * );
int COMN_CA_CalcCrossLine( STR_PLANE *, STR_PLANE *, STR_LINE  * );
int COMN_CA_CalcCrossPoint( double, double, STR_LINE *, STR_POSXYZ *, STR_POSXYZ * );
int COMN_CA_CalcDistance( STR_LATLON *, STR_LATLON *, double * );
int COMN_CA_CalcDirection( STR_LATLON *, STR_LATLON *, double * );
int COMN_CA_CalcPotision(STR_LATLON *, double, double, STR_LATLON * );
int COMN_CA_CalcLation( STR_LATLON *, STR_LATLON *, STR_LATLON *, STR_LATLON *, STR_LATLON *, int * );
#endif

