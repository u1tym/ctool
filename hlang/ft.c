#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>

#include  "v.h"
#include  "ft.h"

ftype *ftyp_init( void )
{
    ftype *epFT;

    userlog( 0, "st %s", __FUNCTION__ );

    epFT = ( ftype * )malloc( sizeof( ftype ) );
    if( epFT == ( ftype * )NULL )
    {
        userlog( 0, "ed %s", __FUNCTION__ );
        return ( ftype * )NULL;
    }

    epFT->iMax   = 0;
    epFT->epElem = ( vrec * )NULL;

    userlog( 0, "ft=0x%x", epFT );

    userlog( 0, "ed %s", __FUNCTION__ );
    return epFT;
}

int ftyp_add( ftype *epFT, char *cpVName )
{
    vrec *epPointer;
    vrec *epPointer_Parent;
    vrec *epNew;
    vtype *epVType;

    userlog( 0, "st %s", __FUNCTION__ );

    epVType = vtbl_get( cpVName );
    if( epVType == ( vtype * )NULL )
    {
        userlog( 0, "ed %s", __FUNCTION__ );
        return -1;
    }

    epPointer_Parent = epFT->epElem;
    epPointer = epFT->epElem;

    for( ;; )
    {
        if( epPointer == NULL )
        {
            break;
        }

        epPointer_Parent = epPointer;
        epPointer        = epPointer->epNext;
    }

    epNew = ( vrec * )malloc( sizeof( vrec ) );
    if( epNew == NULL )
    {
        userlog( 0, "ed %s", __FUNCTION__ );
        return -1;
    }

    epNew->epV = epVType;
    epNew->epNext = NULL;

    if( epFT->iMax == 0 )
    {
        epFT->epElem = epNew;
        userlog( 0, "add p(0x%x) 0x%x", epFT, epNew );
    }
    else
    {
        epPointer_Parent->epNext = epNew;
        userlog( 0, "add p(0x%x) 0x%x", epPointer_Parent, epNew );
    }
    ++( epFT->iMax );


    userlog( 0, "ed %s", __FUNCTION__ );
    return 0;
}

int ftyp_compare( ftype *epFT1, ftype *epFT2 )
{
    vrec *epPointer1;
    vrec *epPointer2;

    if( epFT1->iMax != epFT2->iMax )
    {
        return 1;
    }

    epPointer1 = epFT1->epElem;
    epPointer2 = epFT2->epElem;

    for( ;; )
    {
        if( epPointer1 == NULL )
        {
            break;
        }

        if( epPointer1->epV != epPointer2->epV )
        {
            return 1;
        }

        epPointer1 = epPointer1->epNext;
        epPointer2 = epPointer2->epNext;
    }

    return 0;
}

int ftyp_delete( ftype *epFT )
{
    vrec *epPointer;
    vrec *epPointer_Next;

    userlog( 0, "st %s", __FUNCTION__ );

    epPointer = epFT->epElem;

    for( ;; )
    {
        if( epPointer == NULL )
        {
            break;
        }

        epPointer_Next = epPointer->epNext;
        free( epPointer );

        userlog( 0, "delete 0x%x", epPointer );

        epPointer = epPointer_Next;
    }

    free( epFT );

    userlog( 0, "delete 0x%x", epFT );

    userlog( 0, "ed %s", __FUNCTION__ );
    return 0;
}

int ftyp_print( ftype *epFT, char *cpOutput )
{
    
}


