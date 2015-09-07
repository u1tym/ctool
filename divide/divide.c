#ident "$Header: /home/cvs/cvs/tools/divide/divide.c,v 1.1 2013/07/11 08:17:52 meguro Exp $"
/*=============================================================================
        バージョン      日付            修正内容
  --------------------------------------------------------------------------
        Ver.1.0.0       2007/10/02      新規作成
=============================================================================*/

#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>

/*===========================================================================*/
/* リビジョン                                                                                                                            */
/*===========================================================================*/
#define REVDATA "$Revision: 1.1 $"

/* ソースコード中に現れるキーワードの判別区別 */
#define MODE_IF1        (1)
#define MODE_IF0        (-1)
#define MODE_NONE       (0)

#define C_DATAREC_MAX   (100)

/* 戻り値(汎用) */
#define RTN_ON          (1)
#define RTN_OFF         (0)

/* 戻り値(Func_Extract) */
#define RTN_ETC                         (0)
#define RTN_KEYWORD                     (1)
#define RTN_OVERFLOW            (2)
#define RTN_ERR                         (-1)

/* 戻り値(Func_Judge) */
#define RTN_OUTPUT                      (1)
#define RTN_NOOUTPUT            (-1)

/*===========================================================================*/
/* 構造体宣言                                                                                                                            */
/*===========================================================================*/
/*----------------*/
/* キーワード構造 */
/*----------------*/
typedef struct str_keywordrec_tag
{
        int             iLen;
        char    cValue[64];
} STR_KEYWORDREC;

typedef struct str_keyword_tag
{
        int             iCnt;
        STR_KEYWORDREC  uRec[100];

        char    cExtra[256];

        int             iFlag_M1;

        /* 隠しパラメータ状態 */
        int             iS1;                                    /* キーワードの状態出力                                  */
        int             iS2;                                    /* 各ファイルの解析状態出力                              */
        int             iS3;
} STR_KEYWORD;

/*--------------*/
/* 状態判定構造 */
/*--------------*/
typedef struct str_rdata_tag
{
        int  iMode;             /* モード                       */
                                        /* MODE_IF1                     */
                                        /* MODE_IF0                     */
                                        /* MODE_NONE            */
} STR_RDATA;

typedef struct str_data_tag
{
        int                     iCurrent_Index;
        STR_RDATA       uData[C_DATAREC_MAX];

        int                     iComment;                                       /* コメント処理時、１となる              */
        int                     iInstrings;                                     /* 文字列中時、１となる                  */
} STR_DATA;

/*--------*/
/* 文字列 */
/*--------*/
#define         DEF_DIV_TAB             (1)
#define         DEF_DIV_SP              (2)
#define         DEF_DIV_CR              (3)
#define         DEF_DIV_END             (4)

typedef struct str_string_tag
{
        char    cData[256];                     /* 文字列                                                                        */
        int             iData_Len;                      /* 文字列長                                                                      */

        char    cSep[256];                      /* 区切り文字                                                            */
        int             iSep_Len;                       /* 区切り文字長                                                          */
} STR_STRING;

/*===========================================================================*/
/* マクロ定義                                                                                                                            */
/*===========================================================================*/
#define UMIN(A, B)              (((A) <= (B)) * (A) + ((A) > (B)) * (B))
#define UMAX(A, B)              (((A) >= (B)) * (A) + ((A) < (B)) * (B))

/*===========================================================================*/
/* 関数のプロトタイプ宣言                                                                                                        */
/*===========================================================================*/
int Func_Main(STR_KEYWORD *, char *);
int Func_Extract(STR_STRING *, STR_STRING *, STR_DATA *, STR_KEYWORD *, int *);
int Func_Judge(STR_DATA *);
int Func_Cut_Space(char *);
int Func_Div_Csv(char *, char *, char (*)[], int, int);
int LIBetc_Div_Csv_Core(char *, char *, char (*)[], int, int, int);
int Func_Add_Keyword(STR_KEYWORD *, char *);
int Func_Keyword_Judge(char *, STR_KEYWORD *);

int Func_Div(char *, STR_STRING *);
int Func_No_Output(FILE *, STR_STRING *, int, int);
int Func_Output(FILE *, STR_STRING *, int, int);

