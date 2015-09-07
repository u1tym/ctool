#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>

#include  "v.h"

#ifndef _FT_H
#define _FT_H

typedef struct tag_vrec
{
    vtype           *epV;
    struct tag_vrec *epNext;
} vrec;

typedef struct tag_ftype
{
    int  iMax;
    vrec *epElem;
} ftype;

ftype *ftyp_init( void );
int   ftyp_add( ftype *, char * );
int   ftyp_compare( ftype *, ftype * );
int   ftyp_delete( ftype * );

#endif

