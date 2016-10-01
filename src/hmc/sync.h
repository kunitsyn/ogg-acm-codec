/*
	同期オブジェクトクラス
	Copyright (C)1998 H.Mutsuki
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __INC_HMC_SYNC__
#define __INC_HMC_SYNC__

#include "util.h"

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
*****************************************************************/
class CCriticalSection;
class CSyncObject;
class CEvent;
class CMutex;
class CSemaphore;

/*****************************************************************
	グローバル
*****************************************************************/
// オブジェクトハンドルを複製する
HANDLE DuplicateHandle(HANDLE hSource);

/*****************************************************************
	クリティカルセクション
*****************************************************************/
class CCriticalSection : public CRITICAL_SECTION
{
public:
	DECLARE_SYNONYM(CCriticalSection,TYPE);
	DECLARE_SYNONYM(CRITICAL_SECTION,BASE);
	operator LPTYPE() { return this; }
public:
	CCriticalSection() { ::InitializeCriticalSection(this); }
	~CCriticalSection() { ::DeleteCriticalSection(this); }
public:
	void init() { ::InitializeCriticalSection(this); }
	void destroy() { ::DeleteCriticalSection(this); }
public:
	void enter() { ::EnterCriticalSection(this); }
	void leave() { ::LeaveCriticalSection(this); }
};
// 自動的に初期化、削除されないバージョン
class CCriticalSection0 : public CRITICAL_SECTION
{
public:
	DECLARE_SYNONYM(CCriticalSection0,TYPE);
	DECLARE_SYNONYM(CRITICAL_SECTION,BASE);
	operator LPTYPE() { return this; }
public:
	void init() { ::InitializeCriticalSection(this); }
	void destroy() { ::DeleteCriticalSection(this); }
public:
	void enter() { ::EnterCriticalSection(this); }
	void leave() { ::LeaveCriticalSection(this); }
};
/*****************************************************************
	同期オブジェクト基底クラス
*****************************************************************/
class CSyncObject
{
public:
	DECLARE_SYNONYM(CSyncObject,TYPE);
	//operator LPTYPE() { return this; }	// operator HANDLE()と競合するのでダメ
protected:
	CSyncObject() { init(); }
private:
	void init() { m_bOwner=TRUE; m_hObject=NULL; }
protected:
	void destroy() { if(isOwner()&&handle()) CHECKRESULT_API(::CloseHandle(handle())); init(); }
public:
	// テンポラリマップの作成
	void map(HANDLE h) { destroy(); m_bOwner=FALSE; m_hObject=h; }
public:
	// 各種操作
	BOOL wait(DWORD dwTimeout=INFINITE) const { DWORD dw=::WaitForSingleObject(handle(),dwTimeout); CHECKRESULT_API(dw!=WAIT_FAILED); return dw==WAIT_OBJECT_0; }
	BOOL set() { BOOL b=::SetEvent(handle()); CHECKRESULT_API(b); return b; }
	BOOL reset() { BOOL b=::ResetEvent(handle()); CHECKRESULT_API(b); return b; }
	BOOL pulse() { BOOL b=::PulseEvent(handle()); CHECKRESULT_API(b); return b; }
	BOOL isSignal() const { return wait(0); }
public:
	// 非オブジェクト操作
	static DWORD wait(DWORD nCount, CONST HANDLE *lpHandles, BOOL fWaitAll, DWORD dwTimeout);
	static DWORD wait(BOOL fWaitAll, DWORD dwTimeout, HANDLE h0) { HANDLE a[]={h0}; return wait(ARRAYLEN(a),a,fWaitAll,dwTimeout); }
	static DWORD wait(BOOL fWaitAll, DWORD dwTimeout, HANDLE h0, HANDLE h1) { HANDLE a[]={h0,h1}; return wait(ARRAYLEN(a),a,fWaitAll,dwTimeout); }
	static DWORD wait(BOOL fWaitAll, DWORD dwTimeout, HANDLE h0, HANDLE h1, HANDLE h2) { HANDLE a[]={h0,h1,h2}; return wait(ARRAYLEN(a),a,fWaitAll,dwTimeout); }
	static DWORD wait(BOOL fWaitAll, DWORD dwTimeout, HANDLE h0, HANDLE h1, HANDLE h2, HANDLE h3) { HANDLE a[]={h0,h1,h2,h3}; return wait(ARRAYLEN(a),a,fWaitAll,dwTimeout); }
	static DWORD wait(BOOL fWaitAll, DWORD dwTimeout, HANDLE h0, HANDLE h1, HANDLE h2, HANDLE h3, HANDLE h4) { HANDLE a[]={h0,h1,h2,h3,h4}; return wait(ARRAYLEN(a),a,fWaitAll,dwTimeout); }
	static DWORD wait(BOOL fWaitAll, DWORD dwTimeout, HANDLE h0, HANDLE h1, HANDLE h2, HANDLE h3, HANDLE h4, HANDLE h5) { HANDLE a[]={h0,h1,h2,h3,h4,h5}; return wait(ARRAYLEN(a),a,fWaitAll,dwTimeout); }
	static DWORD wait(BOOL fWaitAll, DWORD dwTimeout, HANDLE h0, HANDLE h1, HANDLE h2, HANDLE h3, HANDLE h4, HANDLE h5, HANDLE h6) { HANDLE a[]={h0,h1,h2,h3,h4,h5,h6}; return wait(ARRAYLEN(a),a,fWaitAll,dwTimeout); }
	static DWORD wait(BOOL fWaitAll, DWORD dwTimeout, HANDLE h0, HANDLE h1, HANDLE h2, HANDLE h3, HANDLE h4, HANDLE h5, HANDLE h6, HANDLE h7) { HANDLE a[]={h0,h1,h2,h3,h4,h5,h6,h7}; return wait(ARRAYLEN(a),a,fWaitAll,dwTimeout); }
public:
	// クラス情報の取得/設定
	HANDLE handle() const { return m_hObject; }
	operator HANDLE() const { return handle(); }
	BOOL isValid() const { return !!handle(); }
	BOOL isOwner() const { return m_bOwner; }
	BOOL ownership(BOOL f) { return m_bOwner=f; }
protected:
	HANDLE handle(HANDLE h) { return m_hObject=h; }
private:
	BOOL m_bOwner;
	HANDLE m_hObject;
};
/*****************************************************************
	イベント
*****************************************************************/
class CEvent : public CSyncObject
{
public:
	DECLARE_SYNONYM(CEvent,TYPE);
	DECLARE_SYNONYM(CSyncObject,BASE);
	operator LPTYPE() { return this; }
public:
	CEvent() { init(); }
	CEvent(BOOL fManual, BOOL fInitial, LPCTSTR lpName=NULL, LPSECURITY_ATTRIBUTES lpsa=NULL) { init(); create(fManual,fInitial,lpName,lpsa); }
	CEvent(LPCTSTR lpName, DWORD dwDesired=EVENT_ALL_ACCESS, BOOL bInherit=TRUE) { init(); create(lpName,dwDesired,bInherit); }
	CEvent(HANDLE hEvent) { init(); map(hEvent); }
	~CEvent() { destroy(); }
private:
	void init() {}
public:
	void destroy() { BASE::destroy(); init(); }
public:
	// オブジェクトの作成/削除
	HANDLE create(BOOL fManual, BOOL fInitial, LPCTSTR lpName=NULL, LPSECURITY_ATTRIBUTES lpsa=NULL);
	HANDLE create(LPCTSTR lpName, DWORD dwDesired=EVENT_ALL_ACCESS, BOOL bInherit=TRUE);
};
/*****************************************************************
	ミューテックス
*****************************************************************/
class CMutex : public CSyncObject
{
public:
	DECLARE_SYNONYM(CMutex,TYPE);
	DECLARE_SYNONYM(CSyncObject,BASE);
	operator LPTYPE() { return this; }
public:
	CMutex() { init(); }
	CMutex(BOOL fInitial, LPCTSTR lpName=NULL, LPSECURITY_ATTRIBUTES lpsa=NULL) { init(); create(fInitial,lpName,lpsa); }
	CMutex(LPCTSTR lpName, DWORD dwDesired=MUTEX_ALL_ACCESS, BOOL bInherit=TRUE) { init(); create(lpName,dwDesired,bInherit); }
	CMutex(HANDLE hMutex) { init(); map(hMutex); }
	~CMutex() { destroy(); }
private:
	void init() {}
public:
	void destroy() { BASE::destroy(); init(); }
public:
	// オブジェクトの作成/削除
	HANDLE create(BOOL fInitial, LPCTSTR lpName=NULL, LPSECURITY_ATTRIBUTES lpsa=NULL);
	HANDLE create(LPCTSTR lpName, DWORD dwDesired=MUTEX_ALL_ACCESS, BOOL bInherit=TRUE);
public:
	// 各種操作
	BOOL release() { BOOL b=::ReleaseMutex(handle()); CHECKRESULT_API(b); return b; }
};
/*****************************************************************
	セマフォ
*****************************************************************/
class CSemaphore : public CSyncObject
{
public:
	DECLARE_SYNONYM(CSemaphore,TYPE);
	DECLARE_SYNONYM(CSyncObject,BASE);
	operator LPTYPE() { return this; }
public:
	CSemaphore() { init(); }
	CSemaphore(LONG lInit, LONG lMax, LPCTSTR lpName=NULL, LPSECURITY_ATTRIBUTES lpsa=NULL) { init(); create(lInit,lMax,lpName,lpsa); }
	CSemaphore(LPCTSTR lpName, DWORD dwDesired=SEMAPHORE_ALL_ACCESS, BOOL bInherit=TRUE) { init(); create(lpName,dwDesired,bInherit); }
	CSemaphore(HANDLE hSemaphore) { init(); map(hSemaphore); }
	~CSemaphore() { destroy(); }
private:
	void init() {}
public:
	void destroy() { BASE::destroy(); init(); }
public:
	// オブジェクトの作成/削除
	HANDLE create(LONG lInit, LONG lMax, LPCTSTR lpName=NULL, LPSECURITY_ATTRIBUTES lpsa=NULL);
	HANDLE create(LPCTSTR lpName, DWORD dwDesired=SEMAPHORE_ALL_ACCESS, BOOL bInherit=TRUE);
};
/*****************************************************************
*****************************************************************/
_HMC_END
/*****************************************************************
*****************************************************************/

#endif	/* __INC_HMC_SYNC__ */

/* Local Variables:						*/
/* tab-width:4							*/
/* c-basic-offset:4						*/
/* c-indent-level:4						*/
/* c-label-offset:-4					*/
/* c-continued-statement-offset:4		*/
/* file-coding-system:*sjis*dos			*/
/* buffer-file-coding-system:sjis-dos	*/
/* End:									*/
