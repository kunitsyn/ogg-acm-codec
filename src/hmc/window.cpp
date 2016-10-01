/*
	ウィンドウサポートクラス
	Copyright (C)1998-99 H.Mutsuki
*/

#include "util.h"
#include "string.h"
#include "sync.h"		// CEvent
#ifndef MSH_MOUSEWHEEL
#include <zmouse.h>		// MSH_MOUSEWHEEL
#endif
#include "window.h"

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
	グローバル
*****************************************************************/
/*****************************************************************
	スタイルの検査
*****************************************************************/
BOOL HaveWindowStyle(HWND hwnd, DWORD fdwStyle)
{
	RESETLASTERROR();
	LONG l = ::GetWindowLong(hwnd,GWL_STYLE);
	CHECKRESULT_API(l);
	return DWORD(l) & fdwStyle;
}
BOOL HaveWindowExStyle(HWND hwnd, DWORD fdwStyle)
{
	RESETLASTERROR();
	LONG l = ::GetWindowLong(hwnd,GWL_EXSTYLE);
	CHECKRESULT_API(l);
	return DWORD(l) & fdwStyle;
}
HWND GetOwner(HWND hwnd)
{
	HWND h = ::GetWindow(hwnd,GW_OWNER);
	CHECKRESULT_API(h);
	return h;
}
/*****************************************************************
	親ウィンドウのクライアント相対のウィンドウ矩形を得る
*****************************************************************/
BOOL GetWindowRectFromParent(HWND hwnd, LPRECT lprc)
{
	// GetWindowRect()の返す矩形はスクリーン相対であり、
	// MoveWindow(),SetWindowPos()に渡す座標は
	// 親ウィンドウ相対なので、そのための変換を行う
	BOOL bRes = CHECKRESULT_API(::GetWindowRect(hwnd,lprc));
	if(HaveWindowStyle(hwnd,WS_CHILD)) {
		CRect rc;
		CPoint &pt = *((CPoint*)(&rc));
		bRes &= CHECKRESULT_API(::GetClientRect(::GetParent(hwnd),&rc));
		bRes &= CHECKRESULT_NT(::ClientToScreen(::GetParent(hwnd),&pt));
		((CRect*)lprc)->offset(-pt.x,-pt.y);
	}
	return bRes;
}

/*****************************************************************
	子ウィンドウを親ウィンドウ中央に移動
*****************************************************************/
BOOL CenterWindow(HWND hwnd, HWND hParent, const RECT *lprc)
{
	ASSERT(::IsWindow(hwnd));

	// 親ウィンドウのクリップ範囲の取得
	CRect rcp;
	if(!::IsWindow(hParent) && !lprc) {
		::SystemParametersInfo(SPI_GETWORKAREA,0,&rcp,0);
	} else if(::IsWindow(hParent)&&lprc) {
		GetWindowRectFromParent(hParent,&rcp);
		CRect rcc = *lprc;
		rcc.offset(rcp.left,rcp.top);
		rcp.intersect(rcc);
	} else if(::IsWindow(hParent)) {
		GetWindowRectFromParent(hParent,&rcp);
	} else {
		rcp = *lprc;
	}
	int pw = rcp.width();
	int ph = rcp.height();

	// 現在のウィンドウ位置を取得
	CRect rc;
	GetWindowRectFromParent(hwnd,&rc);
	int w = rc.width();
	int h = rc.height();

	// 希望するウィンドウ位置を求める
	// ※親子関係がない場合は位置を補正
	int x = (pw - w) / 2;
	int y = (ph - h) / 2;
	if(::IsWindow(hParent) && !::IsChild(hParent,hwnd)) {
		x += rcp.left;
		y += rcp.top;
	}

	// ウィンドウを移動
	BOOL bRes = FALSE;
	if(x!=rc.left && y!=rc.top) {
		bRes = ::SetWindowPos(hwnd,HWND_NOTOPMOST,x,y,w,h,SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);
		CHECKRESULT_API(bRes);
	}
	return bRes;
}

/*****************************************************************
	小ウィンドウを親ウィンドウクライアント中央に移動
*****************************************************************/
BOOL CenterClient(HWND hwnd, HWND hParent, const RECT *lprc)
{
	ASSERT(::IsWindow(hwnd));

	// 親ウィンドウのクリップ範囲の取得
	CRect rcp;
	if(!::IsWindow(hParent) && !lprc) {
		::SystemParametersInfo(SPI_GETWORKAREA,0,&rcp,0);
	} else if(::IsWindow(hParent)&&lprc) {
		GetWindowRectFromParent(hParent,&rcp);
		CRect rcc = *lprc;
		rcc.offset(rcp.left,rcp.top);
		rcp.intersect(rcc);
	} else if(::IsWindow(hParent)) {
		GetWindowRectFromParent(hParent,&rcp);
	} else {
		rcp = *lprc;
	}
	int pw = rcp.width();
	int ph = rcp.height();

	// 現在のウィンドウ位置を取得
	CRect rc;
	GetWindowRectFromParent(hwnd,&rc);
	int w = rc.width();
	int h = rc.height();

	// 希望するウィンドウ位置を求める
	// ※親子関係がない場合は位置を補正
	int x = (pw - w) / 2;
	int y = (ph - h) / 2;
	if(::IsWindow(hParent) && !::IsChild(hParent,hwnd)) {
		POINT pt;
		pt.x = rcp.left;
		pt.y = rcp.top;
		CHECKRESULT_NT(::ClientToScreen(hParent,&pt));
		x += pt.x;
		y += pt.y;
	}

	// ウィンドウを移動
	BOOL bRes = FALSE;
	if(x!=rc.left && y!=rc.top) {
		bRes = ::SetWindowPos(hwnd,HWND_NOTOPMOST,x,y,w,h,SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);
		CHECKRESULT_API(bRes);
	}
	return bRes;
}

/*****************************************************************
	ウィンドウのサイズを変更する
*****************************************************************/
BOOL ResizeWindow(HWND hwnd, int w, int h, const RECT *lprc)
{
	// 現在のウィンドウ位置とサイズを取得
	CRect rc;
	GetWindowRectFromParent(hwnd,&rc);
	int x = rc.left;
	int y = rc.top;

	// lprcが有効な場合サイズ範囲をクリップ
	if(lprc) {
		CRect rc = *lprc;
		x = max(x,rc.left);
		y = max(y,rc.top);
		w = min(w,rc.width ());
		h = min(h,rc.height());
	}

	// ウィンドウを移動
	CHECKRESULT_API(::SetWindowPos(hwnd,HWND_NOTOPMOST,x,y,w,h,SWP_NOACTIVATE|SWP_NOZORDER));

	// サイズ変更の成功の可否をBOOL値で返す
	GetWindowRectFromParent(hwnd,&rc);
	return w==rc.width() && h==rc.height();
}

/*****************************************************************
	クライアント領域のサイズを変更する
*****************************************************************/
BOOL ResizeClient(HWND hwnd, int w, int h, const RECT *lprc)
{
	// ウィンドウスタイルを取得
	DWORD fdwStyle   = ::GetWindowLong(hwnd,GWL_STYLE);
	DWORD fdwExStyle = ::GetWindowLong(hwnd,GWL_EXSTYLE);
	BOOL  fMenu      = HaveMenu(hwnd);

	// 現在のウィンドウ位置とサイズを取得
	CRect rcw;
	GetWindowRectFromParent(hwnd, &rcw);
	int fx = rcw.left;
	int fy = rcw.top;
	int fw = rcw.width();
	int fh = rcw.height();

	// クライアント矩形をウィンドウ矩形に変換
	CRect rcc;
	CHECKRESULT_API(::GetClientRect(hwnd,&rcc));
	rcc.right  = rcc.left + w;	// 希望のサイズを代入
	rcc.bottom = rcc.top  + h;	// 希望のサイズを代入
	CHECKRESULT_API(::AdjustWindowRectEx(&rcc,fdwStyle,fMenu,fdwExStyle));

	// フレームウィンドウの位置とサイズのしきい値を取得
	// ※しきい値に関しては好き嫌いがあるかも……。
	CRect rc;
	if(lprc) {
		rc = *lprc;
	} else {
		if(::GetParent(hwnd)) {
			// MDIの場合はMDICLIENTの矩形になるはず
			CHECKRESULT_API(::GetClientRect(GetParent(hwnd),&rc));
		} else {
			// SDIの場合はワークエリア矩形を取得
			::SystemParametersInfo(SPI_GETWORKAREA,0,&rc,0);
		}
	}
	int tx = rc.left;		// フレームウィンドウの最少位置
	int ty = rc.top;		// フレームウィンドウの最少位置
	int tw = rc.width();	// フレームウィンドウの最大サイズ
	int th = rc.height();	// フレームウィンドウの最大サイズ

	// ウィンドウサイズを変更する(１回目)
	fw = min(tw,rcc.width ());
	fh = min(th,rcc.height());
	fx = min(max(tx,fx),tw-fw);
	fy = min(max(ty,fy),th-fh);
	CHECKRESULT_API(::SetWindowPos(hwnd,HWND_NOTOPMOST,fx,fy,fw,fh,SWP_NOACTIVATE|SWP_NOZORDER));

	// ウィンドウサイズの調整(リトライは２回まで)
	// ※サイズ範囲を優先したので位置が変更されることもある
	for(int i=0; i<2; i++) {
		CRect rcw,rcc;
		GetWindowRectFromParent(hwnd, &rcw);
		CHECKRESULT_API(::GetClientRect(hwnd, &rcc));
		fw = min(tw, w + (rcw.width ()-rcc.width ()));	// サイズを補正
		fh = min(th, h + (rcw.height()-rcc.height()));	// サイズを補正
		fx = min(max(tx, rcw.left), tw-fw);		// 位置を補正
		fy = min(max(ty, rcw.top ), th-fh);		// 位置を補正
		CHECKRESULT_API(::SetWindowPos(hwnd,HWND_NOTOPMOST,fx,fy,fw,fh,SWP_NOACTIVATE|SWP_NOZORDER));
	}

	// サイズ変更の成功の可否をBOOL値で返す
	CHECKRESULT_API(::GetClientRect(hwnd, &rcc));
	return w==rcc.width() && h==rcc.height();
}

#ifdef __AFXWIN_H__
BOOL ResizeClient(const ::CWnd *pWnd, int w, int h, const RECT *lprc)
{
	// pWndはCView派生型またはMDICLIENTの一時ポインタ
	// MDICLIENTの一時ポインタはCView派生型のMDI子ウィンドウから
	// GetParent()->GetParent()で取得できる(けど他に方法ないの？)

	// ウィンドウハンドルの取得
	HWND hwnd = HWND(*pWnd);
	HWND hfrm = ::GetParent(hwnd);
	if(!hfrm) hfrm = ::GetDesktopWindow();

	// フレームウィンドウの位置とサイズのしきい値を取得
	// ※しきい値に関しては好き嫌いがあるかも……。
	CRect rc;
	if(lprc) {
		rc = *lprc;
	} else {
		if(::GetParent(hfrm)) {
			// MDIの場合はMDICLIENTの矩形になるはず
			CHECKRESULT_API(::GetClientRect(::GetParent(hfrm),&rc));
		} else {
			// SDIの場合はワークエリア矩形を取得
			::SystemParametersInfo(SPI_GETWORKAREA,0,&rc,0);
		}
	}
	int tx = rc.left;		// フレームウィンドウの最少位置
	int ty = rc.top;		// フレームウィンドウの最少位置
	int tw = rc.width();	// フレームウィンドウの最大サイズ
	int th = rc.height();	// フレームウィンドウの最大サイズ

	// ウィンドウサイズの調整(リトライは３回まで)
	// ※サイズ範囲を優先したので位置が変更されることもある
	for(int i=0; i<3; i++) {
		CRect rcw,rcc;
		GetWindowRectFromParent(hfrm,&rcw);
		CHECKRESULT_API(::GetClientRect(hwnd,&rcc));
		int fw = min(tw,w+(rcw.width ()-rcc.width ()));	// サイズを補正
		int fh = min(th,h+(rcw.height()-rcc.height()));	// サイズを補正
		int fx = min(max(tx,rcw.left),tw-fw);		// 位置を補正
		int fy = min(max(ty,rcw.top ),th-fh);		// 位置を補正
		CHECKRESULT_API(::SetWindowPos(hfrm,HWND_NOTOPMOST,fx,fy,fw,fh,SWP_NOACTIVATE|SWP_NOZORDER));
	}

	// サイズ変更の成功の可否をBOOL値で返す
	CHECKRESULT_API(::GetClientRect(hwnd,&rc));
	return w==rc.width() && h==rc.height();
}
#endif

