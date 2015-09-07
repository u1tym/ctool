#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>

#include  "log.h"
#include  "tool.h"
#include  "v.h"
#include  "f.h"

#include  "ft.h"

#define MYMAX( A, B ) ( ( ( A ) >= ( B ) ) * ( A ) + ( ( A ) < ( B ) ) * ( B ) )

#define FUNC_ST()    userlog( 0, "ST [%s]", __FUNCTION__ ); \
                     userlog_down()
#define FUNC_ED()    userlog_up();                          \
                     userlog( 0, "ED [%s]", __FUNCTION__ )

#define PSTRING( S, P )  userlog( 0, "%-*.*s    ST=%d, ED=%d PS=%d",                  \
                             GetEd( S, P ) - GetSt( S, P ) + 1,                       \
                             GetEd( S, P ) - GetSt( S, P ) + 1,                       \
                             ( char * )( S + GetSt( S, P ) ),                         \
                             GetSt( S, P ), GetEd( S, P ), P );                       \
                         logtmp[ 0 ] = '\0';                                          \
                         for( logtmpInt = GetSt( S, P ); logtmpInt < P; ++logtmpInt ) \
                         {                                                            \
                             strcat( logtmp, " " );                                   \
                         }                                                            \
                         userlog( 0, "%s^^^^", logtmp )

#define PJUDGE( X )      if( ( X ) == 0 )                \
                         {                               \
                             userlog( 0, "-> match" );   \
                         }                               \
                         else                            \
                         {                               \
                             userlog( 0, "-> unmatch" ); \
                         }

char logtmp[ 256 ];
int  logtmpInt;

/*******************************************************************************
* 関数のプロトタイプ宣言
******************************************************************************/

void usage( void );
void DropComment( char * );
int  Analyze( char *, int * );
int  Analyze_Declare_Function( char *, int *, int * );
int  Analyze_Define_Function( char *, int *, int * );
int  Analyze_expr_cond( char *, int *, int * );
int  Analyze_expr_add( char *, int *, int * );
int  Analyze_expr_mul( char *, int *, int * );
int  Analyze_expr_pri( char *, int *, int * );
int  Analyze_expr_blacket( char *, int *, int * );
int  Analyze_expr_function( char *, int *, int * );
int  Analyze_expr_statement( char *, int *, int * );
void Analyze_Error_Result( char *, int, int );
int  Analyze_Error_Len( char *, int, int );
int  Analyze_Error_Line( char *, int );

int  Analyze_token_function( char *, int *, char *, int * );
int  Analyze_token_coron( char *, int *, char *, int * );
int  Analyze_token_vartype( char *, int *, char *, int * );
int  Analyze_token_arrow( char *, int *, char *, int * );
int  Analyze_token_ident( char *, int *, char *, int * );
int  Analyze_token_vsep( char *, int *, char *, int * );
int  Analyze_token_otherwise( char *, int *, char *, int * );
int  Analyze_token_equal( char *, int *, char *, int * );
int  Analyze_token_void( char *, int *, char *, int * );
int  Analyze_token_where( char *, int *, char *, int * );
int  Analyze_token_compair( char *, int *, char *, int * );
int  Analyze_token_add( char *, int *, char *, int * );
int  Analyze_token_mul( char *, int *, char *, int * );
int  Analyze_token_openblacket( char *, int *, char *, int * );
int  Analyze_token_closeblacket( char *, int *, char *, int * );


int main( int iArgc, char *cpArgv[] )
{
#if 1
    t3();
#else
    int  iRet;
    int  iErrorPos;
    int  iPos;
    char cBuf[ 1024 ];
    FILE *tpFile;

    char  *cpTmp;
    size_t lAnalyzeBufferSize;
    char  *cpAnalyzeBuffer;

    char  *cpInputFile;


    if( iArgc < 2 )
    {
        usage();

        return 1;
    }

    cpInputFile = cpArgv[ 1 ];


    /*======*/
    /* INIT */
    /*======*/
    lAnalyzeBufferSize = 0;
    cpAnalyzeBuffer    = ( char * )NULL;
    vtbl_init();
    ftbl_init();

    tpFile = fopen( cpInputFile, "r" );
    if( tpFile == ( FILE * )NULL )
    {
        return 1;
    }

    for( ;; )
    {
        iRet = feof( tpFile );
        if( iRet != 0 )
        {
            break;
        }

        memset( cBuf, 0x00, sizeof( cBuf ) );
        fgets( cBuf, sizeof( cBuf ) - 1, tpFile );

        DropComment( cBuf );

        if( strlen( cBuf ) == 0 )
        {
            continue;
        }

        if( cpAnalyzeBuffer == ( char * )NULL )
        {
            cpTmp = ( char * )malloc( strlen( cBuf ) );
            if( cpTmp == ( char * )NULL )
            {
                fprintf( stderr, "malloc() error\n" );
                goto LABEL_END;
            }
            cpAnalyzeBuffer = cpTmp;

            fprintf( stderr, "m = 0x%x\n", ( unsigned int )cpAnalyzeBuffer );
            fflush( stderr );
        }
        else
        {
            cpTmp = ( char * )realloc( cpAnalyzeBuffer,
                                      lAnalyzeBufferSize + strlen( cBuf ) );
            if( cpTmp == ( char * )NULL )
            {
                fprintf( stderr, "realloc() error moto=0x%x, siz=%ld\n",
                        ( unsigned int )cpAnalyzeBuffer, lAnalyzeBufferSize + strlen( cBuf ) );
                goto LABEL_END;
            }

            fprintf( stderr, "r = 0x%x -> 0x%x\n", ( unsigned int )cpAnalyzeBuffer, ( unsigned int )cpTmp );
            fflush( stderr );

            cpAnalyzeBuffer = cpTmp;
        }
        memcpy( ( char * )( cpAnalyzeBuffer + lAnalyzeBufferSize ),
                cBuf, strlen( cBuf ) );
        lAnalyzeBufferSize += strlen( cBuf );
    }

    fclose( tpFile );
    tpFile = ( FILE * )NULL;

    cpTmp = ( char * )realloc( cpAnalyzeBuffer,
                              lAnalyzeBufferSize + 1 );
    if( cpTmp == ( char * )NULL )
    {
        fprintf( stderr, "realloc() error moto=0x%x, siz=%ld\n",
                ( unsigned int )cpAnalyzeBuffer, lAnalyzeBufferSize + 1 );
        goto LABEL_END;
    }
    *( cpAnalyzeBuffer + lAnalyzeBufferSize ) = '\0';
    lAnalyzeBufferSize += 1;

    iPos = 0;
    iRet = Analyze( cpAnalyzeBuffer, &iPos );

LABEL_END:
    if( cpAnalyzeBuffer != ( char * )NULL )
    {
        fprintf( stderr, "f = 0x%x\n", ( unsigned int )cpAnalyzeBuffer );
        fflush( stderr );

        free( cpAnalyzeBuffer );
        cpAnalyzeBuffer = ( char * )NULL;
    }

    return 0;
#endif
}

