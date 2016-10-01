/*
	モジュールサポートクラス
	Copyright (C)1998-99 H.Mutsuki
*/

#include "util.h"
#include "module.h"

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
*****************************************************************/
/*****************************************************************
	モジュール表現基底クラス
*****************************************************************/
HANDLE CModule::create(LPCTSTR fname)
{
	destroy();
	m_hModule = ::LoadLibrary(fname);
	CHECKRESULT_API(handle());
	return handle();
}
HANDLE CModule::create(LPCTSTR fname, DWORD dwFlags)
{
	destroy();
	m_hModule = ::LoadLibraryEx(fname, NULL, dwFlags);
	CHECKRESULT_API(handle());
	return handle();
}
BOOL CModule::close()
{
	BOOL bRes = FALSE;
	if(handle()) {
		bRes = ::FreeLibrary(handle());
		CHECKRESULT_API(bRes);
		m_hModule = NULL;
	}
	return bRes;
}
LPCVOID CModule::resource(LPCTSTR name, LPCTSTR type, WORD lang) const
{
	LPCVOID pv = NULL;
	do {
		HRSRC   hrsr = NULL;
		HGLOBAL hglb = NULL;
		if(!(hrsr=::FindResourceEx(handle(),name,type,lang))) break;
		if(!(hglb=::LoadResource(handle(),hrsr))) break;
		pv = ::LockResource(hglb);
	} while(0);
	return pv;
}
LPCVOID CModule::resource(LPCTSTR name, LPCTSTR type) const
{
	LPCVOID pv = NULL;
	do {
		HRSRC   hrsr = NULL;
		HGLOBAL hglb = NULL;
		if(!(hrsr=::FindResource(handle(),name,type))) break;
		if(!(hglb=::LoadResource(handle(),hrsr))) break;
		pv = ::LockResource(hglb);
	} while(0);
	return pv;
}
DWORD CModule::sizeofResource(LPCTSTR name, LPCTSTR type, WORD lang) const
{
	DWORD dw = 0;
	HRSRC hrsr = ::FindResourceEx(handle(),name,type,lang);
	CHECKRESULT_API(hrsr);
	if(hrsr) {
		dw = ::SizeofResource(handle(),hrsr);
		CHECKRESULT_API(dw);
	}
	return dw;
}
DWORD CModule::sizeofResource(LPCTSTR name, LPCTSTR type) const
{
	DWORD dw = 0;
	HRSRC hrsr = ::FindResource(handle(),name,type);
	CHECKRESULT_API(hrsr);
	if(hrsr) {
		dw = ::SizeofResource(handle(),hrsr);
		CHECKRESULT_API(dw);
	}
	return dw;
}
/*****************************************************************
	EnumResourceTypesラッパー
*****************************************************************/
BOOL CEnumResourceTypes::isContain(LPCTSTR lpType) const
{
	BOOL bRes = FALSE;
	TYPE t = *this;
	for(UINT i=0; i<t.count(); i++) {
		LPCTSTR p = t.nth(i);
		if(t.m_type.isEqual(p)) {
			bRes = TRUE;
			break;
		}
	}
	return bRes;
}
LPTSTR CEnumResourceTypes::nth(UINT n)
{
	m_type = NULL;
	m_nCount = 0;
	m_nIndex = n;
	CHECKRESULT_API(::EnumResourceTypes(m_hModule,enumProc,LPARAM(this)));
	return object();
}
BOOL CALLBACK CEnumResourceTypes::enumProc(HMODULE hModule, LPTSTR lpType, LPARAM lParam)
{
	return (LPTYPE(lParam))->enumProc(lpType);
}
BOOL CEnumResourceTypes::enumProc(LPTSTR lpType)
{
	if(m_nIndex==m_nCount) {
		m_type = lpType;
		return FALSE;
	} else {
		m_nCount++;
		return TRUE;
	}
}

/*****************************************************************
	EnumResourceNamesラッパー
*****************************************************************/
void CEnumResourceNames::create(HMODULE hModule, LPCTSTR lpType)
{
	destroy();
	m_hModule = hModule;
	m_type = lpType;
}
BOOL CEnumResourceNames::isContain(LPCTSTR lpName) const
{
	BOOL bRes = FALSE;
	TYPE t = *this;
	for(UINT i=0; i<t.count(); i++) {
		LPCTSTR p = t.nth(i);
		if(t.m_name.isEqual(p)) {
			bRes = TRUE;
			break;
		}
	}
	return bRes;
}
LPTSTR CEnumResourceNames::nth(UINT n)
{
	m_name = NULL;
	m_nCount = 0;
	m_nIndex = n;
	CHECKRESULT_API(::EnumResourceNames(m_hModule,m_type,enumProc,LPARAM(this)));
	return object();
}
BOOL CALLBACK CEnumResourceNames::enumProc(HMODULE hModule, LPCTSTR lpType, LPTSTR lpName, LPARAM lParam)
{
	return (LPTYPE(lParam))->enumProc(lpName);
}
BOOL CEnumResourceNames::enumProc(LPTSTR lpName)
{
	if(m_nIndex==m_nCount) {
		m_name = lpName;
		return FALSE;
	} else {
		m_nCount++;
		return TRUE;
	}
}

/*****************************************************************
	EnumResourceLanguagesラッパー
*****************************************************************/
void CEnumResourceLanguages::create(HMODULE hModule, LPCTSTR lpType, LPCTSTR lpName)
{
	destroy();
	m_hModule = hModule;
	m_type = lpType;
	m_name = lpName;
}
BOOL CEnumResourceLanguages::isContain(WORD wId) const
{
	BOOL bRes = FALSE;
	TYPE t = *this;
	for(UINT i=0; i<t.count(); i++) {
		if(t.nth(i)==wId) {
			bRes=TRUE;
			break;
		}
	}
	return bRes;
}
WORD CEnumResourceLanguages::nth(UINT n)
{
	m_wIdLang = NULL;
	m_nCount = 0;
	m_nIndex = n;
	CHECKRESULT_API(::EnumResourceLanguages(m_hModule,m_type,m_name,enumProc,LPARAM(this)));
	return id();
}
BOOL CALLBACK CEnumResourceLanguages::enumProc(HMODULE hModule, LPCTSTR lpType, LPCTSTR lpName, WORD wIdLang, LPARAM lParam)
{
	return (LPTYPE(lParam))->enumProc(wIdLang);
}
BOOL CEnumResourceLanguages::enumProc(WORD wIdLang)
{
	if(m_nIndex == m_nCount) {
		m_wIdLang = wIdLang;
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