int usage(void)
{
        char    cRevision[256];
        char    *pcTmp;

        /* Revisionから'$'を除去 */
        strcpy(cRevision, REVDATA);
        for(;;)
        {
                pcTmp = strstr(cRevision, "$");
                if(pcTmp != NULL)
                {
                        *pcTmp = ' ';
                        continue;
                }
                break;
        }

        fprintf(stderr, "divide\n");
        fprintf(stderr, "\n");
        fprintf(stderr, "NAME\n");
        fprintf(stderr, "        divide.lmf - #if/#endif除去ツール (%s)\n", cRevision);
        fprintf(stderr, "\n");
        fprintf(stderr, "SYNOPSIS\n");
        fprintf(stderr, "        divide.lmf [-m1] [-k keywords...] [-t extra_id]  -f sourcefiles...\n");
        fprintf(stderr, "\n");
        fprintf(stderr, "DESCRIPTION\n");
        fprintf(stderr, "        #if、#else、#endif等でかこまれた有効な部分のみを出力する。\n");
        fprintf(stderr, "\n");
        fprintf(stderr, "OPTIONS\n");
        fprintf(stderr, "        -m1\n");
        fprintf(stderr, "                無効である部分についても空白で出力する。\n");
        fprintf(stderr, "\n");
        fprintf(stderr, "        -k keywords...\n");
        fprintf(stderr, "                有効とするキーワードを指定する。\n");
        fprintf(stderr, "\n");
        fprintf(stderr, "        -t extra_id\n");
        fprintf(stderr, "                出力するファイルの拡張子を指定する。\n");
        fprintf(stderr, "\n");
        fprintf(stderr, "BUGS\n");
        fprintf(stderr, "        ソースコード中に、「/*/*/」という記述がある場合に、コメント部が終了した\n");
        fprintf(stderr, "        と判断されない。\n");
        fprintf(stderr, "\n");
        fprintf(stderr, "TIPS\n");
        fprintf(stderr, "        divide.lmf -m1 -k DBG1 DBG2 -t notag -f testfile.c testfile2.c\n");
        fflush(stderr);

        return 0;
}

int Func_Ana_Param(STR_KEYWORD *puKeywords, int argc, char *argv[])
{
        int             iParam_Index;
        int             iRet;

        /* パラメータ検索添え字クリア */
        iParam_Index = 1;

        /* キーワード情報クリア */
        memset((char *)puKeywords, 0x00, sizeof(*puKeywords));

        strcpy(puKeywords->cExtra, "notag");

        while(iParam_Index < argc)
        {
                /*======================*/
                /* 隠しコマンドチェック */
                /*======================*/
                if(strcmp(argv[iParam_Index], "-S1") == 0)
                {
                        puKeywords->iS1 = 1;
                        ++iParam_Index;
                        continue;
                }
                if(strcmp(argv[iParam_Index], "-S2") == 0)
                {
                        puKeywords->iS2 = 1;
                        ++iParam_Index;
                        continue;
                }
                if(strcmp(argv[iParam_Index], "-S3") == 0)
                {
                        puKeywords->iS3 = 1;
                        ++iParam_Index;
                        continue;
                }

                if(strcmp(argv[iParam_Index], "-m1") == 0)
                {
                        puKeywords->iFlag_M1 = 1;
                        ++iParam_Index;
                        continue;
                }

                if(iParam_Index < argc
                        && strcmp(argv[iParam_Index], "-t") == 0)
                {
                        ++iParam_Index;

                        strcpy(puKeywords->cExtra, argv[iParam_Index]);
                        ++iParam_Index;
                        continue;
                }

                /*============*/
                /* キーワード */
                /*============*/
                if(iParam_Index < argc
                        && strcmp(argv[iParam_Index], "-k") == 0)
                {
                        ++iParam_Index;

                        for(; iParam_Index < argc; ++iParam_Index)
                        {
                                if(memcmp(argv[iParam_Index], "-", 1) == 0)
                                {
                                        break;
                                }

                                iRet = Func_Add_Keyword(puKeywords, argv[iParam_Index]);
                                if(iRet != 0)
                                {
                                        return 0;
                                }
                        }

                        continue;
                }

                if(strcmp(argv[iParam_Index], "-f") == 0)
                {
                        break;
                }
        }

        /* 固定で「#if 1」を追加 */
        iRet = Func_Add_Keyword(puKeywords, "1");
        if(iRet != 0)
        {
                return -1;
        }

        return iParam_Index;
}