/*****************************************************************
	擬似メインウィンドウ取得
*****************************************************************/
HWND GetPseudoMainWindow()
{
	DWORD pid = ::GetCurrentProcessId();
	DWORD tid = ::GetCurrentThreadId();
	return GetPseudoMainWindow(pid,tid);
}
HWND GetPseudoMainWindow(HWND hwnd)
{
	DWORD pid = ::GetCurrentProcessId();
	DWORD tid = ::GetWindowThreadProcessId(hwnd,&pid);
	return GetPseudoMainWindow(pid,tid);
}
HWND GetPseudoMainWindow(DWORD dwProcessId, DWORD dwThreadId)
{
	HWND hwnd = NULL;
	CEnumWindows e;
	for(CWnd wnd(e.first()); wnd.isValid(); wnd.map(e.next())) {
		// プロセス一致,可視検査(必須条件)
		if(wnd.processId()==dwProcessId && wnd.isVisible()) {
			// スレッド一致()
			if(wnd.threadId()==dwThreadId) {
				hwnd = wnd.handle();	// 決定
				break;
			} else {
				LONG o = GetWindowZOrder(hwnd);
				LONG n = GetWindowZOrder(wnd.handle());
				if(abs(o)<abs(n)) {
					hwnd = wnd.handle();	// 候補
				}
			}
		}
	}
	return hwnd;
}

/*****************************************************************
	Zオーダー取得(正確性は低い)
*****************************************************************/
LONG GetWindowZOrder(HWND hwnd)
{
	LONG n = 0;
	if(::IsWindow(hwnd)) {
		HWND hcur = ::GetWindow(hwnd,GW_HWNDFIRST);
		while(hwnd!=hcur) {
			if(!hcur) {
				n = -1;
				break;
			}
			n++;
			hcur = ::GetWindow(hcur,GW_HWNDNEXT);
		}
	} else {
		n = -1;
	}
	return n;
}

/*****************************************************************
	メニュー関係
*****************************************************************/
BOOL HaveMenu(HWND hwnd)
{
	HMENU hMenu = ::GetMenu(hwnd);
	if(hMenu) {
		int nItem = ::GetMenuItemCount(hMenu);
		CHECKRESULT_API(nItem!=-1);
		return 0<nItem;
	} else {
		return FALSE;
	}
}

/*****************************************************************
	他のウィンドウの入力キューをアタッチする
*****************************************************************/
BOOL AttachThreadInput(HWND hWnd, BOOL fAttach)
{
	DWORD idAttach = ::GetCurrentThreadId();
	DWORD idAttachTo = ::GetWindowThreadProcessId(hWnd,NULL);
	BOOL bResult = ::AttachThreadInput(idAttach,idAttachTo,fAttach);
	// ※最近のMSDN Libraryには拡張エラー情報が無いので
	//   GetLastError()してはいけないとあることに注意
	//CHECKRESULT_API(bResult);
	return bResult;
}

/*****************************************************************
	クライアント座標でのカーソル位置を得る
*****************************************************************/
POINT GetClientCursorPos(HWND hWnd)
{
	CPoint pt(0,0);
	CHECKRESULT_API(::GetCursorPos(&pt));
	CHECKRESULT_NT(::ScreenToClient(hWnd, &pt));
	return pt;
}

/*****************************************************************
	ウィンドウハンドラクラス
*****************************************************************/
/*****************************************************************
	ハンドル更新・子オブジェクトへの伝搬
*****************************************************************/
void CWindowHandler::map(HWND hwnd)
{
	if(BASE::handle()!=hwnd) {
		if(isEnable() && hwnd==NULL) {
			handleDisableProc();
		}
		BASE::map(hwnd);	// 非オーナーハンドル更新
		if(isEnable() && BASE::isValid()) {
			handleEnableProc();
		}
	}
	DWORD nChilds = GetChildCount();
	for(DWORD n=0; n<nChilds; n++) {
		LPTYPE pChild = GetChild(n);
		ASSERT(pChild);
		pChild->map(hwnd);
	}
}
void CWindowHandler::handle(HWND hwnd)
{
	BASE::handle(hwnd);		// オーナーハンドル更新
	DWORD nChilds = GetChildCount();
	for(DWORD n=0; n<nChilds; n++) {
		LPTYPE pChild = GetChild(n);
		ASSERT(pChild);
		pChild->map(hwnd);
	}
}
/*****************************************************************
	ハンドラーチェイン操作
*****************************************************************/
CWindowHandler *CWindowHandler::SetOwner(LPTYPE lpObj, DWORD nIndex)
{
	LPTYPE old = GetOwner();
	DWORD id = ::GetCurrentThreadId();
	if((isValid() && threadId()!=id) || (lpObj && (lpObj->isValid() && lpObj->threadId()!=id))) {
		if(isValid()) {
			LINKTRUSTPARAMS param(this,lpObj,nIndex);
			send(WM_MANHANDLER,MH_SETOWNER,LPARAM(&param));
		} else {
			lpObj->AddChild(this,nIndex);
		}
	} else {
		m_link.setOwner(lpObj?&lpObj->m_link:NULL,nIndex);
		map(lpObj?lpObj->handle():NULL);	// 非オーナーハンドル更新
	}
	return old;
}
CWindowHandler *CWindowHandler::AddChild(LPTYPE lpObj, DWORD nIndex)
{
	LPTYPE old = GetChild(nIndex);
	DWORD id = ::GetCurrentThreadId();
	if((isValid() && threadId()!=id) || (lpObj && (lpObj->isValid() && lpObj->threadId()!=id))) {
		LINKTRUSTPARAMS param(this,lpObj,nIndex);
		send(WM_MANHANDLER,MH_ADDCHILD,LPARAM(&param));
	} else {
		lpObj->SetOwner(NULL,nIndex);	// 直前の親子関係を切断(必要ない？)
		m_link.addChild(&lpObj->m_link,nIndex);
		lpObj->map(handle());			// 非オーナーハンドル更新
	}
	return old;
}

/*****************************************************************
	ハンドラの有効無効切り替え
*****************************************************************/
BOOL CWindowHandler::enable()
{
	BOOL old = isEnable();
	if(isValid() && threadId()!=::GetCurrentThreadId()) {
		send(WM_MANHANDLER,MH_ENABLE,LPARAM(this));
	} else {
		if(!m_bEnable) {
			m_bEnable = TRUE;
			handleEnableProc();
		}
	}
	return old;
}
BOOL CWindowHandler::disable()
{
	BOOL old = isEnable();
	if(isValid() && threadId()!=::GetCurrentThreadId()) {
		send(WM_MANHANDLER,MH_DISABLE,LPARAM(this));
	} else {
		if(m_bEnable) {
			handleDisableProc();
			m_bEnable = FALSE;
		}
	}
	return old;
}
BOOL CWindowHandler::enableByDeferCommit()
{
	return post(WM_MANHANDLER,MH_ENABLE,LPARAM(this));
}
BOOL CWindowHandler::disableByDeferCommit()
{
	return post(WM_MANHANDLER,MH_DISABLE,LPARAM(this));
}
void CWindowHandler::handleEnableProc()
{
	DWORD nChilds = GetChildCount();
	for(LONG n=nChilds-1; 0<=n; n--) {
		LPTYPE pChild = GetChild(n);
		ASSERT(pChild);
		pChild->handleEnableProc();
	}
	if(BASE::isValid() && isEnable()) {
		enableProc();
	}
}
void CWindowHandler::handleDisableProc()
{
	DWORD nChilds = GetChildCount();
	for(LONG n=nChilds-1; 0<=n; n--) {
		LPTYPE pChild = GetChild(n);
		ASSERT(pChild);
		pChild->handleDisableProc();
	}
	if(BASE::isValid() && isEnable()) {
		disableProc();
	}
}

/*****************************************************************
	チェインディスパッチャ
*****************************************************************/
// メッセージフィルタブロードキャスト
BOOL CWindowHandler::handleTranslateMessage(LPMSG lpMsg)
{
	BOOL bRes = FALSE;
	for(LPTYPE p=end(); p&&!bRes; p=p->prev()) {
		if(p->isEnable()||WM_THROUGH<=lpMsg->message) {
			LPTYPE pChild = p->GetChild(0);
			if(pChild) {
				bRes = pChild->handleTranslateMessage(lpMsg);
			}
			if(!bRes) {
				bRes = p->translateProc(lpMsg);
			}
		}
	}
	return bRes;
}
// 高優先順位のwindowProcを呼び出す(一番最初の入り口)
LRESULT CWindowHandler::handleProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	for(LPTYPE p=end(); p; p=p->prev()) {
		if(p->isEnable()||WM_THROUGH<=uMsg) {
			LPTYPE pChild = p->GetChild(0);
			if(pChild) {
				return pChild->handleProc(uMsg,wParam,lParam);
			}
			return p->windowProc(uMsg,wParam,lParam);
		}
	}
	return wmUnhandled(uMsg,wParam,lParam);
}
// 処理されないメッセージを次のwindowProcにリダイレクト
LRESULT CWindowHandler::unhandledProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	for(LPTYPE p=prev(); p; p=p->prev()) {
		if(p->isEnable()||WM_THROUGH<=uMsg) {
			LPTYPE pChild = p->GetChild(0);
			if(pChild) {
				return pChild->handleProc(uMsg,wParam,lParam);
			}
			return p->windowProc(uMsg,wParam,lParam);
		}
	}
	if(LPTYPE p=GetOwner()) {
		if(p->isEnable()||WM_THROUGH<=uMsg) {
			return p->windowProc(uMsg,wParam,lParam);
		} else {
			return p->unhandledProc(uMsg,wParam,lParam);
		}
	}
	return wmDefault(uMsg,wParam,lParam);
}
// WM_CREATE直前処理ブロードキャスト
void CWindowHandler::handleBeforeCreate(LPCREATESTRUCT lpcs)
{
	DWORD nChilds = GetChildCount();
	for(LONG n=nChilds-1; 0<=n; n--) {
		LPTYPE pChild = GetChild(n);
		ASSERT(pChild);
		pChild->handleBeforeCreate(lpcs);
	}
	if(BASE::isValid() && isEnable()) {
		beforeCreateProc(lpcs);
	}
}
// WM_CREATE直後処理ブロードキャスト
void CWindowHandler::handleAfterCreate()
{
	DWORD nChilds = GetChildCount();
	for(LONG n=nChilds-1; 0<=n; n--) {
		LPTYPE pChild = GetChild(n);
		ASSERT(pChild);
		pChild->handleAfterCreate();
	}
	if(BASE::isValid() && isEnable()) {
		afterCreateProc();
	}
}
// WM_CLOSE直前処理ブロードキャスト
BOOL CWindowHandler::handleBeforeClose()
{
	BOOL bRes = TRUE;
	DWORD nChilds = GetChildCount();
	for(LONG n=nChilds-1; 0<=n; n--) {
		LPTYPE pChild = GetChild(n);
		ASSERT(pChild);
		bRes &= pChild->handleBeforeClose();
	}
	if(BASE::isValid() && isEnable()) {
		bRes &= beforeCloseProc();
	}
	return bRes;
}
// WM_CLOSE直後処理ブロードキャスト
BOOL CWindowHandler::handleAfterClose()
{
	BOOL bRes = TRUE;
	DWORD nChilds = GetChildCount();
	for(LONG n=nChilds-1; 0<=n; n--) {
		LPTYPE pChild = GetChild(n);
		ASSERT(pChild);
		bRes &= pChild->handleAfterClose();
	}
	if(/*BASE::isValid() &&*/ isEnable()) {	// ※この時点で既にハンドルは無効なのだ
		bRes &= afterCloseProc();
	}
	return bRes;
}

/*****************************************************************
	ディスパッチャ、ハンドラ
*****************************************************************/
// メッセージディスパッチャ
LRESULT CWindowHandler::windowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
#define METHOD(msg,handler) case msg: return handler(wParam,lParam);
	switch(uMsg) {
	HMC_CWINDOWHANDLER_ALIST
	case WM_MANHANDLER:
		return wmManHandler(wParam,lParam);
	default:
		if(WM_USER<=uMsg&&uMsg<=WM_USER+0x7fff) {
			return wmUser(uMsg,wParam,lParam);
		} else {
			return wmUnhandled(uMsg,wParam,lParam);
		}
	}
