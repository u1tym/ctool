#include  <stdio.h>

#ifndef _F_H
#define _F_H

typedef struct tag_frec
{
    char  *cpName;
    vtype *epParamType[ 256 ];
    vtype *epReturnType;
} frec;

typedef struct tag_ftbl
{
    int  iMax;
    frec *tpRec;
} ftbl;

typedef struct tag_param
{
    int  iMax;
    vtype *epParam[ 256 ];
} param;

int   param_init( param * );
int   param_add( param *, vtype * );
vtype *param_get( param *, int );
int   param_count( param * );
int   ftbl_init( void );
int   ftbl_add( char *, param *, vtype * );
frec  *ftbl_get( char * );

#endif

