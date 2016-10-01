/*
	メニューサポートクラス
	Copyright (C)1998-99 H.Mutsuki
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __INC_HMC_MENU__
#define __INC_HMC_MENU__

#include "util.h"

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
*****************************************************************/
class CMenu;
class CPopupMenu;

/*****************************************************************
	MENUITEMINFOラッパ
*****************************************************************/
class CMenuItemInfo : public MENUITEMINFO
{
public:
	DECLARE_SYNONYM(CMenuItemInfo,TYPE);
	DECLARE_SYNONYM(MENUITEMINFO,OBJ);
	operator LPTYPE() { return this; }
public:
	CMenuItemInfo() { init(); }
	CMenuItemInfo(LPTSTR p, UINT c) { init(); buffer(p,c); }
	~CMenuItemInfo() { /*destroy();*/ }
private:
	void init() { memset(this,0,sizeof(this)); cbSize=sizeof(*this); fMask=MIIM_CHECKMARKS|MIIM_DATA|MIIM_ID|MIIM_STATE|MIIM_SUBMENU|MIIM_TYPE; }
public:
	void destroy() { init(); }
public:
	// クラス情報の取得/設定
	UINT mask() const { return fMask; }
	UINT type() const { return fType; }
	UINT id() const { return wID; }
	HMENU subMenu() const { return hSubMenu; }
	LPTSTR data() const { return dwTypeData; }
	UINT size() const { return cch; }
	void buffer(LPTSTR p, UINT c) { dwTypeData=p; cch=c; }
};

