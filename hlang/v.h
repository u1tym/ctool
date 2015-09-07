#include  <stdio.h>

#ifndef _V_H
#define _V_H

typedef struct tag_vtype
{
    char             *cpName;
    struct tag_vtype *epNext;
} vtype;

typedef struct tag_vtypetbl
{
    int   iMax;
    vtype *epElem;
} vtypetbl;

int   vtbl_init( void );
int   vtbl_add( char * );
vtype *vtbl_get( char * );

#endif