#undef METHOD
}
// スレッドを跨ぐウィンドウ作成処理
LRESULT CWindowHandler::wmCreateTrust(WPARAM wParam, LPARAM lParam)
{
	//LPTYPE pObj = LPTYPE(wParam);
	//LPCREATETRUSTPARAMS pParam = LPCREATETRUSTPARAMS(lParam);
	//※CREATETRUSTPARAMSはクラスローカルな構造
	//※ウィンドウ作成方法に応じてオーバーライドする(例えばダイアログなど)
	//※ここでは具体的なウィンドウが無いのでなにもしない
	return 0L;
}
// スレッドを跨ぐウィンドウ破棄処理
LRESULT CWindowHandler::wmDestroyTrust(WPARAM wParam, LPARAM lParam)
{
	SUGGEST(LPTYPE(wParam)==this && handle()==HWND(lParam));
	BOOL bRes = ::DestroyWindow(HWND(lParam));
	//※ハンドルの更新はここでは行わない
	//※正しく機能していればWindowProcで処理される(はず)
	CHECKRESULT_API(bRes);
	return bRes;
}
// リンク操作ハンドラ(通常オーバーライドしない)
LRESULT CWindowHandler::wmManHandler(WPARAM wParam, LPARAM lParam)
{
	// リンク操作(スレッド間メッセージ)
	switch(wParam) {
	case MH_GETOBJECT:
		return LRESULT(this);
	case MH_ENABLE:
		LPTYPE(lParam)->enable();
		return lParam;
	case MH_DISABLE:
		LPTYPE(lParam)->disable();
		return lParam;
	}
	LPLINKTRUSTPARAMS pParams = LPLINKTRUSTPARAMS(lParam);
	if(pParams->pContext==this) {
		LRESULT lRes = LRESULT(this);
		switch(wParam) {
		case MH_SETOWNER:
			lRes = (LRESULT)SetOwner(pParams->pObj,pParams->nIndex);
			break;
		case MH_ADDCHILD:
			lRes = (LRESULT)AddChild(pParams->pObj,pParams->nIndex);
			break;
		}
		return lRes;
	}
	return wmUnhandled(WM_MANHANDLER,wParam,lParam);
}
// WM_USER～WM_USER+0x7fff
LRESULT CWindowHandler::wmUser(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return wmUnhandled(uMsg,wParam,lParam);
}
// 未ハンドル処理フィルタ
LRESULT CWindowHandler::wmUnhandled(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return unhandledProc(uMsg,wParam,lParam);
}
// デフォルトハンドラ
LRESULT CWindowHandler::wmDefault(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return ::DefWindowProc(handle(),uMsg,wParam,lParam);
}
// ハンドラ宣言本体
#define METHOD(msg,handler) \
LRESULT CWindowHandler::handler(WPARAM wParam, LPARAM lParam) {	\
	return wmUnhandled(msg,wParam,lParam);	\
}
HMC_CWINDOWHANDLER_ALIST
#undef METHOD

/*****************************************************************
	実用ウィンドウクラス
*****************************************************************/
/*****************************************************************
	オブジェクトの作成/削除
*****************************************************************/
void CWindow::destroy()
{
#if 0
	OBJ::destroy();
#else
	//※CWndはスレッドを跨いだ操作を考慮していない
	//※スレッドが一致しない場合CWnd::destroy()は失敗する
	if(isOwner() && ::IsWindow(handle())) {
		DWORD tid0 = ::GetCurrentThreadId();
		DWORD tid1 = ::GetWindowThreadProcessId(handle(),NULL);
		if(tid0 == tid1) {
			::DestroyWindow(handle());
		} else {
			send(WM_DESTROYTRUST,WPARAM(this),LPARAM(handle()));
		}
		//ASSERT(!::IsWindow(handle()));
	}
#endif
	init();
}
HWND CWindow::create(LPCTSTR lpClass, LPCTSTR lpTitle, HWND hParent, HMENU hMenu, DWORD dwStyle, DWORD dwExStyle, int x, int y, int w, int h)
{
	return create(::GetModuleHandle(NULL),lpClass,lpTitle,hParent,hMenu,dwStyle,dwExStyle,x,y,w,h);
}
HWND CWindow::create(HINSTANCE hInstance, LPCTSTR lpClass, LPCTSTR lpTitle, HWND hParent, HMENU hMenu, DWORD dwStyle, DWORD dwExStyle, int x, int y, int w, int h)
{
	destroy();
	do {
		// ウィンドウクラスの登録
		TString<512> tmpClass;
		WNDCLASSEX wc;
		memset(&wc,0,sizeof(wc));
		if(!lpClass) {
			wc.cbSize			= sizeof(wc);
			wc.style			= wndclassStyle();
			wc.lpfnWndProc		= WindowProc;
			wc.cbClsExtra		= wndclassClsExtra();
			wc.cbWndExtra		= wndclassWndExtra();
			wc.hInstance		= hInstance;
			wc.hIcon			= wndclassIcon();
			wc.hCursor			= wndclassCursor();
			wc.hbrBackground	= wndclassBackground();
			wc.lpszMenuName		= wndclassMenuName();
			wc.lpszClassName	= wndclassClassName();
			wc.hIconSm			= wndclassSmallIcon();
			// ウィンドウクラスの登録
			DWORD dw = sizeof(tmpClass);
			if(!RegisterEngine(&wc,tmpClass,&dw)) break;
			lpClass = dw ? tmpClass : wc.lpszClassName;
		}
		// クラス指定もしくは引数に従ってcsを充填
		CREATESTRUCT cs;
		memset(&cs,0,sizeof(cs));
		cs.lpCreateParams	= this;
		cs.hInstance		= hInstance;
		cs.hMenu			= hMenu   ? hMenu   : createWindowMenu();
		cs.hwndParent		= hParent ? hParent : createWindowParent();
		cs.cy				= (h!=CW_USEDEFAULT) ? h : createWindowHeight();
		cs.cx				= (w!=CW_USEDEFAULT) ? w : createWindowWidth();
		cs.y				= (y!=CW_USEDEFAULT) ? y : createWindowPosY();
		cs.x				= (x!=CW_USEDEFAULT) ? x : createWindowPosX();
		cs.style			= dwStyle ? dwStyle : createWindowStyle();
		cs.lpszName			= lpTitle ? lpTitle : createWindowTitle();
		cs.lpszClass		= lpClass;
		cs.dwExStyle		= dwExStyle ? dwExStyle : createWindowExStyle();
		// ウィンドウの作成
		handleBeforeCreate(&cs);
		BASE::create(cs.dwExStyle,cs.lpszClass,cs.lpszName,cs.style,
					 cs.x,cs.y,cs.cx,cs.cy,cs.hwndParent,cs.hMenu,
					 cs.hInstance,cs.lpCreateParams);
		// 登録済みクラスで作成された場合のサブクラス化
		if(wc.lpfnWndProc != WindowProc) {
			m_pWndProc = (WNDPROC)::SetWindowLong(handle(),GWL_WNDPROC,(LONG)WindowProc);
			SetObject(handle(),this);
			handleAfterCreate();
		}
	} while(0);
	return handle();
}

// 他スレッドへcreate依頼(ウィンドウハンドルで相手スレッドを判別)
HWND CWindow::create(HWND hTrust, LPCTSTR lpClass, LPCTSTR lpTitle, HWND hParent, HMENU hMenu, DWORD dwStyle, DWORD dwExStyle, int x, int y, int w, int h)
{
	return create(hTrust,::GetModuleHandle(NULL),lpClass,lpTitle,hParent,hMenu,dwStyle,dwExStyle,x,y,w,h);
}
HWND CWindow::create(HWND hTrust, HINSTANCE hInstance, LPCTSTR lpClass, LPCTSTR lpTitle, HWND hParent, HMENU hMenu, DWORD dwStyle, DWORD dwExStyle, int x, int y, int w, int h)
{
	destroy();
	CREATESTRUCT cs;
	cs.lpCreateParams	= this;
	cs.hInstance		= hInstance;
	cs.hMenu			= hMenu;
	cs.hwndParent		= hParent;
	cs.cy				= h;
	cs.cx				= w;
	cs.y				= y;
	cs.x				= x;
	cs.style			= dwStyle;
	cs.lpszName			= lpTitle;
	cs.lpszClass		= lpClass;
	cs.dwExStyle		= dwExStyle;
	CREATETRUSTPARAMS params;
	params.hEvent	= NULL;
	params.lpcs		= &cs;
	::SendMessage(hTrust,WM_CREATETRUST,WPARAM(this),LPARAM(&params));
	return handle();
}

// 他スレッドへcreate依頼(スレッドIDで相手スレッドを判別)
HWND CWindow::create(DWORD dwTrust, LPCTSTR lpClass, LPCTSTR lpTitle, HWND hParent, HMENU hMenu, DWORD dwStyle, DWORD dwExStyle, int x, int y, int w, int h)
{
	return create(dwTrust,::GetModuleHandle(NULL),lpClass,lpTitle,hParent,hMenu,dwStyle,dwExStyle,x,y,w,h);
}
HWND CWindow::create(DWORD dwTrust, HINSTANCE hInstance, LPCTSTR lpClass, LPCTSTR lpTitle, HWND hParent, HMENU hMenu, DWORD dwStyle, DWORD dwExStyle, int x, int y, int w, int h)
{
	destroy();
	CEvent event(TRUE,FALSE);
	CREATESTRUCT cs;
	cs.lpCreateParams	= this;
	cs.hInstance		= hInstance;
	cs.hMenu			= hMenu;
	cs.hwndParent		= hParent;
	cs.cy				= h;
	cs.cx				= w;
	cs.y				= y;
	cs.x				= x;
	cs.style			= dwStyle;
	cs.lpszName			= lpTitle;
	cs.lpszClass		= lpClass;
	cs.dwExStyle		= dwExStyle;
	CREATETRUSTPARAMS params;
	params.hEvent	= event;
	params.lpcs		= &cs;
	if(::PostThreadMessage(dwTrust,WM_CREATETRUST,WPARAM(this),LPARAM(&params))) {
		event.wait(INFINITE);
	} else {
		CHECKRESULT_API(0);
	}
	return handle();
}
HWND CWindow::subclass(HWND hwnd)
{
	destroy();
	if(::IsWindow(hwnd)) {
		m_pWndProc = (WNDPROC)::SetWindowLong(hwnd,GWL_WNDPROC,(LONG)WindowProc);
		SetObject(hwnd,this);
		handle(hwnd);
		handleAfterCreate();
	}
	return handle();
}

