/*
	���ԏ��\���N���X
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
	�O���[�o��
*****************************************************************/
void FileTimeToTime(CONST FILETIME *lpft, time_t *lptime)
{
	// FILETIME��time_t�̊�_�̍��𓾂�
	CSystemTime st1(1601,1,0,1,0,0,0,0);	// 1601/01/01�c�cFILETIME�̊�_
	CSystemTime st2(1970,1,0,1,0,0,0,0);	// 1970/01/01�c�ctime_t�̊�_
	CFileTime ft1(st1);
	CFileTime ft2(st2);
	ULONGLONG ul = (ULONGLONG)ft2 - (ULONGLONG)ft1;	// 100��s�P�ʂ̍�
	// FILETIME����time_t�֕ϊ�
	ul = MAKELONG64(lpft->dwLowDateTime,lpft->dwHighDateTime) - ul;
	ul = ul / 10000000;	// FILETIME����time_t�֒P�ʕϊ�
	*lptime = (time_t)ul;
}
void TimeToFileTime(const time_t *lptime, FILETIME *lpft)
{
	// FILETIME��time_t�̊�_�̍��𓾂�
	CSystemTime st1(1601,1,0,1,0,0,0,0);	// 1601/01/01�c�cFILETIME�̊�_
	CSystemTime st2(1970,1,0,1,0,0,0,0);	// 1970/01/01�c�ctime_t�̊�_
	CFileTime ft1(st1);
	CFileTime ft2(st2);
	ULONGLONG ul = (ULONGLONG)ft2 - (ULONGLONG)ft1;	// 100��s�P�ʂ̍�
	// time_t����FILETIME�֕ϊ�
	ul += (ULONGLONG)(*lptime) * 10000000;	// time_t����FILETIME�֒P�ʕϊ�
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