/*****************************************************************
	メニュー基底クラス
*****************************************************************/
class CMenu
{
public:
	DECLARE_SYNONYM(CMenu,TYPE);
	operator LPTYPE() { return this; }
public:
	CMenu() { init(); }
	CMenu(HMENU hMenu) { init(); map(hMenu); }
	CMenu(HMENU hMenu, int nPos) { init(); map(hMenu,nPos); }
	CMenu(HINSTANCE hInstance, LPCTSTR lpMenuName) { init(); create(hInstance,lpMenuName); }
	CMenu(CONST MENUTEMPLATE *lpMenuTemplate) { init(); create(lpMenuTemplate); }
	~CMenu() { destroy(); }
private:
	void init() { m_bOwner=TRUE; m_hMenu=NULL; }
public:
	void destroy() { if(isOwner()&&handle()) CHECKRESULT_API(::DestroyMenu(handle())); init(); }
public:
	// オブジェクトの作成/削除
	HMENU create();
	HMENU create(HINSTANCE hInstance, LPCTSTR lpMenuName);
	HMENU create(CONST MENUTEMPLATE *lpMenuTemplate);
	// テンポラリマップの作成/削除
	void map(HMENU hMenu) { destroy(); m_bOwner=FALSE; m_hMenu=hMenu; }
	void map(HMENU hMenu, int nPos) { destroy(); m_bOwner=FALSE; m_hMenu=::GetSubMenu(hMenu,nPos); }
public:
	// 各種操作(アイテム関係)
	BOOL insert(UINT uItem, BOOL fByPos, LPMENUITEMINFO lpmii) { BOOL b=::InsertMenuItem(handle(),uItem,fByPos,lpmii); CHECKRESULT_API(b); return b; }
	BOOL setInfo(UINT uItem, BOOL fByPos, LPMENUITEMINFO lpmii) { BOOL b=::SetMenuItemInfo(handle(),uItem,fByPos,lpmii); CHECKRESULT_API(b); return b; }
	BOOL getInfo(UINT uItem, BOOL fByPos, LPMENUITEMINFO lpmii) const { BOOL b=::GetMenuItemInfo(handle(),uItem,fByPos,lpmii); CHECKRESULT_API(b); return b; }
	int count() const { int nRes=::GetMenuItemCount(handle()); CHECKRESULT_API(nRes!=-1); return nRes; }
	UINT id(int nPos) const { return ::GetMenuItemID(handle(),nPos); }
	BOOL enable(UINT uItem, UINT uEnable) { return ::EnableMenuItem(handle(),uItem,uEnable); }
	BOOL enable(UINT uItem, BOOL fByPos, BOOL fEnable) { return ::EnableMenuItem(handle(),uItem,(fByPos?MF_BYPOSITION:MF_BYCOMMAND)|(fEnable?MF_ENABLED:MF_GRAYED)); }
	BOOL setDefault(UINT uItem, BOOL fByPos) { BOOL b=::SetMenuDefaultItem(handle(),uItem,fByPos); CHECKRESULT_API(b); return b; }
	BOOL getDefault(UINT gmdiFlags, BOOL fByPos) const { BOOL b=::GetMenuDefaultItem(handle(),fByPos,gmdiFlags); CHECKRESULT_API(b); return b; }
	HMENU subMenu(int nPos) const { return ::GetSubMenu(handle(),nPos); }
	// 状態設定
	BOOL setCheck (UINT uItem, BOOL fByPos, BOOL fCheck) { MENUITEMINFO mi={sizeof(mi)}; mi.fMask=MIIM_STATE; getInfo(uItem,fByPos,&mi); mi.fState=(fCheck?(mi.fState|MF_CHECKED):(mi.fState&~MF_CHECKED)); return setInfo(uItem,fByPos,&mi); }
	BOOL setEnable(UINT uItem, BOOL fByPos, BOOL fCheck) { MENUITEMINFO mi={sizeof(mi)}; mi.fMask=MIIM_STATE; getInfo(uItem,fByPos,&mi); mi.fState=(fCheck?(mi.fState|MFS_ENABLED):(mi.fState|MFS_DISABLED)); return setInfo(uItem,fByPos,&mi); }
	// 状態取得
#if 1
	BOOL isChecked (UINT uItem, BOOL fByPos) const { UINT u=::GetMenuState(handle(),uItem,(fByPos?MF_BYPOSITION:MF_BYCOMMAND)); return !!(u&MF_CHECKED); }
	BOOL isEnabled (UINT uItem, BOOL fByPos) const { UINT u=::GetMenuState(handle(),uItem,(fByPos?MF_BYPOSITION:MF_BYCOMMAND)); return !!(u&(MF_DISABLED|MF_GRAYED)); }
	BOOL isGrayed  (UINT uItem, BOOL fByPos) const { UINT u=::GetMenuState(handle(),uItem,(fByPos?MF_BYPOSITION:MF_BYCOMMAND)); return !!(u&MF_GRAYED); }
	BOOL isHilite  (UINT uItem, BOOL fByPos) const { UINT u=::GetMenuState(handle(),uItem,(fByPos?MF_BYPOSITION:MF_BYCOMMAND)); return !!(u&MF_HILITE); }
	int string(UINT uItem, BOOL fByPos, LPTSTR lpBuf, DWORD cchBuf) const { return ::GetMenuString(handle(),uItem,lpBuf,cchBuf,(fByPos?MF_BYPOSITION:MF_BYCOMMAND)); }
#else
	BOOL isChecked (UINT uItem, BOOL fByPos) const { MENUITEMINFO mi={sizeof(mi)}; mi.fMask=MIIM_STATE; getInfo(uItem,fByPos,&mi); return !!(mi.fState&MFS_CHECKED ); }
	BOOL isDefault (UINT uItem, BOOL fByPos) const { MENUITEMINFO mi={sizeof(mi)}; mi.fMask=MIIM_STATE; getInfo(uItem,fByPos,&mi); return !!(mi.fState&MFS_DEFAULT ); }
	BOOL isDisabled(UINT uItem, BOOL fByPos) const { MENUITEMINFO mi={sizeof(mi)}; mi.fMask=MIIM_STATE; getInfo(uItem,fByPos,&mi); return !!(mi.fState&MFS_DISABLED); }
	BOOL isEnabled (UINT uItem, BOOL fByPos) const { MENUITEMINFO mi={sizeof(mi)}; mi.fMask=MIIM_STATE; getInfo(uItem,fByPos,&mi); return !!(mi.fState&MFS_ENABLED ); }
	BOOL isGrayed  (UINT uItem, BOOL fByPos) const { MENUITEMINFO mi={sizeof(mi)}; mi.fMask=MIIM_STATE; getInfo(uItem,fByPos,&mi); return !!(mi.fState&MFS_GRAYED  ); }
	BOOL isHilite  (UINT uItem, BOOL fByPos) const { MENUITEMINFO mi={sizeof(mi)}; mi.fMask=MIIM_STATE; getInfo(uItem,fByPos,&mi); return !!(mi.fState&MFS_HILITE  ); }
	BOOL isUnchecked(UINT uItem, BOOL fByPos) const { MENUITEMINFO mi={sizeof(mi)}; mi.fMask=MIIM_STATE; getInfo(uItem,fByPos,&mi); return !!(mi.fState&MFS_UNCHECKED); }
	BOOL isUnhilite(UINT uItem, BOOL fByPos) const { MENUITEMINFO mi={sizeof(mi)}; mi.fMask=MIIM_STATE; getInfo(uItem,fByPos,&mi); return !!(mi.fState&MFS_UNHILITE); }
#if(WINVER >= 0x0500)
	BOOL string(UINT uItem, BOOL fByPos, LPTSTR lpBuf, DWORD cchBuf) const { MENUITEMINFO mi={sizeof(mi)}; mi.fMask=MIIM_STRING; mi.dwTypeData=lpBuf; mi.cch=cchBuf; return getInfo(uItem,fByPos,&mi); }
#endif
#endif
public:
	// クラス情報の取得/設定
	HMENU handle() const { return m_hMenu; }
	operator HMENU() const { return handle(); }
	BOOL isValid() const { return ::IsMenu(handle()); }
	BOOL isOwner() const { return m_bOwner; }
	BOOL ownership(BOOL f) { return m_bOwner=f; }
protected:
	HMENU handle(HMENU hMenu) { return m_hMenu=hMenu; }
private:
	BOOL m_bOwner;		// 所有権フラグ
	HMENU m_hMenu;		// オブジェクトハンドル
};

/*****************************************************************
	ポップアップメニュー
*****************************************************************/
class CPopupMenu : public CMenu
{
public:
	DECLARE_SYNONYM(CPopupMenu,TYPE);
	DECLARE_SYNONYM(CMenu,BASE);
	operator LPTYPE() { return this; }
public:
	CPopupMenu() { init(); }
	CPopupMenu(HMENU hMenu) { init(); map(hMenu); }
	CPopupMenu(HMENU hMenu, int nPos) { init(); map(hMenu,nPos); }
	~CPopupMenu() { destroy(); }
private:
	void init() {}
public:
	void destroy() { BASE::destroy(); init(); }
public:
	// オブジェクトの作成/削除
	HMENU create();
};

/*****************************************************************
*****************************************************************/
_HMC_END
/*****************************************************************
*****************************************************************/

#endif	/* __INC_HMC_MENU__ */

/* Local Variables:						*/
/* tab-width:4							*/
/* c-basic-offset:4						*/
/* c-indent-level:4						*/
/* c-label-offset:-4					*/
/* c-continued-statement-offset:4		*/
/* file-coding-system:*sjis*dos			*/
/* buffer-file-coding-system:sjis-dos	*/
/* End:									*/
