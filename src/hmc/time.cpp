/*
	時間情報表現クラス
	Copyright (C)1998-99 H.Mutsuki
*/

#include "util.h"
#include "time.h"

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
*****************************************************************/
/*****************************************************************
	グローバル
*****************************************************************/
void FileTimeToTime(CONST FILETIME *lpft, time_t *lptime)
{
	// FILETIMEとtime_tの基点の差を得る
	CSystemTime st1(1601,1,0,1,0,0,0,0);	// 1601/01/01……FILETIMEの基点
	CSystemTime st2(1970,1,0,1,0,0,0,0);	// 1970/01/01……time_tの基点
	CFileTime ft1(st1);
	CFileTime ft2(st2);
	ULONGLONG ul = (ULONGLONG)ft2 - (ULONGLONG)ft1;	// 100μs単位の差
	// FILETIMEからtime_tへ変換
	ul = MAKELONG64(lpft->dwLowDateTime,lpft->dwHighDateTime) - ul;
	ul = ul / 10000000;	// FILETIMEからtime_tへ単位変換
	*lptime = (time_t)ul;
}
void TimeToFileTime(const time_t *lptime, FILETIME *lpft)
{
	// FILETIMEとtime_tの基点の差を得る
	CSystemTime st1(1601,1,0,1,0,0,0,0);	// 1601/01/01……FILETIMEの基点
	CSystemTime st2(1970,1,0,1,0,0,0,0);	// 1970/01/01……time_tの基点
	CFileTime ft1(st1);
	CFileTime ft2(st2);
	ULONGLONG ul = (ULONGLONG)ft2 - (ULONGLONG)ft1;	// 100μs単位の差
	// time_tからFILETIMEへ変換
	ul += (ULONGLONG)(*lptime) * 10000000;	// time_tからFILETIMEへ単位変換
	lpft->dwLowDateTime  = LOLONG(ul);
	lpft->dwHighDateTime = HILONG(ul);
}

/*****************************************************************
*****************************************************************/
_HMC_END
/*****************************************************************
*****************************************************************/

/* Local Variables:						*/
/* tab-width:4							*/
/* c-basic-offset:4						*/
/* c-indent-level:4						*/
/* c-label-offset:-4					*/
/* c-continued-statement-offset:4		*/
/* file-coding-system:*sjis*dos			*/
/* buffer-file-coding-system:sjis-dos	*/
/* End:									*/
