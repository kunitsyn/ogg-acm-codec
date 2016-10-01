/*
	ウィンドウサポートクラス
	Copyright (C)1998-99 H.Mutsuki
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __INC_HMC_WINDOW__
#define __INC_HMC_WINDOW__

#include "util.h"
#include "link.h"
#include "sync.h"
#include "time.h"
#include "accel.h"
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "shell32.lib")

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
	グローバル
*****************************************************************/
// スタイルの検査
BOOL HaveWindowStyle(HWND hwnd, DWORD fdwStyle);
BOOL HaveWindowExStyle(HWND hwnd, DWORD fdwStyle);
HWND GetOwner(HWND hwnd);
// ウィンドウ位置・サイズ操作
BOOL GetWindowRectFromParent(HWND hwnd, const RECT *lprc);
BOOL CenterWindow(HWND hwnd, HWND hParent, const RECT *lprc=NULL);
BOOL CenterClient(HWND hwnd, HWND hParent, const RECT *lprc=NULL);
BOOL ResizeWindow(HWND hwnd, int w, int h, const RECT *lprc=NULL);
BOOL ResizeClient(HWND hwnd, int w, int h, const RECT *lprc=NULL);
#ifdef __AFXWIN_H__
BOOL ResizeClient(const ::CWnd *pWnd, int w, int h, const RECT *lprc=NULL);
#endif
// 擬似メインウィンドウ取得
HWND GetPseudoMainWindow();
HWND GetPseudoMainWindow(HWND hwnd);
HWND GetPseudoMainWindow(DWORD dwProcessId, DWORD dwThreadId);
// Zオーダー取得(正確性は低い)
LONG GetWindowZOrder(HWND hwnd);
// メニュー関係
BOOL HaveMenu(HWND hwnd);
// ウィンドウを作成したスレッドの入力をアタッチする
BOOL AttachThreadInput(HWND hWnd, BOOL fAttach = TRUE);
// マウスカーソル情報
POINT GetClientCursorPos(HWND hWnd);

/*****************************************************************
*****************************************************************/
// ウィンドウハンドル
class CWnd;
// ウィンドウハンドラ
class CWindowHandler;
class CWindow;
class CWindowHandlerFrame;
class CWindowHandlerView;
class CWindowHandlerTrackOwnedPopups;
class CWindowHandlerSetCursor;
class CWindowHandlerMouseMove;
class CWindowHandlerSingleClick;
class CWindowHandlerClientDrag;
class CWindowHandlerTrackQueryCancelAutoPlay;
class CWindowHandlerTrackMouseWheel;
class CWindowHandlerTrackMouseLeave;
class CWindowHandlerTrackMouseHover;
class CWindowHandlerHotKeyClose;
class CWindowHandlerScreenSaver;
class CWindowHandlerMessagePoller;
class CWindowHandlerUserInputPoller;
// フック
//class CWindowsHook;	// 抽象クラス
class CCBTHook;
class CCBTHookCreateWnd;
//class CCallWndProcHook;
//class CGetMessageHook;
// ウィンドウ列挙
class CEnumWindows;
class CEnumChildWindows;
class CEnumThreadWindows;

/*****************************************************************
	オリジナルメッセージ
*****************************************************************/
const UINT WM_THROUGH		= WM_USER + 0x7000;		// WM_THROUGH以上のメッセージはハンドラの無効化を検証しない
const UINT WM_CREATETRUST	= WM_THROUGH + 0x0000;	// 他スレッドへCreateWindow依頼
const UINT WM_DESTROYTRUST	= WM_THROUGH + 0x0001;	// 他スレッドへDestroyWindow依頼
const UINT WM_MANHANDLER	= WM_THROUGH + 0x0002;	// ハンドラのリンク操作
const UINT WM_USERIDLE		= WM_USER + 0x1002;		// 余暇通知メッセージ……WPARAMはコンテキスト,LPARAMは未定義
const UINT WM_LBUTTONCLK	= WM_USER + 0x1003;		// シングルクリック(L)……WPARAMはDOWN時,LPARAMはUP時のカーソル位置
const UINT WM_RBUTTONCLK	= WM_USER + 0x1004;		// シングルクリック(R)……WPARAMはDOWN時,LPARAMはUP時のカーソル位置
const UINT WM_MBUTTONCLK	= WM_USER + 0x1005;		// シングルクリック(M)……WPARAMはDOWN時,LPARAMはUP時のカーソル位置
const UINT WM_MOUSEUNHOVER	= WM_USER + 0x1006;		// WM_MOUSEHOVERの解除メッセージ……WPARAM,LPARAMはWM_MOUSEMOVEと同じ(CWindowHandlerTrackMouseHover使用時)
const UINT WM_QUERYCANCELAUTOPLAY	= WM_USER + 0x1007;		// AutoPlay抑制

const WPARAM MH_GETOBJECT	= 0x0000;	// オブジェクト取得(CWindowHandler型ポインタを返す)
const WPARAM MH_SETOWNER	= 0x0001;	// 間接呼び出しsetOwner()
const WPARAM MH_ADDCHILD	= 0x0002;	// 間接呼び出しaddChild()
const WPARAM MH_ENABLE		= 0x0003;	// 間接呼び出しenable()
const WPARAM MH_DISABLE		= 0x0004;	// 間接呼び出しdisable()

const WPARAM SC_FULLSCREEN		= 0x0100;	// 画面モード切り替え(フルスクリーンとウィンドウのトグル)
const WPARAM SC_FULLSCREENMENU	= 0x0101;	// フルスクリーン状態ならメニューループを実行

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL	0x020A	// winuser.hより
#endif
#ifndef WM_MOUSEHOVER
#define WM_MOUSEHOVER	0x02A1	// winuser.hより
#endif
#ifndef WM_MOUSELEAVE
#define WM_MOUSELEAVE	0x02A3	// winuser.hより
#endif