/*****************************************************************
	メッセージループ(主にモーダルループに用いる)
*****************************************************************/
LRESULT CWindow::PumpMessage()
{
	MSG msg;
	while(::GetMessage(&msg,NULL,0,0)) {
		if(!PreTranslateMessage(&msg)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}
LRESULT CWindow::pumpMessage()
{
	LRESULT lRes = -1;
	if(isValid()) {
		MSG msg;
		while(::GetMessage(&msg,NULL,0,0)) {
			if(!PreTranslateMessage(&msg)) {
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
		lRes = msg.wParam;
	}
	return lRes;
}

/*****************************************************************
	作成/削除エンジン
*****************************************************************/
BOOL CWindow::RegisterEngine(LPWNDCLASSEX lpwcex, LPTSTR lpAltClass, LPDWORD lpcbAltClass)
{
	BOOL bRes = FALSE;
	do {
		WNDCLASSEX wc;
		memset(&wc, 0, sizeof(wc));
		wc.cbSize = sizeof(wc);
		// クラス名の重複チェック
		if(::GetClassInfoEx(lpwcex->hInstance,lpwcex->lpszClassName,&wc)) {
			// クラス定義の一致チェック
			if(lpwcex->hIconSm==NULL) {
				wc.hIconSm = NULL;	// 指定がなければ縮小アイコンは無視する
			}
			if(memcmp(&wc,lpwcex,sizeof(wc))!=0) {
				// テンポラリのクラス名を生成
				memcpy(&wc,lpwcex,sizeof(wc));
				TString<512> tmp(0,"%s:%x:%x:%x:%x:%x:%x",
							(DWORD)wc.lpszClassName,
							(DWORD)wc.hInstance,
							(DWORD)wc.style,
							(DWORD)wc.hCursor,
							(DWORD)wc.hbrBackground,
							(DWORD)wc.hIcon,
							(DWORD)wc.hIconSm
							);
				if(*lpcbAltClass < tmp.length()+1) break;
				*lpcbAltClass = tmp.length()+1;
				strcpy(lpAltClass,tmp);
				wc.lpszClassName = lpAltClass;
				lpwcex = &wc;
			} else {
				*lpcbAltClass = 0;
				bRes = TRUE;
				break;
			}
		} else {
			*lpcbAltClass = 0;
		}
		// ウィンドウクラスを登録
		ATOM atom = ::RegisterClassEx(lpwcex);
		CHECKRESULT_API(atom);
		bRes = !!atom;
	} while(0);
	return bRes;
}

BOOL CWindow::UnregisterEngine(LPCTSTR lpClassName, HINSTANCE hInstance)
{
	BOOL bRes = ::UnregisterClass(lpClassName,hInstance);
	if(!bRes) {
		// クラスに関連付けられたウィンドウが依然として
		// 存在する場合クラスは削除されない
		// これは予期通りの動作なのでエラー処理の必要はない
		if(::GetLastError()!=ERROR_CLASS_HAS_WINDOWS) {
			CHECKRESULT_API(0);
		}
	}
	return bRes;
}

/*****************************************************************
	ビルトインディスパッチャ
*****************************************************************/
BOOL CWindow::PreTranslateMessage(LPMSG lpMsg)
{
	BOOL bRes = FALSE;
	HWND hwnd = lpMsg->hwnd;
	UINT uMsg = lpMsg->message;
	WPARAM wParam = lpMsg->wParam;
	LPARAM lParam = lpMsg->lParam;
	if(hwnd==NULL) {
		switch(uMsg) {
		case WM_CREATETRUST:
			bRes = LPTYPE(wParam)->wmCreateTrust(wParam,lParam);
			break;
		case WM_DESTROYTRUST:
			bRes = LPTYPE(wParam)->wmDestroyTrust(wParam,lParam);
			break;
		}
	} else {
		for(;hwnd;hwnd=::GetParent(hwnd)) {
			if(!::IsWindowEnabled(hwnd)) break;
			LPTYPE pObj = GetObject(hwnd);
			if(pObj) {
				bRes = pObj->handleTranslateMessage(lpMsg);
			}
			if(bRes) break;
		}
	}
	return bRes;
}
LRESULT CALLBACK CWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg) {
	case WM_CREATETRUST:
		return LPTYPE(wParam)->wmCreateTrust(wParam,lParam);
	case WM_DESTROYTRUST:
		return LPTYPE(wParam)->wmDestroyTrust(wParam,lParam);
	}
	LPTYPE pObj = GetObject(hwnd);
	if(!pObj) {
		// WM_CREATEでオブジェクト関連付け
		if(uMsg==WM_CREATE) {
			LPCREATESTRUCT lpcs = LPCREATESTRUCT(lParam);
			pObj = LPTYPE(lpcs->lpCreateParams);
			SetObject(hwnd,pObj);
			pObj->hwnd(hwnd);	// オーナーハンドル更新
			if(pObj->handleProc(uMsg,wParam,lParam)==0) {
				pObj->handleAfterCreate();
				return 0;
			}
			pObj->hwnd(NULL);	// オーナーハンドル更新
			return -1;			// CreateWindow()失敗
		}
		return ::DefWindowProc(hwnd,uMsg,wParam,lParam);
	} else {
		// WM_CLOSEまたはWM_DESTROYでオブジェクト関連付け解除
		if(uMsg==WM_CLOSE||uMsg==WM_DESTROY||(uMsg==WM_SYSCOMMAND&&wParam==SC_CLOSE)) {
			if(pObj->handleBeforeClose()) {
				RemoveObject(hwnd);			// 循環防止のため最初に関連付けを解除する
				if(!pObj->isOwned()) {
					pObj->show(SW_HIDE);	// ※問題回避J047671
				}
				pObj->handleDisableProc();	// ココがdisableProc()を呼ぶ最後のチャンス
				TString<512> str;
				::GetClassName(hwnd,str,str.size());
				HMODULE hmod = (HMODULE)::GetClassLong(hwnd,GCL_HMODULE);
				pObj->handleProc(uMsg,wParam,lParam);
				if(pObj->handleAfterClose()==FALSE) {
					::PostQuitMessage(0);
				}
				pObj->hwnd(NULL);			// オーナーハンドル更新(伝搬)
				UnregisterEngine(str,hmod);
				return 1;
			}
			return 0;
		}
		return pObj->handleProc(uMsg,wParam,lParam);
	}
}
/*****************************************************************
	ディスパッチャ、ハンドラ
*****************************************************************/
// デフォルトハンドラ
LRESULT CWindow::wmDefault(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(m_pWndProc) {
		return ::CallWindowProc(m_pWndProc,hwnd(),uMsg,wParam,lParam);
	} else {
		return BASE::wmDefault(uMsg,wParam,lParam);
	}
}
// スレッドを跨ぐウィンドウ作成処理
LRESULT CWindow::wmCreateTrust(WPARAM wParam, LPARAM lParam)
{
	LPTYPE pObj = LPTYPE(wParam);
	LPCREATETRUSTPARAMS pParams = LPCREATETRUSTPARAMS(lParam);
	pObj->create(pParams->lpcs->lpszClass,
				 pParams->lpcs->lpszName,
				 pParams->lpcs->hwndParent,
				 pParams->lpcs->hMenu,
				 pParams->lpcs->style,
				 pParams->lpcs->dwExStyle,
				 pParams->lpcs->x,
				 pParams->lpcs->y,
				 pParams->lpcs->cx,
				 pParams->lpcs->cy);
	if(pParams->hEvent) {
		::SetEvent(pParams->hEvent);
	}
	return LPARAM(pObj->hwnd());
}
BOOL CWindow::beforeCloseProc()
{
	// 被所有ウィンドウにSC_CLOSEを送出(※擬似モードレス対策)
	//※WM_CLOSEでは一部のモーダルダイアログが終了しない場合があるので注意
	//※こんな基底でこの様な動作を定義してしまっていいのだろうか……。
	BOOL bRes = FALSE;
	while(!bRes) {
		bRes = TRUE;
		CEnumThreadWindows e;
		for(CWnd wnd(e.first()); bRes&&wnd.isValid(); wnd.map(e.next())) {
			if(CWindow::FromHandle(wnd) && wnd.haveStyle(WS_POPUP) && wnd.owner()==hwnd()) {
				wnd.send(WM_SYSCOMMAND,SC_CLOSE);
				bRes = FALSE;
			}
		}
	}
	return bRes;
}
/*****************************************************************
	フックハンドラ(ハンドラの挙動を置き換える)
*****************************************************************/
/*****************************************************************
	SDIフレームウィンドウ
*****************************************************************/
LRESULT CWindowHandlerFrame::wmSize(WPARAM wParam, LPARAM lParam)
{
#if 0
	WORD f = wParam;		// サイズ変更フラグ
	int w = LOWORD(lParam);	// クライアント領域の幅
	int h = HIWORD(lParam);	// クライアント領域の高さ
	for(HWND hwnd=::GetWindow(handle(),GW_CHILD); ::IsWindow(hwnd); hwnd=::GetWindow(hwnd,GW_HWNDNEXT)) {
		::MoveWindow(hwnd,0,0,w,h,TRUE);
	}
#endif
	return BASE::wmSize(wParam,lParam);
}
LRESULT CWindowHandlerFrame::wmWindowPosChanged(WPARAM wParam, LPARAM lParam)
{
	LPWINDOWPOS lpwp = (LPWINDOWPOS)lParam;
	if(!(lpwp->flags & SWP_NOSIZE)) {
		CRect rc;
		CHECKRESULT_API(::GetClientRect(hwnd(),&rc));
		int w = rc.width ();
		int h = rc.height();
		for(HWND hwnd=::GetWindow(handle(),GW_CHILD); ::IsWindow(hwnd); hwnd=::GetWindow(hwnd,GW_HWNDNEXT)) {
			::MoveWindow(hwnd,0,0,w,h,TRUE);
		}
	}
	return BASE::wmWindowPosChanged(wParam,lParam);
}
LRESULT CWindowHandlerFrame::wmActivate(WPARAM wParam, LPARAM lParam)
{
	BOOL fActive = LOWORD(wParam);			// activation flag
	BOOL fMinimized = (BOOL)HIWORD(wParam);	// minimized flag
	HWND hwndPrevious = (HWND)lParam;		// window handle
	for(HWND hwnd=::GetWindow(handle(),GW_CHILD); ::IsWindow(hwnd); hwnd=::GetWindow(hwnd,GW_HWNDNEXT)) {
		::EnableWindow(hwnd,fActive&&!fMinimized);
	}
	return BASE::wmActivate(wParam,lParam);
}
LRESULT CWindowHandlerFrame::wmKeyDown(WPARAM wParam, LPARAM lParam)
{
	for(HWND hwnd=::GetWindow(handle(),GW_CHILD); ::IsWindow(hwnd); hwnd=::GetWindow(hwnd,GW_HWNDNEXT)) {
		::PostMessage(hwnd,WM_KEYDOWN,wParam,lParam);
	}
	return BASE::wmKeyDown(wParam,lParam);
}
LRESULT CWindowHandlerFrame::wmKeyUp(WPARAM wParam, LPARAM lParam)
{
	for(HWND hwnd=::GetWindow(handle(),GW_CHILD); ::IsWindow(hwnd); hwnd=::GetWindow(hwnd,GW_HWNDNEXT)) {
		::PostMessage(hwnd,WM_KEYUP,wParam,lParam);
	}
	return BASE::wmKeyUp(wParam,lParam);
}
LRESULT CWindowHandlerFrame::wmPaletteChanged(WPARAM wParam, LPARAM lParam)
{
	for(HWND hwnd=::GetWindow(handle(),GW_CHILD); ::IsWindow(hwnd); hwnd=::GetWindow(hwnd,GW_HWNDNEXT)) {
		::SendMessage(hwnd,WM_PALETTECHANGED,wParam,lParam);
	}
	return BASE::wmPaletteChanged(wParam,lParam);
}
LRESULT CWindowHandlerFrame::wmPaletteIsChanging(WPARAM wParam, LPARAM lParam)
{
	for(HWND hwnd=::GetWindow(handle(),GW_CHILD); ::IsWindow(hwnd); hwnd=::GetWindow(hwnd,GW_HWNDNEXT)) {
		::SendMessage(hwnd,WM_PALETTEISCHANGING,wParam,lParam);
	}
	return BASE::wmPaletteIsChanging(wParam,lParam);
}
LRESULT CWindowHandlerFrame::wmQueryNewPalette(WPARAM wParam, LPARAM lParam)
{
	BOOL bRes = BASE::wmQueryNewPalette(wParam,lParam);
	for(HWND hwnd=::GetWindow(handle(),GW_CHILD); ::IsWindow(hwnd); hwnd=::GetWindow(hwnd,GW_HWNDNEXT)) {
		bRes |= ::SendMessage(hwnd,WM_QUERYNEWPALETTE,wParam,lParam);
	}
	return bRes;
}
BOOL CWindowHandlerFrame::beforeCloseProc()
{
	BOOL bRes = FALSE;
	// 被所有ウィンドウにSC_CLOSEを送出(※擬似モードレス対策)
	//※WM_CLOSEでは一部のモーダルダイアログが終了しない場合があるので注意
	while(!bRes) {
		bRes = TRUE;
		CEnumThreadWindows e;
		for(CWnd wnd(e.first()); bRes&&wnd.isValid(); wnd.map(e.next())) {
			if(CWindow::FromHandle(wnd) && wnd.haveStyle(WS_POPUP) && wnd.owner()==hwnd()) {
				wnd.send(WM_SYSCOMMAND,SC_CLOSE);
				bRes = FALSE;
			}
		}
	}
	// 被所有ウィンドウがすべて終了している場合だけ
	// 子ウィンドウにWM_CLOSEを送出(※beforeCloseProc()対策)
	if(bRes) {
		for(HWND hwnd=::GetWindow(handle(),GW_CHILD); ::IsWindow(hwnd); hwnd=::GetWindow(hwnd,GW_HWNDNEXT)) {
			bRes &= ::SendMessage(hwnd,WM_CLOSE,0,0L);
		}
	}
	return bRes;
}

/*****************************************************************
	ビューウィンドウ
*****************************************************************/
void CWindowHandlerView::beforeCreateProc(LPCREATESTRUCT lpcs)
{
	CRect rc;
	CHECKRESULT_API(::GetClientRect(lpcs->hwndParent,&rc));
	lpcs->cx = rc.width ();
	lpcs->cy = rc.height();
	lpcs->x  = rc.left;
	lpcs->y  = rc.top;
}

/*****************************************************************
	所有するポップアップを同期的に移動させる
*****************************************************************/
LRESULT CWindowHandlerTrackOwnedPopups::wmEnterSizeMove(WPARAM wParam, LPARAM lParam)
{
	m_bMoving = TRUE;
	m_nLastX = xPos();
	m_nLastY = yPos();
	return BASE::wmEnterSizeMove(wParam,lParam);
}
LRESULT CWindowHandlerTrackOwnedPopups::wmExitSizeMove(WPARAM wParam, LPARAM lParam)
{
	m_bMoving = FALSE;
	return BASE::wmExitSizeMove(wParam,lParam);
}
LRESULT CWindowHandlerTrackOwnedPopups::wmMoving(WPARAM wParam, LPARAM lParam)
{
	if(isMoving()) {
		m_nLastX = xPos();
		m_nLastY = yPos();
	}
	return BASE::wmMoving(wParam,lParam);
}
LRESULT CWindowHandlerTrackOwnedPopups::wmMove(WPARAM wParam, LPARAM lParam)
{
	if(isMoving()) {
		LONG cx = xPos() - m_nLastX;
		LONG cy = yPos() - m_nLastY;
		CEnumThreadWindows e;
		for(CWnd wnd(e.first()); wnd.isValid(); wnd.map(e.next())) {
			if(wnd.owner()==hwnd()) {
				wnd.move(wnd.xPos()+cx,wnd.yPos()+cy,TRUE);
			}
		}
	}
	return BASE::wmMove(wParam,lParam);
}
LRESULT CWindowHandlerTrackOwnedPopups::wmSizing(WPARAM wParam, LPARAM lParam)
{
	if(isMoving()) {
		m_nLastX = xPos();
		m_nLastY = yPos();
	}
	return BASE::wmSizing(wParam,lParam);
}
LRESULT CWindowHandlerTrackOwnedPopups::wmSize(WPARAM wParam, LPARAM lParam)
{
	if(isMoving()) {
		m_nLastX = xPos();
		m_nLastY = yPos();
	}
	return BASE::wmSize(wParam,lParam);
}

/*****************************************************************
	クライアントエリア内のみで有効なカーソル形状設定
*****************************************************************/
void CWindowHandlerSetCursor::enableProc()
{
	BASE::enableProc();
	post(WM_SETCURSOR,WPARAM(hwnd()),MAKELPARAM(HTCLIENT,0));
}
void CWindowHandlerSetCursor::disableProc()
{
	post(WM_SETCURSOR,WPARAM(hwnd()),MAKELPARAM(HTCLIENT,0));
	BASE::disableProc();
}
LRESULT CWindowHandlerSetCursor::wmSetCursor(WPARAM wParam, LPARAM lParam)
{
	if(HWND(wParam)==hwnd() && LOWORD(lParam)==HTCLIENT) {
		::SetCursor(handle());
		return TRUE;
	}
	return BASE::wmSetCursor(wParam,lParam);
}
HCURSOR CWindowHandlerSetCursor::handle(HCURSOR h)
{
	m_hCursor = h;
	post(WM_SETCURSOR,WPARAM(hwnd()),MAKELPARAM(HTCLIENT,0));
	return handle();
}

/*****************************************************************
	マウスカーソル移動
*****************************************************************/
void CWindowHandlerMouseMove::init()
{
	m_nCounter = 0;
	m_nTimerId = 0;
	m_ptLast.let(0,0);
	m_ptStart.let(0,0);
	m_ptGoal.let(0,0);
}
LRESULT CWindowHandlerMouseMove::wmTimer(WPARAM wParam, LPARAM lParam)
{
	if(wParam==m_nTimerId && isMoving()) {
		CPoint pt;
		LONG dx = m_ptGoal.x-m_ptStart.x;
		LONG dy = m_ptGoal.y-m_ptStart.y;
		LONG dv = max((max(abs(dx),abs(dy))/64),1);
		pt.x = m_ptStart.x + dx * m_nCounter / dv;
		pt.y = m_ptStart.y + dy * m_nCounter / dv;
		if(pt.isEqual(m_ptGoal)) {
			m_nCounter = 0;
			endTimer();
		}
		m_ptLast.let(pt);
		CHECKRESULT_NT(::ClientToScreen(handle(),&pt));
		CHECKRESULT_API(::SetCursorPos(pt.x,pt.y));
		m_nCounter++;
	}
	return BASE::wmTimer(wParam,lParam);
}
LRESULT CWindowHandlerMouseMove::wmMouseMove(WPARAM wParam, LPARAM lParam)
{
	if(isMoving()) {
		CPoint pt(LOWORD(lParam),HIWORD(lParam));
		if(!pt.isEqual(m_ptLast)) {
			endTimer();
		}
	}
	return BASE::wmMouseMove(wParam,lParam);
}
void CWindowHandlerMouseMove::move(int x, int y)
{
	CHECKRESULT_API(::GetCursorPos(&m_ptLast));
	CHECKRESULT_NT(::ScreenToClient(handle(),&m_ptLast));
	m_ptStart.let(m_ptLast);
	m_ptGoal.let(x,y);
	m_nCounter = 1;
	beginTimer();
}
void CWindowHandlerMouseMove::beginTimer()
{
	//endTimer();
	m_nTimerId = ::SetTimer(handle(),UINT(this),10,NULL);
	CHECKRESULT_API(m_nTimerId);
}
void CWindowHandlerMouseMove::endTimer()
{
	if(m_nTimerId && isValid()) {
		if(CWnd::isValid()) {
			CHECKRESULT_API(::KillTimer(handle(),m_nTimerId));
		}
		m_nTimerId = 0;
	}
}

/*****************************************************************
	シングルクリックメッセージ変換
*****************************************************************/
void CWindowHandlerSingleClick::init()
{
	m_bLButton = FALSE;
	m_bMButton = FALSE;
	m_bRButton = FALSE;
	m_ptLButton.let(-1,-1);
	m_ptMButton.let(-1,-1);
	m_ptRButton.let(-1,-1);
}
void CWindowHandlerSingleClick::enableProc()
{
	init();
	CHECKRESULT_API(::ReleaseCapture());
	BASE::enableProc();
}
void CWindowHandlerSingleClick::disableProc()
{
	init();
	CHECKRESULT_API(::ReleaseCapture());
	BASE::disableProc();
}
LRESULT CWindowHandlerSingleClick::wmLButtonDown(WPARAM wParam, LPARAM lParam)
{
	if(m_bMButton || m_bMButton || m_bRButton) {
		m_bLButton = FALSE;
		m_bMButton = FALSE;
		m_bRButton = FALSE;
		CHECKRESULT_API(::ReleaseCapture());
	} else {
		m_bLButton = TRUE;
		m_ptLButton.let(LOWORD(lParam),HIWORD(lParam));
		::SetCapture(handle());
	}
	return BASE::wmLButtonDown(wParam,lParam);
}
LRESULT CWindowHandlerSingleClick::wmMButtonDown(WPARAM wParam, LPARAM lParam)
{
	if(m_bMButton || m_bMButton || m_bRButton) {
		m_bLButton = FALSE;
		m_bMButton = FALSE;
		m_bRButton = FALSE;
		CHECKRESULT_API(::ReleaseCapture());
	} else {
		m_bMButton = TRUE;
		m_ptMButton.let(LOWORD(lParam),HIWORD(lParam));
		::SetCapture(handle());
	}
	return BASE::wmMButtonDown(wParam,lParam);
}
LRESULT CWindowHandlerSingleClick::wmRButtonDown(WPARAM wParam, LPARAM lParam)
{
	if(m_bMButton || m_bMButton || m_bRButton) {
		m_bLButton = FALSE;
		m_bMButton = FALSE;
		m_bRButton = FALSE;
		CHECKRESULT_API(::ReleaseCapture());
	} else {
		m_bRButton = TRUE;
		m_ptRButton.let(LOWORD(lParam),HIWORD(lParam));
		::SetCapture(handle());
	}
	return BASE::wmRButtonDown(wParam,lParam);
}
LRESULT CWindowHandlerSingleClick::wmLButtonUp(WPARAM wParam, LPARAM lParam)
{
	if(m_bMButton || m_bRButton) {
		m_bLButton = FALSE;
		m_bMButton = FALSE;
		m_bRButton = FALSE;
		CHECKRESULT_API(::ReleaseCapture());
	} else if(m_bLButton) {
		m_bLButton = FALSE;
		if(!(m_bMButton||m_bRButton)) {
			CHECKRESULT_API(::ReleaseCapture());
		}
		CRect rc;
		clientRect(rc);
		if(rc.isInside(LOWORD(lParam),HIWORD(lParam))) {
			wParam = MAKEWPARAM(m_ptLButton.x,m_ptLButton.y);
			post(WM_LBUTTONCLK,wParam,lParam);
		}
	}
	return BASE::wmLButtonUp(wParam,lParam);
}
LRESULT CWindowHandlerSingleClick::wmMButtonUp(WPARAM wParam, LPARAM lParam)
{
	if(m_bLButton || m_bRButton) {
		m_bLButton = FALSE;
		m_bMButton = FALSE;
		m_bRButton = FALSE;
		CHECKRESULT_API(::ReleaseCapture());
	} else if(m_bMButton) {
		m_bMButton = FALSE;
		if(!(m_bLButton||m_bRButton)) {
			CHECKRESULT_API(::ReleaseCapture());
		}
		CRect rc;
		clientRect(rc);
		if(rc.isInside(LOWORD(lParam),HIWORD(lParam))) {
			wParam = MAKEWPARAM(m_ptMButton.x,m_ptMButton.y);
			post(WM_MBUTTONCLK,wParam,lParam);
		}
	}
	return BASE::wmMButtonUp(wParam,lParam);
}
LRESULT CWindowHandlerSingleClick::wmRButtonUp(WPARAM wParam, LPARAM lParam)
{
	if(m_bLButton || m_bMButton) {
		m_bLButton = FALSE;
		m_bMButton = FALSE;
		m_bRButton = FALSE;
		CHECKRESULT_API(::ReleaseCapture());
	} else if(m_bRButton) {
		m_bRButton = FALSE;
		if(!(m_bLButton||m_bMButton)) {
			CHECKRESULT_API(::ReleaseCapture());
		}
		CRect rc;
		clientRect(rc);
		if(rc.isInside(LOWORD(lParam),HIWORD(lParam))) {
			wParam = MAKEWPARAM(m_ptRButton.x,m_ptRButton.y);
			post(WM_RBUTTONCLK,wParam,lParam);
		}
	}
	return BASE::wmRButtonUp(wParam,lParam);
}
LRESULT CWindowHandlerSingleClick::wmCaptureChanged(WPARAM wParam, LPARAM lParam)
{
	HWND hNewCapture = HWND(lParam);
	if(handle() != hNewCapture) {
		m_bLButton = FALSE;
		m_bMButton = FALSE;
		m_bRButton = FALSE;
	}
	return BASE::wmCaptureChanged(wParam,lParam);
}
LRESULT CWindowHandlerSingleClick::wmCancelMode(WPARAM wParam, LPARAM lParam)
{
	m_bLButton = FALSE;
	m_bMButton = FALSE;
	m_bRButton = FALSE;
	return BASE::wmCancelMode(wParam,lParam);
}

/*****************************************************************
	クライアントエリアドラッグ
*****************************************************************/
void CWindowHandlerClientDrag::init()
{
	m_bMoving = FALSE;
	m_ptCursor.let(0,0);
	m_ptWindow.let(0,0);
}
LRESULT CWindowHandlerClientDrag::wmLButtonDown(WPARAM wParam, LPARAM lParam)
{
	CPoint pt(LOWORD(lParam),HIWORD(lParam));
	CHECKRESULT_NT(::ClientToScreen(handle(),pt));
	m_wnd.map(handle());
	if(m_wnd.isChild()) m_wnd.map(m_wnd.parent());
	m_ptCursor = pt;
	m_ptWindow = CPoint(m_wnd.xPos(),m_wnd.yPos());
	m_bMoving = FALSE;
	::SetCapture(handle());
	return BASE::wmLButtonDown(wParam,lParam);
}
LRESULT CWindowHandlerClientDrag::wmLButtonUp(WPARAM wParam, LPARAM lParam)
{
	if(::GetCapture()==handle()) {
		m_bMoving = FALSE;
		CHECKRESULT_API(::ReleaseCapture());
	}
	return BASE::wmLButtonUp(wParam,lParam);
}
LRESULT CWindowHandlerClientDrag::wmMouseMove(WPARAM wParam, LPARAM lParam)
{
	if(::GetCapture()==handle()) {
		CPoint pt(LOWORD(lParam),HIWORD(lParam));
		CHECKRESULT_NT(::ClientToScreen(handle(),pt));
		int dx = m_ptCursor.x - pt.x;
		int dy = m_ptCursor.y - pt.y;
		if(!m_bMoving) {
			int tx = ::GetSystemMetrics(SM_CXDRAG);
			int ty = ::GetSystemMetrics(SM_CYDRAG);
			if(abs(dx) >= tx || abs(dy) >= ty) {
				m_bMoving = TRUE;
			}
		}
		if(m_bMoving) {
			int x = m_ptWindow.x - dx;
			int y = m_ptWindow.y - dy;
			// ※SPI_GETDRAGFULLWINDOWSの値に依らず
			// 必ずフルドラッグになる(本当は直したい)
			CHECKRESULT_API(::SetWindowPos(m_wnd,HWND_NOTOPMOST,x,y,0,0,SWP_NOSIZE|SWP_NOZORDER));
		}
	}
	return BASE::wmMouseMove(wParam,lParam);
}
LRESULT CWindowHandlerClientDrag::wmCaptureChanged(WPARAM wParam, LPARAM lParam)
{
	HWND hNewCapture = HWND(lParam);
	if(handle() != hNewCapture) {
		m_bMoving = FALSE;
	}
	return BASE::wmCaptureChanged(wParam,lParam);
}

/*****************************************************************
	WM_QUERYCANSELAUTOPLAYメッセージ変換
*****************************************************************/
void CWindowHandlerTrackQueryCancelAutoPlay::init()
{
	m_uQueryCancelAutoPlay = ::RegisterWindowMessage(TEXT("QueryCancelAutoPlay"));
	CHECKRESULT_API(m_uQueryCancelAutoPlay);
}
void CWindowHandlerTrackQueryCancelAutoPlay::destroy()
{
	// ココでは特に何もしない
	// RegisterWindowMessage()で登録したメッセージは
	// Windowsセッションが終了するまで永続的に登録され、
	// これを削除する方法は用意されていない。
}
BOOL CWindowHandlerTrackQueryCancelAutoPlay::translateProc(LPMSG lpMsg)
{
	BOOL bRes = FALSE;
	if(lpMsg->message == m_uQueryCancelAutoPlay) {
		bRes = ::SendMessage(lpMsg->hwnd,WM_QUERYCANCELAUTOPLAY,lpMsg->wParam,lpMsg->lParam);
		CHECKRESULT_API(bRes);
	}
	return bRes;
}

/*****************************************************************
	WM_MOUSEWHEELメッセージ変換
*****************************************************************/
void CWindowHandlerTrackMouseWheel::init()
{
	m_uMSH_MOUSEWHEEL = ::RegisterWindowMessage(MSH_MOUSEWHEEL);
	CHECKRESULT_API(m_uMSH_MOUSEWHEEL);
}
void CWindowHandlerTrackMouseWheel::destroy()
{
	// ココでは特に何もしない
	// RegisterWindowMessage()で登録したメッセージは
	// Windowsセッションが終了するまで永続的に登録され、
	// これを削除する方法は用意されていない。
}
BOOL CWindowHandlerTrackMouseWheel::translateProc(LPMSG lpMsg)
{
	BOOL bRes = FALSE;
	if(lpMsg->message == m_uMSH_MOUSEWHEEL) {
		bRes = ::PostMessage(lpMsg->hwnd,WM_MOUSEWHEEL,lpMsg->wParam,lpMsg->lParam);
		CHECKRESULT_API(bRes);
	}
	return bRes;
}

/*****************************************************************
	WM_MOUSELEAVEエミュレーション
*****************************************************************/
void CWindowHandlerTrackMouseLeave::init()
{
	m_bLeaved = FALSE;
	m_nTimerId = 0;
}
void CWindowHandlerTrackMouseLeave::enableProc()
{
	BASE::enableProc();
	destroy();
	m_nTimerId = ::SetTimer(handle(),UINT(this),pollingInterval(),NULL);
}
void CWindowHandlerTrackMouseLeave::disableProc()
{
	if(m_nTimerId) {
		if(CWnd::isValid()) {
			CHECKRESULT_API(::KillTimer(handle(),m_nTimerId));
		}
		m_nTimerId = 0;
	}
	destroy();
	BASE::disableProc();
}
LRESULT CWindowHandlerTrackMouseLeave::wmTimer(WPARAM wParam, LPARAM lParam)
{
	if(wParam==m_nTimerId && !isLeaved()) {
		CRect rc;
		CPoint ptc,pts;
		CHECKRESULT_API(::GetClientRect(handle(),&rc));
		CHECKRESULT_API(::GetCursorPos(&ptc));
		CHECKRESULT_API(::GetCursorPos(&pts));
		CHECKRESULT_NT(::ScreenToClient(handle(),&ptc));
		if(!rc.isInside(ptc) || ::WindowFromPoint(pts)!=handle()) {
			m_bLeaved = TRUE;
			post(WM_MOUSELEAVE);
		}
	}
	return BASE::wmTimer(wParam,lParam);
}
LRESULT CWindowHandlerTrackMouseLeave::wmMouseMove(WPARAM wParam, LPARAM lParam)
{
	m_bLeaved = FALSE;
	return BASE::wmMouseMove(wParam,lParam);
}

/*****************************************************************
	WM_MOUSEHOVER+WM_MOUSEUNHOVERエミュレーション
*****************************************************************/
void CWindowHandlerTrackMouseHover::init()
{
	m_nTimerId = 0;
	m_bHovered = FALSE;
	m_LastTick.adjust();
	m_ptHovered.let(LONG_MIN,LONG_MIN);
}
void CWindowHandlerTrackMouseHover::enableProc()
{
	BASE::enableProc();
	if(m_nTimerId==0) {
		m_nTimerId = ::SetTimer(handle(),UINT(this),10,NULL);
		CHECKRESULT_API(m_nTimerId);
	}
	m_bHovered = FALSE;
	CHECKRESULT_API(::GetCursorPos(&m_ptHovered));
	CHECKRESULT_NT(::ScreenToClient(handle(),&m_ptHovered));
	m_LastTick.adjust();
}
void CWindowHandlerTrackMouseHover::disableProc()
{
	if(isHovered()) {
		CPoint pt;
		CHECKRESULT_API(::GetCursorPos(&pt));
		CHECKRESULT_NT(::ScreenToClient(handle(),&pt));
		WPARAM wParam = 0;
		LPARAM lParam = MAKELPARAM(pt.x,pt.y);
		wParam |= (::GetKeyState(VK_CONTROL)&0x8000)?MK_CONTROL:0;
		wParam |= (::GetKeyState(VK_LBUTTON)&0x8000)?MK_LBUTTON:0;
		wParam |= (::GetKeyState(VK_MBUTTON)&0x8000)?MK_MBUTTON:0;
		wParam |= (::GetKeyState(VK_RBUTTON)&0x8000)?MK_RBUTTON:0;
		wParam |= (::GetKeyState(VK_SHIFT  )&0x8000)?MK_SHIFT  :0;
		post(WM_MOUSEUNHOVER,wParam,lParam);
		m_bHovered = FALSE;
	}
	if(m_nTimerId) {
		CHECKRESULT_API(::KillTimer(handle(),m_nTimerId));
		m_nTimerId = 0;
	}
	BASE::disableProc();
}
LRESULT CWindowHandlerTrackMouseHover::wmTimer(WPARAM wParam, LPARAM lParam)
{
	if(wParam==m_nTimerId) {
		if(!isHovered() && hoverTime()<m_LastTick.elapse()) {
			CPoint pt;
			::GetCursorPos(&pt);
			if(::WindowFromPoint(pt)==handle()) {
				CRect rc;
				CHECKRESULT_API(::GetClientRect(handle(),&rc));
				CHECKRESULT_NT(::ScreenToClient(handle(),&pt));
				if(rc.isInside(pt)) {
					if((UINT)abs(m_ptHovered.x-pt.x) <= hoverWidth () &&
					   (UINT)abs(m_ptHovered.y-pt.y) <= hoverHeight()) {
						WPARAM wParam = 0;
						LPARAM lParam = MAKELPARAM(pt.x,pt.y);
						wParam |= (::GetKeyState(VK_CONTROL)&0x8000)?MK_CONTROL:0;
						wParam |= (::GetKeyState(VK_LBUTTON)&0x8000)?MK_LBUTTON:0;
						wParam |= (::GetKeyState(VK_MBUTTON)&0x8000)?MK_MBUTTON:0;
						wParam |= (::GetKeyState(VK_RBUTTON)&0x8000)?MK_RBUTTON:0;
						wParam |= (::GetKeyState(VK_SHIFT  )&0x8000)?MK_SHIFT  :0;
						post(WM_MOUSEHOVER,wParam,lParam);
					}
				}
			}
		}
	}
	return BASE::wmTimer(wParam,lParam);
}
LRESULT CWindowHandlerTrackMouseHover::wmMouseMove(WPARAM wParam, LPARAM lParam)
{
	CPoint pt(LOWORD(lParam),HIWORD(lParam));
	if(isHovered()) {
		if(hoverWidth () <= (UINT)abs(m_ptHovered.x-pt.x) ||
		   hoverHeight() <= (UINT)abs(m_ptHovered.y-pt.y)) {
			post(WM_MOUSEUNHOVER,wParam,lParam);
		}
	} else {
		m_ptHovered.let(pt);
		m_LastTick.adjust();
	}
	return BASE::wmMouseMove(wParam,lParam);
}
LRESULT CWindowHandlerTrackMouseHover::wmMouseHover(WPARAM wParam, LPARAM lParam)
{
	if(!isHovered()) {
		m_bHovered = TRUE;
		CHECKRESULT_API(::GetCursorPos(&m_ptHovered));
		CHECKRESULT_NT(::ScreenToClient(handle(),&m_ptHovered));
	}
	return BASE::wmMouseHover(wParam,lParam);
}
LRESULT CWindowHandlerTrackMouseHover::wmMouseUnhover(WPARAM wParam, LPARAM lParam)
{
	if(isHovered()) {
		m_bHovered = FALSE;
		m_LastTick.adjust();
	}
	return BASE::wmMouseUnhover(wParam,lParam);
}

/*****************************************************************
	ホットキー(SHIFT+ESC)によるWM_CLOSE
*****************************************************************/
BOOL CWindowHandlerHotKeyClose::translateProc(LPMSG lpMsg)
{
	return m_accel.translate(hwnd(),lpMsg);
}
LRESULT CWindowHandlerHotKeyClose::wmCommand(WPARAM wParam, LPARAM lParam)
{
	WORD wNotifyCode = HIWORD(wParam);
	WORD wID = LOWORD(wParam);
	if(wNotifyCode == 1) {
		switch(wID) {
		case SC_CLOSE:
			//post(WM_CLOSE);
			send(WM_SYSCOMMAND,SC_CLOSE);
			return 0;
		}
	}
	return BASE::wmCommand(wParam, lParam);
}

/*****************************************************************
	スクリーンセーバーの挙動
*****************************************************************/
LRESULT CWindowHandlerScreenSaver::wmSysCommand(WPARAM wParam, LPARAM lParam)
{
#ifndef _DEBUG
	WORD uCmdType = wParam;
	switch(uCmdType) {
	case SC_SCREENSAVE:
		return FALSE;
	}
#endif
	return BASE::wmSysCommand(wParam,lParam);
}
LRESULT CWindowHandlerScreenSaver::wmSetCursor(WPARAM wParam, LPARAM lParam)
{
#ifndef _DEBUG
	::SetCursor(NULL);
	return TRUE;
#else
	return BASE::wmSetCursor(wParam,lParam);
#endif
}
LRESULT CWindowHandlerScreenSaver::wmActivate(WPARAM wParam, LPARAM lParam)
{
#ifndef _DEBUG
	UINT fActive = LOWORD(wParam);
	if(fActive==WA_INACTIVE) {
		show(SW_HIDE);
		post(WM_CLOSE);
	}
#endif
	return BASE::wmActivate(wParam,lParam);
}
LRESULT CWindowHandlerScreenSaver::wmActivateApp(WPARAM wParam, LPARAM lParam)
{
#ifndef _DEBUG
	BOOL fActive = BOOL(wParam);
	if(!fActive) {
		show(SW_HIDE);
		post(WM_CLOSE);
	}
#endif
	return BASE::wmActivateApp(wParam,lParam);
}
LRESULT CWindowHandlerScreenSaver::wmMouseMove(WPARAM wParam, LPARAM lParam)
{
#ifndef _DEBUG
	CPoint pt(LOWORD(lParam),HIWORD(lParam));
	if(m_pt == CPoint(-1,-1)) {
		m_pt = pt;
	} else if(m_pt != pt) {
		show(SW_HIDE);
		post(WM_CLOSE);
	}
#endif
	return BASE::wmMouseMove(wParam,lParam);
}
LRESULT CWindowHandlerScreenSaver::wmLButtonDown(WPARAM wParam, LPARAM lParam)
{
#ifndef _DEBUG
	show(SW_HIDE);
	post(WM_CLOSE);
#endif
	return BASE::wmLButtonDown(wParam,lParam);
}
LRESULT CWindowHandlerScreenSaver::wmRButtonDown(WPARAM wParam, LPARAM lParam)
{
#ifndef _DEBUG
	show(SW_HIDE);
	post(WM_CLOSE);
#endif
	return BASE::wmRButtonDown(wParam,lParam);
}
LRESULT CWindowHandlerScreenSaver::wmMButtonDown(WPARAM wParam, LPARAM lParam)
{
#ifndef _DEBUG
	show(SW_HIDE);
	post(WM_CLOSE);
#endif
	return BASE::wmMButtonDown(wParam,lParam);
}
LRESULT CWindowHandlerScreenSaver::wmKeyDown(WPARAM wParam, LPARAM lParam)
{
#ifndef _DEBUG
	show(SW_HIDE);
	post(WM_CLOSE);
#endif
	return BASE::wmKeyDown(wParam,lParam);
}
LRESULT CWindowHandlerScreenSaver::wmSysKeyDown(WPARAM wParam, LPARAM lParam)
{
#ifndef _DEBUG
	show(SW_HIDE);
	post(WM_CLOSE);
#endif
	return BASE::wmSysKeyDown(wParam,lParam);
}

/*****************************************************************
	メッセージ調査
*****************************************************************/
void CWindowHandlerMessagePoller::destroy()
{
	m_cs.enter();
	// 待ちリストを開放するため全てをシグナル状態にする
	for(LPMSGPOLL p=m_list.next(); p; p=p->next()) {
		if(p->hEvent) {
			::SetEvent(p->hEvent);
		}
	}
	m_list.purge();
	m_cs.leave();
}
LRESULT CWindowHandlerMessagePoller::windowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_cs.enter();
	for(LPMSGPOLL p=m_list.next(); p; p=p->next()) {
		if(p->pFilter) {
			if(p->pFilter(hwnd(),uMsg,wParam,lParam)) {
				if(p->hEvent) {
					::SetEvent(p->hEvent);
				}
			}
		} else {
			DWORD dw = 0;
			dw |= uMsg  ==p->uMsg   ? MSGPOLL_FILTERF_MESSAGE : 0;
			dw |= wParam==p->wParam ? MSGPOLL_FILTERF_WPARAM  : 0;
			dw |= lParam==p->lParam ? MSGPOLL_FILTERF_LPARAM  : 0;
			if((dw&p->dwFilter)==p->dwFilter) {
				if(p->hEvent) {
					::SetEvent(p->hEvent);
				}
			}
		}
	}
	m_cs.leave();
	return BASE::windowProc(uMsg,wParam,lParam);
}
BOOL CWindowHandlerMessagePoller::waitForMessage(LPMSGPOLL pMsgPoll, DWORD dwTimeout) const
{
	DWORD dwRes = ::WaitForSingleObject(pMsgPoll->hEvent,dwTimeout);
	CHECKRESULT_API(dwRes!=WAIT_FAILED);
	return dwRes==WAIT_OBJECT_0;
}
DWORD CWindowHandlerMessagePoller::waitForMessage(UINT nCount, LPMSGPOLL pMsgPoll, DWORD dwTimeout, BOOL fWaitAll) const
{
	// WaitForMultipleObjects()用に配列を作成
	HANDLE aEvent[MAXIMUM_WAIT_OBJECTS];
	nCount = min(nCount,MAXIMUM_WAIT_OBJECTS);
	for(UINT n=0; n<nCount; n++) {
		aEvent[n] = pMsgPoll[n].hEvent;
	}
	// イベント待ち
	DWORD dwRes = ::WaitForMultipleObjects(nCount,aEvent,fWaitAll,dwTimeout);
	CHECKRESULT_API(dwRes!=WAIT_FAILED);
	return dwRes;
}
void CWindowHandlerMessagePoller::add(LPMSGPOLL pMsgPoll)
{
	m_cs.enter();
	m_list.end(pMsgPoll);
	m_cs.leave();
}
void CWindowHandlerMessagePoller::del(LPMSGPOLL pMsgPoll)
{
	m_cs.enter();
	pMsgPoll->unlink();
	m_cs.leave();
}

/*****************************************************************
	メッセージ調査(ゲーム用UI)
*****************************************************************/
void CWindowHandlerUserInputPoller::init()
{
	int n;
	// VK_ESCAPE,VK_NUMPAD0,VK_RBUTTON
	for(n=0; n<ARRAYLEN(m_mpEsc); n++) {
		m_mpEsc[n].dwFilter = MSGPOLL_FILTERF_MESSAGE;
		m_mpEsc[n].uMsg   = 0;
		m_mpEsc[n].wParam = 0;
		m_mpEsc[n].lParam = 0;
		m_mpEsc[n].hEvent = m_evEsc[n].create(TRUE,FALSE);
		m_mpEsc[n].pFilter = NULL;
	}
	// VK_ESCAPE
	m_mpEsc[0].dwFilter = MSGPOLL_FILTERF_MESSAGE|MSGPOLL_FILTERF_WPARAM;
	m_mpEsc[0].uMsg = WM_CHAR;
	m_mpEsc[0].wParam = VK_ESCAPE;
	// VK_NAMPAD0
	m_mpEsc[1].dwFilter = MSGPOLL_FILTERF_MESSAGE|MSGPOLL_FILTERF_WPARAM;
	m_mpEsc[1].uMsg = WM_KEYDOWN;
	m_mpEsc[1].wParam = VK_NUMPAD0;
	// VK_RBUTTON
	m_mpEsc[2].dwFilter = MSGPOLL_FILTERF_MESSAGE;
	m_mpEsc[2].uMsg = WM_RBUTTONCLK;
	for(n=0; n<ARRAYLEN(m_mpEsc); n++) {
		add(&m_mpEsc[n]);
	}

	// VK_SPACE,VK_RETURN,VK_LBUTTON
	for(n=0; n<ARRAYLEN(m_mpKey); n++) {
		m_mpKey[n].dwFilter = MSGPOLL_FILTERF_MESSAGE;
		m_mpKey[n].uMsg   = 0;
		m_mpKey[n].wParam = 0;
		m_mpKey[n].lParam = 0;
		m_mpKey[n].hEvent = m_evKey[n].create(TRUE,FALSE);
		m_mpKey[n].pFilter = NULL;
	}
	// VK_RETURN
	m_mpKey[0].dwFilter = MSGPOLL_FILTERF_MESSAGE|MSGPOLL_FILTERF_WPARAM;
	m_mpKey[0].uMsg = WM_KEYDOWN;
	m_mpKey[0].wParam = VK_RETURN;
	// VK_SPACE
	m_mpKey[1].dwFilter = MSGPOLL_FILTERF_MESSAGE|MSGPOLL_FILTERF_WPARAM;
	m_mpKey[1].uMsg = WM_KEYDOWN;
	m_mpKey[1].wParam = VK_SPACE;
	// VK_LBUTTON
	m_mpKey[2].dwFilter = MSGPOLL_FILTERF_MESSAGE;
	m_mpKey[2].uMsg = WM_LBUTTONCLK;
	for(n=0; n<ARRAYLEN(m_mpKey); n++) {
		add(&m_mpKey[n]);
	}

	// VK_LEFT,VK_RIGHT,VK_UP,VK_DOWN,VK_NUMPAD4,6,8,2/VK_TAB,VK_PRIOR,VK_NEXT,VK_HOME,VK_END
	for(n=0; n<ARRAYLEN(m_mpPad); n++) {
		m_mpPad[n].dwFilter = MSGPOLL_FILTERF_MESSAGE;
		m_mpPad[n].uMsg   = 0;
		m_mpPad[n].wParam = 0;
		m_mpPad[n].lParam = 0;
		m_mpPad[n].hEvent = m_evPad[n].create(TRUE,FALSE);
		m_mpPad[n].pFilter = NULL;
	}
	// VK_LEFT
	m_mpPad[0].dwFilter = MSGPOLL_FILTERF_MESSAGE|MSGPOLL_FILTERF_WPARAM;
	m_mpPad[0].uMsg = WM_KEYDOWN;
	m_mpPad[0].wParam = VK_LEFT;
	// VK_RIGHT
	m_mpPad[1].dwFilter = MSGPOLL_FILTERF_MESSAGE|MSGPOLL_FILTERF_WPARAM;
	m_mpPad[1].uMsg = WM_KEYDOWN;
	m_mpPad[1].wParam = VK_RIGHT;
	// VK_UP
	m_mpPad[2].dwFilter = MSGPOLL_FILTERF_MESSAGE|MSGPOLL_FILTERF_WPARAM;
	m_mpPad[2].uMsg = WM_KEYDOWN;
	m_mpPad[2].wParam = VK_UP;
	// VK_DOWN
	m_mpPad[3].dwFilter = MSGPOLL_FILTERF_MESSAGE|MSGPOLL_FILTERF_WPARAM;
	m_mpPad[3].uMsg = WM_KEYDOWN;
	m_mpPad[3].wParam = VK_DOWN;
	// VK_NUMPAD4
	m_mpPad[4].dwFilter = MSGPOLL_FILTERF_MESSAGE|MSGPOLL_FILTERF_WPARAM;
	m_mpPad[4].uMsg = WM_KEYDOWN;
	m_mpPad[4].wParam = VK_NUMPAD4;
	// VK_NUMPAD6
	m_mpPad[5].dwFilter = MSGPOLL_FILTERF_MESSAGE|MSGPOLL_FILTERF_WPARAM;
	m_mpPad[5].uMsg = WM_KEYDOWN;
	m_mpPad[5].wParam = VK_NUMPAD6;
	// VK_NUMPAD8
	m_mpPad[6].dwFilter = MSGPOLL_FILTERF_MESSAGE|MSGPOLL_FILTERF_WPARAM;
	m_mpPad[6].uMsg = WM_KEYDOWN;
	m_mpPad[6].wParam = VK_NUMPAD8;
	// VK_NUMPAD2
	m_mpPad[7].dwFilter = MSGPOLL_FILTERF_MESSAGE|MSGPOLL_FILTERF_WPARAM;
	m_mpPad[7].uMsg = WM_KEYDOWN;
	m_mpPad[7].wParam = VK_NUMPAD2;
	for(n=0; n<ARRAYLEN(m_mpPad); n++) {
		add(&m_mpPad[n]);
	}
	// VK_TAB
	m_mpPad[8].dwFilter = MSGPOLL_FILTERF_MESSAGE|MSGPOLL_FILTERF_WPARAM;
	m_mpPad[8].uMsg = WM_KEYDOWN;
	m_mpPad[8].wParam = VK_TAB;
	for(n=0; n<ARRAYLEN(m_mpPad); n++) {
		add(&m_mpPad[n]);
	}
	// VK_PRIOR
	m_mpPad[9].dwFilter = MSGPOLL_FILTERF_MESSAGE|MSGPOLL_FILTERF_WPARAM;
	m_mpPad[9].uMsg = WM_KEYDOWN;
	m_mpPad[9].wParam = VK_PRIOR;
	for(n=0; n<ARRAYLEN(m_mpPad); n++) {
		add(&m_mpPad[n]);
	}
	// VK_NEXT
	m_mpPad[10].dwFilter = MSGPOLL_FILTERF_MESSAGE|MSGPOLL_FILTERF_WPARAM;
	m_mpPad[10].uMsg = WM_KEYDOWN;
	m_mpPad[10].wParam = VK_NEXT;
	for(n=0; n<ARRAYLEN(m_mpPad); n++) {
		add(&m_mpPad[n]);
	}
	// VK_HOME
	m_mpPad[11].dwFilter = MSGPOLL_FILTERF_MESSAGE|MSGPOLL_FILTERF_WPARAM;
	m_mpPad[11].uMsg = WM_KEYDOWN;
	m_mpPad[11].wParam = VK_HOME;
	for(n=0; n<ARRAYLEN(m_mpPad); n++) {
		add(&m_mpPad[n]);
	}
	// VK_END
	m_mpPad[12].dwFilter = MSGPOLL_FILTERF_MESSAGE|MSGPOLL_FILTERF_WPARAM;
	m_mpPad[12].uMsg = WM_KEYDOWN;
	m_mpPad[12].wParam = VK_END;
	for(n=0; n<ARRAYLEN(m_mpPad); n++) {
		add(&m_mpPad[n]);
	}

	// WM_CHAR処理関係
	m_evChar.create(TRUE,FALSE);
	for(n=0; n<ARRAYLEN(m_aChar); n++) {
		m_aChar[n] = 0;
	}
	m_iChar = 0;
	m_nChar = 0;

	// WM_KEYDOWN処理関係
	m_evDown.create(TRUE,FALSE);
	for(n=0; n<ARRAYLEN(m_aDown); n++) {
		m_aDown[n] = 0;
	}
	m_iDown = 0;
	m_nDown = 0;
}
void CWindowHandlerUserInputPoller::destroy()
{
	int n;
	for(n=0; n<ARRAYLEN(m_mpEsc); n++) {
		del(&m_mpEsc[n]);
	}
	for(n=0; n<ARRAYLEN(m_mpKey); n++) {
		del(&m_mpKey[n]);
	}
	for(n=0; n<ARRAYLEN(m_mpPad); n++) {
		del(&m_mpPad[n]);
	}
	init();
}
LRESULT CWindowHandlerUserInputPoller::wmChar(WPARAM wParam, LPARAM lParam)
{
	m_cs.enter();
	if(m_nChar<ARRAYLEN(m_aChar)) {
		m_aChar[m_iChar] = wParam;
		m_iChar = m_iChar+1==ARRAYLEN(m_aChar) ? 0 : m_iChar+1;
		m_nChar++;
		m_evChar.set();
	}
	m_cs.leave();
	return BASE::wmChar(wParam,lParam);
}
LRESULT CWindowHandlerUserInputPoller::wmKeyDown(WPARAM wParam, LPARAM lParam)
{
	m_cs.enter();
	if(m_nDown<ARRAYLEN(m_aDown)) {
		SHORT nKey = LOBYTE(wParam);
		nKey |= MAKEWORD(0,(::GetKeyState(VK_SHIFT  )&0x8000)?0x01:0);
		nKey |= MAKEWORD(0,(::GetKeyState(VK_CONTROL)&0x8000)?0x02:0);
		nKey |= MAKEWORD(0,(::GetKeyState(VK_MENU   )&0x8000)?0x04:0);
		nKey |= MAKEWORD(0,(::GetKeyState(VK_KANA   )&0x8001)?0x08:0);
		nKey |= MAKEWORD(0,(::GetKeyState(VK_CAPITAL)&0x8001)?0x10:0);
		nKey |= MAKEWORD(0,(::GetKeyState(VK_NUMLOCK)&0x8001)?0x20:0);
		m_aDown[m_iDown] = nKey;
		m_iDown = m_iDown+1==ARRAYLEN(m_aDown) ? 0 : m_iDown+1;
		m_nDown++;
		m_evDown.set();
	}
	m_cs.leave();
	return BASE::wmKeyDown(wParam,lParam);
}
TCHAR CWindowHandlerUserInputPoller::waitForChar(DWORD dwTimeout, BOOL fRemove) const
{
	UINT nRes = 0;
	m_evChar.wait(dwTimeout);
	m_cs.enter();
	if(0<m_nChar) {
		if(m_nChar<=m_iChar) {
			nRes = m_aChar[m_iChar-m_nChar];
		} else {
			nRes = m_aChar[ARRAYLEN(m_aChar)-m_nChar+m_iChar];
		}
		if(fRemove) {
			if(--m_nChar==0) {
				m_evChar.reset();
			}
		}
	}
	m_cs.leave();
	return nRes;
}
SHORT CWindowHandlerUserInputPoller::waitForVkKey(DWORD dwTimeout, BOOL fRemove) const
{
	UINT nRes = 0;
	m_evDown.wait(dwTimeout);
	m_cs.enter();
	if(0<m_nDown) {
		if(m_nDown<=m_iDown) {
			nRes = m_aDown[m_iDown-m_nDown];
		} else {
			nRes = m_aDown[ARRAYLEN(m_aDown)-m_nDown+m_iDown];
		}
		if(fRemove) {
			if(--m_nDown==0) {
				m_evDown.reset();
			}
		}
	}
	m_cs.leave();
	return nRes;
}
SHORT CWindowHandlerUserInputPoller::waitForEscape(DWORD dwTimeout, BOOL fReset) const
{
	// メッセージ待ち
	DWORD dw = waitForMessage(ARRAYLEN(m_mpEsc),&m_mpEsc[0],dwTimeout,FALSE);
	int nRes = 0;
	switch(dw) {
	case WAIT_OBJECT_0+0:
		nRes = VK_ESCAPE;
		break;
	case WAIT_OBJECT_0+1:
		nRes = VK_NUMPAD0;
		break;
	case WAIT_OBJECT_0+2:
		nRes = VK_RBUTTON;
		break;
	}
	if(nRes && fReset) {
		for(int n=0; n<ARRAYLEN(m_evEsc); n++) {
			m_evEsc[n].reset();
		}
	}
	return nRes;
}
SHORT CWindowHandlerUserInputPoller::waitForHitKey(DWORD dwTimeout, BOOL fReset) const
{
	// メッセージ待ち
	DWORD dw = waitForMessage(ARRAYLEN(m_mpKey),&m_mpKey[0],dwTimeout,FALSE);
	int nRes = 0;
	switch(dw) {
	case WAIT_OBJECT_0+0:
		nRes = VK_RETURN;
		break;
	case WAIT_OBJECT_0+1:
		nRes = VK_SPACE;
		break;
	case WAIT_OBJECT_0+2:
		nRes = VK_LBUTTON;
		break;
	}
	if(nRes && fReset) {
		for(int n=0; n<ARRAYLEN(m_evKey); n++) {
			m_evKey[n].reset();
		}
	}
	return nRes;
}
SHORT CWindowHandlerUserInputPoller::waitForCursor(DWORD dwTimeout, BOOL fReset) const
{
	// メッセージ待ち
	DWORD dw = waitForMessage(ARRAYLEN(m_mpPad),&m_mpPad[0],dwTimeout,FALSE);
	int nRes = 0;
	switch(dw) {
	case WAIT_OBJECT_0+0:
		nRes = VK_LEFT;
		break;
	case WAIT_OBJECT_0+1:
		nRes = VK_RIGHT;
		break;
	case WAIT_OBJECT_0+2:
		nRes = VK_UP;
		break;
	case WAIT_OBJECT_0+3:
		nRes = VK_DOWN;
		break;
	case WAIT_OBJECT_0+4:
		nRes = VK_NUMPAD4;
		break;
	case WAIT_OBJECT_0+5:
		nRes = VK_NUMPAD6;
		break;
	case WAIT_OBJECT_0+6:
		nRes = VK_NUMPAD8;
		break;
	case WAIT_OBJECT_0+7:
		nRes = VK_NUMPAD2;
		break;
	case WAIT_OBJECT_0+8:
		nRes = VK_TAB;
		break;
	case WAIT_OBJECT_0+9:
		nRes = VK_PRIOR;
		break;
	case WAIT_OBJECT_0+10:
		nRes = VK_NEXT;
		break;
	case WAIT_OBJECT_0+11:
		nRes = VK_HOME;
		break;
	case WAIT_OBJECT_0+12:
		nRes = VK_END;
		break;
	}
	if(nRes && fReset) {
		for(int n=0; n<ARRAYLEN(m_evPad); n++) {
			m_evPad[n].reset();
		}
	}
	return nRes;
}

/*****************************************************************
	CBTフック(詳細はSetWindowsHookExを参照)
*****************************************************************/
CCBTHook::LPLINK CCBTHook::m_anchor;
ULONG CCBTHook::m_cRef;
CCriticalSection0 CCBTHook::m_cs;
void CCBTHook::init()
{
	// やや危ない気もするが取り敢えず放置
	ASSERT(m_cRef<ULONG_MAX);
	if(::InterlockedIncrement((LPLONG)&m_cRef)==1) {
		m_cs.init();
	}
	m_hHook		= NULL;
	m_nThreadId = 0;
}
void CCBTHook::destroy()
{
	disable();
	// やや危ない気もするが取り敢えず放置
	ASSERT(0<m_cRef);
	if(::InterlockedDecrement((LPLONG)&m_cRef)==0) {
		m_cs.destroy();
	}
}
BOOL CCBTHook::enable()
{
	m_cs.enter();
	__try {
		// 既に同一スレッドで登録済みかチェックする
		LPLINK l = m_anchor;
		for(; l; l=l->next()) {
			LPTYPE p = l->object();
			if(p!=this) {
				if(p->isEnable()) {
					if(p->m_nThreadId==::GetCurrentThreadId()) {
						break;
					}
				}
			}
		}
		// 登録済みでない場合のみフック登録
		if(l) {
			m_hHook = l->object()->m_hHook;	// 登録済みフックを利用
		} else {
			m_hHook = ::SetWindowsHookEx(WH_CBT,CBTProc,NULL,::GetCurrentThreadId());
			CHECKRESULT_API(m_hHook);
		}
		if(m_hHook) {
			m_nThreadId = ::GetCurrentThreadId();
			if(m_anchor) {
				m_anchor->end(&m_link);
			} else {
				m_anchor = &m_link;
			}
		}
	}
	__finally {
		m_cs.leave();
	}
	return isEnable();
}
BOOL CCBTHook::disable()
{
	m_cs.enter();
	__try {
		if(isEnable()) {
			// 同一スレッドで利用中かチェックする
			LPLINK l = m_anchor;
			for(; l; l=l->next()) {
				LPTYPE p = l->object();
				if(p!=this) {
					if(::GetCurrentThreadId()==p->m_nThreadId) {
						break;
					}
				}
			}
			// 利用中でない場合のみフック解除
			if(!l) {
				CHECKRESULT_API(::UnhookWindowsHookEx(m_hHook));
			}
			if(m_anchor==&m_link) {
				m_anchor = m_link.next();
			}
			m_link.unlink();
			m_hHook		= NULL;
			m_nThreadId	= 0;
		}
	}
	__finally {
		m_cs.leave();
	}
	return isEnable();
}
// ※CBTProcはスレッドIDを基にして該当オブジェクトを決定する
LRESULT CALLBACK CCBTHook::CBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0L;
	m_cs.enter();
	__try {
		for(LPLINK l=m_anchor; l&&lRes==0L; l=l->next()) {
			LPTYPE p = l->object();
			if(p->isEnable()) {
				if(p->m_nThreadId==::GetCurrentThreadId()) {
					lRes = p->cbtProc(nCode,wParam,lParam);
				}
			}
		}
	}
	__finally {
		m_cs.leave();
	}
	return lRes;
}
LRESULT CCBTHook::cbtProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if(nCode<0) {
		return ::CallNextHookEx(m_hHook,nCode,wParam,lParam);
	}
