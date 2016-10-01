/*
	レジストリサポートクラス
	Copyright (C)1998 H.Mutsuki
*/

#include "util.h"
#include "registry.h"

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
*****************************************************************/
/*****************************************************************
	レジストリ基底クラス
*****************************************************************/
/*****************************************************************
	オブジェクトの生成/削除
*****************************************************************/
LONG CRegistry::create(HKEY hKey, LPCTSTR lpSubKey, DWORD Reserved, LPTSTR lpClass, DWORD dwOptions, REGSAM samDesired, LPSECURITY_ATTRIBUTES lpsa, LPDWORD lpdwDisposition)
{
	destroy();
	LONG lRes = ERROR_LASTERROR;
	lRes = ::RegCreateKeyEx(hKey,lpSubKey,Reserved,lpClass,dwOptions,samDesired,lpsa,&m_hKey,lpdwDisposition);
	CHECKRESULT_API(lRes==ERROR_SUCCESS);
	return lRes;
}
LONG CRegistry::open(HKEY hKey, LPCTSTR lpSubKey, DWORD ulOption, REGSAM samDesired)
{
	destroy();
	LONG lRes = ERROR_LASTERROR;
	lRes = ::RegOpenKeyEx(hKey,lpSubKey,ulOption,samDesired,&m_hKey);
	CHECKRESULT_API(lRes==ERROR_SUCCESS);
	return lRes;
}
LONG CRegistry::close()
{
	LONG lRes = ERROR_LASTERROR;
	if(handle()) {
		lRes = RegCloseKey(handle());
		CHECKRESULT_API(lRes==ERROR_SUCCESS);
		m_hKey = NULL;
	}
	return lRes;
}
/*****************************************************************
	各種操作
*****************************************************************/
LONG CRegistry::query(LPCTSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) const
{
	LONG lRes = ERROR_LASTERROR;
	lRes = ::RegQueryValueEx(handle(),lpValueName,lpReserved,lpType,lpData,lpcbData);
	CHECKRESULT_API(lRes==ERROR_SUCCESS);
	return lRes;
}
LONG CRegistry::set(LPCTSTR lpValueName, DWORD Reserved, DWORD dwType, const LPBYTE lpData, DWORD cbData)
{
	LONG lRes = ERROR_LASTERROR;
	lRes = ::RegSetValueEx(handle(),lpValueName,Reserved,dwType,lpData,cbData);
	CHECKRESULT_API(lRes==ERROR_SUCCESS);
	return lRes;
}

/*****************************************************************
*****************************************************************/
/*****************************************************************
	サブキー列挙(RegEnumKeyExラッパー)
*****************************************************************/
void CEnumRegKey::init()
{
	m_hKey = NULL;
	m_szName[0] = NULL;
	m_cbName = 0;
	m_szClass[0] = NULL;
	m_cbClass = 0;
	memset(&m_ft, 0, sizeof(m_ft));
	m_nCount = 0;
}
UINT CEnumRegKey::count() const
{
	TYPE t(m_hKey);
	UINT i = 0;
	for(;;i++) {
		if(!t.nth(i)) break;
	}
	return i;
}
LPCTSTR CEnumRegKey::nth(UINT n)
{
	m_szName[0] = NULL;
	m_cbName = sizeof(m_szName);
	m_szClass[0] = NULL;
	m_cbClass = sizeof(m_szClass);
	memset(&m_ft, 0, sizeof(m_ft));
	m_nCount = n;
	if(::RegEnumKeyEx(m_hKey, m_nCount, m_szName, &m_cbName, NULL,
					  m_szClass,&m_cbClass, &m_ft) != ERROR_SUCCESS) {
		m_cbName = 0;
		m_cbClass = 0;
	}
	return object();
}
/*****************************************************************
	値列挙(RegEnumValueラッパー)
*****************************************************************/
void CEnumRegValue::init()
{
	m_hKey = NULL;
	m_szName[0] = NULL;
	m_cbName = 0;
	m_dwType = REG_NONE;
	m_nCount = 0;
}
UINT CEnumRegValue::count() const
{
	TYPE t(m_hKey);
	UINT i = 0;
	for(;;i++) {
		if(!t.nth(i)) break;
	}
	return i;
}
LPCTSTR CEnumRegValue::nth(UINT n)
{
	m_szName[0] = NULL;
	m_cbName = sizeof(m_szName);
	m_dwType = REG_NONE;
	m_nCount  = n;
	if(::RegEnumValue(m_hKey, m_nCount, m_szName, &m_cbName, NULL,
					  &m_dwType, NULL, NULL) != ERROR_SUCCESS) {
		m_cbName = 0;
	}
	return object();
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