/*===========================================================================*/
/*      メイン処理                                                                                                                               */
/*      引数            I       int             argc                                                                                     */
/*                              I       char    *argv[]                                                                                  */
/*      戻り値          0       正常終了                                                                                                 */
/*                              1       異常終了                                                                                                 */
/*===========================================================================*/
int main(int argc, char *argv[])
{
        int             iCnt;                   /* 汎用カウンタ                                                                  */
        int             iRet;                   /* 戻り値判別用                                                                  */
        int             iParam_Index;   /* 添字カウンタ                                                                  */
        int             iProc_Cnt;              /* 処理ファイル数                                                                */
        STR_KEYWORD     uKeywords;      /* キーワード構造体                                                              */

        /*====================*/
        /* パラメータチェック */
        /*====================*/

        if(argc < 2)
        {
                usage();
                return 1;
        }

        /* 処理ファイル数クリア */
        iProc_Cnt = 0;

#if     1       /* [ */
        iParam_Index = Func_Ana_Param(&uKeywords, argc, argv);
        if(iParam_Index <= 0)
        {
                usage();
                return 0;
        }
#else   /* ][ */
        /* パラメータ検索添え字クリア */
        iParam_Index = 1;

        /* キーワード情報クリア */
        memset((char *)&uKeywords, 0x00, sizeof(uKeywords));

        /*======================*/
        /* 隠しコマンドチェック */
        /*======================*/
        if(strcmp(argv[iParam_Index], "-S1") == 0)
        {
                uKeywords.iS1 = 1;
                ++iParam_Index;
        }
        if(strcmp(argv[iParam_Index], "-S2") == 0)
        {
                uKeywords.iS2 = 1;
                ++iParam_Index;
        }
        if(strcmp(argv[iParam_Index], "-S3") == 0)
        {
                uKeywords.iS3 = 1;
                ++iParam_Index;
        }


        /*==========================*/
        /* パラメータチェック／解析 */
        /*==========================*/
        if(iParam_Index < argc
                && strcmp(argv[iParam_Index], "-D") == 0)
        {
                ++iParam_Index;

                for(; iParam_Index < argc; ++iParam_Index)
                {
                        if(strcmp(argv[iParam_Index], "-F") == 0)
                        {
                                break;
                        }

                        iRet = Func_Add_Keyword(&uKeywords, argv[iParam_Index]);
                        if(iRet != 0)
                        {
                                return 1;
                        }
                }
        }
        /* 固定で「#if 1」を追加 */
        iRet = Func_Add_Keyword(&uKeywords, "1");
        if(iRet != 0)
        {
                return 1;
        }
#endif  /* ] */

        /* キーワードの内容を出力 */
        if(uKeywords.iS1 == 1)
        {
                printf("keyword %d\n", uKeywords.iCnt);
                for(iCnt = 0; iCnt < uKeywords.iCnt; ++iCnt)
                {
                        printf("[%2d][%s]\n", (iCnt + 1), uKeywords.uRec[iCnt].cValue);
                }
                fflush(stdout);
        }

        /*========*/
        /* 主処理 */
        /*========*/
        if(iParam_Index < argc
                && strcmp(argv[iParam_Index], "-f") == 0)
        {
                ++iParam_Index;

                /* 指定されたファイル数分、繰り返す */
                for(; iParam_Index < argc; ++iParam_Index)
                {
                        /* 主処理 */
                        iRet = Func_Main(&uKeywords, argv[iParam_Index]);
                        if(iRet != 0)
                        {
                                fprintf(stdout,
                                                "ファイル[%s]の処理中にエラーが発生しました。\n",
                                                argv[iParam_Index]);
                                break;
                        }
                        ++iProc_Cnt;

                        fprintf(stdout, "ファイル[%s]の処理終了\n", argv[iParam_Index]);
                        fflush(stdout);
                }
        }
        else if(iParam_Index < argc)
        {
                fprintf(stderr, "引数[%s]は不正です。\n", argv[iParam_Index]);
                fflush(stderr);
        }

        fprintf(stdout, "%dファイルを処理しました。\n", iProc_Cnt);

        return 0;
}

int Func_Main(STR_KEYWORD *puKeywords, char *pcFile_Read)
{
        int                     iRet;
        int                     iRead_Line;
        char            *pcFile_Write;
        char            cString[1024];
        char            cFile_Write[256];
        char            *pcRet;
        FILE            *uFile_Read;
        FILE            *uFile_Write;
        STR_DATA        uData;
#if     1       /* [ */
        int                     iCnt;
        int                     iCol;
        int                     iJudge_Col;
        int                     iResult;
        STR_STRING      uDiv[100];
#endif  /* ] */

        /*==================*/
        /* ファイルオープン */
        /*==================*/
        sprintf(cFile_Write, "%s.%s", pcFile_Read, puKeywords->cExtra);

        uFile_Read = fopen(pcFile_Read, "r");
        if(uFile_Read == NULL)
        {
                fprintf(stderr, "ファイルオープン鰓 [%s]\n", pcFile_Read);
                return -1;
        }
        iRead_Line = 0;

        uFile_Write = fopen(cFile_Write, "w");
        if(uFile_Write == NULL)
        {
                fprintf(stderr, "ファイルオープン鰓 [%s]\n", cFile_Write);
                fclose(uFile_Read);
                return -1;
        }

        /*====================*/
        /* 最下位レベルを設定 */
        /*====================*/
        memset((char *)&uData, 0x00, sizeof(uData));
        uData.uData[0].iMode = MODE_NONE;
        uData.iCurrent_Index = 1;

        /*================*/
        /* 読み込みループ */
        /*================*/
        for(;;)
        {
                /*=============*/
                /* 1行読み込み */
                /*=============*/
                pcRet = fgets(cString, sizeof(cString), uFile_Read);
                if(pcRet == NULL)
                {
                        if(feof(uFile_Read) != 0)
                        {
                                break;
                        }
                        else
                        {
                                fprintf(stderr, "読み込み鰓\n");
                                break;
                        }
                }
                ++iRead_Line;

                /*======================*/
                /* 読み込んだ内容を分解 */
                /*======================*/
                memset((char *)uDiv, 0x00, sizeof(uDiv));
                iCol = Func_Div(cString, uDiv);

                /*==============================*/
                /* 分解したパーツ数分、繰り返し */
                /*==============================*/
                iCnt = 0;
                while(iCnt < iCol)
                {
                        /*------------------*/
                        /* 読み込み内容判定 */
                        /*------------------*/
                        iJudge_Col = Func_Extract(&(uDiv[iCnt]), &(uDiv[iCnt + 1]), &uData, puKeywords, &iResult);

                        if(iResult == RTN_OVERFLOW)
                        {
                                fprintf(stderr,
                                                "ファイル[%s]の行[%d]でネスト数が規定数を超えました。\n",
                                                pcFile_Read, iRead_Line);

                                /* ファイルクローズ */
                                fclose(uFile_Read);
                                fclose(uFile_Write);

                                return -1;
                        }
                        else if(iResult == RTN_ERR)
                        {
                                fprintf(stderr,
                                                "ファイル[%s]の行[%d]でキーワード出現不正を検出しました。\n",
                                                pcFile_Read, iRead_Line);

                                /* ファイルクローズ */
                                fclose(uFile_Read);
                                fclose(uFile_Write);

                                return -1;
                        }
                        /*----------*/
                        /* 出力判定 */
                        /*----------*/
                        if(iResult == RTN_KEYWORD)
                        {
                                if(puKeywords->iFlag_M1 == 1)
                                {
                                        Func_No_Output(uFile_Write, uDiv, iCnt, iJudge_Col);
                                }
                                iCnt += iJudge_Col;
                        }
                        else
                        {
                                iRet = Func_Judge(&uData);
                                if(iRet == RTN_OUTPUT)
                                {
                                        /* 出力する */
                                        Func_Output(uFile_Write, uDiv, iCnt, iJudge_Col);
                                }
                                else if(iRet == RTN_NOOUTPUT)
                                {
                                        if(puKeywords->iFlag_M1 == 1)
                                        {
                                                Func_No_Output(uFile_Write, uDiv, iCnt, iJudge_Col);
                                        }
                                }
                                iCnt += iJudge_Col;
                        }
                }
        }

        /*==========================*/
        /* 終了時のレベルをチェック */
        /*==========================*/
        if(uData.iCurrent_Index != 1)
        {
                fprintf(stderr,
                                "ファイル[%s]を最後まで処理しましが、キーワードの不整合を検知しました。\n",
                                pcFile_Read);

                /* ファイルクローズ */
                fclose(uFile_Read);
                fclose(uFile_Write);

                return -1;
        }

        /*==================*/
        /* ファイルクローズ */
        /*==================*/
        fclose(uFile_Read);
        fclose(uFile_Write);

        return 0;
}

