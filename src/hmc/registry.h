/*
	レジストリサポートクラス
	Copyright (C)1998 H.Mutsuki
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __INC_HMC_REGISTRY__
#define __INC_HMC_REGISTRY__

#include "util.h"
#include "string.h"

#pragma comment(lib, "advapi32.lib")

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
*****************************************************************/
class CRegistry;
class CProfile;
class CEnumRegKey;
class CEnumRegValue;

/*****************************************************************
	レジストリ基底クラス
*****************************************************************/
class CRegistry
{
public:
	DECLARE_SYNONYM(CRegistry,TYPE);
	operator LPTYPE() { return this; }
public:
	CRegistry() { init(); }
	CRegistry(HKEY hKey, LPCTSTR lpSubKey, REGSAM samDesired=KEY_READ) { init(); create(hKey,lpSubKey,samDesired); }
	CRegistry(HKEY hKey) { init(); map(hKey); }
	~CRegistry() { destroy(); }
private:
	void init() { m_bOwner=TRUE; m_hKey=NULL; }
public:
	void destroy() { if(isOwner()) close(); init(); }
public:
	// オブジェクトの作成/削除
	LONG create(HKEY hKey, LPCTSTR lpSubKey, DWORD Reserved, LPTSTR lpClass, DWORD dwOptions, REGSAM samDesired, LPSECURITY_ATTRIBUTES lpsa, LPDWORD lpdwDisposition);
	LONG create(HKEY hKey, LPCTSTR lpSubKey, REGSAM samDesired=KEY_READ) { DWORD dw; return create(hKey,lpSubKey,0,NULL,REG_OPTION_NON_VOLATILE,samDesired,NULL,&dw); }
	LONG open(HKEY hKey, LPCTSTR lpSubKey, DWORD ulOption, REGSAM samDesired);
	LONG open(HKEY hKey, LPCTSTR lpSubKey, REGSAM samDesired=KEY_READ) { return open(hKey,lpSubKey,0,samDesired); }
	LONG close();
	// テンポラリマップの作成/削除
	void map(HKEY hKey) { destroy(); m_bOwner=FALSE; m_hKey=hKey; }
	// 各種操作
	LONG query(LPCTSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) const;
	LONG query(LPCTSTR lpValueName, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) const { return query(lpValueName,NULL,lpType,lpData,lpcbData); }
	LONG set(LPCTSTR lpValueName, DWORD Reserved, DWORD dwType, const LPBYTE lpData, DWORD cbData);
	LONG set(LPCTSTR lpValueName, DWORD dwType, const LPBYTE lpData, DWORD cbData) { return set(lpValueName,NULL,dwType,lpData,cbData); }
	// タイプとサイズの検査
	DWORD size(LPCTSTR lpValueName) const { DWORD size=0; query(lpValueName,NULL,NULL,&size); return size; };
	DWORD type(LPCTSTR lpValueName) const { DWORD type=REG_NONE; query(lpValueName,&type,NULL,NULL); return type; };
	BOOL isBinary(LPCTSTR lpValueName) const { return type(lpValueName)==REG_BINARY; }
	BOOL isString(LPCTSTR lpValueName) const { return type(lpValueName)==REG_SZ; }
	BOOL isValue (LPCTSTR lpValueName) const { return type(lpValueName)==REG_DWORD; }
	// 値の取得
	DWORD binary(LPCTSTR lpValueName, LPVOID pBuf, DWORD cbBuf) const { query(lpValueName,NULL,LPBYTE(pBuf),&cbBuf); return cbBuf; }
	DWORD string(LPCTSTR lpValueName, LPTSTR pBuf, DWORD cbBuf) const { query(lpValueName,NULL,LPBYTE(pBuf),&cbBuf); return cbBuf; }
	DWORD value (LPCTSTR lpValueName) const { DWORD dw=0,cb=sizeof(dw); query(lpValueName,NULL,LPBYTE(&dw),&cb); return dw; }
	// 値の設定
	LONG set(LPCTSTR lpValueName, LPCVOID pBuf, DWORD cbBuf) { return set(lpValueName,REG_BINARY,LPBYTE(pBuf),cbBuf); }
	LONG set(LPCTSTR lpValueName, LPCTSTR pBuf) { return set(lpValueName,REG_SZ,LPBYTE(pBuf),strlen(pBuf)+1); }
	LONG set(LPCTSTR lpValueName, DWORD dw) { return set(lpValueName,REG_DWORD,LPBYTE(&dw),sizeof(dw)); }
	// 非オブジェクト操作
	static LONG deleteKey  (HKEY hKey, LPCTSTR lpSubKey) { return ::RegDeleteKey  (hKey,lpSubKey); }
	static LONG deleteValue(HKEY hKey, LPCTSTR lpValueName) { return ::RegDeleteValue(hKey,lpValueName); }
	static LONG flushValue (HKEY hKey) { return ::RegFlushKey(hKey); }
public:
	// クラス情報の取得/設定
	HKEY handle() const { return m_hKey; }
	operator HKEY() const { return handle(); }
	BOOL isValid() const { return !!handle(); }
	BOOL isOwner() const { return m_bOwner; }
	BOOL ownership(BOOL f) { return m_bOwner=f; }
private:
	BOOL m_bOwner;	// 所有権フラグ
	HKEY m_hKey;	// オブジェクトハンドル
};
/*****************************************************************
	プロファイル(INIファイル)基底クラス
*****************************************************************/
class CProfile
{
public:
	DECLARE_SYNONYM(CProfile,TYPE);
	operator LPTYPE() { return this; }
public:
	CProfile() { init(); }
	CProfile(LPCTSTR lpFileName) { init(); create(lpFileName); }
	~CProfile() { destroy(); }
private:
	void init() {}
public:
	void destroy() { m_fname.setEmpty(); init(); }
public:
	// オブジェクトの作成/削除
	BOOL create(LPCTSTR lpFileName) { destroy(); m_fname=lpFileName; return !m_fname.isEmpty(); }
public:
	// 値の取得
	DWORD binary(LPCTSTR lpSectionName, LPCTSTR lpValueName, LPVOID pBuf, DWORD cbBuf) const { return ::GetPrivateProfileStruct(lpSectionName,lpValueName,pBuf,cbBuf,m_fname); }
	DWORD string(LPCTSTR lpSectionName, LPCTSTR lpValueName, LPTSTR pBuf, DWORD cbBuf, LPCTSTR lpDefault="") const { return ::GetPrivateProfileString(lpSectionName,lpValueName,lpDefault,LPTSTR(pBuf),cbBuf,m_fname); }
	DWORD value(LPCTSTR lpSectionName, LPCTSTR lpValueName) const { return ::GetPrivateProfileInt(lpSectionName,lpValueName,0,m_fname); }
	DWORD value(LPCTSTR lpSectionName, LPCTSTR lpValueName, DWORD dwDefault) const { return ::GetPrivateProfileInt(lpSectionName,lpValueName,dwDefault,m_fname); }
	// 値の設定
	BOOL set(LPCTSTR lpSectionName, LPCTSTR lpValueName, LPCVOID pBuf, DWORD cbBuf) { return ::WritePrivateProfileStruct(lpSectionName,lpValueName,LPTSTR(pBuf),cbBuf,m_fname); }
	BOOL set(LPCTSTR lpSectionName, LPCTSTR lpValueName, LPCTSTR pBuf) { return ::WritePrivateProfileString(lpSectionName,lpValueName,pBuf,m_fname); }
	BOOL set(LPCTSTR lpSectionName, LPCTSTR lpValueName, DWORD dw) { TString<32> s(0,"%u",dw); return set(lpSectionName,lpValueName,s); }
public:
	// クラス情報の取得/設定
	LPCTSTR name() const { return m_fname; };
	LPCTSTR fname() const { return name(); };
private:
	TString<MAX_PATH> m_fname;	// INIファイル名
};
/*****************************************************************
	サブキー列挙(RegEnumKeyExラッパー)
*****************************************************************/
class CEnumRegKey
{
public:
	DECLARE_SYNONYM(CEnumRegKey,TYPE);
	operator LPTYPE() { return this; }
public:
	CEnumRegKey() { init(); }
	CEnumRegKey(HKEY hKey) { init(); m_hKey=hKey; }
private:
	void init();
public:
	void destroy() { init(); }
public:
	// オブジェクトの作成/削除
	void create(HKEY hKey) { destroy(); m_hKey=hKey; }
public:
	// 各種操作
	UINT count() const;
	LPCTSTR first() { return nth(0); }
	LPCTSTR last() { return nth(count()-1);  }
	LPCTSTR next() { return nth(m_nCount+1); }
	LPCTSTR prev() { return nth(m_nCount-1); }
	// クラス情報の取得/設定
	LPCTSTR object() const { return m_cbName?m_szName:NULL; }
	operator LPCTSTR() const { return object(); }
	LPCTSTR keyName() const { return object(); }
	LPCTSTR className() const { return m_cbClass?m_szClass:NULL; }
	LPFILETIME time() const { return const_cast<LPFILETIME>(&m_ft); }
	BOOL isValid() const { return !!object(); }
public:
	// 序数nのサブキー情報を取得する
	LPCTSTR nth(UINT n);
private:
	HKEY m_hKey;			// 検索対象のキー
	TCHAR m_szName[256];	// 現在の検索結果(キー名)
	DWORD m_cbName;			// 現在の検索結果(キー名の文字数)
	TCHAR m_szClass[256];	// 現在の検索結果(クラス名)
	DWORD m_cbClass;		// 現在の検索結果(クラス名の文字数)
	FILETIME m_ft;			// 現在の検索結果(最終書き込み時間)
	UINT m_nCount;			// 列挙カウンタ(カーソル)
};