void usage( void )
{
    fprintf( stdout, "鰓\n" );

    return;
}

void DropComment( char *cpData )
{
    int  iIndex;

    static int  s_iComm = 0;


    iIndex = 0;
    while( iIndex < strlen( cpData ) )
    {
        if( *( cpData + iIndex ) == '-'
            && *( cpData + iIndex + 1 ) == '-' )
        {
            *( cpData + iIndex ) = '\0';
            break;
        }

        iIndex += 1;
    }

    return;
}

#define  SKIP()    ( void )Is_Match_Pattern( cpString, piPos, "[ \t\n]{0,}", NULL, NULL )

int Analyze( char *cpString, int *piPos )
{
    int  iRet;
    int  iRetValue;
    int  iPos_Saved;
    int  iTmpErrorPos;
    int  iErrorPos;

    fprintf( stdout, "len=%d\n", strlen( cpString ) );

    while( *piPos < strlen( cpString ) )
    {
        iPos_Saved = *piPos;
        iErrorPos  = *piPos;

        fprintf( stdout, "(%d)(0x%02x)\n", *piPos, *( cpString + *piPos ) & 0xff );

        /*==============================*/
        /* スペース、タブ、改行の早送り */
        /*==============================*/
        iRet = Is_Match_Pattern( cpString, piPos, "[ \t\n]{1,}", NULL, NULL );
        if( iRet == 0 )
        {
            continue;
        }

        /*==================*/
        /* DECLARE FUNCTION */
        /*==================*/
        *piPos = iPos_Saved;
        iRet = Analyze_Declare_Function( cpString, piPos, &iTmpErrorPos );
        if( iRet == 0 )
        {
            continue;
        }
        iErrorPos = MYMAX( iErrorPos, iTmpErrorPos );

        /*=================*/
        /* DEFINE FUNCTION */
        /*=================*/
        *piPos = iPos_Saved;
        iRet = Analyze_Define_Function( cpString, piPos, &iTmpErrorPos );
        if( iRet == 0 )
        {
            continue;
        }
        iErrorPos = MYMAX( iErrorPos, iTmpErrorPos );

        /* 解析異常 */

        *piPos = iPos_Saved;
        Analyze_Error_Result( cpString, *piPos, iErrorPos );

        fprintf( stdout, "pos=%d, errpos=%d\n", *piPos, iErrorPos );

        iRetValue = -1;
        goto LABEL_END;
    }

    iRetValue = 0;

    fprintf( stdout, "analyze complete\n" );

LABEL_END:
    return iRetValue;
}

int Analyze_Init( void )
{
    vtbl_init();
    ftbl_init();

    Analyze_add_VariableType( "Void" );
    Analyze_add_VariableType( "Int" );

    return 0;
}

int Analyze_add_VariableType( char *cpName )
{
    FUNC_ST();

    vtbl_add( cpName );
    userlog( 0, "VARIABLE TYPE[%s]", cpName );

    FUNC_ED();
    return 0;
}

int Analyze_add_Function( char *cpName )
{
    FUNC_ST();

    ftbl_add( cpName, NULL, NULL );
    userlog( 0, "FUNCTION[%s]", cpName );

    FUNC_ED();
    return 0;
}