/*===========================================================================*/
/*      行解析処理                                                                                                                               */
/*                                                                                                                                                       */
/*      行の解析を行い、状態が変化した場合にはpuDataを更新する。また、その行の種 */
/*      別を戻り値として返す。                                                                                                   */
/*                                                                                                                                                       */
/*      戻り値          RTN_KEYWORD             「#if」「#ifdef」「#ifndef」「#else」「#endi */
/*                                                              f」である場合。                                                          */
/*                              RTN_OVERFLOW    puDataがオーバーフローした場合                           */
/*                              RTN_ERR                 キーワードの出現位置不正を検出した場合           */
/*                              RTN_ETC                 その他                                                                           */
/*===========================================================================*/
int Func_Extract(STR_STRING *puDiv1, STR_STRING *puDiv2, STR_DATA *puData, STR_KEYWORD *puKeywords, int *piResult)
{
        int  iRet;
        int  iString_Size;
        int  iSub_Cnt;
        int  iColumn;
        char *pcTmp;
        char *pcPosition;
        char cDiv[30][30];
        char cString_Work[256];

#if     1       /* [ */
        if(puData->iInstrings == 0
                && puData->iComment == 0
                && strcmp(puDiv1->cData, "\"") == 0)
#else   /* ][ */
        if(puData->iInstrings == 0
                && strcmp(puDiv1->cData, "\"") == 0)
#endif  /* ] */
        {
                /* コメント中でなければ、文字列の開始 */
                puData->iInstrings = 1;
                *piResult = RTN_ETC;
                return 1;
        }

#if     1       /* [ */
        if(puData->iInstrings == 1
                && puData->iComment == 0
                && strcmp(puDiv1->cData, "\"") == 0)
#else   /* ][ */
        if(puData->iInstrings == 1
                && strcmp(puDiv1->cData, "\"") == 0)
#endif  /* ] */
        {
                /* コメント中でなければ、文字列の終了 */
                puData->iInstrings = 0;
                *piResult = RTN_ETC;
                return 1;
        }

        if(puData->iInstrings == 1)
        {
                /* 文字列中は判定しない */
                *piResult = RTN_ETC;
                return 1;
        }

        if(strcmp(puDiv1->cData, "/*") == 0)
        {
                puData->iComment = 1;
                *piResult = RTN_ETC;
                return 1;
        }

        if(strcmp(puDiv1->cData, "*/") == 0)
        {
                puData->iComment = 0;
                *piResult = RTN_ETC;
                return 1;
        }

        if(puData->iComment == 1)
        {
                /* コメント中の文字は判定外 */
                *piResult = RTN_ETC;
                return 1;
        }

        /*======*/
        /* 判定 */
        /*======*/
        if(strcmp(puDiv1->cData, "#if") == 0
                || strcmp(puDiv1->cData, "#ifdef") == 0)
        {
                /* オーバーフローチェック */
                if(puData->iCurrent_Index == C_DATAREC_MAX)
                {
                        /* オーバーフロー */
                        *piResult = RTN_OVERFLOW;
                        return 1;;
                }

                /*===========*/
                /* #ifの場合 */
                /*===========*/
                iRet = Func_Keyword_Judge(puDiv2->cData, puKeywords);
                if(iRet == RTN_ON)
                {
                        puData->uData[puData->iCurrent_Index].iMode = MODE_IF1;
                        ++(puData->iCurrent_Index);
                }
                if(iRet == RTN_OFF)
                {
                        puData->uData[puData->iCurrent_Index].iMode = MODE_IF0;
                        ++(puData->iCurrent_Index);
                }

                *piResult = RTN_KEYWORD;
                return 2;
        }
        if(strcmp(puDiv1->cData, "#ifndef") == 0)
        {
                /* オーバーフローチェック */
                if(puData->iCurrent_Index == C_DATAREC_MAX)
                {
                        /* オーバーフロー */
                        *piResult = RTN_OVERFLOW;
                        return 1;
                }

                /*===============*/
                /* #ifndefの場合 */
                /*===============*/
                iRet = Func_Keyword_Judge(puDiv2->cData, puKeywords);
                if(iRet == RTN_OFF)
                {
                        /* ifdefとON/OFFが逆 */
                        puData->uData[puData->iCurrent_Index].iMode = MODE_IF1;
                        ++(puData->iCurrent_Index);
                }
                if(iRet == RTN_ON)
                {
                        /* ifdefとON/OFFが逆 */
                        puData->uData[puData->iCurrent_Index].iMode = MODE_IF0;
                        ++(puData->iCurrent_Index);
                }

                *piResult = RTN_KEYWORD;
                return 2;
        }
        else if(strcmp(puDiv1->cData, "#else") == 0)
        {
                /* レベルチェック */
                if(puData->iCurrent_Index == 0)
                {
                        /* 不正（いきなり「#else」が出現した場合） */
                        *piResult = RTN_ERR;
                        return 1;
                }

                /*=============*/
                /* #elseの場合 */
                /*=============*/
                /* 逆転 */
                (puData->uData[(puData->iCurrent_Index) - 1].iMode) *= -1;

                *piResult = RTN_KEYWORD;
                return 1;
        }
        else if(strcmp(puDiv1->cData, "#endif") == 0)
        {
                /* レベルチェック */
                if(puData->iCurrent_Index == 0)
                {
                        /* 不正（いきなり「#endif」が出現した場合） */
                        *piResult = RTN_ERR;
                        return 1;
                }

                /*==============*/
                /* #endifの場合 */
                /*==============*/
                /* このレベルを終了 */
                puData->uData[puData->iCurrent_Index].iMode = MODE_NONE;
                --(puData->iCurrent_Index);

                *piResult = RTN_KEYWORD;
                return 1;
        }

        *piResult = RTN_ETC;
        return 1;
}

