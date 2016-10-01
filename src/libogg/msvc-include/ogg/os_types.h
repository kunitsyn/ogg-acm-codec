/*
	Ogg Vorbis 置き換えヘッダ
	Copyright (C)2000 H.Mutsuki
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __INC_SITE_OS_TYPE__
#define __INC_SITE_OS_TYPE__

#ifdef _DEBUG
// デバッグヒープ用小細工
// ※_CrtSetDbgFlag()で_CRTDBG_LEAK_CHECK_DFを
// 有効にしないとレポートされないので注意
#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#endif
#include "stdlib.h"
#include "malloc.h"
#include "crtdbg.h"
#endif

#include "../include/ogg/os_types.h"

#endif	/* __INC_SITE_OS_TYPE__ */

/* Local Variables:						*/
/* tab-width:4							*/
/* c-basic-offset:4						*/
/* c-indent-level:4						*/
/* c-label-offset:-4					*/
/* c-continued-statement-offset:4		*/
/* file-coding-system:*sjis*dos			*/
/* buffer-file-coding-system:sjis-dos	*/
/* End:									*/