int Analyze_Declare_Function( char *cpString, int *piPos, int *piErrorPos )
{
    int  iRet;
    int  iRetValue;
    int  iSize;
    char cFunctionName[ 256 ];

    FUNC_ST();
    PSTRING( cpString, *piPos );

    /*================*/
    /* token function */
    /*================*/
#if 1 /* [ xxxxx */
    memset( cFunctionName, 0x00, sizeof( cFunctionName ) );
    iRet = Analyze_token_ident_v( cpString, piPos, cFunctionName, NULL );
#else /* ][ */
    iRet = Analyze_token_function( cpString, piPos, NULL, NULL );
#endif /* ] */
    if( iRet != 0 )
    {
        fprintf( stderr, "err=%d errpos=%d\n", __LINE__, *piPos );

        *piErrorPos = *piPos;
        iRetValue = -1;
        goto LABEL_END;
    }

    SKIP();

    /*======*/
    /* "::" */
    /*======*/
#if 1
    iRet = Analyze_token_coron( cpString, piPos, NULL, NULL );
#else
    iRet = Is_Match_Pattern( cpString, piPos, "::", NULL, NULL );
#endif
    if( iRet != 0 )
    {
        fprintf( stderr, "err=%d errpos=%d\n", __LINE__, *piPos );

        *piErrorPos = *piPos;
        iRetValue = -1;
        goto LABEL_END;
    }

    SKIP();

    /*=====================*/
    /* token variable type */
    /*=====================*/
#if 1
    iRet = Analyze_token_vartype( cpString, piPos, NULL, NULL );
#else
    iRet = Is_Match_Pattern( cpString, piPos, "(Int|Void)", NULL, NULL );
#endif
    if( iRet != 0 )
    {
        fprintf( stderr, "err=%d errpos=%d\n", __LINE__, *piPos );

        *piErrorPos = *piPos;
        iRetValue = -1;
        goto LABEL_END;
    }

    SKIP();

    for( ;; )
    {
        /*======*/
        /* "->" */
        /*======*/
#if 1
        iRet = Analyze_token_arrow( cpString, piPos, NULL, NULL );
#else
        iRet = Is_Match_Pattern( cpString, piPos, "->", NULL, NULL );
#endif
        if( iRet != 0 )
        {
            break;
        }

        SKIP();

        /*=====================*/
        /* token variable type */
        /*=====================*/
#if 1
        iRet = Analyze_token_vartype( cpString, piPos, NULL, NULL );
#else
        iRet = Is_Match_Pattern( cpString, piPos, "(Int|Void)", NULL, NULL );
#endif
        if( iRet != 0 )
        {
            fprintf( stderr, "err=%d errpos=%d\n", __LINE__, *piPos );

            *piErrorPos = *piPos;
            iRetValue = -1;
            goto LABEL_END;
        }

        SKIP();
    }

#if 1 /* [ xxxx */
    ( void )Analyze_add_Function( cFunctionName );
#endif /* ] */

    iRetValue = 0;

LABEL_END:
    PJUDGE( iRetValue );
    FUNC_ED();
    return iRetValue;
}

int Analyze_Define_Function( char *cpString, int *piPos, int *piErrorPos )
{
    int  iRet;
    int  iRetValue;
    int  iPos_Saved;
    int  iTmpErrorPos;
    int  iErrorPos;


    FUNC_ST();
    PSTRING( cpString, *piPos );


    /*================*/
    /* token function */
    /*================*/
#if 1
    iRet = Analyze_token_function( cpString, piPos, NULL, NULL );
#else
    iRet = Is_Match_Pattern( cpString, piPos, "[a-z_][a-zA-Z0-9_]{0,}", NULL, NULL );
#endif
    if( iRet != 0 )
    {
        fprintf( stderr, "err=%d errpos=%d\n", __LINE__, *piPos );

        *piErrorPos = *piPos;
        iRetValue = -1;
        goto LABEL_END;
    }

    SKIP();

    /*================*/
    /* token variable */
    /*================*/
#if 1
    iRet = Analyze_token_ident( cpString, piPos, NULL, NULL );
#else
    iRet = Is_Match_Pattern( cpString, piPos, "[a-z_][a-zA-Z0-9_]{0,}", NULL, NULL );
#endif
    if( iRet != 0 )
    {
        fprintf( stderr, "err=%d errpos=%d\n", __LINE__, *piPos );

        *piErrorPos = *piPos;
        iRetValue = -1;
        goto LABEL_END;
    }

    SKIP();

    for( ;; )
    {
        /*================*/
        /* token variable */
        /*================*/
#if 1
        iRet = Analyze_token_ident( cpString, piPos, NULL, NULL );
#else
        iRet = Is_Match_Pattern( cpString, piPos, "[a-z_][a-zA-Z0-9_]{0,}", NULL, NULL );
#endif
        if( iRet != 0 )
        {
            break;
        }

        SKIP();
    }

    for( ;; )
    {
        /*=====*/
        /* "|" */
        /*=====*/
#if 1
        iRet = Analyze_token_vsep( cpString, piPos, NULL, NULL );
#else
        iRet = Is_Match_Pattern( cpString, piPos, "\\|", NULL, NULL );
#endif
        if( iRet != 0 )
        {
            break;
        }

        SKIP();


        for( ;; )
        {
            iPos_Saved = *piPos;
            iErrorPos  = *piPos;

            /*===========*/
            /* EXPR COND */
            /*===========*/
            *piPos = iPos_Saved;
            iRet = Analyze_expr_cond( cpString, piPos, &iTmpErrorPos );
            if( iRet == 0 )
            {
                break;
            }
            iErrorPos = MYMAX( iErrorPos, iTmpErrorPos );

            /*=============*/
            /* "otherwise" */
            /*=============*/
            *piPos = iPos_Saved;
#if 1
            iRet = Analyze_token_otherwise( cpString, piPos, NULL, NULL );
#else
            iRet = Is_Match_Pattern( cpString, piPos, "otherwise", NULL, NULL );
#endif
            if( iRet == 0 )
            {
                break;
            }
            iErrorPos = MYMAX( iErrorPos, iTmpErrorPos );

            /* 解析異常 */
            *piErrorPos = iErrorPos;
            iRetValue = -1;
            goto LABEL_END;
        }

        SKIP();
    }

    /*=====*/
    /* "=" */
    /*=====*/
#if 1
    iRet = Analyze_token_equal( cpString, piPos, NULL, NULL );
#else
    iRet = Is_Match_Pattern( cpString, piPos, "=", NULL, NULL );
#endif
    if( iRet != 0 )
    {
        *piErrorPos = *piPos;
        iRetValue = -1;
        goto LABEL_END;
    }

    SKIP();

    for( ;; )
    {
        iPos_Saved = *piPos;
        iErrorPos  = *piPos;

        /*==========*/
        /* EXPR ADD */
        /*==========*/
        *piPos = iPos_Saved;
        iRet = Analyze_expr_add( cpString, piPos, &iTmpErrorPos );
        if( iRet == 0 )
        {
            break;
        }
        iErrorPos = MYMAX( iErrorPos, iTmpErrorPos );

        /*========*/
        /* "void" */
        /*========*/
        *piPos = iPos_Saved;
#if 1
        iRet = Analyze_token_void( cpString, piPos, NULL, NULL );
#else
        iRet = Is_Match_Pattern( cpString, piPos, "void", NULL, NULL );
#endif
        if( iRet == 0 )
        {
            break;
        }
        iErrorPos = MYMAX( iErrorPos, iTmpErrorPos );

        /* 解析異常 */
        *piErrorPos = iErrorPos;
        iRetValue = -1;
        goto LABEL_END;
    }

    for( ;; )
    {
        /*=========*/
        /* "where" */
        /*=========*/
#if 1
        iRet = Analyze_token_where( cpString, piPos, NULL, NULL );
#else
        iRet = Is_Match_Pattern( cpString, piPos, "where", NULL, NULL );
#endif
        if( iRet != 0 )
        {
            break;
        }

        SKIP();

        /*===========*/
        /* STATEMENT */
        /*===========*/
        iRet = Analyze_expr_statement( cpString, piPos, &iTmpErrorPos );
        if( iRet != 0 )
        {
            *piErrorPos = iTmpErrorPos;
            iRetValue = -1;
            goto LABEL_END;
        }

        SKIP();

        for( ;; )
        {
            /*===========*/
            /* STATEMENT */
            /*===========*/
            iRet = Analyze_expr_statement( cpString, piPos, &iTmpErrorPos );
            if( iRet != 0 )
            {
                break;
            }

            SKIP();
        }

        break; 
    }

    iRetValue = 0;

LABEL_END:
    PJUDGE( iRetValue );
    FUNC_ED();
    return iRetValue;
}