/*===========================================================================*/
/*      状態判定構造体の判定処理                                                                                                 */
/*                                                                                                                                                       */
/*      状態判定構造体を判定し、出力すべきか否かを判定する。                                     */
/*                                                                                                                                                       */
/*      戻り値          RTN_OUTPUT                                                                                                       */
/*                              RTN_NOOUTPUT                                                                                             */
/*===========================================================================*/
int Func_Judge(STR_DATA *puData)
{
        int             iFlag_IF1;
        int             iFlag_IF0;
        int             iCnt;

        iFlag_IF1 = 0;
        iFlag_IF0 = 0;
        for(iCnt = 0; iCnt < puData->iCurrent_Index; ++iCnt)
        {
                if(puData->uData[iCnt].iMode == MODE_NONE)
                {
                        continue;
                }

                if(puData->uData[iCnt].iMode == MODE_IF1)
                {
                        iFlag_IF1 |= 1;
                }
                else if(puData->uData[iCnt].iMode == MODE_IF0)
                {
                        iFlag_IF0 |= 1;
                }
        }

        if(iFlag_IF1 == 1 && iFlag_IF0 == 0)
        {
                return RTN_OUTPUT;
        }
        else if(iFlag_IF1 == 0 && iFlag_IF0 == 1)
        {
                return RTN_NOOUTPUT;
        }
        else if(iFlag_IF1 == 1 && iFlag_IF0 == 1)
        {
                return RTN_NOOUTPUT;
        }
        else if(iFlag_IF1 == 0 && iFlag_IF0 == 0)
        {
                return RTN_OUTPUT;
        }

        return 0;
}

int Func_Cut_Space(char *pcString)
{
        int             iCnt;
        char    cString_Work[256];

        /* 初期化 */
        memset(cString_Work, 0x00, sizeof(cString_Work));

        /* スペースが出現しない最初の位置を検索 */
        for(iCnt = 0; iCnt < strlen(pcString); ++iCnt)
        {
                if(*(pcString + iCnt) != ' ')
                {
                        break;
                }
        }

        if(iCnt != strlen(pcString))
        {
                /* スペースが出現しない位置からコピー */
                memcpy(cString_Work,
                                (char *)(pcString + iCnt), strlen(pcString) - iCnt);
        }
        else
        {
                /* 全部コピー */
                memcpy(cString_Work, pcString, strlen(pcString));
        }

        /* 末尾のスペースを削除 */
        iCnt = strlen(cString_Work) - 1;
        for(; iCnt >= 0; --iCnt)
        {
                if(cString_Work[iCnt] != ' ')
                {
                        break;
                }
        }
        if(iCnt != 0)
        {
                cString_Work[iCnt + 1] = '\0';
        }

        /* 結果をコピー */
        strcpy(pcString, cString_Work);

        return 0;
}

