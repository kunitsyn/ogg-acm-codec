/*
	���W���[���T�|�[�g�N���X
	Copyright (C)1998-99 H.Mutsuki
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __INC_HMC_MODULE__
#define __INC_HMC_MODULE__

#include "util.h"
#include "string.h"

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
*****************************************************************/
class CModule;
class CEnumResourceTypes;
class CEnumResourceNames;
class CEnumResourceLanguages;

/*****************************************************************
	���W���[���\�����N���X
*****************************************************************/
class CModule
{
public:
	DECLARE_SYNONYM(CModule,TYPE);
	operator LPTYPE() { return this; }
public:
	CModule() { init(); }
	CModule(LPCTSTR fname) { init(); create(fname); }
	CModule(LPCTSTR fname, DWORD dwFlags) { init(); create(fname,dwFlags); }
	CModule(HMODULE hmod) { init(); map(hmod); }
	~CModule() { destroy(); }
private:
	void init() { m_bOwner=TRUE; m_hModule=NULL; }
public:
	void destroy() { if(isOwner()) close(); init(); }
public:
	// �I�u�W�F�N�g�̍쐬/�폜
	HANDLE create(LPCTSTR fname);
	HANDLE create(LPCTSTR fname, DWORD dwFlags);
	BOOL close();
	// �e���|�����}�b�v�̍쐬/�폜
	void map(HMODULE hmod) { destroy(); m_bOwner=FALSE; m_hModule=hmod; }
	// �e�푀��
public:
	// �N���X���̎擾/�ݒ�
	HMODULE handle() const { return m_hModule; }
	operator HMODULE() const { return handle(); }
	FARPROC proc(LPCSTR p) const { return ::GetProcAddress(handle(),p); }
	LPCVOID res(LPCTSTR name, LPCTSTR type, WORD lang) const { return resource(name,type,lang); }
	LPCVOID res(LPCTSTR name, LPCTSTR type) const { return resource(name,type); }
	LPCVOID resource(LPCTSTR name, LPCTSTR type, WORD lang) const;
	LPCVOID resource(LPCTSTR name, LPCTSTR type) const;
	DWORD sizeofRes(LPCTSTR name, LPCTSTR type, WORD lang) const { return sizeofResource(name,type,lang); }
	DWORD sizeofRes(LPCTSTR name, LPCTSTR type) const { return sizeofResource(name,type); }
	DWORD sizeofResource(LPCTSTR name, LPCTSTR type, WORD lang) const;
	DWORD sizeofResource(LPCTSTR name, LPCTSTR type) const;
	BOOL isValid() const { return !!handle(); }
	BOOL isOwner() const { return m_bOwner; }
	BOOL ownership(BOOL f) { return m_bOwner=f; }
private:
	BOOL m_bOwner;			// ���L���t���O
	HMODULE m_hModule;		// �I�u�W�F�N�g�n���h��
};