int Analyze_expr_cond( char *cpString, int *piPos, int *piErrorPos )
{
    int  iRet;
    int  iRetValue;
    int  iTmpErrorPos;

    FUNC_ST();
    PSTRING( cpString, *piPos );

    /*================*/
    /* token variable */
    /*================*/
    iRet = Analyze_expr_add( cpString, piPos, &iTmpErrorPos );
    if( iRet != 0 )
    {
        *piErrorPos = iTmpErrorPos;
        iRetValue = -1;
        goto LABEL_END;
    }

    SKIP();

    /*================*/
    /* token variable */
    /*================*/
#if 1
    iRet = Analyze_token_compair( cpString, piPos, NULL, NULL );
#else
    iRet = Is_Match_Pattern( cpString, piPos, "(==|!=|>|>=|<|<=)", NULL, NULL );
#endif
    if( iRet != 0 )
    {
        *piErrorPos = *piPos;
        iRetValue = -1;
        goto LABEL_END;
    }

    SKIP();

    /*================*/
    /* token variable */
    /*================*/
    iRet = Analyze_expr_add( cpString, piPos, &iTmpErrorPos );
    if( iRet != 0 )
    {
        *piErrorPos = iTmpErrorPos;
        iRetValue = -1;
        goto LABEL_END;
    }

    iRetValue = 0;

LABEL_END:
    PJUDGE( iRetValue );
    FUNC_ED();
    return iRetValue;
}

int Analyze_expr_add( char *cpString, int *piPos, int *piErrorPos )
{
    int  iRet;
    int  iRetValue;
    int  iTmpErrorPos;

    FUNC_ST();
    PSTRING( cpString, *piPos );

    /*==========*/
    /* EXPR MUL */
    /*==========*/
    iRet = Analyze_expr_mul( cpString, piPos, &iTmpErrorPos );
    if( iRet != 0 )
    {
        fprintf( stderr, "err=%d errpos=%d\n", __LINE__, iTmpErrorPos );

        *piErrorPos = iTmpErrorPos;
        iRetValue = -1;
        goto LABEL_END;
    }

    SKIP();

    for( ;; )
    {
        /*=======*/
        /* + / - */
        /*=======*/
#if 1
        iRet = Analyze_token_add( cpString, piPos, NULL, NULL );
#else
        iRet = Is_Match_Pattern( cpString, piPos, "(\\+|-)", NULL, NULL );
#endif
        if( iRet != 0 )
        {
            break;
        }

        SKIP();

        /*==========*/
        /* EXPR MUL */
        /*==========*/
        iRet = Analyze_expr_mul( cpString, piPos, &iTmpErrorPos );
        if( iRet != 0 )
        {
            fprintf( stderr, "err=%d errpos=%d\n", __LINE__, iTmpErrorPos );

            *piErrorPos = iTmpErrorPos;
            iRetValue = -1;
            goto LABEL_END;
        }

        SKIP();
    }

    iRetValue = 0;

LABEL_END:
    PJUDGE( iRetValue );
    FUNC_ED();
    return iRetValue;
}