#if     0       /* [ */
#define         DEF_LIBETC_MODE_COMBINE         (1)
#define         DEF_LIBETC_MAXCOLUMN            (40)
#define         DEL_LIBETC_MAXARRAY                     (40)
#define         DEF_LIBETC_MAXSTRING            (256)

#define         RTN_COM_ERROR                           (-1)
#define         RTN_COM_ERROR_PARAM                     (-2)

int Func_Div_Csv(char *pcString, char *pcDiv, char (*pcArrayOut)[],
                   int iArrayMax, int iColumnMax)
{
        /* 変数宣言 */
    int  iRet;                                  /* 戻り値参照用              */

        /* 『CSV分解処理』を呼び出す */
    iRet = LIBetc_Div_Csv_Core(pcString, pcDiv, pcArrayOut, iArrayMax,
                               iColumnMax, DEF_LIBETC_MODE_COMBINE);

    return(iRet);
}

int LIBetc_Div_Csv_Core(char *pcString, char *pcDiv, char (*pcArrayOut)[],
                   int iArrayMax, int iColumnMax, int iFlag)
{
        /* 変数宣言 */
    int  iRetValue;                             /* 戻り値設定用              */
    char cWorkString[DEF_LIBETC_MAXCOLUMN + 1]; /* 分解対象文字列ワーク      */
    int  iArrayPoint;                           /* 配列位置                  */
    char *pcWork;                               /* ワーク用文字列ポインタ    */
    char *pcWorkBase;                           /* ワーク用文字列ポインタ    */

        /*==========*/
        /* 初期処理 */
        /*==========*/

        /* <戻り値>にRTN_COM_ERROR_PARAM（パラメータ異常）を設定する。 */
    iRetValue = RTN_COM_ERROR_PARAM;

        /*====================*/
        /* パラメータチェック */
        /*====================*/

        /* <分解対象文字列>のアドレスをチェックする。 */

        /* NULLの場合 */
    if(pcString == NULL)
    {
                /* 「処理終了」のラベルにジャンプする。 */
        goto LABEL_LIBetc_Div_Csv_END;

    }

        /* <区切り対象文字列>の文字列長をチェックする。 */
    if((int)strlen(pcString) <= 0 || (int)strlen(pcString) >= DEF_LIBETC_MAXSTRING)
    {
        /* 0以下または【分解対象文字列最大長】より大きい場合 */
                /* 「処理終了」のラベルにジャンプする。 */
        goto LABEL_LIBetc_Div_Csv_END;
    }

        /* <区切り文字>のアドレスをチェックする。 */
    if(pcDiv == NULL)
    {
        /* NULLの場合 */

                /* 「処理終了」のラベルにジャンプする。 */
        goto LABEL_LIBetc_Div_Csv_END;

    }

        /* <区切り文字>の文字列長をチェックする。 */
    if((int)strlen(pcDiv) <= 0)
    {
        /* 0以下の場合 */
                /* 「処理終了」のラベルにジャンプする。 */
        goto LABEL_LIBetc_Div_Csv_END;

    }

        /* <分解後格納配列>のアドレスをチェックする。 */

        /* NULLの場合 */
    if(pcArrayOut == NULL)
    {
                /* 「処理終了」のラベルにジャンプする。 */
        goto LABEL_LIBetc_Div_Csv_END;

    }

        /* 格納配列上限の値をチェックする。 */

        /* 0以下または【配列上限規定値】以上の場合 */
    if(iArrayMax <= 0 || iArrayMax > DEL_LIBETC_MAXARRAY)
    {
                /* 「処理終了」のラベルにジャンプする。 */
        goto LABEL_LIBetc_Div_Csv_END;

    }

        /* 一項目最大長の値をチェックする。 */

        /* 0以下または【一項目最大長規定値】以上の場合 */
    if(iColumnMax <= 0 || iColumnMax > DEF_LIBETC_MAXCOLUMN)
    {
                /* 「処理終了」のラベルにジャンプする。 */
        goto LABEL_LIBetc_Div_Csv_END;

    }

        /*==========*/
        /* 分解処理 */
        /*==========*/

        /* <戻り値>にRTN_COM_ERROR（異常終了）を設定する。 */
    iRetValue = RTN_COM_ERROR;

        /* <配列位置>を0で初期化する。 */
    pcWorkBase = pcString;

    for(iArrayPoint = 0; iArrayPoint < iArrayMax;)
    {
                /* ワークポインタにNULLを設定する。 */
        pcWork = NULL;

                /* <分解対象文字列ワーク>に対して<区切り文字>を検索し、先頭位置（ア  */
                /* ドレス）を<ワーク用文字列ポインタ>に設定する。                    */
        pcWork = strstr(pcWorkBase, pcDiv);

                /* NULL以外（<区切り文字>が存在する）場合 */
        if(pcWork != NULL)
        {

            memset(cWorkString, 0x00, sizeof(cWorkString));
/*----- Modified 2007.01.11 Higo -----*/
            memcpy(cWorkString, pcWorkBase,
                   (size_t)UMIN(pcWork - pcWorkBase, iColumnMax - 1));
            pcWorkBase = pcWork + strlen(pcDiv);

            if(iFlag == DEF_LIBETC_MODE_COMBINE && strlen(cWorkString) == 0)
            {
                continue;
            }

                        /* <分解後格納配列アドレス> + <配列位置> * 一項目最大長 から一項 */
                        /* 目最大長のメモリを0x00で初期化する。                          */
/*----- Modified 2007.01.11 Higo -----*/
            memset((char *)(*pcArrayOut + iArrayPoint * iColumnMax),
                   0x00, (size_t)iColumnMax);

                        /* <分解後格納配列アドレス> + <配列位置> * 一項目最大長 の位置に */
                        /* 、<ワーク用文字列ポインタ>からの値を最大、（<一項目最大長>-1）*/
                        /* バイト文字列としてコピーする。                                */
/*----- Modified 2007.01.11 Higo -----*/
            strncpy((char *)(*pcArrayOut + iArrayPoint * iColumnMax),
                    cWorkString, (size_t)(iColumnMax - 1) );

        }
        /* NULL（<区切り文字>が存在しない）場合 */
        else
        {

            memset(cWorkString, 0x00, sizeof(cWorkString));
            memcpy(cWorkString, pcWorkBase,
                   (size_t)UMIN((int)strlen(pcWorkBase), iColumnMax - 1) );


            if(iFlag == DEF_LIBETC_MODE_COMBINE && strlen(cWorkString) == 0)
            {
                break;
            }

                        /* <分解後格納配列アドレス> + <配列位置> * 一項目最大長 から一項 */
                        /* 目最大長のメモリを0x00で初期化する。                          */
/*----- Modified 2007.01.11 Higo -----*/
            memset((char *)(*pcArrayOut + iArrayPoint * iColumnMax),
                   0x00, (size_t)iColumnMax);

                        /* <分解後格納配列アドレス> + <配列位置> * 一項目最大長 の位置に */
                        /* 、<ワーク用文字列ポインタ>からの値を最大、（<一項目最大長>-1）*/
                        /* バイト文字列としてコピーする。                                */
/*----- Modified 2007.01.11 Higo -----*/
            strncpy((char *)(*pcArrayOut + iArrayPoint * iColumnMax),
                    cWorkString, (size_t)(iColumnMax - 1));

            ++iArrayPoint;

            break;
        }

        ++iArrayPoint;
    }

        /*==========*/
        /* 終了処理 */
        /*==========*/

        /* <戻り値>に<配列位置>を設定する。 */
        iRetValue = iArrayPoint;

        /* ラベル「処理終了」を設定する。 */
LABEL_LIBetc_Div_Csv_END:

        /* <戻り値>を返し、関数を終了する。 */
    return(iRetValue);

}
#endif  /* ] */