#define METHOD(code,handler) case code: return handler(wParam,lParam);
	switch(nCode) {
	HMC_CCBTHOOK_ALIST
	}
	return 0;
#undef METHOD
}
// ハンドラ宣言本体
#define METHOD(code,handler) \
LRESULT CCBTHook::handler(WPARAM wParam, LPARAM lParam) {	\
	return 0;	/* 0:続行 1:中断 */	\
}
HMC_CCBTHOOK_ALIST
#undef METHOD

/*****************************************************************
	ウィンドウ列挙クラス
*****************************************************************/
/*****************************************************************
	トップレベルウィンドウの列挙(EnumWindowsラッパー)
*****************************************************************/
HWND CEnumWindows::nth(UINT n)
{
	m_hWnd = NULL;
	m_nCount = 0;
	m_nIndex = n;
	BOOL bRes=::EnumWindows(enumProc,LPARAM(this));
	//CHECKRESULT_API(bRes);	// 何故かSetLastError()されてない
	return handle();
}
BOOL CALLBACK CEnumWindows::enumProc(HWND hwnd, LPARAM lParam)
{
	return (LPTYPE(lParam))->enumProc(hwnd);
}
BOOL CEnumWindows::enumProc(HWND hwnd)
{
	if(m_nIndex == m_nCount) {
		m_hWnd = hwnd;
		return FALSE;
	} else {
		m_nCount++;
		return TRUE;
	}
}

