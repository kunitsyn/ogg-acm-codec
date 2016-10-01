/*
	ドライバーサポートクラス
	Copyright (C)1998 H.Mutsuki
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __INC_HMC_DRIVER__
#define __INC_HMC_DRIVER__

#include "util.h"
#include "link.h"
#include <mmsystem.h>

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
*****************************************************************/
class CDrvr;
class CDriver;
/*****************************************************************
	ドライバー基底クラス
*****************************************************************/
class CDrvr
{
public:
	DECLARE_SYNONYM(CDrvr,TYPE);
	operator LPTYPE() { return this; }
public:
	CDrvr() { init(); }
	CDrvr(HDRVR h) { init(); map(h); }
	~CDrvr() { destroy(); }
private:
	void init() { m_bOwner=TRUE; m_hdrvr=NULL; }
public:
	void destroy() { if(isOwner()) close(0,0); init(); }
public:
	// オブジェクトの作成/削除
	HDRVR create(LPCWSTR lpDriverName, LPCWSTR lpSectionName, LONG lParam);
	LRESULT close(LPARAM lParam1, LPARAM lParam2);
	// テンポラリマップの作成/削除
	void map(HDRVR h) { destroy(); m_bOwner=FALSE; m_hdrvr=h; }
protected:
	void handle(HDRVR h) { m_hdrvr=h; }	// オーナーハンドル更新用(苦肉の策だねえ……)
public:
	// 各種操作
	LRESULT send(UINT uMsg, LONG lParam1, LONG lParam2) { return ::SendDriverMessage(handle(),uMsg,lParam1,lParam2); }
public:
	// 非オブジェクト操作
	static HMODULE GetModuleHandle(HDRVR hdrvr) { return ::GetDriverModuleHandle(hdrvr); }
	static HDRVR Open(LPCWSTR lpDriverName, LPCWSTR lpSectionName, LONG lParam) { return ::OpenDriver(lpDriverName,lpSectionName,lParam); }
	static LRESULT Close(HDRVR hdrvr, LONG lParam1, LONG lParam2) { return ::CloseDriver(hdrvr,lParam1,lParam2); }
	static LRESULT Send(HDRVR hdrvr, UINT uMsg, LONG lParam1, LONG lParam2) { return ::SendDriverMessage(hdrvr,uMsg,lParam1,lParam2); }
public:
	// クラス情報の取得/設定
	HDRVR handle() const { return m_hdrvr; }
	operator HDRVR() const { return handle(); }
	BOOL isValid() const { return !!handle(); }
	BOOL isOwner() const { return m_bOwner; }
	BOOL ownership(BOOL f) { return m_bOwner=f; }
private:
	BOOL m_bOwner;		// 所有権フラグ
	HDRVR m_hdrvr;		// オブジェクトハンドル
};
/*****************************************************************
	ドライバーハンドラクラス
*****************************************************************/
#pragma warning(push)
#pragma warning(disable: 4355)
class CDriverHandler : public CDrvr
{
public:
	DECLARE_SYNONYM(CDriverHandler,TYPE);
	DECLARE_SYNONYM(CDrvr,BASE);
	DECLARE_SYNONYM(TLinkTree<TYPE>,LINK);
	operator LPTYPE() { return this; }
	friend TYPE;
public:
	CDriverHandler() : m_link(this) { init(); }
	CDriverHandler(LPTYPE p) : m_link(this) { init(); SetOwner(p,-1); }
	virtual ~CDriverHandler() { destroy(); SetOwner(NULL,-1); }
private:
	void init() { m_bEnable=TRUE; }
public:
	void destroy() { disable(); init(); }
protected:
	// テンポラリマップの作成/削除
	void map(HDRVR hdrvr);
protected:
	// オーナーハンドル更新
	void handle(HDRVR hdrvr);
public:
	// 名前支配
	HDRVR handle() const { return BASE::handle(); }
public:
	// リンク操作
	LPTYPE GetRoot() const { LPLINK p=m_link.getRoot(); return p?p->object():NULL; }
	LPTYPE GetOwner() const { LPLINK p=m_link.getOwner(); return p?p->object():NULL; }
	LPTYPE SetOwner(LPTYPE lpObj, DWORD nIndex) { LPTYPE old=GetOwner(); m_link.setOwner(lpObj?&lpObj->m_link:NULL); return old; }
	DWORD GetChildCount() const { LPLINK p=m_link.getChild(); return p?p->count():0; }
	DWORD GetOrder() const { return m_link.getOrder(); }
	LPTYPE GetChild(DWORD nIndex) const { LPLINK p=m_link.getChild(nIndex); return p?p->object():NULL; }
	LPTYPE AddChild(LPTYPE lpObj, DWORD nIndex) { LPTYPE old=GetChild(nIndex); m_link.addChild(&lpObj->m_link); return old; }
private:
	LPTYPE prev() const { LPLINK p=m_link.prev(); return p?p->object():NULL; }
	LPTYPE next() const { LPLINK p=m_link.next(); return p?p->object():NULL; }
	LPTYPE top() const { LPLINK p=m_link.top(); return p?p->object():NULL; }
	LPTYPE end() const { LPLINK p=m_link.end(); return p?p->object():NULL; }
protected:
	// チェインディスパッチャ
	LRESULT handleProc(UINT uMsg, LPARAM lParam1, LPARAM lParam2);
	LRESULT unhandledProc(UINT uMsg, LPARAM lParam1, LPARAM lParam2);
	void handleEnableProc();
	void handleDisableProc();
protected:
	// enable/disable時のフック
	virtual void enableProc() {}
	virtual void disableProc() {}
protected:
	// ディスパッチャ、ハンドラ
	virtual LRESULT driverProc(UINT uMsg, LPARAM lParam1, LPARAM lParam2);	// ディスパッチャ(通常オーバーライドしない)
	virtual LRESULT dmDefault(UINT uMsg, LPARAM lParam1, LPARAM lParam2);	// デフォルト処理(通常オーバーライドしない)
	virtual LRESULT dmUnhandled(UINT uMsg, LPARAM lParam1, LPARAM lParam2);	// 未ハンドル処理フィルタ
protected:
#define HMC_CDRIVERHANDLER_ALIST	\
	METHOD(DRV_LOAD,dmLoad)	\
	METHOD(DRV_ENABLE,dmEnable)	\
	METHOD(DRV_OPEN,dmOpen)	\
	METHOD(DRV_CLOSE,dmClose)	\
	METHOD(DRV_DISABLE,dmDisable)	\
	METHOD(DRV_FREE,dmFree)	\
	METHOD(DRV_CONFIGURE,dmConfigure)	\
	METHOD(DRV_QUERYCONFIGURE,dmQueryConfigure)	\
	METHOD(DRV_INSTALL,dmInstall)	\
	METHOD(DRV_REMOVE,dmRemove)	\
	METHOD(DRV_EXITSESSION,dmExitSession)	\
	METHOD(DRV_POWER,dmPower)
#define METHOD(msg,handler) virtual LRESULT handler(LPARAM,LPARAM);
	HMC_CDRIVERHANDLER_ALIST
#undef METHOD
public:
	BOOL isEnable() const { return m_bEnable; }
	BOOL enable(BOOL f) { return f?enable():disable(); }
	BOOL enable();
	BOOL disable();
private:
	LINK m_link;		// オブジェクトチェイン
	BOOL m_bEnable;		// ハンドラの有効無効フラグ
};
#pragma warning(pop)
/*****************************************************************
	実用ドライバークラス
*****************************************************************/
class CDriver : public CDriverHandler
{
public:
	DECLARE_SYNONYM(CDriver,TYPE);
	DECLARE_SYNONYM(CDriverHandler,BASE);
	DECLARE_SYNONYM(CDriverHandler,HANDLER);
	DECLARE_SYNONYM(CDrvr,OBJ);
	operator LPTYPE() { return this; }
public:
	CDriver() { init(); }
	virtual ~CDriver() { destroy(); }
private:
	void init() {}
public:
	void destroy() { init(); BASE::destroy(); }
public:
	// オブジェクトの作成/削除
	// テンポラリマップの作成/削除
	// 各種操作
	// クラス情報の取得/設定
public:
	// メッセージディスパッチャ
	static LRESULT CALLBACK DriverProc(DWORD dwId, HDRVR hdrvr, UINT uMsg, LPARAM lParam1, LPARAM lParam2);
};
/*****************************************************************
*****************************************************************/
_HMC_END
/*****************************************************************
*****************************************************************/

#endif	/* __INC_HMC_DRIVER__ */

/* Local Variables:						*/
/* tab-width:4							*/
/* c-basic-offset:4						*/
/* c-indent-level:4						*/
/* c-label-offset:-4					*/
/* c-continued-statement-offset:4		*/
/* file-coding-system:*sjis*dos			*/
/* buffer-file-coding-system:sjis-dos	*/
/* End:									*/