int Func_Add_Keyword(STR_KEYWORD *puKeywords, char *pcKeyword)
{
        if(puKeywords->iCnt == 100)
        {
                return -1;
        }

        memset(puKeywords->uRec[puKeywords->iCnt].cValue,
                        0x00, sizeof(puKeywords->uRec[puKeywords->iCnt].cValue));
        puKeywords->uRec[puKeywords->iCnt].iLen = strlen(pcKeyword);
        memcpy(puKeywords->uRec[puKeywords->iCnt].cValue,
                        pcKeyword, strlen(pcKeyword));

        ++(puKeywords->iCnt);

        return 0;
}

int Func_Keyword_Judge(char *pcString, STR_KEYWORD *puKeywords)
{
        int     iCnt;
        int     iRetValue = RTN_OFF;

        for(iCnt = 0; iCnt < puKeywords->iCnt; ++iCnt)
        {
                if(strcmp(puKeywords->uRec[iCnt].cValue, pcString) == 0)
                {
                        iRetValue = RTN_ON;
                        break;
                }
        }

        return iRetValue;
}

int Func_Div(char *pcString, STR_STRING *puOut)
{
        int             iIndex;
        int             iChar;
        int             iChar_Sub;
        int             iSep;


        iIndex = 0;
        iChar = 0;
        iChar_Sub = 0;
        while(*(pcString + iChar) != '\0')
        {
                if(*(pcString + iChar) == '\n'
                        || *(pcString + iChar) == '\t'
                        || *(pcString + iChar) == ' ')
                {
                        /* 文字列長をセット */
                        (puOut + iIndex)->iData_Len = iChar_Sub;

                        /* ヌル文字で止める */
                        (puOut + iIndex)->cData[iChar_Sub] = '\0';

                        /* 区切り文字を抜き出す */
                        iSep = 0;
                        for(;;)
                        {
                                if(*(pcString + iChar + iSep) != '\n'
                                        && *(pcString + iChar + iSep) != '\t'
                                        && *(pcString + iChar + iSep) != ' ')
                                {
                                        break;
                                }

                                (puOut + iIndex)->cSep[iSep] = *(pcString + iChar + iSep);
                                ++iSep;
                        }
                        (puOut + iIndex)->iSep_Len = iSep;
                        iChar += iSep;

                        /* 次へ */
                        ++iIndex;

                        /* クリア */
                        iChar_Sub = 0;
                }
                else if(memcmp((char *)(pcString + iChar), "/*", 2) == 0)
                {
                        /* 文字列長をセット */
                        (puOut + iIndex)->iData_Len = iChar_Sub;
                        /* ヌル文字で止める */
                        (puOut + iIndex)->cData[iChar_Sub] = '\0';
                        /* 区切り文字長は0 */
                        (puOut + iIndex)->cSep[0] = '\0';
                        (puOut + iIndex)->iSep_Len = 0;

                        /* 次へ */
                        ++iIndex;

                        strcpy((puOut + iIndex)->cData, "/*");
                        (puOut + iIndex)->iData_Len = 2;
                        (puOut + iIndex)->cSep[0] = '\0';
                        (puOut + iIndex)->iSep_Len = 0;

                        ++iIndex;
                        iChar_Sub = 0;

                        iChar += 2;
                }
                else if(memcmp((char *)(pcString + iChar), "*/", 2) == 0)
                {
                        /* 文字列長をセット */
                        (puOut + iIndex)->iData_Len = iChar_Sub;
                        /* ヌル文字で止める */
                        (puOut + iIndex)->cData[iChar_Sub] = '\0';
                        /* 区切り文字長は0 */
                        (puOut + iIndex)->cSep[0] = '\0';
                        (puOut + iIndex)->iSep_Len = 0;

                        /* 次へ */
                        ++iIndex;

                        strcpy((puOut + iIndex)->cData, "*/");
                        (puOut + iIndex)->iData_Len = 2;
                        (puOut + iIndex)->cSep[0] = '\0';
                        (puOut + iIndex)->iSep_Len = 0;

                        ++iIndex;
                        iChar_Sub = 0;

                        iChar += 2;
                }
                else if(memcmp((char *)(pcString + iChar), "\\\"", 2) == 0)
                {
                        /* 文字列長をセット */
                        (puOut + iIndex)->iData_Len = iChar_Sub;
                        /* ヌル文字で止める */
                        (puOut + iIndex)->cData[iChar_Sub] = '\0';
                        /* 区切り文字長は0 */
                        (puOut + iIndex)->cSep[0] = '\0';
                        (puOut + iIndex)->iSep_Len = 0;

                        /* 次へ */
                        ++iIndex;

                        strcpy((puOut + iIndex)->cData, "\\\"");
                        (puOut + iIndex)->iData_Len = 2;
                        (puOut + iIndex)->cSep[0] = '\0';
                        (puOut + iIndex)->iSep_Len = 0;

                        ++iIndex;
                        iChar_Sub = 0;

                        iChar += 2;
                }
                else if(memcmp((char *)(pcString + iChar), "\"", 1) == 0)
                {
                        /* 文字列長をセット */
                        (puOut + iIndex)->iData_Len = iChar_Sub;
                        /* ヌル文字で止める */
                        (puOut + iIndex)->cData[iChar_Sub] = '\0';
                        /* 区切り文字長は0 */
                        (puOut + iIndex)->cSep[0] = '\0';
                        (puOut + iIndex)->iSep_Len = 0;

                        /* 次へ */
                        ++iIndex;

                        strcpy((puOut + iIndex)->cData, "\"");
                        (puOut + iIndex)->iData_Len = 1;
                        (puOut + iIndex)->cSep[0] = '\0';
                        (puOut + iIndex)->iSep_Len = 0;

                        ++iIndex;
                        iChar_Sub = 0;

                        iChar += 1;
                }
                else
                {
                        (puOut + iIndex)->cData[iChar_Sub] = *(pcString + iChar);
                        ++iChar_Sub;
                        ++iChar;
                }

        }

        (puOut + iIndex)->iData_Len = iChar_Sub;
        (puOut + iIndex)->cData[iChar_Sub] = '\0';
        (puOut + iIndex)->cSep[0] = '\0';
        (puOut + iIndex)->iSep_Len = 0;
        ++iIndex;

        return iIndex;
}

