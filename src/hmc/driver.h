/*
	�h���C�o�[�T�|�[�g�N���X
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
	�h���C�o�[���N���X
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
	// �I�u�W�F�N�g�̍쐬/�폜
	HDRVR create(LPCWSTR lpDriverName, LPCWSTR lpSectionName, LONG lParam);
	LRESULT close(LPARAM lParam1, LPARAM lParam2);
	// �e���|�����}�b�v�̍쐬/�폜
	void map(HDRVR h) { destroy(); m_bOwner=FALSE; m_hdrvr=h; }
protected:
	void handle(HDRVR h) { m_hdrvr=h; }	// �I�[�i�[�n���h���X�V�p(����̍􂾂˂��c�c)
public:
	// �e�푀��
	LRESULT send(UINT uMsg, LONG lParam1, LONG lParam2) { return ::SendDriverMessage(handle(),uMsg,lParam1,lParam2); }
public:
	// ��I�u�W�F�N�g����
	static HMODULE GetModuleHandle(HDRVR hdrvr) { return ::GetDriverModuleHandle(hdrvr); }
	static HDRVR Open(LPCWSTR lpDriverName, LPCWSTR lpSectionName, LONG lParam) { return ::OpenDriver(lpDriverName,lpSectionName,lParam); }
	static LRESULT Close(HDRVR hdrvr, LONG lParam1, LONG lParam2) { return ::CloseDriver(hdrvr,lParam1,lParam2); }
	static LRESULT Send(HDRVR hdrvr, UINT uMsg, LONG lParam1, LONG lParam2) { return ::SendDriverMessage(hdrvr,uMsg,lParam1,lParam2); }
public:
	// �N���X���̎擾/�ݒ�
	HDRVR handle() const { return m_hdrvr; }
	operator HDRVR() const { return handle(); }
	BOOL isValid() const { return !!handle(); }
	BOOL isOwner() const { return m_bOwner; }
	BOOL ownership(BOOL f) { return m_bOwner=f; }
private:
	BOOL m_bOwner;		// ���L���t���O
	HDRVR m_hdrvr;		// �I�u�W�F�N�g�n���h��
};
/*****************************************************************
	�h���C�o�[�n���h���N���X
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
	// �e���|�����}�b�v�̍쐬/�폜
	void map(HDRVR hdrvr);
protected:
	// �I�[�i�[�n���h���X�V
	void handle(HDRVR hdrvr);
public:
	// ���O�x�z
	HDRVR handle() const { return BASE::handle(); }
public:
	// �����N����
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
	// �`�F�C���f�B�X�p�b�`��
	LRESULT handleProc(UINT uMsg, LPARAM lParam1, LPARAM lParam2);
	LRESULT unhandledProc(UINT uMsg, LPARAM lParam1, LPARAM lParam2);
	void handleEnableProc();
	void handleDisableProc();
protected:
	// enable/disable���̃t�b�N
	virtual void enableProc() {}
	virtual void disableProc() {}
protected:
	// �f�B�X�p�b�`���A�n���h��
	virtual LRESULT driverProc(UINT uMsg, LPARAM lParam1, LPARAM lParam2);	// �f�B�X�p�b�`��(�ʏ�I�[�o�[���C�h���Ȃ�)
	virtual LRESULT dmDefault(UINT uMsg, LPARAM lParam1, LPARAM lParam2);	// �f�t�H���g����(�ʏ�I�[�o�[���C�h���Ȃ�)
	virtual LRESULT dmUnhandled(UINT uMsg, LPARAM lParam1, LPARAM lParam2);	// ���n���h�������t�B���^
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
	LINK m_link;		// �I�u�W�F�N�g�`�F�C��
	BOOL m_bEnable;		// �n���h���̗L�������t���O
};
#pragma warning(pop)
/*****************************************************************
	���p�h���C�o�[�N���X
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
	// �I�u�W�F�N�g�̍쐬/�폜
	// �e���|�����}�b�v�̍쐬/�폜
	// �e�푀��
	// �N���X���̎擾/�ݒ�
public:
	// ���b�Z�[�W�f�B�X�p�b�`��
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
