/*
	COM サポートクラス
	Copyright (C)1999 H.Mutsuki
*/

#include "util.h"
#include <initguid.h>
#include <typeinfo.h>
#include "com.h"

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
*****************************************************************/
/*****************************************************************
	IUnknown(unknwn.h)
*****************************************************************/
/*****************************************************************
	オブジェクトの作成・削除
*****************************************************************/
void CDelegateUnknown::init()
{
	m_bOwner	= TRUE;
	m_cRef		= 0;
	m_pObject	= NULL;
}
void CDelegateUnknown::destroy()
{
	if(isValid()&&isOwner()) {
		Release();	// ※返り値を検査してはいけないことに注意
	}
	init();
}
HRESULT CDelegateUnknown::create(LPUNKNOWN pUnk, REFIID riid)
{
	destroy();
	HRESULT h = E_POINTER;
	if(pUnk) {
		h = pUnk->QueryInterface(riid,(void**)&m_pObject);
	} else {
		h = E_POINTER;
	}
	CHECKRESULT_COM(h);
	return h;
}
HRESULT CDelegateUnknown::create(REFCLSID rclsid, REFIID riid, DWORD dwClsContext)
{
	destroy();
	HRESULT h = ::CoCreateInstance(rclsid,NULL,dwClsContext,riid,(void**)&m_pObject);
	CHECKRESULT_COM(h);
	return h;
}
/*****************************************************************
	Iunknown実装部
*****************************************************************/
HRESULT CDelegateUnknown::QueryInterface(REFIID riid, void **ppvObject)
{
	HRESULT h = E_NOINTERFACE;
	LPOBJ p = object();
	if(p && p!=iptr()) {
		h = p->QueryInterface(riid,ppvObject);
	} else {
		if(riid==iid()) {
			*ppvObject = iptr();
			AddRef();
			h = S_OK;
		} else {
			h = E_NOINTERFACE;
		}
	}
	CHECKRESULT_COM(h);
	return h;
}
ULONG CDelegateUnknown::AddRef()
{
	ULONG ul = 0;
	LPOBJ p = object();
	if(p && p!=iptr()) {
		ul = p->AddRef();
	} else {
		ul = ::InterlockedIncrement((LPLONG)&m_cRef);
	}
	return ul;
}
ULONG CDelegateUnknown::Release()
{
	ULONG ul = 0;
	LPOBJ p = object();
	if(p && p!=iptr()) {
		ul = p->Release();
	} else {
		ul = ::InterlockedDecrement((LPLONG)&m_cRef);
	}
	return ul;
}
/*****************************************************************
	リザルトコード検証
*****************************************************************/
HRESULT CDelegateUnknown::CheckResult(HRESULT hResult, LPCTSTR szFileName, UINT nLineNumber)
{
	if(FAILED(hResult)) {
		PrintResult(hResult,szFileName,nLineNumber);
	}
	return hResult;
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