int Func_Output(FILE *puFile_Write, STR_STRING *puDiv, int iCnt_Start, int iJudge_Col)
{
        int             iCnt;

        for(iCnt = 0; iCnt < iJudge_Col; ++iCnt)
        {
                if((puDiv + iCnt_Start + iCnt)->iData_Len > 0)
                {
                        fprintf(puFile_Write, "%s", (puDiv + iCnt_Start + iCnt)->cData);
                }

                if((puDiv + iCnt_Start + iCnt)->iSep_Len > 0)
                {
                        fprintf(puFile_Write, "%s", (puDiv + iCnt_Start + iCnt)->cSep);
                }
        }
        fflush(puFile_Write);

        return 0;
}

int Func_No_Output(FILE *puFile_Write, STR_STRING *puDiv, int iCnt_Start, int iJudge_Col)
{
        int             iSp;
        int             iCnt;

        for(iCnt = 0; iCnt < iJudge_Col; ++iCnt)
        {
                if((puDiv + iCnt_Start + iCnt)->iData_Len > 0)
                {
                        for(iSp = 0; iSp < (puDiv + iCnt_Start + iCnt)->iData_Len; ++iSp)
                        {
                                fprintf(puFile_Write, " ");
                        }
                }

                if((puDiv + iCnt_Start + iCnt)->iSep_Len > 0)
                {
                        fprintf(puFile_Write, "%s", (puDiv + iCnt_Start + iCnt)->cSep);
                }
        }
        fflush(puFile_Write);

        return 0;
}