int Analyze_expr_mul( char *cpString, int *piPos, int *piErrorPos )
{
    int  iRet;
    int  iRetValue;
    int  iTmpErrorPos;

    FUNC_ST();
    PSTRING( cpString, *piPos );

    /*==========*/
    /* EXPR PRI */
    /*==========*/
    iRet = Analyze_expr_pri( cpString, piPos, &iTmpErrorPos );
    if( iRet != 0 )
    {
        fprintf( stderr, "err=%d errpos=%d\n", __LINE__, iTmpErrorPos );

        *piErrorPos = iTmpErrorPos;
        iRetValue = -1;
        goto LABEL_END;
    }

    SKIP();

    for( ;; )
    {
        /*=======*/
        /* * / / */
        /*=======*/
#if 1
        iRet = Analyze_token_mul( cpString, piPos, NULL, NULL );
#else
        iRet = Is_Match_Pattern( cpString, piPos, "(\\*|/)", NULL, NULL );
#endif
        if( iRet != 0 )
        {
            break;
        }

        SKIP();

        /*==========*/
        /* EXPR PRI */
        /*==========*/
        iRet = Analyze_expr_pri( cpString, piPos, &iTmpErrorPos );
        if( iRet != 0 )
        {
            fprintf( stderr, "err=%d errpos=%d\n", __LINE__, iTmpErrorPos );

            *piErrorPos = iTmpErrorPos;
            iRetValue = -1;
            goto LABEL_END;
        }

        SKIP();
    }

    iRetValue = 0;

LABEL_END:
    PJUDGE( iRetValue );
    FUNC_ED();
    return iRetValue;
}

int Analyze_expr_pri( char *cpString, int *piPos, int *piErrorPos )
{
    int  iRet;
    int  iRetValue;
    int  iPos_Saved;
    int  iTmpErrorPos;
    int  iErrorPos;

    FUNC_ST();
    PSTRING( cpString, *piPos );

    for( ;; )
    {
        iPos_Saved = *piPos;
        iErrorPos  = *piPos;

        /*=========*/
        /* BLACKET */
        /*=========*/
        *piPos = iPos_Saved;
        iRet = Analyze_expr_blacket( cpString, piPos, &iTmpErrorPos );
        if( iRet == 0 )
        {
            break;
        }
        fprintf( stderr, " %d tmp_err=%d\n", __LINE__, iTmpErrorPos );
        iErrorPos = MYMAX( iErrorPos, iTmpErrorPos );

        /*==========*/
        /* FUNCTION */
        /*==========*/
        *piPos = iPos_Saved;
        iRet = Analyze_expr_function( cpString, piPos, &iTmpErrorPos );
        if( iRet == 0 )
        {
            break;
        }
        fprintf( stderr, " %d tmp_err=%d\n", __LINE__, iTmpErrorPos );
        iErrorPos = MYMAX( iErrorPos, iTmpErrorPos );

        fprintf( stderr, "err=%d errpos=%d\n", __LINE__, iErrorPos );

        /* 解析異常 */
        *piErrorPos = iErrorPos;
        iRetValue = -1;
        goto LABEL_END;
    }

    SKIP();

    iRetValue = 0;

LABEL_END:
    PJUDGE( iRetValue );
    FUNC_ED();
    return iRetValue;
}

int Analyze_expr_blacket( char *cpString, int *piPos, int *piErrorPos )
{
    int  iRet;
    int  iRetValue;
    int  iTmpErrorPos;

    FUNC_ST();
    PSTRING( cpString, *piPos );

    /*=====*/
    /* "(" */
    /*=====*/
#if 1
    iRet = Analyze_token_openblacket( cpString, piPos, NULL, NULL );
#else
    iRet = Is_Match_Pattern( cpString, piPos, "\\(", NULL, NULL );
#endif
    if( iRet != 0 )
    {
        fprintf( stderr, "err=%d errpos=%d\n", __LINE__, *piPos );

        *piErrorPos = *piPos;
        iRetValue = -1;
        goto LABEL_END;
    }

    SKIP();

    /*==========*/
    /* EXPR ADD */
    /*==========*/
    iRet = Analyze_expr_add( cpString, piPos, &iTmpErrorPos );
    if( iRet != 0 )
    {
        fprintf( stderr, "err=%d errpos=%d\n", __LINE__, *piPos );

        *piErrorPos = iTmpErrorPos;
        iRetValue = -1;
        goto LABEL_END;
    }

    SKIP();

    /*=====*/
    /* ")" */
    /*=====*/
#if 1
    iRet = Analyze_token_closeblacket( cpString, piPos, NULL, NULL );
#else
    iRet = Is_Match_Pattern( cpString, piPos, "\\)", NULL, NULL );
#endif
    if( iRet != 0 )
    {
        fprintf( stderr, "err=%d errpos=%d\n", __LINE__, *piPos );

        *piErrorPos = *piPos;
        iRetValue = -1;
        goto LABEL_END;
    }

    SKIP();

    iRetValue = 0;

LABEL_END:
    PJUDGE( iRetValue );
    FUNC_ED();
    return iRetValue;
}

int Analyze_expr_function( char *cpString, int *piPos, int *piErrorPos )
{
    int  iRet;
    int  iRetValue;
    int  iErrorPos;
    int  iPos_Saved;
    int  iTmpErrorPos;


    FUNC_ST();
    PSTRING( cpString, *piPos );

    for( ;; )
    {
        iPos_Saved = *piPos;
        iErrorPos  = *piPos;

        /*=========*/
        /* BLACKET */
        /*=========*/
        *piPos = iPos_Saved;

        iRet = Analyze_token_ident_c( cpString, piPos, NULL, NULL );
        if( iRet == 0 )
        {
            break;
        }
        fprintf( stderr, " %d tmp_err=%d\n", __LINE__, *piPos );
        iErrorPos = MYMAX( iErrorPos, *piPos );

        /*==========*/
        /* FUNCTION */
        /*==========*/
        *piPos = iPos_Saved;
        iRet = Analyze_expr_function2( cpString, piPos, &iTmpErrorPos );
        if( iRet == 0 )
        {
            break;
        }
        fprintf( stderr, " %d tmp_err=%d\n", __LINE__, iTmpErrorPos );
        iErrorPos = MYMAX( iErrorPos, iTmpErrorPos );

        fprintf( stderr, "err=%d errpos=%d\n", __LINE__, iErrorPos );

        /* 解析異常 */
        *piErrorPos = iErrorPos;
        iRetValue = -1;
        goto LABEL_END;
    }

    SKIP();

    iRetValue = 0;

LABEL_END:
    PJUDGE( iRetValue );
    FUNC_ED();
    return iRetValue;
}

