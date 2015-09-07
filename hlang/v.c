#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>

#include  "v.h"

vtypetbl g_tVTbl;

int vtbl_init( void )
{
    g_tVTbl.iMax   = 0;
    g_tVTbl.epElem = ( vtype * )NULL;
    return 0;
}

int vtbl_add( char *cpName )
{
    vtype *epPointer;              /* テーブル走査用ポインタ                  */
    vtype *epPointer_Before;       /* ポインタ1個前                           */
    vtype *epNew;                  /* 新規追加用                              */


    epPointer_Before = g_tVTbl.epElem;
    epPointer        = g_tVTbl.epElem;

    for( ;; )
    {
        /* 終端チェック */
        if( epPointer == NULL )
        {
            break;
        }

        /* 登録済みチェック */
        if( strcmp( epPointer->cpName, cpName ) == 0 )
        {
            /* 既に登録済み */
            return 1;
        }

        /* インクリメント */
        epPointer_Before = epPointer;
        epPointer        = epPointer->epNext;
    }

    epNew = ( vtype * )malloc( sizeof( vtype ) );
    if( epNew == ( vtype * )NULL )
    {
        /* 処理異常 */
        return -1;
    }

    epNew->cpName = ( char * )malloc( strlen( cpName ) + 1 );
    if( epNew->cpName == ( char * )NULL )
    {
        /* 処理異常 */
        free( epNew );
        return -1;
    }

    strcpy( epNew->cpName, cpName );
    epNew->epNext = ( vtype * )NULL;

    if( g_tVTbl.iMax == 0 )
    {
        g_tVTbl.epElem = epNew;
    }
    else
    {
        epPointer_Before->epNext = epNew;
    }
    ++( g_tVTbl.iMax );

    return 0;
}

vtype *vtbl_get( char *cpName )
{
    vtype *epPointer;


    epPointer = g_tVTbl.epElem;

    for( ;; )
    {
        /* 終端チェック */
        if( epPointer == NULL )
        {
            break;
        }

        /* 名称比較 */
        if( strcmp( epPointer->cpName, cpName ) == 0 )
        {
            /* 該当あり */
            return epPointer;
        }

        /* インクリメント */
        epPointer        = epPointer->epNext;
    }

    /* 該当なし */
    return ( vtype * )NULL;
}

