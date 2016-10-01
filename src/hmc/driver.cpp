/*
	ドライバーサポートクラス
	Copyright (C)1998 H.Mutsuki
*/

#include "util.h"
#include "driver.h"

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
	ドライバー基底クラス
*****************************************************************/
HDRVR CDrvr::create(LPCWSTR lpDriverName, LPCWSTR lpSectionName, LONG lParam)
{
	destroy();
	m_hdrvr = ::OpenDriver(lpDriverName,lpSectionName,lParam);
	return handle();
}
LRESULT CDrvr::close(LPARAM lParam1, LPARAM lParam2)
{
	LRESULT lRes = 0;
	if(handle()) {
		lRes = ::CloseDriver(handle(),lParam1,lParam2);
		m_hdrvr = NULL;
	}
	return lRes;
}

/*****************************************************************
	ドライバーハンドラクラス
*****************************************************************/
/*****************************************************************
	テンポラリマップの作成/削除
*****************************************************************/
void CDriverHandler::map(HDRVR hdrvr)
{
	if(BASE::handle()!=hdrvr) {
		if(isEnable() && hdrvr==NULL) {
			handleDisableProc();
		}
		BASE::map(hdrvr);	// 非オーナーハンドル更新
		if(isEnable() && BASE::isValid()) {
			handleEnableProc();
		}
	}
	DWORD nChilds = GetChildCount();
	for(DWORD n=0; n<nChilds; n++) {
		LPTYPE pChild = GetChild(n);
		ASSERT(pChild);
		pChild->map(hdrvr);
	}
}
/*****************************************************************
	オーナーハンドル更新
*****************************************************************/
void CDriverHandler::handle(HDRVR hdrvr)
{
	BASE::handle(hdrvr);		// オーナーハンドル更新
	DWORD nChilds = GetChildCount();
	for(DWORD n=0; n<nChilds; n++) {
		LPTYPE pChild = GetChild(n);
		ASSERT(pChild);
		pChild->map(hdrvr);
	}
}
/*****************************************************************
	ハンドラの有効無効切り替え
*****************************************************************/
BOOL CDriverHandler::enable()
{
	BOOL old = isEnable();
	if(!m_bEnable) {
		m_bEnable = TRUE;
		handleEnableProc();
	}
	return old;
}
BOOL CDriverHandler::disable()
{
	BOOL old = isEnable();
	if(!m_bEnable) {
		handleDisableProc();
		m_bEnable = FALSE;
	}
	return old;
}
void CDriverHandler::handleEnableProc()
{
	DWORD nChilds = GetChildCount();
	for(LONG n=nChilds-1; 0<=n; n--) {
		LPTYPE pChild = GetChild(n);
		ASSERT(pChild);
		pChild->handleEnableProc();
	}
	if(isEnable()) {
		enableProc();
	}
}
void CDriverHandler::handleDisableProc()
{
	DWORD nChilds = GetChildCount();
	for(LONG n=nChilds-1; 0<=n; n--) {
		LPTYPE pChild = GetChild(n);
		ASSERT(pChild);
		pChild->handleDisableProc();
	}
	if(isEnable()) {
		disableProc();
	}
}
/*****************************************************************
	チェインディスパッチャ
*****************************************************************/
// 高優先順位のdriverProcを呼び出す(一番最初の入り口)
LRESULT CDriverHandler::handleProc(UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
	for(LPTYPE p=end(); p; p=p->prev()) {
		if(p->isEnable()) {
			LPTYPE pChild = p->GetChild(0);
			if(pChild) {
				return pChild->handleProc(uMsg,lParam1,lParam2);
			}
			return p->driverProc(uMsg,lParam1,lParam2);
		}
	}
	return dmUnhandled(uMsg,lParam1,lParam2);
}
// 処理されないメッセージを次のdriverProcにリダイレクト
LRESULT CDriverHandler::unhandledProc(UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
	LPTYPE p = prev();
	for(; p; p=p->prev()) {
		if(p->isEnable()) {
			LPTYPE pChild = p->GetChild(0);
			if(pChild) {
				return pChild->handleProc(uMsg,lParam1,lParam2);
			}
			return p->driverProc(uMsg,lParam1,lParam2);
		}
	}
	if(p=GetOwner()) {
		if(p->isEnable()) {
			return p->driverProc(uMsg,lParam1,lParam2);
		} else {
			return p->unhandledProc(uMsg,lParam1,lParam2);
		}
	}
	return dmDefault(uMsg,lParam1,lParam2);
}
/*****************************************************************
	ディスパッチャ、ハンドラ
*****************************************************************/
// メッセージディスパッチャ
LRESULT CDriverHandler::driverProc(UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
#define METHOD(msg,handler) case msg: return handler(lParam1,lParam2);
	switch(uMsg) {
	HMC_CDRIVERHANDLER_ALIST
	default:
		return dmUnhandled(uMsg,lParam1,lParam2);
	}
#undef METHOD
}
// デフォルトハンドラ
LRESULT CDriverHandler::dmDefault(UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
	return ::DefDriverProc(DWORD(this),handle(),uMsg,lParam1,lParam2);
}
// 未ハンドル処理フィルタ
LRESULT CDriverHandler::dmUnhandled(UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
	return unhandledProc(uMsg,lParam1,lParam2);
}
// ハンドラ宣言本体
#if 0
#define METHOD(msg,handler) \
LRESULT CDriverHandler::handler(LPARAM lParam1, LPARAM lParam2) {	\
	return dmUnhandled(msg,lParam1,lParam2);	\
}
HMC_CDRIVERHANDLER_ALIST
#undef METHOD
#else
LRESULT CDriverHandler::dmLoad(LPARAM lParam1, LPARAM lParam2)
{
	return 1L;	// 非0で成功
}
LRESULT CDriverHandler::dmEnable(LPARAM lParam1, LPARAM lParam2)
{
	return 0L;	// 返り値は無視される
}
LRESULT CDriverHandler::dmOpen(LPARAM lParam1, LPARAM lParam2)
{
	return LRESULT(this);	// 次回からdwIdとして使用される
}
LRESULT CDriverHandler::dmClose(LPARAM lParam1, LPARAM lParam2)
{
	return 1L;	// 非0で成功
}
LRESULT CDriverHandler::dmDisable(LPARAM lParam1, LPARAM lParam2)
{
	return 0L;	// 返り値は無視される
}
LRESULT CDriverHandler::dmFree(LPARAM lParam1, LPARAM lParam2)
{
	return 0L;	// 返り値は無視される
}
LRESULT CDriverHandler::dmConfigure(LPARAM lParam1, LPARAM lParam2)
{
	return DRVCNF_OK;
	//return DRVCNF_CANCEL;
	//return DRVCNF_RESTART;
}
LRESULT CDriverHandler::dmQueryConfigure(LPARAM lParam1, LPARAM lParam2)
{
	return 0L;	// 非0でConfigure対応
}
LRESULT CDriverHandler::dmInstall(LPARAM lParam1, LPARAM lParam2)
{
	return DRVCNF_OK;
	//return DRVCNF_CANCEL;
	//return DRVCNF_RESTART;
}
LRESULT CDriverHandler::dmRemove(LPARAM lParam1, LPARAM lParam2)
{
	return 0L;	// 返り値は無視される
}
LRESULT CDriverHandler::dmExitSession(LPARAM lParam1, LPARAM lParam2)
{
	return 0L;	// 返り値は無視される
}
LRESULT CDriverHandler::dmPower(LPARAM lParam1, LPARAM lParam2)
{
	return 1L;	// 非0で成功
}
#endif
/*****************************************************************
	実用ドライバークラス
*****************************************************************/
/*****************************************************************
	メッセージディスパッチャ
*****************************************************************/
LRESULT CALLBACK CDriver::DriverProc(DWORD dwId, HDRVR hdrvr, UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
	LRESULT lRes = 0L;
	LPTYPE p = LPTYPE(dwId);
	do {
		if(uMsg==DRV_LOAD) {
			lRes = 1L;			// 0以外の値を返すと成功
			break;
		} else if(uMsg==DRV_OPEN) {
			p = new TYPE();		// 新規インスタンス作成
			if(!p) {
				lRes = 0L;		// 新規インスタンスの作成に失敗した場合は0を返す
				break;
			}
			p->map(hdrvr);		// ハンドルと関連付け(CDrvrがclose()するのを防ぐため非オーナーとする)
		}
		if(p) {
			if(p->handle()==hdrvr) {
				lRes = p->driverProc(uMsg,lParam1,lParam2);
				if(uMsg==DRV_OPEN) {
					lRes = (LRESULT)p;	// 返り値は次回からdwIdとして使われる
				} else if(uMsg==DRV_CLOSE) {
					delete p;	// インスタンス削除
				}
			} else {
				lRes = MMSYSERR_INVALHANDLE;
			}
		} else {
			lRes = ::DefDriverProc(dwId,hdrvr,uMsg,lParam1,lParam2);
		}
	} while(0);
	return lRes;
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