int Analyze_expr_function2( char *cpString, int *piPos, int *piErrorPos )
{
    int  iRet;
    int  iRetValue;
    int  iPos_Saved;
    int  iTmpErrorPos;

    FUNC_ST();
    PSTRING( cpString, *piPos );

    iRet = Analyze_token_ident_f( cpString, piPos, NULL, NULL );
    if( iRet != 0 )
    {
        *piErrorPos = *piPos;
        iRetValue = -1;
        goto LABEL_END;
    }

    SKIP();

    for( ;; )
    {
        iPos_Saved = *piPos;

        *piPos = iPos_Saved;
        iRet = Analyze_expr_function( cpString, piPos, &iTmpErrorPos );
        if( iRet != 0 )
        {
            *piPos = iPos_Saved;

            break;
        }

        SKIP();
    }

    SKIP();

    iRetValue = 0;

LABEL_END:
    PJUDGE( iRetValue );
    FUNC_ED();
    return iRetValue;
}

int Analyze_expr_statement( char *cpString, int *piPos, int *piErrorPos )
{
    int  iRet;
    int  iRetValue;
    int  iTmpErrorPos;

    FUNC_ST();
    PSTRING( cpString, *piPos );


    for( ;; )
    {
        /*================*/
        /* token variable */
        /*================*/
#if 1
        iRet = Analyze_token_ident_v( cpString, piPos, NULL, NULL );
#else
        iRet = Is_Match_Pattern( cpString, piPos, "[a-z_][a-zA-Z0-9_]{0,}", NULL, NULL );
#endif
        if( iRet != 0 )
        {
            break;
        }

        SKIP();

        /*================*/
        /* token function */
        /*================*/
#if 1
        iRet = Analyze_token_equal( cpString, piPos, NULL, NULL );
#else
        iRet = Is_Match_Pattern( cpString, piPos, "=", NULL, NULL );
#endif
        if( iRet != 0 )
        {
            fprintf( stderr, "err=%d errpos=%d\n", __LINE__, *piPos );

            *piErrorPos = *piPos;
            iRetValue = -1;
            goto LABEL_END;
        }

        SKIP();

        break;
    }

    /*==========*/
    /* EXPR ADD */
    /*==========*/
    iRet = Analyze_expr_add( cpString, piPos, &iTmpErrorPos );
    if( iRet != 0 )
    {
        fprintf( stderr, "err=%d errpos=%d\n", __LINE__, iTmpErrorPos );

        *piErrorPos = iTmpErrorPos;
        iRetValue = -1;
        goto LABEL_END;
    }

    for( ;; )
    {
        /*======*/
        /* "::" */
        /*======*/
#if 1
        iRet = Analyze_token_coron( cpString, piPos, NULL, NULL );
#else
        iRet = Is_Match_Pattern( cpString, piPos, "::", NULL, NULL );
#endif
        if( iRet != 0 )
        {
            break;
        }

        SKIP();

        /*=====================*/
        /* token variable type */
        /*=====================*/
#if 1
        iRet = Analyze_token_vartype( cpString, piPos, NULL, NULL );
#else
        iRet = Is_Match_Pattern( cpString, piPos, "(Int|Void)", NULL, NULL );
#endif
        if( iRet != 0 )
        {
            fprintf( stderr, "err=%d errpos=%d\n", __LINE__, *piPos );

            *piErrorPos = *piPos;
            iRetValue = -1;
            goto LABEL_END;
        }

        SKIP();

        break;
    }

    iRetValue = 0;

LABEL_END:
    PJUDGE( iRetValue );
    FUNC_ED();
    return iRetValue;
}

void Analyze_Error_Result( char *cpString, int iPos, int iErrorPos )
{
    int  iIndex;
    int  iLen;
    int  iLine;
    int  iErrorSt;

    iLine = Analyze_Error_Line( cpString, iErrorPos );
    iLen  = Analyze_Error_Len( cpString, iPos, iErrorPos );

    iErrorSt = Analyze_Error_LineSt( cpString, iErrorPos );

    fprintf( stdout, "Syntax error at line : %d\n", iLine );
    fprintf( stdout, "%-*.*s\n", iLen, iLen, ( char * )( cpString + iPos ) );
    for( iIndex = 0; iIndex < ( iErrorPos - iErrorSt ); ++iIndex )
    {
        fprintf( stdout, " " );
    }
    fprintf( stdout, "^^^^\n" );
    fflush( stdout );

    return;
}

int Analyze_Error_Len( char *cpString, int iPos, int iErrorPos )
{
    int  iIndex;

    iIndex = 0;
    for( ;; )
    {
        if( iPos + iIndex >= iErrorPos )
        {
            break;
        }

        ++iIndex;
    }

    for( ;; )
    {
        if( *( cpString + iPos + iIndex ) == '\0'
            || *( cpString + iPos + iIndex ) == '\n' )
        {
            break;
        }

        ++iIndex;
    }

    return iIndex;
}

int Analyze_Error_LineSt( char *cpString, int iPos ) {
    int  iIndex;

    iIndex = iPos;
    for( iIndex = iPos; iIndex >= 0; --iIndex )
    {
        if( iIndex == 0 )
        {
            break;
        }
        if( *( cpString + iIndex - 1 ) == '\n' )
        {
            break;
        }
    }

    return iIndex;
}

int Analyze_Error_Line( char *cpString, int iPos ) {
    int  iIndex;
    int  iLine;

    iLine = 1;
    for( iIndex = 0; iIndex < iPos; ++iIndex )
    {
        if( *( cpString + iIndex ) == '\n' )
        {
            ++iLine;
        }
    }

    return iLine;
}

