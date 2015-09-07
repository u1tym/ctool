#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>
#include  <regex.h>

#include  "tool.h"

/******************************************************************************

関数名	正規表現に基づく文字列分解処理
Is_Match_Pattern(char *pcCom, int *piPos, char *pcPattern, char *pcValue, int *piValue_Size)

機能	正規表現に基づき、文字列分解を行なう。

機能詳細	指定された分解文字列（正規表現）を元に正規表現パターンを作成す
る。

指定文字列の指定された処理開始位置から正規表現に則ったチェック
を行なう。一致しなかった場合には、DEF_COM_FALSEを返し、関数を終
了する。

一致した場合には、指定文字列処理開始位置を一致した文字列数分、
インクリメントする。

また、分解後文字列格納先アドレス、分解後文字列サイズ格納先アド
レスがNULLではない場合には、一致した文字列およびそのサイズを格
納する。

RTN_COM_TRUEを返し、関数を終了する。

引数	I	pcCom	指定文字列
IO	piPos	指定文字列処理開始位置アドレス
I	pcPattern	分解文字列（正規表現）
O	pcValue	分解後文字列格納先アドレス
O	piValue_Size	分解後文字列サイズ格納先アドレス

戻り値	RTN_COM_TRUE	正常終了（一致）
DEF_COM_FALSE	正常終了（不一致）
RTN_COM_ERROR	異常終了

備考

******************************************************************************/
int Is_Match_Pattern(char *pcCom, int *piPos, char *pcPattern, char *pcValue, int *piValue_Size)
{
    int  iRet;	/* 戻り値参照用	*/
    int  iEqual;	/* 一致フラグ	*/
    int  iSize;	/* 一致サイズ	*/
    char cCheck_Pattern[ 256 + 1 ];        /* パターン作成用	*/
                                           /* ※パターン作成用のサイズ説明	*/
                                           /*  [SIZ_FS_FUNC_PARA_STR +	*/
                                           /*    "^"の1バイト +	*/
                                           /*    NULLの1バイト]	*/
    size_t    iMatch = 1;	/* パターンチェック用	*/
    regmatch_t uMatch[ 1 ];	/* パターンチェック用	*/
    regex_t    uReg;	/* パターン作成用	*/


    fprintf( stdout, "CHECK [%-16.16s] ptn=[%s]\n", ( char * )( pcCom + *piPos ), pcPattern );

    /*==============*/
    /* パターン作成 */
    /*==============*/
    sprintf(cCheck_Pattern, "^%s", pcPattern);
    iRet = regcomp(&uReg, cCheck_Pattern, REG_EXTENDED);
    if(iRet != 0)
    {
        return -1;
    }


/*==================*/
/* パターンチェック */
/*==================*/
iEqual = 1;
iRet = regexec(&uReg, (char *)(pcCom + *piPos), iMatch, uMatch, 0); if(iRet != 0) {
/* 不一致 */
iEqual = 0;
}

/*==============*/
/* パターン解放 */
/*==============*/
regfree(&uReg);


    /*==========*/
    /* 不一致時 */
    /*==========*/
    if(iEqual == 0)
    {
        fprintf( stdout, "     -> NO MATCH\n" );
        return 1;
    }


/* 一致時のサイズ取得 */
iSize = (int)(uMatch[0].rm_eo);


/*==============*/
/* 各変数に格納 */
/*==============*/
if(piValue_Size != NULL)
{
*piValue_Size = iSize;
}
if(pcValue != NULL)
{
memcpy(pcValue, (char *)(pcCom + *piPos), (size_t)iSize); }

*piPos += iSize;

    fprintf( stdout, "     -> MATCH\n" );
    return 0;
}