/*****************************************************************
	ウィンドウの基底クラス
*****************************************************************/
class CWnd
{
public:
	DECLARE_SYNONYM(CWnd,TYPE);
	operator LPTYPE() { return this; }
public:
	CWnd() { init(); }
	CWnd(HWND hwnd) { init(); map(hwnd); }
	CWnd(LPCTSTR lpClass, LPCTSTR lpTitle, DWORD dwStyle, int x, int y, int w, int h, HWND hParent, HMENU hMenu, HINSTANCE hInst, LPVOID lpParam) { init(); create(lpClass, lpTitle, dwStyle, x, y, w, h, hParent, hMenu, hInst, lpParam); }
	CWnd(DWORD dwExStyle, LPCTSTR lpClass, LPCTSTR lpTitle, DWORD dwStyle, int x, int y, int w, int h, HWND hParent, HMENU hMenu, HINSTANCE hInst, LPVOID lpParam) { init(); create(dwExStyle,lpClass,lpTitle,dwStyle,x,y,w,h,hParent,hMenu,hInst,lpParam); }
	CWnd(const TYPE &obj) { init(); map(obj.handle()); }
	~CWnd() { destroy(); }
private:
	void init() { m_bOwner=TRUE; m_hWnd=NULL; }
public:
	void destroy() { if(isOwner()&&isValid()) CHECKRESULT_API(::DestroyWindow(handle())); init(); }	// ※別スレッドで作成されたHWNDの場合DestroyWindow()は失敗する
protected:
	// オブジェクトの作成/削除
	HWND create(LPCTSTR lpClass, LPCTSTR lpTitle, DWORD dwStyle, int x, int y, int w, int h, HWND hParent, HMENU hMenu, HINSTANCE hInst, LPVOID lpParam) { destroy(); m_hWnd=::CreateWindow(lpClass,lpTitle,dwStyle,x,y,w,h,hParent,hMenu,hInst,lpParam); CHECKRESULT_API(handle()); return handle(); }
	HWND create(DWORD dwExStyle, LPCTSTR lpClass, LPCTSTR lpTitle, DWORD dwStyle, int x, int y, int w, int h, HWND hParent, HMENU hMenu, HINSTANCE hInst, LPVOID lpParam) { destroy(); m_hWnd=::CreateWindowEx(dwExStyle,lpClass,lpTitle,dwStyle,x,y,w,h,hParent,hMenu,hInst,lpParam); CHECKRESULT_API(handle()); return handle(); }
public:
	// テンポラリマップの作成/削除
	void map(HWND hwnd) { destroy(); m_bOwner=FALSE; m_hWnd=hwnd; }
protected:
	void handle(HWND hwnd) { m_hWnd=hwnd; }	// オーナーハンドル更新用(苦肉の策だねえ……)
	void hwnd  (HWND hwnd) { m_hWnd=hwnd; }	// オーナーハンドル更新用(苦肉の策だねえ……)
public:
	// 各種操作
	// メッセージ送信
	BOOL post(UINT uMsg, WPARAM wParam=0, LPARAM lParam=0) { BOOL b=::PostMessage(handle(),uMsg,wParam,lParam); CHECKRESULT_API(b); return b; }
	LRESULT send(UINT uMsg, WPARAM wParam=0, LPARAM lParam=0) { return ::SendMessage(handle(),uMsg,wParam,lParam); }
	LRESULT send(UINT uMsg, WPARAM wParam, LPARAM lParam, UINT fuFlags, UINT uTimeout, LPDWORD lpdwResult) { LRESULT l=::SendMessageTimeout(handle(),uMsg,wParam,lParam,fuFlags,uTimeout,lpdwResult); CHECKRESULT_API(l); return l; }
	BOOL post(UINT uMsg, WPARAM wParam=0, LPARAM lParam=0) const { BOOL b=::PostMessage(handle(),uMsg,wParam,lParam); CHECKRESULT_API(b); return b; }
	LRESULT send(UINT uMsg, WPARAM wParam=0, LPARAM lParam=0) const { return ::SendMessage(handle(),uMsg,wParam,lParam); }
	LRESULT send(UINT uMsg, WPARAM wParam, LPARAM lParam, UINT fuFlags, UINT uTimeout, LPDWORD lpdwResult) const { LRESULT l=::SendMessageTimeout(handle(),uMsg,wParam,lParam,fuFlags,uTimeout,lpdwResult); CHECKRESULT_API(l); return l; }
	// 親(オーナー)ウィンドウの取得/設定
	HWND parent() const { HWND h=::GetParent(handle()); /*CHECKRESULT_API(h);*/ return h; }
	HWND parent(HWND hwnd) { HWND h=::SetParent(handle(),hwnd); CHECKRESULT_API(h); return h; }
	HWND owner() const { HWND h=::GetWindow(handle(),GW_OWNER); /*CHECKRESULT_API(h);*/ return h; }
	// 位置・サイズの取得/設定
	BOOL move(int x, int y, BOOL fRedraw=TRUE) { BOOL b=::MoveWindow(handle(),x,y,width(),height(),fRedraw); CHECKRESULT_API(b); return b; }
	BOOL move(int x, int y, int w, int h, BOOL fRedraw=TRUE) { BOOL b=::MoveWindow(handle(),x,y,w,h,fRedraw); CHECKRESULT_API(b); return b; }
	BOOL move(const RECT *lprc, BOOL fRedraw=TRUE) { return move(lprc->left,lprc->top,lprc->right-lprc->left,lprc->bottom-lprc->top,fRedraw); }
	BOOL resize(int w, int h, const RECT *lprc=NULL) { return ResizeClient(handle(),w,h,lprc); }
	BOOL recenter(const RECT *lprc=NULL) { return CenterClient(handle(),parent(),lprc); }
	BOOL windowRect(LPRECT lprc) const { BOOL b=::GetWindowRect(handle(),lprc); CHECKRESULT_API(b); return b; }
	BOOL clientRect(LPRECT lprc) const { BOOL b=::GetClientRect(handle(),lprc); CHECKRESULT_API(b); return b; }
	RECT windowRect() const { CRect rc(0,0,0,0); windowRect(&rc); return rc; }
	RECT clientRect() const { CRect rc(0,0,0,0); clientRect(&rc); return rc; }
	LONG windowWidth () const { CRect rc(0,0,0,0); windowRect(&rc); return rc.width (); }
	LONG windowHeight() const { CRect rc(0,0,0,0); windowRect(&rc); return rc.height(); }
	LONG clientWidth () const { CRect rc(0,0,0,0); clientRect(&rc); return rc.width (); }
	LONG clientHeight() const { CRect rc(0,0,0,0); clientRect(&rc); return rc.height(); }
	//LONG width () const { CRect rc(0,0,0,0); clientRect(&rc); return rc.width (); }
	//LONG height() const { CRect rc(0,0,0,0); clientRect(&rc); return rc.height(); }
	LONG cwidth () const { CRect rc(0,0,0,0); clientRect(&rc); return rc.width (); }
	LONG cheight() const { CRect rc(0,0,0,0); clientRect(&rc); return rc.height(); }
	LONG width () const { CRect rc(0,0,0,0); windowRect(&rc); return rc.width (); }
	LONG height() const { CRect rc(0,0,0,0); windowRect(&rc); return rc.height(); }
	LONG xPos() const { CRect rc(0,0,0,0); windowRect(rc); return rc.left; }
	LONG yPos() const { CRect rc(0,0,0,0); windowRect(rc); return rc.top;  }
	// 表示状態の取得/設定
	BOOL isVisible() const { return ::IsWindowVisible(handle()); }
	BOOL isIconic() const { return ::IsIconic(handle()); }
	BOOL isZoomed() const { return ::IsZoomed(handle()); }
	BOOL isParent(HWND hwnd) const { return ::IsChild(hwnd,handle()); }
	BOOL isChild(HWND hwnd) const { return ::IsChild(handle(),hwnd); }
	BOOL isPopup(HWND hwnd) const { return isPopup()&&isParent(hwnd); }
	BOOL isChild() const { return haveStyle(WS_CHILD); }
	BOOL isOwned() const { return !!owner(); }
	BOOL isPopup() const { return !haveStyle(WS_POPUP); }
	BOOL show(int nCmdShow=SW_SHOW) { return ::ShowWindow(handle(),nCmdShow); }
	BOOL showAsync(int nCmdShow=SW_SHOW) { return ::ShowWindowAsync(handle(),nCmdShow); }
	BOOL showPopups(BOOL fShow) { BOOL b=::ShowOwnedPopups(handle(),fShow); CHECKRESULT_API(b); return b; }
	// フォーカスの取得/設定
	BOOL isEnable() const { return ::IsWindowEnabled(handle()); }
	BOOL enable(BOOL fEnable=TRUE) { RESETLASTERROR(); BOOL b=::EnableWindow(handle(),fEnable); CHECKRESULT_API(0); return b; }	// ※返り値についてはMSDN Libraryを参照のこと
	BOOL disable() { return enable(FALSE); }
	BOOL setFocus() { HWND h=::SetFocus(handle()); CHECKRESULT_API(h); return handle()==h; }
	BOOL isFocus() const { return handle()==::GetFocus(); }
	BOOL isCapture() const { return handle()==::GetCapture(); }
	BOOL activate(BOOL fActive=TRUE) { HWND h=::SetActiveWindow(fActive?handle():NULL); CHECKRESULT_API(h); return !!h; }
	BOOL isActive() const { return handle()==::GetActiveWindow(); }
	// 更新領域の取得/設定
	BOOL invalidate(BOOL fErase=FALSE) { return invalidate(LPRECT(NULL),fErase); }
	BOOL invalidate(const RECT *lprc, BOOL fErase=FALSE) { RESETLASTERROR(); BOOL b=::InvalidateRect(handle(),lprc,fErase); CHECKRESULT_NT(b); return b; }
	BOOL invalidate(HRGN hRgn, BOOL fErase=FALSE) { return ::InvalidateRgn(handle(),hRgn,fErase); }	// MSDN Libraryでは常に0以外の値を返すとあるが、0を返す場合もあるぞ……。
	BOOL validate() { return validate(LPRECT(NULL)); }
	BOOL validate(const RECT *lprc) { RESETLASTERROR(); BOOL b=::ValidateRect(handle(),lprc); CHECKRESULT_NT(b); return b; }
	BOOL validate(HRGN hRgn) { RESETLASTERROR(); BOOL b=::ValidateRgn(handle(),hRgn); CHECKRESULT_NT(b); return b; }
	BOOL getUpdate(LPRECT lprc, BOOL fErase=FALSE) const { RESETLASTERROR(); BOOL b=::GetUpdateRect(handle(),lprc,fErase); CHECKRESULT_NT(b); return b; }
	int  getUpdate(HRGN   hrgn, BOOL fErase=FALSE) const { RESETLASTERROR(); int  n=::GetUpdateRgn (handle(),hrgn,fErase); CHECKRESULT_NT(n); return n; }
	BOOL update() { RESETLASTERROR(); BOOL b=::UpdateWindow(handle()); CHECKRESULT_NT(b); return b; }
	BOOL update(const RECT *lprc, BOOL fErase=FALSE) { invalidate(lprc,fErase); return update(); }
	BOOL lockUpdate() { RESETLASTERROR(); BOOL b=::LockWindowUpdate(handle()); CHECKRESULT_NT(b); return b; }
	BOOL unlockUpdate() { RESETLASTERROR(); BOOL b=::LockWindowUpdate(NULL); CHECKRESULT_NT(b); return b; }
	// ウィンドウスタイルの取得/設定
	DWORD style()   const { RESETLASTERROR(); LONG l=::GetWindowLong(handle(),GWL_STYLE  ); CHECKRESULT_API(l); return l; }
	DWORD exStyle() const { RESETLASTERROR(); LONG l=::GetWindowLong(handle(),GWL_EXSTYLE); CHECKRESULT_API(l); return l; }
	DWORD style  (DWORD fdw) { RESETLASTERROR(); LONG l=::SetWindowLong(handle(),GWL_STYLE  ,fdw); CHECKRESULT_API(l); return l; }
	DWORD exStyle(DWORD fdw) { RESETLASTERROR(); LONG l=::SetWindowLong(handle(),GWL_EXSTYLE,fdw); CHECKRESULT_API(l); return l; }
	DWORD addStyle(DWORD fdwStyle) { return style(style()|fdwStyle); }
	DWORD delStyle(DWORD fdwStyle) { return style(style()&~fdwStyle); }
	DWORD style  (DWORD fdwAdd, DWORD fdwDel) { return style  (  style()&~fdwDel|fdwAdd); }
	DWORD exStyle(DWORD fdwAdd, DWORD fdwDel) { return exStyle(exStyle()&~fdwDel|fdwAdd); }
	BOOL haveStyle  (DWORD fdw) const { return (style  ()&fdw)==fdw; }
	BOOL haveExStyle(DWORD fdw) const { return (exStyle()&fdw)==fdw; }
	// メニュー関係
	HMENU menu() const { return ::GetMenu(handle()); }
	BOOL menu(HMENU hMenu) { BOOL b=::SetMenu(handle(),hMenu); CHECKRESULT_API(b); return b; }
	BOOL haveMenu() const { return HaveMenu(handle()); }
	// テキストの取得/設定
	int textLength() const { int i=::GetWindowTextLength(handle()); CHECKRESULT_API(i); return i; }
	BOOL text(LPCTSTR buf) { BOOL b=::SetWindowText(handle(),buf); CHECKRESULT_API(b); return b; }
	int text(LPTSTR buf, int cbBuf) { int i=::GetWindowText(handle(),buf,cbBuf); CHECKRESULT_API(i); return i; }
	// フォントの取得設定
	HFONT font() const { return (HFONT)send(WM_GETFONT); }
	HFONT font(HFONT hFont, BOOL fRedraw=TRUE) { return (HFONT)send(WM_SETFONT,(WPARAM)hFont,(LPARAM)fRedraw); }
	// スレッド、プロセス情報の取得
	DWORD threadId() const { return ::GetWindowThreadProcessId(handle(),NULL); }
	DWORD processId() const { DWORD dw=0; ::GetWindowThreadProcessId(handle(),&dw); return dw; }
public:
	// クラス情報の取得/設定
	HWND handle() const { return m_hWnd; }
	HWND hwnd() const { return handle(); }
	operator HWND() const { return handle(); }
	BOOL isValid() const { return ::IsWindow(handle()); }
	BOOL isOwner() const { return m_bOwner; }
	BOOL ownership(BOOL f) { return m_bOwner=f; }
private:
	BOOL m_bOwner;	// 所有権フラグ
	HWND m_hWnd;	// ハンドル(派生クラスでDestroyWindowする可能性がある)
};