/*****************************************************************
	子ウィンドウの列挙(EnumChildWindowsラッパー)
*****************************************************************/
HWND CEnumChildWindows::nth(UINT n)
{
	m_hWnd = NULL;
	m_nCount = 0;
	m_nIndex = n;
	BOOL bRes = ::EnumChildWindows(m_hWndParent,enumProc,LPARAM(this));
	//CHECKRESULT_API(bRes);	// 何故かSetLastError()されてない
	return handle();
}
BOOL CALLBACK CEnumChildWindows::enumProc(HWND hwnd, LPARAM lParam)
{
	return (LPTYPE(lParam))->enumProc(hwnd);
}
BOOL CEnumChildWindows::enumProc(HWND hwnd)
{
	if(m_nIndex == m_nCount) {
		m_hWnd = hwnd;
		return FALSE;
	} else {
		m_nCount++;
		return TRUE;
	}
}

/*****************************************************************
	スレッドに属するウィンドウの列挙(EnumThreadWindowsラッパー)
*****************************************************************/
HWND CEnumThreadWindows::nth(UINT n)
{
	m_hWnd = NULL;
	m_nCount = 0;
	m_nIndex = n;
	BOOL bRes = ::EnumThreadWindows(m_dwThreadId,enumProc,LPARAM(this));
	//CHECKRESULT_API(bRes);	// 何故かSetLastError()されてない
	return handle();
}
BOOL CALLBACK CEnumThreadWindows::enumProc(HWND hwnd, LPARAM lParam)
{
	return (LPTYPE(lParam))->enumProc(hwnd);
}
BOOL CEnumThreadWindows::enumProc(HWND hwnd)
{
	if(m_nIndex == m_nCount) {
		m_hWnd = hwnd;
		return FALSE;
	} else {
		m_nCount++;
		return TRUE;
	}
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