int GetSt( char *cpString, int iPos )
{
    int  iIndex;


    for( iIndex = iPos; iIndex > 0; --iIndex )
    {
        if( *( cpString + iIndex - 1 ) == '\n' )
        {
            return iIndex;
        }
    }

    return iIndex;
}

int GetEd( char *cpString, int iPos )
{
    int  iIndex;


    for( iIndex = iPos; iIndex < strlen( cpString ); ++iIndex )
    {
        if( *( cpString + iIndex ) == '\n' )
        {
            return ( iIndex - 1 );
        }
    }

    return iIndex;
}

int Analyze_token_function( char *cpString, int *piPos, char *cpRet, int *ipRet )
{
    int  iRet;

    FUNC_ST();
    PSTRING( cpString, *piPos );

    /*================*/
    /* token function */
    /*================*/
    iRet = Is_Match_Pattern( cpString, piPos, "[a-z_][a-zA-Z0-9_]{0,}", cpRet, ipRet );

    PJUDGE( iRet );
    FUNC_ED();
    return iRet;
}

int Analyze_token_ident_v( char *cpString, int *piPos, char *cpRet, int *ipRet )
{
    int  iRet;

    FUNC_ST();
    PSTRING( cpString, *piPos );

    /*================*/
    /* token function */
    /*================*/
    iRet = Is_Match_Pattern( cpString, piPos, "[a-z_][a-z0-9_]{0,}", cpRet, ipRet );

    PJUDGE( iRet );
    FUNC_ED();
    return iRet;
}

int Analyze_token_ident_c( char *cpString, int *piPos, char *cpRet, int *ipRet )
{
    int  iRet;

    FUNC_ST();
    PSTRING( cpString, *piPos );

    /*================*/
    /* token function */
    /*================*/
    iRet = Is_Match_Pattern( cpString, piPos, "(0|(-){0,}[1-9][0-9]{0,})", cpRet, ipRet );

    PJUDGE( iRet );
    FUNC_ED();
    return iRet;
}

int Analyze_token_ident_f( char *cpString, int *piPos, char *cpRet, int *ipRet )
{
    int  iRet;
    int  iRetValue;
    frec *epRet;

    FUNC_ST();
    PSTRING( cpString, *piPos );

    iRet = Analyze_token_ident_v( cpString, piPos, cpRet, ipRet );
    if( iRet != 0 )
    {
        iRetValue = -1;
        goto LABEL_END;
    }

    epRet = ftbl_get( cpString );
    if( epRet == ( frec * )NULL )
    {
        userlog( 0, "function listに該当なし" );

        iRetValue = -1;
        goto LABEL_END;
    }

    iRetValue = 0;

LABEL_END:

    PJUDGE( iRetValue );
    FUNC_ED();
    return iRetValue;
}

int Analyze_token_coron( char *cpString, int *piPos, char *cpRet, int *ipRet )
{
    int  iRet;

    FUNC_ST();
    PSTRING( cpString, *piPos );

    /*================*/
    /* token function */
    /*================*/
    iRet = Is_Match_Pattern( cpString, piPos, "::", cpRet, ipRet );

    PJUDGE( iRet );
    FUNC_ED();
    return iRet;
}

int Analyze_token_vartype( char *cpString, int *piPos, char *cpRet, int *ipRet )
{
    int  iRet;

    FUNC_ST();
    PSTRING( cpString, *piPos );

    /*================*/
    /* token function */
    /*================*/
    iRet = Is_Match_Pattern( cpString, piPos, "(Int|Void)", cpRet, ipRet );

    PJUDGE( iRet );
    FUNC_ED();
    return iRet;
}

int Analyze_token_arrow( char *cpString, int *piPos, char *cpRet, int *ipRet )
{
    int  iRet;

    FUNC_ST();
    PSTRING( cpString, *piPos );

    /*================*/
    /* token function */
    /*================*/
    iRet = Is_Match_Pattern( cpString, piPos, "->", cpRet, ipRet );

    PJUDGE( iRet );
    FUNC_ED();
    return iRet;
}

int Analyze_token_ident( char *cpString, int *piPos, char *cpRet, int *ipRet )
{
    int  iRet;

    FUNC_ST();
    PSTRING( cpString, *piPos );

    /*================*/
    /* token function */
    /*================*/
    iRet = Is_Match_Pattern( cpString, piPos, "([a-z_][a-zA-Z0-9_]{0,}|(-){0,}[0-9]{1,})", cpRet, ipRet );

    PJUDGE( iRet );
    FUNC_ED();
    return iRet;
}

int Analyze_token_vsep( char *cpString, int *piPos, char *cpRet, int *ipRet )
{
    int  iRet;

    FUNC_ST();
    PSTRING( cpString, *piPos );

    /*================*/
    /* token function */
    /*================*/
    iRet = Is_Match_Pattern( cpString, piPos, "\\|", cpRet, ipRet );

    PJUDGE( iRet );
    FUNC_ED();
    return iRet;
}

int Analyze_token_otherwise( char *cpString, int *piPos, char *cpRet, int *ipRet )
{
    int  iRet;

    FUNC_ST();
    PSTRING( cpString, *piPos );

    /*================*/
    /* token function */
    /*================*/
    iRet = Is_Match_Pattern( cpString, piPos, "otherwise", cpRet, ipRet );

    PJUDGE( iRet );
    FUNC_ED();
    return iRet;
}

