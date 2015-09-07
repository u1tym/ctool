#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>

#include  "tool.h"

#define MYMAX( A, B ) ( ( ( A ) >= ( B ) ) * ( A ) + ( ( A ) < ( B ) ) * ( B ) )

/*******************************************************************************
 * 関数のプロトタイプ宣言
 ******************************************************************************/


int main( int iArgc, char *cpArgv[] )
{
    int  iRet;
    int  iPos;
    char *cpString = "+5";

    iRet = Is_Match_Pattern( cpString, &iPos, "(\\+|-)", NULL, NULL );

    fprintf( stdout, "return=%d\n", iRet );

    return 0;
}