/*****************************************************************
	EnumResourceTypes���b�p�[
*****************************************************************/
class CEnumResourceTypes
{
public:
	DECLARE_SYNONYM(CEnumResourceTypes,TYPE);
	operator LPTYPE() { return this; }
friend TYPE;
public:
	CEnumResourceTypes() { init(); }
	CEnumResourceTypes(HMODULE hModule) { init(); create(hModule); }
private:
	void init() { m_hModule=NULL; m_type=NULL; m_nIndex=0; m_nCount=0; }
public:
	void destroy() { init(); }
public:
	// �I�u�W�F�N�g�̍쐬/�폜
	void create(HMODULE hModule) { destroy(); m_hModule=hModule; }
public:
	// �e�푀��
	UINT count() const { TYPE t=*this; t.nth(UINT_MAX); return t.m_nCount; }
	LPTSTR first() { return nth(0); }
	LPTSTR last() { return nth(count()-1);  }
	LPTSTR next() { return nth(m_nCount+1); }
	LPTSTR prev() { return nth(m_nCount-1); }
	BOOL isContain(LPCTSTR lpType) const;
	// �N���X���̎擾/�ݒ�
	LPTSTR object() const { return m_type.object(); }
	operator LPTSTR() const { return object(); }
	BOOL isValid() const { return !!object(); }
public:
	// ����n�̃��\�[�X�^�C�v���擾����
	LPTSTR nth(UINT n);
private:
	// ���\�[�X�^�C�v�񋓃R�[���o�b�N
	static BOOL CALLBACK enumProc(HMODULE hModule, LPTSTR lpType, LPARAM lParam);
	BOOL enumProc(LPTSTR lpType);
private:
	HMODULE m_hModule;	// �����Ώۂ̃��W���[���C���X�^���X
	TINTRES m_type;		// ���݂̃��\�[�X�^�C�v(�R�s�[)
	UINT m_nIndex;		// �擾�ΏۃC���f�b�N�X
	UINT m_nCount;		// �񋓃J�E���^(�J�[�\��)
};
/*****************************************************************
	EnumResourceNames���b�p�[
*****************************************************************/
class CEnumResourceNames
{
public:
	DECLARE_SYNONYM(CEnumResourceNames,TYPE);
	operator LPTYPE() { return this; }
friend TYPE;
public:
	CEnumResourceNames() { init(); }
	CEnumResourceNames(HMODULE hModule, LPCTSTR lpType) { init(); create(hModule, lpType); }
private:
	void init() { m_hModule=NULL; m_type=NULL; m_name=NULL; m_nIndex=0; m_nCount=0; }
public:
	void destroy() { init(); }
public:
	// �I�u�W�F�N�g�̍쐬/�폜
	void create(HMODULE hModule, LPCTSTR lpType);
public:
	// �e�푀��
	UINT count() const { TYPE t=*this; t.nth(UINT_MAX); return t.m_nCount; }
	LPTSTR first() { return nth(0); }
	LPTSTR last() { return nth(count()-1);  }
	LPTSTR next() { return nth(m_nCount+1); }
	LPTSTR prev() { return nth(m_nCount-1); }
	// �N���X���̎擾/�ݒ�
	LPTSTR object() const { return m_name.object(); }
	operator LPTSTR() const { return object(); }
	BOOL isValid() const { return !!object(); }
	BOOL isContain(LPCTSTR lpName) const;
public:
	// ����n�̃��\�[�X�^�C�v���擾����
	LPTSTR nth(UINT n);
private:
	// ���\�[�X�^�C�v�񋓃R�[���o�b�N
	static BOOL CALLBACK enumProc(HMODULE hModule, LPCTSTR lpType, LPTSTR lpName, LPARAM lParam);
	BOOL enumProc(LPTSTR lpName);
private:
	HMODULE m_hModule;	// �����Ώۂ̃��W���[���C���X�^���X
	TINTRES m_type;		// �����Ώۂ̃��\�[�X�^�C�v(�R�s�[)
	TINTRES m_name;		// ���݂̃��\�[�X��(�R�s�[)
	UINT m_nIndex;		// �擾�ΏۃC���f�b�N�X
	UINT m_nCount;		// �񋓃J�E���^(�J�[�\��)
};
/*****************************************************************
	EnumResourceLanguages���b�p�[
*****************************************************************/
class CEnumResourceLanguages
{
public:
	DECLARE_SYNONYM(CEnumResourceLanguages,TYPE);
	operator LPTYPE() { return this; }
friend TYPE;
public:
	CEnumResourceLanguages() { init(); }
	CEnumResourceLanguages(HMODULE hModule, LPCTSTR lpType, LPCTSTR lpName) { init(); create(hModule, lpType, lpName); }
private:
	void init() { m_hModule=NULL; m_type=NULL; m_name=NULL; m_wIdLang=0; m_nIndex=0; m_nCount=0; }
public:
	void destroy() { init(); }
public:
	// �I�u�W�F�N�g�̍쐬/�폜
	void create(HMODULE hModule, LPCTSTR lpType, LPCTSTR lpName);
public:
	// �e�푀��
	UINT count() const { TYPE t=*this; t.nth(UINT_MAX); return t.m_nCount; }
	WORD first() { return nth(0); }
	WORD last() { return nth(count()-1);  }
	WORD next() { return nth(m_nCount+1); }
	WORD prev() { return nth(m_nCount-1); }
	BOOL isContain(WORD wId) const;
	// �N���X���̎擾/�ݒ�
	WORD id() const { return m_wIdLang; }
	operator WORD() const { return id(); }
	BOOL isValid() const { return !!id(); }
public:
	// ����n�̌���ID���擾����
	WORD nth(UINT n);
private:
	// ���\�[�X�^�C�v�񋓃R�[���o�b�N
	static BOOL CALLBACK enumProc(HMODULE hModule, LPCTSTR lpType, LPCTSTR lpName, WORD wIdLang, LPARAM lParam);
	BOOL enumProc(WORD wIdLang);
private:
	HMODULE m_hModule;	// �����Ώۂ̃��W���[���C���X�^���X
	TINTRES m_type;		// �����Ώۂ̃��\�[�X�^�C�v(�R�s�[)
	TINTRES m_name;		// �����Ώۂ̃��\�[�X��(�R�s�[)
	WORD m_wIdLang;		// ���݂̌���ID
	UINT m_nIndex;		// �擾�ΏۃC���f�b�N�X
	UINT m_nCount;		// �񋓃J�E���^(�J�[�\��)
};

/*****************************************************************
*****************************************************************/
_HMC_END
/*****************************************************************
*****************************************************************/

#endif	/* __INC_HMC_MODULE__ */

/* Local Variables:						*/
/* tab-width:4							*/
/* c-basic-offset:4						*/
/* c-indent-level:4						*/
/* c-label-offset:-4					*/
/* c-continued-statement-offset:4		*/
/* file-coding-system:*sjis*dos			*/
/* buffer-file-coding-system:sjis-dos	*/
/* End:									*/
