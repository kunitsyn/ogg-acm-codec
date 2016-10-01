/*
	同期オブジェクトクラス
	Copyright (C)1998 H.Mutsuki
*/

#include "util.h"
#include "sync.h"

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
*****************************************************************/
/*****************************************************************
	グローバル
*****************************************************************/
/*****************************************************************
	オブジェクトハンドルを複製する
*****************************************************************/
HANDLE DuplicateHandle(HANDLE hSource)
{
	HANDLE hTarget = INVALID_HANDLE_VALUE;
	::DuplicateHandle(GetCurrentProcess(),	// 複製元のプロセスのハンドル
					hSource,				// 複製するハンドル
					GetCurrentProcess(),	// 複製先のプロセスのハンドル
					&hTarget,				// 複製されたハンドルのアドレス
					0,//PROCESS_ALL_ACCESS,	// 複製ハンドルに対するアクセス
					TRUE,					// ハンドル継承フラグ
					DUPLICATE_SAME_ACCESS	// オプションの動作
					);
	CHECKRESULT_API(hTarget && hTarget != INVALID_HANDLE_VALUE);
	return hTarget;
}

/*****************************************************************
	同期オブジェクト基底クラス
*****************************************************************/
/*****************************************************************
	非オブジェクト操作
*****************************************************************/
DWORD CSyncObject::wait(DWORD nCount, CONST HANDLE *lpHandles, BOOL fWaitAll, DWORD dwTimeout)
{
	DWORD dw = 0;
	dw = ::WaitForMultipleObjects(nCount,lpHandles,fWaitAll,dwTimeout);
	CHECKRESULT_API(dw!=WAIT_FAILED);
	return dw;
}
/*****************************************************************
	イベント
*****************************************************************/
/*****************************************************************
	オブジェクトの作成/削除
*****************************************************************/
HANDLE CEvent::create(BOOL fManual, BOOL fInitial, LPCTSTR lpName, LPSECURITY_ATTRIBUTES lpsa)
{
	destroy();
	HANDLE h = ::CreateEvent(lpsa,fManual,fInitial,lpName);
	CHECKRESULT_API(h);
	BASE::handle(h);
	return h;
}
HANDLE CEvent::create(LPCTSTR lpName, DWORD dwDesired, BOOL bInherit)
{
	destroy();
	HANDLE h = ::OpenEvent(dwDesired,bInherit,lpName);
	CHECKRESULT_API(h);
	BASE::handle(h);
	return h;
}
/*****************************************************************
	ミューテックス
*****************************************************************/
/*****************************************************************
	オブジェクトの作成/削除
*****************************************************************/
HANDLE CMutex::create(BOOL fInitial, LPCTSTR lpName, LPSECURITY_ATTRIBUTES lpsa)
{
	destroy();
	HANDLE h = ::CreateMutex(lpsa,fInitial,lpName);
	CHECKRESULT_API(h);
	BASE::handle(h);
	return h;
}
HANDLE CMutex::create(LPCTSTR lpName, DWORD dwDesired, BOOL bInherit)
{
	destroy();
	HANDLE h = ::OpenMutex(dwDesired,bInherit,lpName);
	CHECKRESULT_API(h);
	BASE::handle(h);
	return h;
}
/*****************************************************************
	セマフォ
*****************************************************************/
/*****************************************************************
	オブジェクトの作成/削除
*****************************************************************/
HANDLE CSemaphore::create(LONG lInit, LONG lMax, LPCTSTR lpName, LPSECURITY_ATTRIBUTES lpsa)
{
	destroy();
	HANDLE h = ::CreateSemaphore(lpsa,lInit,lMax,lpName);
	CHECKRESULT_API(h);
	BASE::handle(h);
	return h;
}
HANDLE CSemaphore::create(LPCTSTR lpName, DWORD dwDesired, BOOL bInherit)
{
	destroy();
	HANDLE h = ::OpenSemaphore(dwDesired,bInherit,lpName);
	CHECKRESULT_API(h);
	BASE::handle(h);
	return h;
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