/*****************************************************************
	ウィンドウハンドラクラス
*****************************************************************/
#pragma warning(push)
#pragma warning(disable: 4355)
class CWindowHandler : public CWnd
{
public:
	DECLARE_SYNONYM(CWindowHandler,TYPE);
	DECLARE_SYNONYM(CWnd,BASE);
	DECLARE_SYNONYM(TLinkTree<TYPE>,LINK);
	operator LPTYPE() { return this; }
	friend TYPE;
	friend CCBTHookCreateWnd;
public:
	CWindowHandler() : m_link(this) { init(); }
	CWindowHandler(LPTYPE p) : m_link(this) { init(); SetOwner(p,-1); }
	virtual ~CWindowHandler() { destroy(); }
private:
	void init() { m_bEnable=TRUE; }	// ※初期状態が有効であることに注意
public:
	void destroy() { disable(); init(); }
public:
	// テンポラリマップの作成(子オブジェクトへ伝搬するので注意)
	void map(HWND hwnd);
	void map(HWND hwnd, UINT nId) { destroy(); map(::GetDlgItem(hwnd,nId)); }
protected:
	// オーナーハンドル更新
	void handle(HWND hwnd);
	void hwnd(HWND hwnd) { handle(hwnd); }
public:
	// 名前支配
	HWND handle() const { return BASE::handle(); }
	HWND hwnd  () const { return BASE::hwnd  (); }
public:
	// リンク操作
	LPTYPE GetRoot() const { LPLINK p=m_link.getRoot(); return p?p->object():NULL; }
	LPTYPE GetOwner() const { LPLINK p=m_link.getOwner(); return p?p->object():NULL; }
	LPTYPE SetOwner(LPTYPE lpObj, DWORD nIndex);
	DWORD GetChildCount() const { LPLINK p=m_link.getChild(); return p?p->count():0; }
	DWORD GetOrder() const { return m_link.getOrder(); }
	LPTYPE GetChild(DWORD nIndex) const { LPLINK p=m_link.getChild(nIndex); return p?p->object():NULL; }
	LPTYPE AddChild(LPTYPE lpObj, DWORD nIndex);
private:
	LPTYPE prev() const { LPLINK p=m_link.prev(); return p?p->object():NULL; }
	//LPTYPE next() const { LPLINK p=m_link.next(); return p?p->object():NULL; }
	//LPTYPE top() const { LPLINK p=m_link.top(); return p?p->object():NULL; }
	LPTYPE end() const { LPLINK p=m_link.end(); return p?p->object():NULL; }
protected:
	// チェインディスパッチャ
	BOOL handleTranslateMessage(LPMSG lpMsg);
	LRESULT handleProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT unhandledProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void handleBeforeCreate(LPCREATESTRUCT lpcs);
	void handleAfterCreate();
	BOOL handleBeforeClose();
	BOOL handleAfterClose();
	void handleEnableProc();
	void handleDisableProc();
protected:
	// enable/disable時のフック
	virtual void enableProc() {}
	virtual void disableProc() {}
protected:
	// メッセージフィルタ
	virtual BOOL translateProc(LPMSG lpMsg) { return FALSE; }
protected:
	// WM_CREATE,WM_CLOSE時のフック
	virtual LRESULT beforeCreateHook(HWND hwnd, LPCBT_CREATEWND pCreateWnd) { return 0L; }	// CCBTHookCreateWndが利用するプレースフォルダ
	virtual void beforeCreateProc(LPCREATESTRUCT lpcs) {}	// WM_CREATEの処理直前に呼ばれる(csを変更する最後のチャンス)
	virtual void afterCreateProc()	{}	// WM_CREATEの処理直後に呼ばれる(既に有効なHWND値が利用可能)
	virtual BOOL beforeCloseProc()	{ return TRUE; }	// WM_CLOSEの処理直前に呼ばれる(FALSEでWM_CLOSEを拒否する)
	virtual BOOL afterCloseProc()	{ return TRUE; }	// WM_CLOSEの処理直後に呼ばれる(FALSEで::PostQuitMessage(0)する)
protected:
	// ディスパッチャ、ハンドラ
	virtual LRESULT windowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);	// ディスパッチャ(通常オーバーライドしない)
	virtual LRESULT wmDefault(UINT uMsg, WPARAM wParam, LPARAM lParam);		// デフォルト処理(通常オーバーライドしない)
	virtual LRESULT wmUnhandled(UINT uMsg, WPARAM wParam, LPARAM lParam);	// 未ハンドル処理フィルタ
	virtual LRESULT wmUser(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmCreateTrust(WPARAM wParam, LPARAM lParam);			// スレッドを跨ぐウィンドウ作成処理
	virtual LRESULT wmDestroyTrust(WPARAM wParam, LPARAM lParam);			// スレッドを跨ぐウィンドウ破棄処理
	virtual LRESULT wmManHandler(WPARAM wParam, LPARAM lParam);				// リンク操作ハンドラ(通常オーバーライドしない)
protected:
#define HMC_CWINDOWHANDLER_ALIST	\
	/* 一般ハンドラ */	\
	METHOD(WM_CREATE,wmCreate)	\
	METHOD(WM_DESTROY,wmDestroy)	\
	METHOD(WM_CLOSE,wmClose)	\
	METHOD(WM_COMMAND,wmCommand)	\
	METHOD(WM_SYSCOMMAND,wmSysCommand)	\
	METHOD(WM_PARENTNOTIFY,wmParentNotify)	\
	/* 描画、パレット */	\
	METHOD(WM_PAINT,wmPaint)	\
	METHOD(WM_ERASEBKGND,wmEraseBkgnd)	\
	METHOD(WM_QUERYNEWPALETTE,wmQueryNewPalette)	\
	METHOD(WM_PALETTECHANGED,wmPaletteChanged)	\
	METHOD(WM_PALETTEISCHANGING,wmPaletteIsChanging)	\
	METHOD(WM_SYSCOLORCHANGE,wmSysColorChange)	\
	/* マウス関係 */	\
	METHOD(WM_SETCURSOR,wmSetCursor)	\
	METHOD(WM_MOUSEMOVE,wmMouseMove)	\
	METHOD(WM_MOUSEWHEEL,wmMouseWheel)	\
	METHOD(WM_LBUTTONDOWN,wmLButtonDown)	\
	METHOD(WM_RBUTTONDOWN,wmRButtonDown)	\
	METHOD(WM_MBUTTONDOWN,wmMButtonDown)	\
	METHOD(WM_LBUTTONUP,wmLButtonUp)	\
	METHOD(WM_RBUTTONUP,wmRButtonUp)	\
	METHOD(WM_MBUTTONUP,wmMButtonUp)	\
	METHOD(WM_LBUTTONDBLCLK,wmLButtonDblClk)	\
	METHOD(WM_RBUTTONDBLCLK,wmRButtonDblClk)	\
	METHOD(WM_MBUTTONDBLCLK,wmMButtonDblClk)	\
	METHOD(WM_MOUSELEAVE,wmMouseLeave)	\
	METHOD(WM_MOUSEHOVER,wmMouseHover)	\
	/* キーボード関係 */	\
	METHOD(WM_CHAR,wmChar)	\
	METHOD(WM_KEYDOWN,wmKeyDown)	\
	METHOD(WM_KEYUP,wmKeyUp)	\
	METHOD(WM_SYSCHAR,wmSysChar)	\
	METHOD(WM_SYSKEYDOWN,wmSysKeyDown)	\
	METHOD(WM_SYSKEYUP,wmSysKeyUp)	\
	/* フォーカス、キャプチャ */	\
	METHOD(WM_ACTIVATE,wmActivate)	\
	METHOD(WM_ACTIVATEAPP,wmActivateApp)	\
	METHOD(WM_MOUSEACTIVATE,wmMouseActivate)	\
	METHOD(WM_ENABLE,wmEnable)	\
	METHOD(WM_SETFOCUS,wmSetFocus)	\
	METHOD(WM_KILLFOCUS,wmKillFocus)	\
	METHOD(WM_CAPTURECHANGED,wmCaptureChanged)	\
	METHOD(WM_CANCELMODE,wmCancelMode)	\
	/* 移動、サイズ変更 */	\
	METHOD(WM_SHOWWINDOW,wmShowWindow)	\
	METHOD(WM_QUERYOPEN,wmQueryOpen)	\
	METHOD(WM_SIZE,wmSize)	\
	METHOD(WM_MOVE,wmMove)	\
	METHOD(WM_SIZING,wmSizing)	\
	METHOD(WM_MOVING,wmMoving)	\
	METHOD(WM_ENTERSIZEMOVE,wmEnterSizeMove)	\
	METHOD(WM_EXITSIZEMOVE,wmExitSizeMove)	\
	METHOD(WM_WINDOWPOSCHANGED,wmWindowPosChanged)	\
	METHOD(WM_WINDOWPOSCHANGING,wmWindowPosChanging)	\
	METHOD(WM_GETMINMAXINFO,wmGetMinMaxInfo)	\
	/* メニュー関係 */	\
	METHOD(WM_INITMENU,wmInitMenu)	\
	METHOD(WM_INITMENUPOPUP,wmInitMenuPopup)	\
	METHOD(WM_ENTERMENULOOP,wmEnterMenuLoop)	\
	METHOD(WM_EXITMENULOOP,wmExitMenuLoop)	\
	METHOD(WM_ENTERIDLE,wmEnterIdle)	\
	METHOD(WM_MENUCHAR,wmMenuChar)	\
	METHOD(WM_MENUSELECT,wmMenuSelect)	\
	/* スクロールバー関係 */	\
	METHOD(WM_HSCROLL,wmHScroll)	\
	METHOD(WM_VSCROLL,wmVScroll)	\
	/* 非クライアント系 */	\
	METHOD(WM_NCACTIVATE,wmNcActivate)	\
	METHOD(WM_NCHITTEST,wmNcHitTest)	\
	/* シェル系 */	\
	METHOD(WM_CONTEXTMENU,wmContextMenu)	\
	METHOD(WM_DROPFILES,wmDropFiles)	\
	METHOD(WM_HELP,wmHelp)	\
	/* タイマー系 */	\
	METHOD(WM_TIMER,wmTimer)	\
	/* WM_USER系 */	\
	METHOD(WM_USERIDLE,wmUserIdle)	\
	METHOD(WM_LBUTTONCLK,wmLButtonClk)	\
	METHOD(WM_RBUTTONCLK,wmRButtonClk)	\
	METHOD(WM_MBUTTONCLK,wmMButtonClk)	\
	METHOD(WM_MOUSEUNHOVER,wmMouseUnhover)	\
	METHOD(WM_QUERYCANCELAUTOPLAY,wmQueryCancelAutoPlay)	\
	/* ダイアログ系 */	\
	METHOD(WM_INITDIALOG,wmInitDialog)	\
	METHOD(WM_GETDLGCODE,wmGetDlgCode)	\
	/* コントロール系 */	\
	METHOD(WM_NOTIFY,wmNotify)	\
	METHOD(WM_GETFONT,wmGetFont)	\
	METHOD(WM_SETFONT,wmSetFont)	\
	METHOD(WM_DRAWITEM,wmDrawItem)	\
	METHOD(WM_MEASUREITEM,wmMeasureItem)	\
	METHOD(WM_CTLCOLORMSGBOX,wmCtlColorMsgBox)	\
	METHOD(WM_CTLCOLOREDIT,wmCtlColorEdit)	\
	METHOD(WM_CTLCOLORLISTBOX,wmCtlColorListBox)	\
	METHOD(WM_CTLCOLORBTN,wmCtlColorBtn)	\
	METHOD(WM_CTLCOLORDLG,wmCtlColorDlg)	\
	METHOD(WM_CTLCOLORSCROLLBAR,wmCtlColorScrollBar)	\
	METHOD(WM_CTLCOLORSTATIC,wmCtlColorStatic)	\
	/* システム系 */	\
	METHOD(WM_SETTINGCHANGE,wmSettingChange)	\
	METHOD(WM_DEVICECHANGE,wmDeviceChange)	\
	METHOD(WM_DEVMODECHANGE,wmDevModeChange)	\
	METHOD(WM_DISPLAYCHANGE,wmDisplayChange)	\
	METHOD(WM_TIMECHANGE,wmTimeChange)	\
	METHOD(WM_POWER,wmPower)	\
	METHOD(WM_POWERBROADCAST,wmPowerBroadcast)	\
	METHOD(WM_QUERYENDSESSION,wmQueryEndSession)	\
	METHOD(WM_ENDSESSION,wmEndSession)
#define METHOD(msg,handler) virtual LRESULT handler(WPARAM,LPARAM);
	HMC_CWINDOWHANDLER_ALIST
#undef METHOD
private:
	// 関節呼び出しパラメータ
	struct LINKTRUSTPARAMS {
		LINKTRUSTPARAMS() {}
		LINKTRUSTPARAMS(LPTYPE pContext, LPTYPE pObj=NULL, size_t nIndex=-1) : pContext(pContext), pObj(pObj), nIndex(nIndex) {}
		LPTYPE pContext;
		LPTYPE pObj;
		size_t nIndex;
	};
	DECLARE_SYNONYM(LINKTRUSTPARAMS,LINKTRUSTPARAMS);
public:
	// クラス情報の取得/設定
	BOOL isEnable() const { return m_bEnable; }
	BOOL enable(BOOL f) { return f?enable():disable(); }
	BOOL enable();
	BOOL disable();
	BOOL enableByDeferCommit();		// PostMessage()を利用した遅延enable()
	BOOL disableByDeferCommit();	// PostMessage()を利用した遅延disable()
private:
	LINK m_link;		// オブジェクトチェイン
	BOOL m_bEnable;		// ハンドラの有効無効フラグ
};
#pragma warning(pop)
/*****************************************************************
	実用ウィンドウクラス
*****************************************************************/
class CWindow : public CWindowHandler
{
public:
	DECLARE_SYNONYM(CWindow,TYPE);
	DECLARE_SYNONYM(CWindowHandler,BASE);
	DECLARE_SYNONYM(CWnd,OBJ);
	operator LPTYPE() { return this; }
public:
	CWindow() { init(); }
	CWindow(HWND hwnd) { init(); map(hwnd); }
	CWindow(HWND hwnd, UINT nId) { init(); map(hwnd,nId); }
	CWindow(LPCTSTR lpClass, LPCTSTR lpTitle=NULL, HWND hParent=NULL, HMENU hMenu=NULL, DWORD dwStyle=0L, DWORD dwExStyle=0L, int x=CW_USEDEFAULT, int y=CW_USEDEFAULT, int w=CW_USEDEFAULT, int h=CW_USEDEFAULT) { init(); create(lpClass,lpTitle,hParent,hMenu,dwStyle,dwExStyle,x,y,w,h); }
	CWindow(HWND hTrust, LPCTSTR lpClass=NULL, LPCTSTR lpTitle=NULL, HWND hParent=NULL, HMENU hMenu=NULL, DWORD dwStyle=0L, DWORD dwExStyle=0L, int x=CW_USEDEFAULT, int y=CW_USEDEFAULT, int w=CW_USEDEFAULT, int h=CW_USEDEFAULT) { init(); create(hTrust,lpClass,lpTitle,hParent,hMenu,dwStyle,dwExStyle,x,y,w,h); }
	CWindow(DWORD dwTrust, LPCTSTR lpClass=NULL, LPCTSTR lpTitle=NULL, HWND hParent=NULL, HMENU hMenu=NULL, DWORD dwStyle=0L, DWORD dwExStyle=0L, int x=CW_USEDEFAULT, int y=CW_USEDEFAULT, int w=CW_USEDEFAULT, int h=CW_USEDEFAULT) { init(); create(dwTrust,lpClass,lpTitle,hParent,hMenu,dwStyle,dwExStyle,x,y,w,h); }
	CWindow(HINSTANCE hInstance, LPCTSTR lpClass, LPCTSTR lpTitle=NULL, HWND hParent=NULL, HMENU hMenu=NULL, DWORD dwStyle=0L, DWORD dwExStyle=0L, int x=CW_USEDEFAULT, int y=CW_USEDEFAULT, int w=CW_USEDEFAULT, int h=CW_USEDEFAULT) { init(); create(hInstance,lpClass,lpTitle,hParent,hMenu,dwStyle,dwExStyle,x,y,w,h); }
	CWindow(HWND hTrust, HINSTANCE hInstance, LPCTSTR lpClass=NULL, LPCTSTR lpTitle=NULL, HWND hParent=NULL, HMENU hMenu=NULL, DWORD dwStyle=0L, DWORD dwExStyle=0L, int x=CW_USEDEFAULT, int y=CW_USEDEFAULT, int w=CW_USEDEFAULT, int h=CW_USEDEFAULT) { init(); create(hTrust,hInstance,lpClass,lpTitle,hParent,hMenu,dwStyle,dwExStyle,x,y,w,h); }
	CWindow(DWORD dwTrust, HINSTANCE hInstance, LPCTSTR lpClass=NULL, LPCTSTR lpTitle=NULL, HWND hParent=NULL, HMENU hMenu=NULL, DWORD dwStyle=0L, DWORD dwExStyle=0L, int x=CW_USEDEFAULT, int y=CW_USEDEFAULT, int w=CW_USEDEFAULT, int h=CW_USEDEFAULT) { init(); create(dwTrust,hInstance,lpClass,lpTitle,hParent,hMenu,dwStyle,dwExStyle,x,y,w,h); }
	virtual ~CWindow() { destroy(); }
private:
	void init() { m_pWndProc=NULL; }
public:
	void destroy();
public:
	// オブジェクトの作成/削除
	HWND create(LPCTSTR lpClass=NULL, LPCTSTR lpTitle=NULL, HWND hParent=NULL, HMENU hMenu=NULL, DWORD dwStyle=0L, DWORD dwExStyle=0L, int x=CW_USEDEFAULT, int y=CW_USEDEFAULT, int w=CW_USEDEFAULT, int h=CW_USEDEFAULT);
	HWND create(HWND hTrust, LPCTSTR lpClass=NULL, LPCTSTR lpTitle=NULL, HWND hParent=NULL, HMENU hMenu=NULL, DWORD dwStyle=0L, DWORD dwExStyle=0L, int x=CW_USEDEFAULT, int y=CW_USEDEFAULT, int w=CW_USEDEFAULT, int h=CW_USEDEFAULT);
	HWND create(DWORD dwTrust, LPCTSTR lpClass=NULL, LPCTSTR lpTitle=NULL, HWND hParent=NULL, HMENU hMenu=NULL, DWORD dwStyle=0L, DWORD dwExStyle=0L, int x=CW_USEDEFAULT, int y=CW_USEDEFAULT, int w=CW_USEDEFAULT, int h=CW_USEDEFAULT);
	HWND create(HINSTANCE hInstance, LPCTSTR lpClass=NULL, LPCTSTR lpTitle=NULL, HWND hParent=NULL, HMENU hMenu=NULL, DWORD dwStyle=0L, DWORD dwExStyle=0L, int x=CW_USEDEFAULT, int y=CW_USEDEFAULT, int w=CW_USEDEFAULT, int h=CW_USEDEFAULT);
	HWND create(HWND hTrust, HINSTANCE hInstance, LPCTSTR lpClass=NULL, LPCTSTR lpTitle=NULL, HWND hParent=NULL, HMENU hMenu=NULL, DWORD dwStyle=0L, DWORD dwExStyle=0L, int x=CW_USEDEFAULT, int y=CW_USEDEFAULT, int w=CW_USEDEFAULT, int h=CW_USEDEFAULT);
	HWND create(DWORD dwTrust, HINSTANCE hInstance, LPCTSTR lpClass=NULL, LPCTSTR lpTitle=NULL, HWND hParent=NULL, HMENU hMenu=NULL, DWORD dwStyle=0L, DWORD dwExStyle=0L, int x=CW_USEDEFAULT, int y=CW_USEDEFAULT, int w=CW_USEDEFAULT, int h=CW_USEDEFAULT);
	HWND subclass(HWND hwnd);
	// テンポラリマップの作成/削除
	void map(HWND hwnd) { destroy(); BASE::map(hwnd); }
	void map(HWND hwnd, UINT nId) { destroy(); map(::GetDlgItem(hwnd,nId)); }
public:
	// メッセージループ(主にモーダルループに用いる)
	static LRESULT PumpMessage();
	LRESULT pumpMessage();
private:
	// 作成/削除エンジン
	BOOL createEngine(const LPWNDCLASSEX lpwcex);
	BOOL createEngine(LPCTSTR lpClassName);
	static BOOL RegisterEngine(LPWNDCLASSEX lpwcex, LPTSTR lpAltClass, LPDWORD lpcbAltClass);
	static BOOL UnregisterEngine(LPCTSTR lpClassName, HINSTANCE hInstance);
protected:
	// ビルトインディスパッチャ
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	// CWindow用のメッセージフィルタ
	static BOOL PreTranslateMessage(LPMSG lpMsg);
protected:
	// ディスパッチャ、ハンドラ
	virtual LRESULT wmDefault(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmCreateTrust(WPARAM wParam, LPARAM lParam);
	virtual BOOL beforeCloseProc();	// 擬似モードレス対策……やばい？
protected:
	// ウィンドウ作成時のフック
	virtual DWORD	createWindowExStyle()	const { return 0L; }
	virtual LPCTSTR	createWindowTitle()		const { return identityName(); }
	virtual DWORD	createWindowStyle()		const { return WS_OVERLAPPEDWINDOW; }
	virtual int		createWindowPosX()		const { return CW_USEDEFAULT; }
	virtual int		createWindowPosY()		const { return CW_USEDEFAULT; }
	virtual int		createWindowWidth()		const { return CW_USEDEFAULT; }
	virtual int		createWindowHeight()	const { return CW_USEDEFAULT; }
	virtual HWND	createWindowParent()	const { return NULL; }
	virtual HMENU	createWindowMenu()		const { return NULL; }	// 子ウィンドウの場合はIDを返す
protected:
	// クラス作成時のフック
	virtual UINT	wndclassStyle()			const { return 0/*CS_VREDRAW|CS_HREDRAW*/; }
	virtual int		wndclassClsExtra()		const { return 0; }
	virtual int		wndclassWndExtra()		const { return 0; }
	virtual HICON	wndclassIcon()			const { return ::LoadIcon(NULL,IDI_APPLICATION); }
	virtual HCURSOR	wndclassCursor()		const { return ::LoadCursor(NULL,IDC_ARROW); }
	virtual HBRUSH	wndclassBackground()	const { return HBRUSH(COLOR_WINDOW+1); }
	virtual LPCTSTR	wndclassMenuName()		const { return NULL; }
	virtual LPCTSTR	wndclassClassName()		const { return identityName(); }
	virtual HICON	wndclassSmallIcon()		const { return NULL; }
public:
	// 各種操作
	BOOL isEnable() const { return OBJ::isEnable(); }
	BOOL enable(BOOL fEnable=TRUE) { return OBJ::enable(fEnable); }
	BOOL disable() { return enable(FALSE); }
	// クラス情報の取得/設定
	BOOL isValid() const { return OBJ::isValid(); }
public:
	// 識別名(SetPropやデフォルトクラス名に利用する)
	static  LPCTSTR PropertyName() { return "HMC::CWindow"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
protected:
	// HWNDとLPTYPEの関連付け操作
	//※GetProp()を利用した方がGetWindowLong()を利用するより汎用性が高い
	//※GetProp()はGetWindowLong()の２倍以上(実測)コストが掛かる(よく考えると驚くほど高速)
	//※GetProp()は自前で関連付けテーブルを検索するより高速(実測)
#if 1
	static BOOL SetObject(HWND hwnd, LPTYPE pObj) { BOOL b=::SetProp(hwnd,PropertyName(),HANDLE(pObj)); CHECKRESULT_API(b); return b; }
	static LPTYPE RemoveObject(HWND hwnd) { LPTYPE p=LPTYPE(::RemoveProp(hwnd,PropertyName())); CHECKRESULT_API(p); return p; }
	static LPTYPE GetObject(HWND hwnd) { return LPTYPE(::GetProp(hwnd,PropertyName())); }
#else
	static LPTYPE SetObject(HWND hwnd, LPTYPE pObj) { return LPTYPE(::SetWindowLongPtr(hwnd,GWLP_USERDATA,LONG_PTR(pObj))); }
	static LPTYPE RemoveObject(HWND hwnd) { return SetObject(hwnd,NULL); }
	static LPTYPE GetObject(HWND hwnd) { return LPTYPE(::GetWindowLongPtr(hwnd,GWLP_USERDATA)); }
#endif
public:
	// ハンドルからオブジェクトポインタを得る(CWindowHandler::FromHandle()とは型が違うことに注意)
	static LPTYPE FromHandle(HWND hwnd) { return GetObject(hwnd); }
private:
	// 他スレッドへのCREATE依頼用パラメータ
	struct CREATETRUSTPARAMS {
		HANDLE hEvent;
		LPCREATESTRUCT lpcs;
	};
	DECLARE_SYNONYM(CREATETRUSTPARAMS,CREATETRUSTPARAMS);
private:
	WNDPROC m_pWndProc;	// スーパークラスのWNDPROC
};

/*****************************************************************
	フックハンドラ(ハンドラの挙動を置き換える)
*****************************************************************/
/*****************************************************************
	SDIフレームウィンドウ
*****************************************************************/
class CWindowHandlerFrame : public CWindowHandler
{
public:
	DECLARE_SYNONYM(CWindowHandlerFrame,TYPE);
	DECLARE_SYNONYM(CWindowHandler,BASE);
public:
	CWindowHandlerFrame() { init(); }
	CWindowHandlerFrame(LPBASE p) : BASE(p) { init(); }
	virtual ~CWindowHandlerFrame() { destroy(); }
private:
	void init() {}
public:
	void destroy() { init(); }
protected:
	virtual LRESULT wmSize(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmWindowPosChanged(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmActivate(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmKeyDown(WPARAM wParam, LPARAM lParam);			// redirect
	virtual LRESULT wmKeyUp(WPARAM wParam, LPARAM lParam);				// redirect
	virtual LRESULT wmPaletteChanged(WPARAM wParam, LPARAM lParam);		// redirect
	virtual LRESULT wmPaletteIsChanging(WPARAM wParam, LPARAM lParam);	// redirect
	virtual LRESULT wmQueryNewPalette(WPARAM wParam, LPARAM lParam);	// redirect
protected:
	virtual BOOL beforeCloseProc();
	virtual BOOL afterCloseProc() { return FALSE; }
public:
	// 識別名(SetPropやデフォルトクラス名に利用する)
	static  LPCTSTR PropertyName() { return "HMC::CWindowHandlerFrame"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
};

/*****************************************************************
	ビューウィンドウ
*****************************************************************/
class CWindowHandlerView : public CWindowHandler
{
public:
	DECLARE_SYNONYM(CWindowHandlerView,TYPE);
	DECLARE_SYNONYM(CWindowHandler,BASE);
public:
	CWindowHandlerView() { init(); }
	CWindowHandlerView(LPBASE p) : BASE(p) { init(); }
	virtual ~CWindowHandlerView() { destroy(); }
private:
	void init() {}
public:
	void destroy() { init(); }
protected:
	virtual void beforeCreateProc(LPCREATESTRUCT lpcs);
public:
	// 識別名(SetPropやデフォルトクラス名に利用する)
	static  LPCTSTR PropertyName() { return "HMC::CWindowHandlerView"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
};

/*****************************************************************
	所有するポップアップを同期的に移動させる
*****************************************************************/
class CWindowHandlerTrackOwnedPopups : public CWindowHandler
{
public:
	DECLARE_SYNONYM(CWindowHandlerTrackOwnedPopups,TYPE);
	DECLARE_SYNONYM(CWindowHandler,BASE);
public:
	CWindowHandlerTrackOwnedPopups() { init(); }
	CWindowHandlerTrackOwnedPopups(LPBASE p) : BASE(p) { init(); }
	virtual ~CWindowHandlerTrackOwnedPopups() { destroy(); }
private:
	void init() { m_bMoving=FALSE; m_nLastX=0; m_nLastY=0; }
public:
	void destroy() { init(); }
protected:
	virtual LRESULT wmEnterSizeMove(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmExitSizeMove(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmMoving(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmMove(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmSizing(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmSize(WPARAM wParam, LPARAM lParam);
public:
	BOOL isMoving() const { return m_bMoving; }
public:
	// 識別名(SetPropやデフォルトクラス名に利用する)
	static  LPCTSTR PropertyName() { return "HMC::CWindowHandlerTrackOwnedPopups"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
private:
	BOOL m_bMoving;	// 移動中フラグ
	LONG m_nLastX;	// 移動開始時のウィンドウ位置
	LONG m_nLastY;	// 移動開始時のウィンドウ位置
};

/*****************************************************************
	クライアントエリア内のみで有効なカーソル形状設定
*****************************************************************/
class CWindowHandlerSetCursor : public CWindowHandler
{
public:
	DECLARE_SYNONYM(CWindowHandlerSetCursor,TYPE);
	DECLARE_SYNONYM(CWindowHandler,BASE);
public:
	CWindowHandlerSetCursor() { init(); }
	CWindowHandlerSetCursor(LPBASE p) : BASE(p) { init(); }
	virtual ~CWindowHandlerSetCursor() { destroy(); }
private:
	void init() { handle(IDC_ARROW); }
public:
	void destroy() { init(); }
protected:
	virtual void enableProc();
	virtual void disableProc();
protected:
	virtual LRESULT wmSetCursor(WPARAM wParam, LPARAM lParam);
public:
	HCURSOR handle() const { return m_hCursor; }
	HCURSOR handle(HCURSOR h);
	HCURSOR handle(LPCTSTR p) { return handle(NULL,p); }
	HCURSOR handle(HMODULE h, LPCTSTR p) { return handle(::LoadCursor(h,p)); }
public:
	// 識別名(SetPropやデフォルトクラス名に利用する)
	static  LPCTSTR PropertyName() { return "HMC::CWindowHandlerSetCursor"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
private:
	HCURSOR m_hCursor;	// カーソルハンドル
};

/*****************************************************************
	マウスカーソル移動
*****************************************************************/
class CWindowHandlerMouseMove : public CWindowHandler
{
public:
	DECLARE_SYNONYM(CWindowHandlerMouseMove,TYPE);
	DECLARE_SYNONYM(CWindowHandler,BASE);
public:
	CWindowHandlerMouseMove() { init(); }
	CWindowHandlerMouseMove(LPBASE p) : BASE(p) { init(); }
	virtual ~CWindowHandlerMouseMove() { destroy(); }
private:
	void init();
public:
	void destroy() { endTimer(); init(); }
protected:
	virtual void enableProc() { BASE::enableProc(); destroy(); }
	virtual void disableProc() { destroy(); BASE::disableProc(); }
public:
	virtual LRESULT wmTimer(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmMouseMove(WPARAM wParam, LPARAM lParam);
public:
	void move(int x, int y);
public:
	BOOL isMoving() const { return !!m_nCounter; }
private:
	void beginTimer();
	void endTimer();
public:
	// 識別名(SetPropやデフォルトクラス名に利用する)
	static  LPCTSTR PropertyName() { return "HMC::CWindowHandlerMouseMove"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
private:
	LONG m_nCounter;
	UINT m_nTimerId;
	CPoint m_ptLast;
	CPoint m_ptStart;
	CPoint m_ptGoal;
};

/*****************************************************************
	シングルクリックメッセージ変換
*****************************************************************/
class CWindowHandlerSingleClick : public CWindowHandler
{
public:
	DECLARE_SYNONYM(CWindowHandlerSingleClick,TYPE);
	DECLARE_SYNONYM(CWindowHandler,BASE);
public:
	CWindowHandlerSingleClick() { init(); }
	CWindowHandlerSingleClick(LPBASE p) : BASE(p) { init(); }
	virtual ~CWindowHandlerSingleClick() { destroy(); }
private:
	void init();
public:
	void destroy() { init(); }
protected:
	virtual void enableProc();
	virtual void disableProc();
protected:
	virtual LRESULT wmLButtonDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmMButtonDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmRButtonDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmLButtonUp(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmMButtonUp(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmRButtonUp(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmCaptureChanged(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmCancelMode(WPARAM wParam, LPARAM lParam);
public:
	BOOL isPressedLButton() const { return m_bLButton; }
	BOOL isPressedMButton() const { return m_bMButton; }
	BOOL isPressedRButton() const { return m_bRButton; }
public:
	// 識別名(SetPropやデフォルトクラス名に利用する)
	static  LPCTSTR PropertyName() { return "HMC::CWindowHandlerSingleClick"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
private:
	BOOL m_bLButton;	// 現在の左ボタン押下情報
	BOOL m_bMButton;	// 現在の中ボタン押下情報
	BOOL m_bRButton;	// 現在の右ボタン押下情報
	CPoint m_ptLButton;	// 左ボタン押下時のカーソル位置
	CPoint m_ptMButton;	// 中ボタン押下時のカーソル位置
	CPoint m_ptRButton;	// 右ボタン押下時のカーソル位置
};

/*****************************************************************
	クライアントエリアドラッグ
*****************************************************************/
class CWindowHandlerClientDrag : public CWindowHandler
{
public:
	DECLARE_SYNONYM(CWindowHandlerClientDrag,TYPE);
	DECLARE_SYNONYM(CWindowHandler,BASE);
public:
	CWindowHandlerClientDrag() { init(); }
	CWindowHandlerClientDrag(LPBASE p) : BASE(p) { init(); }
	virtual ~CWindowHandlerClientDrag() { destroy(); }
private:
	void init();
public:
	void destroy() { init(); }
protected:
	virtual void enableProc() { BASE::enableProc(); destroy(); }
	virtual void disableProc() { destroy(); BASE::disableProc(); }
protected:
	virtual LRESULT wmLButtonDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmLButtonUp(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmMouseMove(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmCaptureChanged(WPARAM wParam, LPARAM lParam);
public:
	BOOL isMoving() const { return m_bMoving; }
public:
	// 識別名(SetPropやデフォルトクラス名に利用する)
	static  LPCTSTR PropertyName() { return "HMC::CWindowHandlerClientDrag"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
private:
	BOOL m_bMoving;		// ドラッグ中であることを表すフラグ
	CWnd m_wnd;			// 移動対象のウィンドウ
	CPoint m_ptCursor;	// ドラッグ開始時のポインタ位置
	CPoint m_ptWindow;	// ドラッグ開始時のウィンドウ位置
};

/*****************************************************************
	WM_QUERYCANSELAUTOPLAYメッセージ変換
*****************************************************************/
//※HWND値に無関係に動作するので、各スレッドに一個あれば全てのウィンドウに作用する
class CWindowHandlerTrackQueryCancelAutoPlay : public CWindowHandler
{
public:
	DECLARE_SYNONYM(CWindowHandlerTrackQueryCancelAutoPlay,TYPE);
	DECLARE_SYNONYM(CWindowHandler,BASE);
public:
	CWindowHandlerTrackQueryCancelAutoPlay() { init(); }
	CWindowHandlerTrackQueryCancelAutoPlay(LPBASE p) : BASE(p) { init(); }
	virtual ~CWindowHandlerTrackQueryCancelAutoPlay() { destroy(); }
private:
	void init();
public:
	void destroy();
protected:
	virtual BOOL translateProc(LPMSG lpMsg);
public:
	// 識別名(SetPropやデフォルトクラス名に利用する)
	static  LPCTSTR PropertyName() { return "HMC::CWindowHandlerTrackQueryCancelAutoPlay"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
private:
	UINT m_uQueryCancelAutoPlay;		// 登録済みメッセージ
};

/*****************************************************************
	WM_MOUSEWHEELメッセージ変換
*****************************************************************/
//※HWND値に無関係に動作するので、各スレッドに一個あれば全てのウィンドウに作用する
class CWindowHandlerTrackMouseWheel : public CWindowHandler
{
public:
	DECLARE_SYNONYM(CWindowHandlerTrackMouseWheel,TYPE);
	DECLARE_SYNONYM(CWindowHandler,BASE);
public:
	CWindowHandlerTrackMouseWheel() { init(); }
	CWindowHandlerTrackMouseWheel(LPBASE p) : BASE(p) { init(); }
	virtual ~CWindowHandlerTrackMouseWheel() { destroy(); }
private:
	void init();
public:
	void destroy();
protected:
	virtual BOOL translateProc(LPMSG lpMsg);
public:
	// 識別名(SetPropやデフォルトクラス名に利用する)
	static  LPCTSTR PropertyName() { return "HMC::CWindowHandlerTrackMouseWheel"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
private:
	UINT m_uMSH_MOUSEWHEEL;		// 登録済みメッセージ
};

/*****************************************************************
	WM_MOUSELEAVEエミュレーション
*****************************************************************/
class CWindowHandlerTrackMouseLeave : public CWindowHandler
{
public:
	DECLARE_SYNONYM(CWindowHandlerTrackMouseLeave,TYPE);
	DECLARE_SYNONYM(CWindowHandler,BASE);
public:
	CWindowHandlerTrackMouseLeave() { init(); }
	CWindowHandlerTrackMouseLeave(LPBASE p) : BASE(p) { init(); }
	virtual ~CWindowHandlerTrackMouseLeave() { destroy(); }
private:
	void init();
public:
	void destroy() { init(); }
protected:
	virtual void enableProc();
	virtual void disableProc();
protected:
	virtual LRESULT wmTimer(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmMouseMove(WPARAM wParam, LPARAM lParam);
protected:
	virtual UINT pollingInterval() const { return 250; }
public:
	BOOL isLeaved() const { return m_bLeaved; }
public:
	// 識別名(SetPropやデフォルトクラス名に利用する)
	static  LPCTSTR PropertyName() { return "HMC::CWindowHandlerTrackMouseLeave"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
private:
	UINT m_nTimerId;
	BOOL m_bLeaved;
};

/*****************************************************************
	WM_MOUSEHOVER+WM_MOUSEUNHOVERエミュレーション
*****************************************************************/
class CWindowHandlerTrackMouseHover : public CWindowHandler
{
public:
	DECLARE_SYNONYM(CWindowHandlerTrackMouseHover,TYPE);
	DECLARE_SYNONYM(CWindowHandler,BASE);
public:
	CWindowHandlerTrackMouseHover() { init(); }
	CWindowHandlerTrackMouseHover(LPBASE p) : BASE(p) { init(); }
	virtual ~CWindowHandlerTrackMouseHover() { destroy(); }
private:
	void init();
public:
	void destroy() { BASE::destroy(); init(); }
protected:
	virtual void enableProc();
	virtual void disableProc();
protected:
	virtual LRESULT wmTimer(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmMouseMove(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmMouseHover(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmMouseUnhover(WPARAM wParam, LPARAM lParam);
protected:
	virtual UINT hoverTime() const { return ::GetDoubleClickTime(); }
	virtual UINT hoverWidth() const { return ::GetSystemMetrics(SM_CXDOUBLECLK); }
	virtual UINT hoverHeight() const { return ::GetSystemMetrics(SM_CYDOUBLECLK); }
public:
	BOOL isHovered() const { return m_bHovered; }
public:
	// 識別名(SetPropやデフォルトクラス名に利用する)
	static  LPCTSTR PropertyName() { return "HMC::CWindowHandlerTrackMouseHover"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
private:
	UINT m_nTimerId;
	BOOL m_bHovered;
	CTickTime m_LastTick;
	CPoint m_ptHovered;
};

/*****************************************************************
	ホットキー(SHIFT+ESC)によるWM_CLOSE
*****************************************************************/
// ※現状ではホットキーのオーバーライドは出来ない
class CWindowHandlerHotKeyClose : public CWindowHandler
{
public:
	DECLARE_SYNONYM(CWindowHandlerHotKeyClose,TYPE);
	DECLARE_SYNONYM(CWindowHandler,BASE);
public:
	CWindowHandlerHotKeyClose() : m_accel(FNOINVERT|FSHIFT|FVIRTKEY,VK_ESCAPE,SC_CLOSE) { init(); }
	CWindowHandlerHotKeyClose(LPBASE p) : BASE(p), m_accel(FNOINVERT|FSHIFT|FVIRTKEY,VK_ESCAPE,SC_CLOSE) { init(); }
	virtual ~CWindowHandlerHotKeyClose() { destroy(); }
private:
	void init() {}
public:
	void destroy() { init(); }
protected:
	virtual BOOL translateProc(LPMSG lpMsg);
	virtual LRESULT wmCommand(WPARAM wParam, LPARAM lParam);
public:
	// 識別名(SetPropやデフォルトクラス名に利用する)
	static  LPCTSTR PropertyName() { return "HMC::CWindowHandlerHotKeyClose"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
private:
	CAccel m_accel;	// 終了アクセラレータ
};

/*****************************************************************
	スクリーンセーバーの挙動
*****************************************************************/
#pragma warning(push)
#pragma warning(disable: 4355)
class CWindowHandlerScreenSaver : public CWindowHandler
{
public:
	DECLARE_SYNONYM(CWindowHandlerScreenSaver,TYPE);
	DECLARE_SYNONYM(CWindowHandler,BASE);
public:
#ifdef _DEBUG
	CWindowHandlerScreenSaver() : m_hotkey(this) { init(); }
	CWindowHandlerScreenSaver(LPBASE p) : BASE(p), m_hotkey(this) { init(); }
#else
	CWindowHandlerScreenSaver() { init(); }
	CWindowHandlerScreenSaver(LPBASE p) : BASE(p) { init(); }
#endif
	virtual ~CWindowHandlerScreenSaver() { destroy(); }
private:
	void init() { m_pt.let(-1,-1); }
public:
	void destroy() { init(); }
protected:
	virtual void enableProc() { BASE::enableProc(); destroy(); }
	virtual void disableProc() { destroy(); BASE::disableProc(); }
public:
	virtual LRESULT wmSysCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmSetCursor(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmActivate(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmActivateApp(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmMouseMove(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmLButtonDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmRButtonDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmMButtonDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmKeyDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmSysKeyDown(WPARAM wParam, LPARAM lParam);
public:
	// 識別名(SetPropやデフォルトクラス名に利用する)
	static  LPCTSTR PropertyName() { return "HMC::CWindowHandlerScreenSaver"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
private:
	CPoint m_pt;	// 開始時のポインタ位置
#ifdef _DEBUG
	CWindowHandlerHotKeyClose m_hotkey;
#endif
};
#pragma warning(pop)
/*****************************************************************
	メッセージ調査
*****************************************************************/
const DWORD MSGPOLL_FILTERF_MESSAGE	= 0x00000001;
const DWORD MSGPOLL_FILTERF_WPARAM	= 0x00000002;
const DWORD MSGPOLL_FILTERF_LPARAM	= 0x00000004;
struct MSGPOLL : public CLinkList<MSGPOLL> {
public:
	MSGPOLL() {
		dwFilter	= 0;
		uMsg		= 0;
		wParam		= 0;
		lParam		= 0;
		hEvent		= NULL;
		pFilter		= NULL;
	}
public:
	DWORD	dwFilter;	// フィルタフラグ(上を参照)
	UINT	uMsg;		// 対象メッセージ
	WPARAM	wParam;		// 対象メッセージ
	LPARAM	lParam;		// 対象メッセージ
	HANDLE	hEvent;		// ヒット時にセットされるイベント
	WNDPROC pFilter;	// ヒット時に呼び出されるプロシージャ(返り値が非０の場合にイベントがセットされる)
};
DECLARE_SYNONYM(MSGPOLL,MSGPOLL);
class CWindowHandlerMessagePoller : public CWindowHandler
{
public:
	DECLARE_SYNONYM(CWindowHandlerMessagePoller,TYPE);
	DECLARE_SYNONYM(CWindowHandler,BASE);
public:
	CWindowHandlerMessagePoller() { init(); }
	CWindowHandlerMessagePoller(LPBASE p) : BASE(p) { init(); }
	virtual ~CWindowHandlerMessagePoller() { destroy(); }
private:
	void init() {}
public:
	void destroy();
protected:
	virtual void enableProc() { BASE::enableProc(); destroy(); }
	virtual void disableProc() { destroy(); BASE::disableProc(); }
protected:
	virtual LRESULT windowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	// メッセージ調査(nCountを使うとMAXIMUM_WAIT_OBJECTS個以下のMSGPOLLを同時に渡すことができる)
	BOOL waitForMessage(LPMSGPOLL pMsgPoll, DWORD dwTimeout) const;
	DWORD waitForMessage(UINT nCount, LPMSGPOLL pMsgPoll, DWORD dwTimeout, BOOL fWaitAll=FALSE) const;
	void add(LPMSGPOLL pMsgPoll);
	void del(LPMSGPOLL pMsgPoll);
public:
	// 識別名(SetPropやデフォルトクラス名に利用する)
	static  LPCTSTR PropertyName() { return "HMC::CWindowHandlerMessagePoller"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
private:
	mutable CCriticalSection m_cs;
	mutable MSGPOLL m_list;
};
/*****************************************************************
	メッセージ調査(ゲーム用UI)
*****************************************************************/
class CWindowHandlerUserInputPoller : public CWindowHandlerMessagePoller
{
public:
	DECLARE_SYNONYM(CWindowHandlerUserInputPoller,TYPE);
	DECLARE_SYNONYM(CWindowHandlerMessagePoller,BASE);
public:
	CWindowHandlerUserInputPoller() { init(); }
	CWindowHandlerUserInputPoller(BASE::LPBASE p) : BASE(p) { init(); }
	virtual ~CWindowHandlerUserInputPoller() { destroy(); }
private:
	void init();
public:
	void destroy();
protected:
	virtual void enableProc() { BASE::enableProc(); destroy(); }
	virtual void disableProc() { destroy(); BASE::disableProc(); }
protected:
	virtual LRESULT wmChar(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmKeyDown(WPARAM wParam, LPARAM lParam);
public:
	TCHAR waitForChar (DWORD dwTimeout, BOOL fRemove=TRUE) const;	// キャラクタコード
	SHORT waitForVkKey(DWORD dwTimeout, BOOL fRemove=TRUE) const;	// VK+SHIFT(HIWORDに関してはVkKeyScanを参照)
	SHORT waitForEscape(DWORD dwTimeout, BOOL fReset=TRUE) const;	// VK_ESCAPE,VK_NUMPAD0,VK_RBUTTON
	SHORT waitForHitKey(DWORD dwTimeout, BOOL fReset=TRUE) const;	// VK_SPACE,VK_RETURN,VK_LBUTTON
	SHORT waitForCursor(DWORD dwTimeout, BOOL fReset=TRUE) const;	// VK_LEFT,VK_RIGHT,VK_UP,VK_DOWN,VK_NUMPAD4,6,8,2/VK_TAB,VK_PRIOR,VK_NEXT,VK_HOME,VK_END
public:
	// 識別名(SetPropやデフォルトクラス名に利用する)
	static  LPCTSTR PropertyName() { return "HMC::CWindowHandlerUserInputPoller"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
private:
	mutable MSGPOLL m_mpEsc[3];
	mutable MSGPOLL m_mpKey[3];
	mutable MSGPOLL m_mpPad[13];
	mutable CEvent m_evEsc[3];
	mutable CEvent m_evKey[3];
	mutable CEvent m_evPad[13];
	mutable CEvent m_evChar;	// WM_CHAR処理用
	mutable CEvent m_evDown;	// WM_KEYDOWN処理用
	mutable SHORT m_aChar[256];	// キーバッファ(深度は取り敢えず256固定)
	mutable SHORT m_iChar;		// インデックス
	mutable SHORT m_nChar;		// 格納数
	mutable SHORT m_aDown[256];	// キーバッファ(深度は取り敢えず256固定)
	mutable SHORT m_iDown;		// インデックス
	mutable SHORT m_nDown;		// 格納数
	mutable CCriticalSection m_cs;
};
/*****************************************************************
	フック(詳細はSetWindowsHookExを参照)
*****************************************************************/
/*****************************************************************
	CBTフック(詳細はSetWindowsHookExを参照)
*****************************************************************/
/*****************************************************************
	CBTフック基底クラス
*****************************************************************/
#pragma warning(push)
#pragma warning(disable: 4355)
class CCBTHook
{
public:
	DECLARE_SYNONYM(CCBTHook,TYPE);
	DECLARE_SYNONYM(TLinkList<TYPE>,LINK);
	operator LPTYPE() { return this; }
	friend TYPE;
public:
	CCBTHook() : m_link(this) { init(); }
	virtual ~CCBTHook() { destroy(); }
private:
	void init();
	void destroy();
protected:
	// フックプロシージャ
	static LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam);
	virtual LRESULT CALLBACK cbtProc(int nCode, WPARAM wParam, LPARAM lParam);
public:
#define HMC_CCBTHOOK_ALIST	\
	METHOD(HCBT_ACTIVATE,hcbtActivate)	\
	METHOD(HCBT_CLICKSKIPPED,hcbtClickSkipped)	\
	METHOD(HCBT_CREATEWND,hcbtCreateWnd)	\
	METHOD(HCBT_DESTROYWND,hcbtDestroyWnd)	\
	METHOD(HCBT_KEYSKIPPED,hcbtKeySkipped)	\
	METHOD(HCBT_MINMAX,hcbtMinMax)	\
	METHOD(HCBT_MOVESIZE,hcbtMoveSize)	\
	METHOD(HCBT_QS,hcbtQS)	\
	METHOD(HCBT_SETFOCUS,hcbtSetFocus)	\
	METHOD(HCBT_SYSCOMMAND,hcbtSysCommand)
#define METHOD(code,handler) virtual LRESULT handler(WPARAM,LPARAM);
	HMC_CCBTHOOK_ALIST
#undef METHOD
public:
	// クラス情報の取得/設定
	BOOL isEnable() const { return m_hHook&&m_nThreadId; }
	BOOL enable(BOOL f) { return f?enable():disable(); }
	BOOL enable();
	BOOL disable();
private:
	HHOOK m_hHook;
	DWORD m_nThreadId;
	LINK m_link;
	static LPLINK m_anchor;
	static ULONG m_cRef;
	static CCriticalSection0 m_cs;
};
#pragma warning(pop)
/*****************************************************************
	CWindow派生クラスで、HWND、LPTYPE関連付けのためのフック
*****************************************************************/
class CCBTHookCreateWnd : public CCBTHook
{
public:
	DECLARE_SYNONYM(CCBTHookCreateWnd,TYPE);
	DECLARE_SYNONYM(CCBTHook,BASE);
	DECLARE_SYNONYM(CWindowHandler,OBJ);
public:
	CCBTHookCreateWnd(LPOBJ p) { pObj=p; }	// ※初期状態はdisableなことに注意
	virtual ~CCBTHookCreateWnd() { disable(); }
protected:
	LRESULT hcbtCreateWnd(WPARAM wParam, LPARAM lParam) {
		disable();
		return pObj->beforeCreateHook(HWND(wParam),LPCBT_CREATEWND(lParam));
	}
private:
	LPOBJ pObj;
};

/*****************************************************************
	ウィンドウ列挙クラス
*****************************************************************/
/*****************************************************************
	トップレベルウィンドウの列挙(EnumWindowsラッパー)
*****************************************************************/
class CEnumWindows
{
public:
	DECLARE_SYNONYM(CEnumWindows,TYPE);
	operator LPTYPE() { return this; }
friend TYPE;
public:
	CEnumWindows() { init(); }
private:
	void init() { m_hWnd=NULL; m_nIndex=0; m_nCount=0; }
public:
	void destroy() { init(); }
public:
	// オブジェクトの作成/削除
	void create() { destroy(); }
public:
	// 各種操作
	UINT count() const { TYPE t=*this; t.nth(UINT_MAX); return t.m_nCount; }
	HWND first() { return nth(0); }
	HWND last() { return nth(count()-1);  }
	HWND next() { return nth(m_nCount+1); }
	HWND prev() { return nth(m_nCount-1); }
	// クラス情報の取得/設定
	HWND handle() const { return m_hWnd; }
	operator HWND() const { return handle(); }
	BOOL isValid() const { return !!handle(); }
public:
	// 序数nのウィンドウを取得する
	HWND nth(UINT n);
private:
	// ウィンドウ列挙コールバック
	static BOOL CALLBACK enumProc(HWND hwnd, LPARAM lParam);
	BOOL enumProc(HWND hwnd);
private:
	HWND m_hWnd;		// 現在のウィンドウハンドル
	UINT m_nIndex;		// 取得対象インデックス
	UINT m_nCount;		// 列挙カウンタ(カーソル)
};

/*****************************************************************
	子ウィンドウの列挙(EnumChildWindowsラッパー)
*****************************************************************/
class CEnumChildWindows
{
public:
	DECLARE_SYNONYM(CEnumChildWindows,TYPE);
	operator LPTYPE() { return this; }
friend TYPE;
public:
	CEnumChildWindows() { init(); }
	CEnumChildWindows(HWND hwnd) { init(); create(hwnd); }
private:
	void init() { m_hWndParent=NULL; m_hWnd=NULL; m_nIndex=0; m_nCount=0; }
public:
	void destroy() { init(); }
public:
	// オブジェクトの作成/削除
	void create(HWND hwnd) { destroy(); m_hWndParent=hwnd; }
public:
	// 各種操作
	UINT count() const { TYPE t=*this; t.nth(UINT_MAX); return t.m_nCount; }
	HWND first() { return nth(0); }
	HWND last() { return nth(count()-1);  }
	HWND next() { return nth(m_nCount+1); }
	HWND prev() { return nth(m_nCount-1); }
	// クラス情報の取得/設定
	HWND handle() const { return m_hWnd; }
	operator HWND() const { return handle(); }
	BOOL isValid() const { return !!handle(); }
public:
	// 序数nのウィンドウを取得する
	HWND nth(UINT n);
private:
	// ウィンドウ列挙コールバック
	static BOOL CALLBACK enumProc(HWND hwnd, LPARAM lParam);
	BOOL enumProc(HWND hwnd);
private:
	HWND m_hWndParent;	// 検索対象の親ウィンドウ
	HWND m_hWnd;		// 現在のウィンドウハンドル
	UINT m_nIndex;		// 取得対象インデックス
	UINT m_nCount;		// 列挙カウンタ(カーソル)
};

/*****************************************************************
	スレッドに属するウィンドウの列挙(EnumThreadWindowsラッパー)
*****************************************************************/
//※チャイルドウィンドウは列挙されないことに注意
class CEnumThreadWindows
{
public:
	DECLARE_SYNONYM(CEnumThreadWindows,TYPE);
	operator LPTYPE() { return this; }
friend TYPE;
public:
	CEnumThreadWindows() { init(); }
	CEnumThreadWindows(DWORD id) { init(); create(id); }
private:
	void init() { m_dwThreadId=::GetCurrentThreadId(); m_hWnd=NULL; m_nIndex=0; m_nCount=0; }
public:
	void destroy() { init(); }
public:
	// オブジェクトの作成/削除
	void create(DWORD id) { destroy(); m_dwThreadId=id? id: ::GetCurrentThreadId(); }
public:
	// 各種操作
	UINT count() const { TYPE t=*this; t.nth(UINT_MAX); return t.m_nCount; }
	HWND first() { return nth(0); }
	HWND last() { return nth(count()-1);  }
	HWND next() { return nth(m_nCount+1); }
	HWND prev() { return nth(m_nCount-1); }
	// クラス情報の取得/設定
	HWND handle() const { return m_hWnd; }
	operator HWND() const { return handle(); }
	BOOL isValid() const { return !!handle(); }
public:
	// 序数nのウィンドウを取得する
	HWND nth(UINT n);
private:
	// ウィンドウ列挙コールバック
	static BOOL CALLBACK enumProc(HWND hwnd, LPARAM lParam);
	BOOL enumProc(HWND hwnd);
private:
	DWORD m_dwThreadId;	// 検索対象のスレッド
	HWND m_hWnd;		// 現在のウィンドウハンドル
	UINT m_nIndex;		// 取得対象インデックス
	UINT m_nCount;		// 列挙カウンタ(カーソル)
};

/*****************************************************************
*****************************************************************/
_HMC_END
/*****************************************************************
*****************************************************************/

#endif	/* __INC_HMC_WINDOW__ */

/* Local Variables:						*/
/* tab-width:4							*/
/* c-basic-offset:4						*/
/* c-indent-level:4						*/
/* c-label-offset:-4					*/
/* c-continued-statement-offset:4		*/
/* file-coding-system:*sjis*dos			*/
/* buffer-file-coding-system:sjis-dos	*/
/* End:									*/