/*****************************************************************
	値列挙(RegEnumValueラッパー)
*****************************************************************/
class CEnumRegValue
{
public:
	DECLARE_SYNONYM(CEnumRegValue,TYPE);
	operator LPTYPE() { return this; }
public:
	CEnumRegValue() { init(); }
	CEnumRegValue(HKEY hKey) { init(); m_hKey=hKey; }
private:
	void init();
public:
	void destroy() { init(); }
public:
	// オブジェクトの作成/削除
	void create(HKEY hKey) { destroy(); m_hKey=hKey; }
public:
	// 各種操作
	UINT count() const;
	LPCTSTR first() { return nth(0); }
	LPCTSTR last() { return nth(count()-1);  }
	LPCTSTR next() { return nth(m_nCount+1); }
	LPCTSTR prev() { return nth(m_nCount-1); }
	// クラス情報の取得/設定
	LPCTSTR object() const { return m_cbName?m_szName:NULL; }
	operator LPCTSTR() const { return object(); }
	LPCTSTR valueName() const { return object(); }
	DWORD valueType() const { return m_dwType; }
	BOOL isValid() const { return !!object(); }
public:
	// 序数nのサブキー情報を取得する
	LPCTSTR nth(UINT n);
private:
	HKEY m_hKey;			// 検索対象のキー
	TCHAR m_szName[256];	// 現在の検索結果(キー名)
	DWORD m_cbName;			// 現在の検索結果(キー名の文字数)
	DWORD m_dwType;			// 現在の検索結果(値の種類)
	UINT m_nCount;			// 列挙カウンタ(カーソル)
};

/*****************************************************************
*****************************************************************/
_HMC_END
/*****************************************************************
*****************************************************************/

#endif	/* __INC_HMC_REGISTRY__ */

/* Local Variables:						*/
/* tab-width:4							*/
/* c-basic-offset:4						*/
/* c-indent-level:4						*/
/* c-label-offset:-4					*/
/* c-continued-statement-offset:4		*/
/* file-coding-system:*sjis*dos			*/
/* buffer-file-coding-system:sjis-dos	*/
/* End:									*/
