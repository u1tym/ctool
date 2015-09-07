#include  <stdio.h>
#include  <string.h>
#include  <stdarg.h>
#include  <time.h>
#include  <sys/time.h>

#include  "log.h"


static FILE *s_tpFile = ( FILE * )NULL;
static int  s_iLevel = 0;

void userlog( int iKind, char *cpFormat, ... )
{
    struct timeval tTime;
    struct tm      tTimeStr;
    char           cTimeStamp[ 128 ];

    va_list        vAp;
    char           cMessage[ 1024 ];

    char           cOutputLog[ 1024 ];

    int            iCnt;


    /*======================*/
    /* ログファイルＯＰＥＮ */
    /*======================*/
    if( s_tpFile == ( FILE * )NULL )
    {
        s_tpFile = fopen( "userlog.log", "a" );
        if( s_tpFile == ( FILE * )NULL )
        {
            return;
        }
    }


    /*============*/
    /* 可変部整形 */
    /*============*/
    va_start( vAp, cpFormat );
    vsprintf( cMessage, cpFormat, vAp );
    va_end( vAp );


    /*==========*/
    /* 時刻整形 */
    /*==========*/
    gettimeofday( &tTime, NULL );
    localtime_r( &( tTime.tv_sec ), &tTimeStr );
    strftime( cTimeStamp, sizeof( cTimeStamp ) - 1, "%Y/%m/%d %H:%M:%S", &tTimeStr );

    sprintf( cOutputLog, "%s.%03d ", cTimeStamp, ( tTime.tv_usec / 1000 ) );
    fprintf( s_tpFile, "%s", cOutputLog );

    cOutputLog[ 0 ] = '\0';
    for( iCnt = 0; iCnt < s_iLevel; ++iCnt )
    {
        strcat( cOutputLog, "  " );
    }
    fprintf( s_tpFile, "%s", cOutputLog );

    sprintf( cOutputLog, "%s", cMessage );
    fprintf( s_tpFile, "%s\n", cOutputLog );

    fflush( s_tpFile );
    
    return;
}

void userlog_down( void )
{
    ++s_iLevel;
    return;
}

void userlog_up( void )
{
    --s_iLevel;
    return;
}


