/*
	キーボードアクセラレータ
	Copyright (C)1998 H.Mutsuki
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __INC_HMC_ACCEL__
#define __INC_HMC_ACCEL__

#include "util.h"

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
*****************************************************************/
class CAccel;
/*****************************************************************
	キーボードアクセラレータ
*****************************************************************/
class CAccel
{
public:
	DECLARE_SYNONYM(CAccel,TYPE);
	operator LPTYPE() { return this; }
public:
	CAccel() { init(); }
	CAccel(LPACCEL lpaccl, int cEntries) { init(); create(lpaccl,cEntries); }
	CAccel(HINSTANCE hInstance, LPCTSTR lpTableName) { init(); create(hInstance,lpTableName); }
	CAccel(BYTE fVirt, WORD key, WORD cmd) { init(); create(fVirt,key,cmd); }
	CAccel(size_t c, BYTE fVirt, WORD key, WORD cmd, ...) { init(); va_list args; va_start(args,c); create(c,args); }
	~CAccel() { destroy(); }
private:
	void init() { m_bOwner=TRUE; m_hAccel=NULL; }
public:
	void destroy();
public:
	// オブジェクトの作成/削除
	HACCEL create(LPACCEL lpaccl, int cEntries);
	HACCEL create(HINSTANCE hInstance, LPCTSTR lpTableName);
	HACCEL create(BYTE fVirt, WORD key, WORD cmd);
	HACCEL create(size_t c, BYTE fVirt, WORD key, WORD cmd, ...);
	HACCEL create(size_t c, va_list args);
	// テンポラリマップの作成/削除
	void map(HACCEL hAccel) { destroy(); m_bOwner=FALSE; m_hAccel=hAccel; }
public:
	// 各種操作
	int translate(HWND hwnd, LPMSG lpMsg) const;
public:
	// クラス情報の取得/設定
	HACCEL handle() const { return m_hAccel; }
	operator HACCEL() const { return handle(); }
	BOOL isValid() const { return !!handle(); }
	BOOL isOwner() const { return m_bOwner; }
	BOOL ownership(BOOL f) { return m_bOwner=f; }
private:
	BOOL m_bOwner;		// 所有権フラグ
	HACCEL m_hAccel;	// オブジェクトハンドル
};

/*****************************************************************
*****************************************************************/
_HMC_END
/*****************************************************************
*****************************************************************/

#endif	/* __INC_HMC_ACCEL__ */

/* Local Variables:						*/
/* tab-width:4							*/
/* c-basic-offset:4						*/
/* c-indent-level:4						*/
/* c-label-offset:-4					*/
/* c-continued-statement-offset:4		*/
/* file-coding-system:*sjis*dos			*/
/* buffer-file-coding-system:sjis-dos	*/
/* End:									*/
