#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>

#include  "log.h"

#include  "v.h"
#include  "f.h"

ftbl     g_tFTbl;

int param_init( param *epParam )
{
    memset( epParam, 0x00, sizeof( *epParam ) );
    return 0;
}

int param_add( param *epParam, vtype *epType )
{
    if( epParam->iMax >= 256 )
    {
        return -1;
    }

    epParam->epParam[ epParam->iMax ] = epType;
    ++( epParam->iMax );

    return 0;
}

vtype *param_get( param *epParam, int iIndex )
{
    if( iIndex >= epParam->iMax )
    {
        return NULL;
    }

    return ( epParam->epParam[ iIndex ] );
}

int param_count( param *epParam )
{
    return epParam->iMax;
}

int ftbl_init( void )
{
    memset( &g_tFTbl, 0x00, sizeof( g_tFTbl ) );
    return 0;
}

int ftbl_add( char *cpName, param *epParam, vtype *epRetType )
{
    int  iIndex;
    char *cpTmp;
    frec *epRet;
    frec *epTmp;
    frec *epTarget;

    userlog( 0, "st (%s:%d)", __FUNCTION__, __LINE__ );

    epRet = ftbl_get( cpName );
    if( epRet != NULL )
    {
        /* すでに登録済み */
        userlog( 0, "ed (%s:%d)", __FUNCTION__, __LINE__ );
        return -1;
    }

    /* 領域拡張 */
    if( g_tFTbl.tpRec == NULL )
    {
        epTmp = ( frec * )malloc( sizeof( frec ) );
        if( epTmp == NULL )
        {
            userlog( 0, "ed (%s:%d)", __FUNCTION__, __LINE__ );
            return -2;
        }
    }
    else
    {
        epTmp = ( frec * )realloc( g_tFTbl.tpRec, sizeof( frec ) * ( g_tFTbl.iMax + 1 ) );
        if( epTmp == NULL )
        {
            userlog( 0, "ed (%s:%d)", __FUNCTION__, __LINE__ );
            return -2;
        }
    }
    g_tFTbl.tpRec = epTmp;
    ++( g_tFTbl.iMax );

    /* 登録位置 */
    epTarget = g_tFTbl.tpRec + g_tFTbl.iMax - 1;
    memset( epTarget, 0x00, sizeof( frec ) );

    /* 登録 */

    cpTmp = ( char * )malloc( strlen( cpName ) + 1 );
    if( cpTmp == NULL )
    {
        userlog( 0, "ed (%s:%d)", __FUNCTION__, __LINE__ );
        return -3;
    }
    strcpy( cpTmp, cpName );

    epTarget->cpName = cpTmp;

    for( iIndex = 0; iIndex < 256; ++iIndex )
    {
        if( epParam == NULL )
        {
            break;
        }

        if( iIndex >= epParam->iMax )
        {
            break;
        }
        epTarget->epParamType[ iIndex ] = epParam->epParam[ iIndex ];

        userlog( 0, "   inf2 iIndex = %d, iMax=%d",
                 iIndex, epParam->iMax );
    }
    userlog( 0, "   inf3" );

    epTarget->epReturnType = epRetType;

    userlog( 0, "ed (%s:%d)", __FUNCTION__, __LINE__ );
    return 0;
}

frec *ftbl_get( char *cpName )
{
    int  iIndex;

    for( iIndex = 0; iIndex < g_tFTbl.iMax; ++iIndex )
    {
        if( strcmp( cpName, ( g_tFTbl.tpRec + iIndex )->cpName ) == 0 )
        {
            /* すでに登録済み */
            return ( g_tFTbl.tpRec + iIndex );
        }
    }

    return NULL;
}




