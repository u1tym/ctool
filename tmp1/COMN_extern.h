#include  <stdio.h>
#include  <stdarg.h>

#ifndef _COMN_EXTERN_H
#define _COMN_EXTERN_H

#define COMN_ALOG_ENTER( ... )         printf( __VA_ARGS__ ); \
                                       printf( "\n" );        \
                                       fflush( stdout )
#define COMN_ALOG_EXIT( ... )          printf( __VA_ARGS__ ); \
                                       printf( "\n" );        \
                                       fflush( stdout )
#define COMN_ALOG_INFORMATION( ... )   printf( __VA_ARGS__ ); \
                                       printf( "\n" );        \
                                       fflush( stdout )

#define COMN_ALOG_ERROR( ... )         printf( __VA_ARGS__ ); \
                                       printf( "\n" );        \
                                       fflush( stdout )

#define COMN_ALOG_DBGUT1_TRACE( ... )  printf( __VA_ARGS__ ); \
                                       printf( "\n" );        \
                                       fflush( stdout )

#endif