int Analyze_token_equal( char *cpString, int *piPos, char *cpRet, int *ipRet )
{
    int  iRet;

    FUNC_ST();
    PSTRING( cpString, *piPos );

    /*================*/
    /* token function */
    /*================*/
    iRet = Is_Match_Pattern( cpString, piPos, "=", cpRet, ipRet );

    PJUDGE( iRet );
    FUNC_ED();
    return iRet;
}

int Analyze_token_void( char *cpString, int *piPos, char *cpRet, int *ipRet )
{
    int  iRet;

    FUNC_ST();
    PSTRING( cpString, *piPos );

    /*================*/
    /* token function */
    /*================*/
    iRet = Is_Match_Pattern( cpString, piPos, "void", cpRet, ipRet );

    PJUDGE( iRet );
    FUNC_ED();
    return iRet;
}

int Analyze_token_where( char *cpString, int *piPos, char *cpRet, int *ipRet )
{
    int  iRet;

    FUNC_ST();
    PSTRING( cpString, *piPos );

    /*================*/
    /* token function */
    /*================*/
    iRet = Is_Match_Pattern( cpString, piPos, "where", cpRet, ipRet );

    PJUDGE( iRet );
    FUNC_ED();
    return iRet;
}

int Analyze_token_compair( char *cpString, int *piPos, char *cpRet, int *ipRet )
{
    int  iRet;

    FUNC_ST();
    PSTRING( cpString, *piPos );

    /*================*/
    /* token function */
    /*================*/
    iRet = Is_Match_Pattern( cpString, piPos, "(==|!=|>|>=|<|<=)", cpRet, ipRet );

    PJUDGE( iRet );
    FUNC_ED();
    return iRet;
}

int Analyze_token_add( char *cpString, int *piPos, char *cpRet, int *ipRet )
{
    int  iRet;

    FUNC_ST();
    PSTRING( cpString, *piPos );

    /*================*/
    /* token function */
    /*================*/
    iRet = Is_Match_Pattern( cpString, piPos, "(\\+|-)", cpRet, ipRet );

    PJUDGE( iRet );
    FUNC_ED();
    return iRet;
}

int Analyze_token_mul( char *cpString, int *piPos, char *cpRet, int *ipRet )
{
    int  iRet;

    FUNC_ST();
    PSTRING( cpString, *piPos );

    /*================*/
    /* token function */
    /*================*/
    iRet = Is_Match_Pattern( cpString, piPos, "(\\*|/)", cpRet, ipRet );

    PJUDGE( iRet );
    FUNC_ED();
    return iRet;
}

int Analyze_token_openblacket( char *cpString, int *piPos, char *cpRet, int *ipRet )
{
    int  iRet;

    FUNC_ST();
    PSTRING( cpString, *piPos );

    /*================*/
    /* token function */
    /*================*/
    iRet = Is_Match_Pattern( cpString, piPos, "\\(", cpRet, ipRet );

    PJUDGE( iRet );
    FUNC_ED();
    return iRet;
}

int Analyze_token_closeblacket( char *cpString, int *piPos, char *cpRet, int *ipRet )
{
    int  iRet;

    FUNC_ST();
    PSTRING( cpString, *piPos );

    /*================*/
    /* token function */
    /*================*/
    iRet = Is_Match_Pattern( cpString, piPos, "\\)", cpRet, ipRet );

    PJUDGE( iRet );
    FUNC_ED();
    return iRet;
}

int t1( void )
{
    int   iRet;
    vtype *epRet;


    vtbl_init();

    iRet = vtbl_add( "Void" );
    fprintf( stdout, "Void -> %d\n", iRet );

    iRet = vtbl_add( "Int" );
    fprintf( stdout, "Int  -> %d\n", iRet );

    iRet = vtbl_add( "Void" );
    fprintf( stdout, "Void -> %d\n", iRet );

    iRet = vtbl_add( "Int" );
    fprintf( stdout, "Int  -> %d\n", iRet );

    epRet = vtbl_get( "Void" );
    fprintf( stdout, "Void -> 0x%x\n", epRet );

    epRet = vtbl_get( "Int" );
    fprintf( stdout, "Int  -> 0x%x\n", epRet );

    epRet = vtbl_get( "Void" );
    fprintf( stdout, "Void -> 0x%x\n", epRet );

    epRet = vtbl_get( "Int" );
    fprintf( stdout, "Int  -> 0x%x\n", epRet );

    return 0;
}

int t2( void )
{
    int   iRet;
    param eParam;

    param_init( &eParam );

    iRet = param_add( &eParam, vtbl_get( "Void" ) );
    fprintf( stdout, "ret %d\n", iRet );

    param_add( &eParam, vtbl_get( "Int" ) );
    fprintf( stdout, "ret %d\n", iRet );

    param_add( &eParam, vtbl_get( "Void" ) );
    fprintf( stdout, "ret %d\n", iRet );

    iRet = param_count( &eParam );
    fprintf( stdout, "ret %d\n", iRet );


    ftbl_init();

    iRet = ftbl_add( "f1", &eParam, vtbl_get( "Void" ) );
    fprintf( stdout, "f1 %d\n", iRet );

    iRet = ftbl_add( "f2", &eParam, vtbl_get( "Void" ) );
    fprintf( stdout, "f2 %d\n", iRet );

    iRet = ftbl_add( "f1", &eParam, vtbl_get( "Void" ) );
    fprintf( stdout, "f1 %d\n", iRet );


    return 0;
}

int t3( void )
{
    ftype *epFT;

    vtbl_init();
    vtbl_add( "Void" );
    vtbl_add( "Int" );

    epFT = ftyp_init();

    ftyp_add( epFT, "Int" );
    ftyp_add( epFT, "Int" );
    ftyp_add( epFT, "Int" );
    ftyp_add( epFT, "Int" );
    ftyp_add( epFT, "Void" );

    ftyp_delete( epFT );

    return 0;
}

