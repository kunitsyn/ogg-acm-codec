/*
	�_�C�A���O�T�|�[�g�N���X
	Copyright (C)1998-2000 H.Mutsuki
*/

#include "util.h"
#include <cderr.h>	// CommonDialog�n�̃G���[�R�[�h
#include "dialog.h"

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
*****************************************************************/
/*****************************************************************
	�O���[�o��
*****************************************************************/
// hwnd�Ŏ��ʂ����E�B���h�E���_�C�A���O���ǂ�������
//���P��DLGPROC���L���Ȃ�_�C�A���O�Ƃ݂Ȃ��c�c�����̂��R���H
BOOL IsDialog(HWND hwnd)
{
	BOOL bRes = FALSE;
	DLGPROC pDlgProc = (DLGPROC)::GetWindowLong(hwnd,DWL_DLGPROC);
	if(pDlgProc) {
		bRes = TRUE;
	} else {
#ifdef _DEBUG
		DWORD dwErr = ::GetLastError();
		if(dwErr!=ERROR_INVALID_INDEX) {
			CHECKRESULT_API(pDlgProc);
		}
#endif
	}
	return bRes;
}

// hwnd�Ŏ��ʂ����_�C�A���O�����[�_�����ǂ�������
//���P�ɐe�A�I�[�i�[�E�B���h�E��������Ԃł���ꍇ���[�_���Ƃ݂Ȃ��c�c�����̂��R���H
BOOL IsModal(HWND hwnd)
{
	BOOL bRes = FALSE;
	do {
		HWND hParent = ::GetParent(hwnd);
		HWND hOwner  = ::GetWindow(hwnd,GW_OWNER);
		if(hParent!=hOwner) break;
		bRes = ::IsWindowEnabled(hParent);
	} while(0);
	return bRes;
}

/*****************************************************************
	�[�����[�h���X�_�C�A���O�̋���
*****************************************************************/
HWND CWindowHandlerPseudoModeless::attach(HWND hOwner)
{
	// CCBTHookCreateWnd���g���āA�_�C�A���O���쐬�����^�C�~���O��
	// �I�[�i�[�̌������ł���΁A���X�}�[�g�Ȃ̂����A���ۂɂ́A���ɐe�E�B���h�E�͖�����
	// ����Ă��邽��(MessageBoxIndirect()�Ō��؍ς�)�A�蓮��HWND��o�^����`�ɂȂ���
	// ���ʁA�t�b�N�̂悤�ȃX���b�h�̖����͂�܂Ȃ��̂ŁACREATETRUST�𗘗p����ꍇ�ł��ʓ|���Ȃ�
	if(::IsWindow(hOwner) && ::IsWindowEnabled(hOwner)) {
		m_hOwner = hOwner;
	} else {
		m_hOwner = NULL;
	}
	return m_hOwner;
}
LRESULT CWindowHandlerPseudoModeless::wmInitDialog(WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = BASE::wmInitDialog(wParam,lParam);
	if(owner()==m_hOwner && ::IsWindow(m_hOwner) && !::IsWindowEnabled(m_hOwner)) {
		::EnableWindow(m_hOwner,TRUE);
	}
	return lRes;
}

/*****************************************************************
	���p�_�C�A���O�{�b�N�X
*****************************************************************/
/*****************************************************************
	���[�h���X�_�C�A���O�̐���/�I��
*****************************************************************/
HWND CDialog::create(HWND hParent, HMODULE hModule, LPCTSTR lpTemplate)
{
	destroy();
	CHECKRESULT_API(::CreateDialogParam(hModule,lpTemplate,hParent,DialogProc,LPARAM(this)));
	return hwnd();
}
HWND CDialog::create(HWND hParent, HMODULE hModule, LPCDLGTEMPLATE lpTemplate)
{
	destroy();
	CHECKRESULT_API(::CreateDialogIndirectParam(hModule,lpTemplate,hParent,DialogProc,LPARAM(this)));
	return hwnd();
}
// ���[�h���X�E���[�_���_�C�A���O�̐���(���X���b�h�ˑ�)
HWND CDialog::create(HWND hTrust, HWND hParent, HMODULE hModule, LPCTSTR lpTemplate)
{
	destroy();
	CREATETRUSTPARAMS params;
	params.hEvent		= NULL;
	params.hParent		= hParent;
	params.hModule		= hModule;
	params.lpResource	= lpTemplate;
	params.lpTemplate	= NULL;
	::SendMessage(hTrust,WM_CREATETRUST,WPARAM(this),LPARAM(&params));
	return hwnd();
}
HWND CDialog::create(HWND hTrust, HWND hParent, HMODULE hModule, LPCDLGTEMPLATE lpTemplate)
{
	destroy();
	CREATETRUSTPARAMS params;
	params.hEvent		= NULL;
	params.hParent		= hParent;
	params.hModule		= hModule;
	params.lpResource	= NULL;
	params.lpTemplate	= lpTemplate;
	::SendMessage(hTrust,WM_CREATETRUST,WPARAM(this),LPARAM(&params));
	return hwnd();
}
HWND CDialog::create(DWORD dwTrust, HWND hParent, HMODULE hModule, LPCTSTR lpTemplate)
{
	destroy();
	CEvent event(TRUE,FALSE);
	CREATETRUSTPARAMS params;
	params.hEvent	= event;
	params.hParent		= hParent;
	params.hModule		= hModule;
	params.lpResource	= lpTemplate;
	params.lpTemplate	= NULL;
	if(::PostThreadMessage(dwTrust,WM_CREATETRUST,WPARAM(this),LPARAM(&params))) {
		event.wait(INFINITE);
	} else {
		CHECKRESULT_API(0);
	}
	return hwnd();
}
HWND CDialog::create(DWORD dwTrust, HWND hParent, HMODULE hModule, LPCDLGTEMPLATE lpTemplate)
{
	destroy();
	CEvent event(TRUE,FALSE);
	CREATETRUSTPARAMS params;
	params.hEvent	= event;
	params.hParent		= hParent;
	params.hModule		= hModule;
	params.lpResource	= NULL;
	params.lpTemplate	= lpTemplate;
	if(::PostThreadMessage(dwTrust,WM_CREATETRUST,WPARAM(this),LPARAM(&params))) {
		event.wait(INFINITE);
	} else {
		CHECKRESULT_API(0);
	}
	return hwnd();
}
/*****************************************************************
	���[�_���_�C�A���O�̐���/�I��
*****************************************************************/
int CDialog::run(HWND hParent, HMODULE hModule, LPCTSTR lpTemplate)
{
	destroy();
	int n = ::DialogBoxParam(hModule,lpTemplate,hParent,DialogProc,LPARAM(this));
	CHECKRESULT_API(n!=-1);
	return n;
}
int CDialog::run(HWND hParent, HMODULE hModule, LPCDLGTEMPLATE lpTemplate)
{
	destroy();
	int n = ::DialogBoxIndirectParam(hModule,lpTemplate,hParent,DialogProc,LPARAM(this));
	CHECKRESULT_API(n!=-1);
	return n;
}
BOOL CDialog::end(int nResult)
{
	return ::EndDialog(hwnd(),nResult);
}

/*****************************************************************
	�r���g�C���f�B�X�p�b�`��
*****************************************************************/
BOOL CALLBACK CDialog::DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPTYPE pObj = LPTYPE(GetObject(hwnd));
	if(!pObj) {
		if(uMsg==WM_INITDIALOG) {
			pObj = LPTYPE(lParam);
			SetObject(hwnd,pObj);
			pObj->hwnd(hwnd);		// �I�[�i�[�n���h���X�V
			return pObj->handleProc(uMsg,wParam,lParam);
		} else {
			return FALSE;
		}
	} else {
		if(uMsg==WM_CLOSE||uMsg==WM_DESTROY||(uMsg==WM_SYSCOMMAND&&wParam==SC_CLOSE)) {
			RemoveObject(hwnd);		// �z�h�~�̂��ߍŏ��Ɋ֘A�t������������
			if(!pObj->isOwned()) {
				pObj->show(SW_HIDE);	// �������J047671
			}
			pObj->handleDisableProc();	// �R�R��disableProc()���ĂԍŌ�̃`�����X
			LRESULT lRes = pObj->handleProc(uMsg,wParam,lParam);
			if(pObj->handleAfterClose()==FALSE) {
				::PostQuitMessage(0);
			}
			pObj->hwnd(NULL);		// �I�[�i�[�n���h���X�V(�`��)
			return lRes;
		} else {
			return pObj->handleProc(uMsg,wParam,lParam);
		}
	}
}

/*****************************************************************
	�f�B�X�p�b�`���A�n���h��
*****************************************************************/
// WM_INITDIALOG�̃f�t�H���g����
LRESULT CDialog::wmInitDialog(WPARAM wParam, LPARAM lParam)
{
	BOOL bRes = BASE::wmInitDialog(wParam,lParam);
	HWND hParent = parent();
	if(!::IsWindow(hParent)) {
		hParent = ::GetDesktopWindow();
	}
	CenterWindow(hwnd(),hParent);
	return bRes;
}
LRESULT CDialog::wmCommand(WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = BASE::wmCommand(wParam,lParam);
	WORD notif = HIWORD(wParam);
	UINT id    = LOWORD(wParam);
	HWND hwnd  = HWND(lParam);
	switch(notif) {
	case BN_CLICKED:
		switch(id) {
		case IDABORT:
		case IDCANCEL:
		case IDIGNORE:
		case IDNO:
		case IDOK:
		case IDRETRY:
		case IDYES:
			end(id);
			lRes = 0L;
		}
		break;
	}
	return lRes;
}
LRESULT CDialog::wmSysCommand(WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = BASE::wmSysCommand(wParam,lParam);
	UINT uCmdType = wParam;
	SHORT xPos = LOWORD(lParam);
	SHORT yPos = HIWORD(lParam);
	switch(uCmdType) {
	case SC_CLOSE:
		end(0);
		lRes = 0L;
	}
	return lRes;
}
LRESULT CDialog::wmClose(WPARAM wParam, LPARAM lParam)
{
	end(0);
	return BASE::wmClose(wParam,lParam);
}
// �X���b�h���ׂ��E�B���h�E�쐬����
LRESULT CDialog::wmCreateTrust(WPARAM wParam, LPARAM lParam)
{
	LPTYPE pObj = LPTYPE(wParam);
	LPCREATETRUSTPARAMS pParams = LPCREATETRUSTPARAMS(lParam);
	if(pParams->lpResource) {
		pObj->create(pParams->hParent,pParams->hModule,pParams->lpResource);
	} else if(pParams->lpTemplate) {
		pObj->create(pParams->hParent,pParams->hModule,pParams->lpTemplate);
	}
	if(pParams->hEvent) {
		::SetEvent(pParams->hEvent);
	}
	return LPARAM(pObj->hwnd());
}
// �f�t�H���g�n���h��
LRESULT CDialog::wmDefault(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg==WM_INITDIALOG) {
		return 1L;	// �f�t�H���g�A�C�e���Ƀt�H�[�J�X��^����
	} else {
		return 0L;	// ��DefDlgProc()���Ăяo���Ă͂����Ȃ����Ƃɒ���
	}
}

/*****************************************************************
	���b�Z�[�W�{�b�N�X
*****************************************************************/
/*****************************************************************
	���[�_���_�C�A���O�̐���
*****************************************************************/
int CMessageBox::run(LPMSGBOXPARAMS lpMsgBoxParams)
{
	destroy();
	CCBTHookCreateWnd hook(this);
	hook.enable();
	return ::MessageBoxIndirect(lpMsgBoxParams);
}
int CMessageBox::run(HWND hParent, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, WORD wLanguageId)
{
	MSGBOXPARAMS mbp = { sizeof(mbp) };
	mbp.hwndOwner			= hParent;
	mbp.hInstance			= NULL;
	mbp.lpszText			= lpText;
	mbp.lpszCaption			= lpCaption;
	mbp.dwStyle				= uType;
	mbp.lpszIcon			= NULL;
	mbp.dwContextHelpId		= DWORD(this);
	mbp.lpfnMsgBoxCallback	= MsgBoxCallback;
	mbp.dwLanguageId		= wLanguageId;
	return run(&mbp);
}
/*****************************************************************
	���[�_���_�C�A���O�̐���(���X���b�h�ˑ�)
*****************************************************************/
int CMessageBox::run(HWND hTrust, LPMSGBOXPARAMS lpMsgBoxParams)
{
	destroy();
	CREATETRUSTPARAMS params;
	params.hEvent  = NULL;
	params.nResult = 0;
	params.lpMsgBoxParams = lpMsgBoxParams;
	::SendMessage(hTrust,WM_CREATETRUST,WPARAM(this),LPARAM(&params));
	return params.nResult;
}
int CMessageBox::run(HWND hTrust, HWND hParent, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, WORD wLanguageId)
{
	MSGBOXPARAMS mbp = { sizeof(mbp) };
	mbp.hwndOwner			= hParent;
	mbp.hInstance			= NULL;
	mbp.lpszText			= lpText;
	mbp.lpszCaption			= lpCaption;
	mbp.dwStyle				= uType;
	mbp.lpszIcon			= NULL;
	mbp.dwContextHelpId		= 0;
	mbp.lpfnMsgBoxCallback	= NULL;
	mbp.dwLanguageId		= wLanguageId;
	return run(hTrust,&mbp);
}
int CMessageBox::run(DWORD dwTrust, LPMSGBOXPARAMS lpMsgBoxParams)
{
	destroy();
	CEvent event(TRUE,FALSE);
	CREATETRUSTPARAMS params;
	params.hEvent  = event;
	params.nResult = 0;
	params.lpMsgBoxParams = lpMsgBoxParams;
	if(::PostThreadMessage(dwTrust,WM_CREATETRUST,WPARAM(this),LPARAM(&params))) {
		event.wait(INFINITE);
	} else {
		CHECKRESULT_API(0);
	}
	return params.nResult;
}
int CMessageBox::run(DWORD dwTrust, HWND hParent, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, WORD wLanguageId)
{
	MSGBOXPARAMS mbp = { sizeof(mbp) };
	mbp.hwndOwner			= hParent;
	mbp.hInstance			= NULL;
	mbp.lpszText			= lpText;
	mbp.lpszCaption			= lpCaption;
	mbp.dwStyle				= uType;
	mbp.lpszIcon			= NULL;
	mbp.dwContextHelpId		= 0;
	mbp.lpfnMsgBoxCallback	= NULL;
	mbp.dwLanguageId		= wLanguageId;
	return run(dwTrust,&mbp);
}
/*****************************************************************
	�f�B�X�p�b�`���A�n���h��
*****************************************************************/
VOID CALLBACK CMessageBox::MsgBoxCallback(LPHELPINFO lpHelpInfo)
{
	LPTYPE pObj = LPTYPE(lpHelpInfo->dwContextId);
	ASSERT(pObj);
	pObj->onHelp(lpHelpInfo);
}
// �X���b�h���ׂ��E�B���h�E�쐬����
LRESULT CMessageBox::wmCreateTrust(WPARAM wParam, LPARAM lParam)
{
	LPTYPE pObj = LPTYPE(wParam);
	LPCREATETRUSTPARAMS pParams = LPCREATETRUSTPARAMS(lParam);
	pParams->nResult = pObj->run(pParams->lpMsgBoxParams);
	if(pParams->hEvent) {
		::SetEvent(pParams->hEvent);
	}
	return pParams->nResult;
}

/*****************************************************************
	�[�����[�h���X���b�Z�[�W�{�b�N�X
*****************************************************************/
/*****************************************************************
	���[�_���_�C�A���O�̐���
*****************************************************************/
int CPseudoModelessMessageBox::run(LPMSGBOXPARAMS lpMsgBoxParams)
{
	destroy();
	MSGBOXPARAMS params = *lpMsgBoxParams;
	CWindowHandlerPseudoModeless hook(this,lpMsgBoxParams->hwndOwner);
	return BASE::run(&params);
}
int CPseudoModelessMessageBox::run(HWND hParent, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, WORD wLanguageId)
{
	MSGBOXPARAMS mbp = { sizeof(mbp) };
	mbp.hwndOwner			= hParent;
	mbp.hInstance			= NULL;
	mbp.lpszText			= lpText;
	mbp.lpszCaption			= lpCaption;
	mbp.dwStyle				= uType;
	mbp.lpszIcon			= NULL;
	mbp.dwContextHelpId		= 0;
	mbp.lpfnMsgBoxCallback	= NULL;
	mbp.dwLanguageId		= wLanguageId;
	return run(&mbp);
}
/*****************************************************************
	���[�_���_�C�A���O�̐���(���X���b�h�ˑ�)
*****************************************************************/
int CPseudoModelessMessageBox::run(HWND hTrust, LPMSGBOXPARAMS lpMsgBoxParams)
{
	destroy();
	CREATETRUSTPARAMS params;
	params.hEvent  = NULL;
	params.nResult = 0;
	params.lpMsgBoxParams = lpMsgBoxParams;
	::SendMessage(hTrust,WM_CREATETRUST,WPARAM(this),LPARAM(&params));
	return params.nResult;
}
int CPseudoModelessMessageBox::run(HWND hTrust, HWND hParent, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, WORD wLanguageId)
{
	MSGBOXPARAMS mbp = { sizeof(mbp) };
	mbp.hwndOwner			= hParent;
	mbp.hInstance			= NULL;
	mbp.lpszText			= lpText;
	mbp.lpszCaption			= lpCaption;
	mbp.dwStyle				= uType;
	mbp.lpszIcon			= NULL;
	mbp.dwContextHelpId		= 0;
	mbp.lpfnMsgBoxCallback	= NULL;
	mbp.dwLanguageId		= wLanguageId;
	return run(hTrust,&mbp);
}
int CPseudoModelessMessageBox::run(DWORD dwTrust, LPMSGBOXPARAMS lpMsgBoxParams)
{
	destroy();
	CEvent event(TRUE,FALSE);
	CREATETRUSTPARAMS params;
	params.hEvent  = event;
	params.nResult = 0;
	params.lpMsgBoxParams = lpMsgBoxParams;
	if(::PostThreadMessage(dwTrust,WM_CREATETRUST,WPARAM(this),LPARAM(&params))) {
		event.wait(INFINITE);
	} else {
		CHECKRESULT_API(0);
	}
	return params.nResult;
}
int CPseudoModelessMessageBox::run(DWORD dwTrust, HWND hParent, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, WORD wLanguageId)
{
	MSGBOXPARAMS mbp = { sizeof(mbp) };
	mbp.hwndOwner			= hParent;
	mbp.hInstance			= NULL;
	mbp.lpszText			= lpText;
	mbp.lpszCaption			= lpCaption;
	mbp.dwStyle				= uType;
	mbp.lpszIcon			= NULL;
	mbp.dwContextHelpId		= 0;
	mbp.lpfnMsgBoxCallback	= NULL;
	mbp.dwLanguageId		= wLanguageId;
	return run(dwTrust,&mbp);
}
/*****************************************************************
	�f�B�X�p�b�`���A�n���h��
*****************************************************************/
LRESULT CPseudoModelessMessageBox::wmCreateTrust(WPARAM wParam, LPARAM lParam)
{
	LPTYPE pObj = LPTYPE(wParam);
	LPCREATETRUSTPARAMS pParams = LPCREATETRUSTPARAMS(lParam);
	pParams->nResult = pObj->run(pParams->lpMsgBoxParams);
	if(pParams->hEvent) {
		::SetEvent(pParams->hEvent);
	}
	return pParams->nResult;
}

/*****************************************************************
	�v���p�e�B�V�[�g
*****************************************************************/
/*****************************************************************
	���[�h���X�_�C�A���O�̐���
*****************************************************************/
HWND CPropertySheet::create(LPCPROPSHEETHEADER lppsh)
{
	destroy();
	PROPSHEETHEADER psh = *lppsh;
	psh.dwFlags |= PSH_MODELESS;		// ���[�h���X
	//psh.dwFlags |= PSH_USECALLBACK;		// �R�[���o�b�N�g�p
	psh.pfnCallback	= PropSheetProc;	// �R�[���o�b�N
	CCBTHookCreateWnd hook(this);
	hook.enable();
	int n = ::PropertySheet(&psh);
	CHECKRESULT_API(n!=-1);
	return hwnd();
}
HWND CPropertySheet::create(HWND hParent, HMODULE hModule, LPCTSTR pszCaption, UINT nStartPage, UINT nPages, HPROPSHEETPAGE *phpsh)
{
	PROPSHEETHEADER psh = { sizeof(psh) };
	psh.dwFlags		= PSH_DEFAULT /*| PSH_PROPTITLE*/;
	psh.hwndParent	= hParent;
	psh.hInstance	= hModule;
	psh.hIcon		= NULL;
	psh.pszCaption	= pszCaption;
	psh.nPages		= nPages;
	psh.nStartPage	= nStartPage;
	psh.phpage		= phpsh;
	psh.pfnCallback	= NULL;
	return create(&psh);
}
HWND CPropertySheet::create(HWND hParent, HMODULE hModule, LPCTSTR pszCaption, UINT nStartPage, HPROPSHEETPAGE hpsh0, HPROPSHEETPAGE hpsh1, HPROPSHEETPAGE hpsh2, HPROPSHEETPAGE hpsh3, HPROPSHEETPAGE hpsh4, HPROPSHEETPAGE hpsh5, HPROPSHEETPAGE hpsh6, HPROPSHEETPAGE hpsh7, HPROPSHEETPAGE hpsh8, HPROPSHEETPAGE hpsh9, HPROPSHEETPAGE hpsh10, HPROPSHEETPAGE hpsh11, HPROPSHEETPAGE hpsh12, HPROPSHEETPAGE hpsh13, HPROPSHEETPAGE hpsh14, HPROPSHEETPAGE hpsh15)
{
	HPROPSHEETPAGE p[16] = {hpsh0,hpsh1,hpsh2,hpsh3,hpsh4,hpsh5,hpsh6,hpsh7,hpsh8,hpsh9,hpsh10,hpsh11,hpsh12,hpsh13,hpsh14,hpsh15};
	HPROPSHEETPAGE a[16] = {0};
	int n = 0;
	for(; n<ARRAYLEN(p)&&p[n]; n++) {
		a[n] = p[n];
	}
	return create(hParent,hModule,pszCaption,nStartPage,n,a);
}
/*****************************************************************
	���[�h���X�_�C�A���O�̐���(���X���b�h�ˑ�)
*****************************************************************/
HWND CPropertySheet::create(HWND hTrust, LPCPROPSHEETHEADER lppsh)
{
	destroy();
	PROPSHEETHEADER psh = *lppsh;
	psh.dwFlags |= PSH_MODELESS;		// ���[�h���X
	//psh.dwFlags |= PSH_USECALLBACK;		// �R�[���o�b�N�g�p
	psh.pfnCallback	= PropSheetProc;	// �R�[���o�b�N
	CREATETRUSTPARAMS params;
	params.hEvent	= NULL;
	params.nResult	= -1;
	params.lppsh	= &psh;
	::SendMessage(hTrust,WM_CREATETRUST,WPARAM(this),LPARAM(&params));
	return (HWND)params.nResult;
}
HWND CPropertySheet::create(HWND hTrust, HWND hParent, HMODULE hModule, LPCTSTR pszCaption, UINT nStartPage, UINT nPages, HPROPSHEETPAGE *phpsh)
{
	PROPSHEETHEADER psh = { sizeof(psh) };
	psh.dwFlags		= PSH_DEFAULT /*| PSH_PROPTITLE*/;
	psh.hwndParent	= hParent;
	psh.hInstance	= hModule;
	psh.hIcon		= NULL;
	psh.pszCaption	= pszCaption;
	psh.nPages		= nPages;
	psh.nStartPage	= nStartPage;
	psh.phpage		= phpsh;
	psh.pfnCallback	= NULL;
	return create(hTrust,&psh);
}
HWND CPropertySheet::create(HWND hTrust, HWND hParent, HMODULE hModule, LPCTSTR pszCaption, UINT nStartPage, HPROPSHEETPAGE hpsh0, HPROPSHEETPAGE hpsh1, HPROPSHEETPAGE hpsh2, HPROPSHEETPAGE hpsh3, HPROPSHEETPAGE hpsh4, HPROPSHEETPAGE hpsh5, HPROPSHEETPAGE hpsh6, HPROPSHEETPAGE hpsh7, HPROPSHEETPAGE hpsh8, HPROPSHEETPAGE hpsh9, HPROPSHEETPAGE hpsh10, HPROPSHEETPAGE hpsh11, HPROPSHEETPAGE hpsh12, HPROPSHEETPAGE hpsh13, HPROPSHEETPAGE hpsh14, HPROPSHEETPAGE hpsh15)
{
	HPROPSHEETPAGE p[16] = {hpsh0,hpsh1,hpsh2,hpsh3,hpsh4,hpsh5,hpsh6,hpsh7,hpsh8,hpsh9,hpsh10,hpsh11,hpsh12,hpsh13,hpsh14,hpsh15};
	HPROPSHEETPAGE a[16] = {0};
	int n = 0;
	for(; n<ARRAYLEN(p)&&p[n]; n++) {
		a[n] = p[n];
	}
	return create(hTrust,hParent,hModule,pszCaption,nStartPage,n,a);
}
HWND CPropertySheet::create(DWORD dwTrust, LPCPROPSHEETHEADER lppsh)
{
	destroy();
	CEvent event(TRUE,FALSE);
	CREATETRUSTPARAMS params;
	params.hEvent	= event;
	params.nResult	= -1;
	params.lppsh	= lppsh;
	if(::PostThreadMessage(dwTrust,WM_CREATETRUST,WPARAM(this),LPARAM(&params))) {
		event.wait(INFINITE);
	} else {
		CHECKRESULT_API(0);
	}
	return (HWND)params.nResult;
}
HWND CPropertySheet::create(DWORD dwTrust, HWND hParent, HMODULE hModule, LPCTSTR pszCaption, UINT nStartPage, UINT nPages, HPROPSHEETPAGE *phpsh)
{
	PROPSHEETHEADER psh = { sizeof(psh) };
	psh.dwFlags		= PSH_DEFAULT /*| PSH_PROPTITLE*/;
	psh.hwndParent	= hParent;
	psh.hInstance	= hModule;
	psh.hIcon		= NULL;
	psh.pszCaption	= pszCaption;
	psh.nPages		= nPages;
	psh.nStartPage	= nStartPage;
	psh.phpage		= phpsh;
	psh.pfnCallback	= NULL;
	return create(dwTrust,&psh);
}
HWND CPropertySheet::create(DWORD dwTrust, HWND hParent, HMODULE hModule, LPCTSTR pszCaption, UINT nStartPage, HPROPSHEETPAGE hpsh0, HPROPSHEETPAGE hpsh1, HPROPSHEETPAGE hpsh2, HPROPSHEETPAGE hpsh3, HPROPSHEETPAGE hpsh4, HPROPSHEETPAGE hpsh5, HPROPSHEETPAGE hpsh6, HPROPSHEETPAGE hpsh7, HPROPSHEETPAGE hpsh8, HPROPSHEETPAGE hpsh9, HPROPSHEETPAGE hpsh10, HPROPSHEETPAGE hpsh11, HPROPSHEETPAGE hpsh12, HPROPSHEETPAGE hpsh13, HPROPSHEETPAGE hpsh14, HPROPSHEETPAGE hpsh15)
{
	HPROPSHEETPAGE p[16] = {hpsh0,hpsh1,hpsh2,hpsh3,hpsh4,hpsh5,hpsh6,hpsh7,hpsh8,hpsh9,hpsh10,hpsh11,hpsh12,hpsh13,hpsh14,hpsh15};
	HPROPSHEETPAGE a[16] = {0};
	int n = 0;
	for(; n<ARRAYLEN(p)&&p[n]; n++) {
		a[n] = p[n];
	}
	return create(dwTrust,hParent,hModule,pszCaption,nStartPage,n,a);
}
/*****************************************************************
	���[�_���_�C�A���O�̐���
*****************************************************************/
int CPropertySheet::run(LPCPROPSHEETHEADER lppsh)
{
	destroy();
	PROPSHEETHEADER psh = *lppsh;
	//psh.dwFlags |= PSH_MODELESS;		// ���[�h���X
	//psh.dwFlags |= PSH_USECALLBACK;	// �R�[���o�b�N�g�p
	psh.pfnCallback	= PropSheetProc;	// �R�[���o�b�N
	CCBTHookCreateWnd hook(this);
	hook.enable();
	int n = ::PropertySheet(&psh);
	CHECKRESULT_API(n!=-1);
	return n;
}
int CPropertySheet::run(HWND hParent, HMODULE hModule, LPCTSTR pszCaption, UINT nStartPage, UINT nPages, HPROPSHEETPAGE *phpsh)
{
	PROPSHEETHEADER psh = { sizeof(psh) };
	psh.dwFlags		= PSH_DEFAULT /*| PSH_PROPTITLE*/;
	psh.hwndParent	= hParent;
	psh.hInstance	= hModule;
	psh.hIcon		= NULL;
	psh.pszCaption	= pszCaption;
	psh.nPages		= nPages;
	psh.nStartPage	= nStartPage;
	psh.phpage		= phpsh;
	psh.pfnCallback	= NULL;
	return run(&psh);
}
int CPropertySheet::run(HWND hParent, HMODULE hModule, LPCTSTR pszCaption, UINT nStartPage, HPROPSHEETPAGE hpsh0, HPROPSHEETPAGE hpsh1, HPROPSHEETPAGE hpsh2, HPROPSHEETPAGE hpsh3, HPROPSHEETPAGE hpsh4, HPROPSHEETPAGE hpsh5, HPROPSHEETPAGE hpsh6, HPROPSHEETPAGE hpsh7, HPROPSHEETPAGE hpsh8, HPROPSHEETPAGE hpsh9, HPROPSHEETPAGE hpsh10, HPROPSHEETPAGE hpsh11, HPROPSHEETPAGE hpsh12, HPROPSHEETPAGE hpsh13, HPROPSHEETPAGE hpsh14, HPROPSHEETPAGE hpsh15)
{
	HPROPSHEETPAGE p[16] = {hpsh0,hpsh1,hpsh2,hpsh3,hpsh4,hpsh5,hpsh6,hpsh7,hpsh8,hpsh9,hpsh10,hpsh11,hpsh12,hpsh13,hpsh14,hpsh15};
	HPROPSHEETPAGE a[16] = {0};
	int n = 0;
	for(; n<ARRAYLEN(p)&&p[n]; n++) {
		a[n] = p[n];
	}
	return run(hParent,hModule,pszCaption,nStartPage,n,a);
}
/*****************************************************************
	���[�h���X�_�C�A���O�̐���(���X���b�h�ˑ�)
*****************************************************************/
int CPropertySheet::run(HWND hTrust, LPCPROPSHEETHEADER lppsh)
{
	destroy();
	PROPSHEETHEADER psh = *lppsh;
	//psh.dwFlags |= PSH_MODELESS;		// ���[�h���X
	//psh.dwFlags |= PSH_USECALLBACK;	// �R�[���o�b�N�g�p
	psh.pfnCallback	= PropSheetProc;	// �R�[���o�b�N
	CREATETRUSTPARAMS params;
	params.hEvent	= NULL;
	params.nResult	= -1;
	params.lppsh	= &psh;
	::SendMessage(hTrust,WM_CREATETRUST,WPARAM(this),LPARAM(&params));
	return params.nResult;
}
int CPropertySheet::run(HWND hTrust, HWND hParent, HMODULE hModule, LPCTSTR pszCaption, UINT nStartPage, UINT nPages, HPROPSHEETPAGE *phpsh)
{
	PROPSHEETHEADER psh = { sizeof(psh) };
	psh.dwFlags		= PSH_DEFAULT /*| PSH_PROPTITLE*/;
	psh.hwndParent	= hParent;
	psh.hInstance	= hModule;
	psh.hIcon		= NULL;
	psh.pszCaption	= pszCaption;
	psh.nPages		= nPages;
	psh.nStartPage	= nStartPage;
	psh.phpage		= phpsh;
	psh.pfnCallback	= NULL;
	return run(hTrust,&psh);
}
int CPropertySheet::run(HWND hTrust, HWND hParent, HMODULE hModule, LPCTSTR pszCaption, UINT nStartPage, HPROPSHEETPAGE hpsh0, HPROPSHEETPAGE hpsh1, HPROPSHEETPAGE hpsh2, HPROPSHEETPAGE hpsh3, HPROPSHEETPAGE hpsh4, HPROPSHEETPAGE hpsh5, HPROPSHEETPAGE hpsh6, HPROPSHEETPAGE hpsh7, HPROPSHEETPAGE hpsh8, HPROPSHEETPAGE hpsh9, HPROPSHEETPAGE hpsh10, HPROPSHEETPAGE hpsh11, HPROPSHEETPAGE hpsh12, HPROPSHEETPAGE hpsh13, HPROPSHEETPAGE hpsh14, HPROPSHEETPAGE hpsh15)
{
	HPROPSHEETPAGE p[16] = {hpsh0,hpsh1,hpsh2,hpsh3,hpsh4,hpsh5,hpsh6,hpsh7,hpsh8,hpsh9,hpsh10,hpsh11,hpsh12,hpsh13,hpsh14,hpsh15};
	HPROPSHEETPAGE a[16] = {0};
	int n = 0;
	for(; n<ARRAYLEN(p)&&p[n]; n++) {
		a[n] = p[n];
	}
	return run(hTrust,hParent,hModule,pszCaption,nStartPage,n,a);
}
int CPropertySheet::run(DWORD dwTrust, LPCPROPSHEETHEADER lppsh)
{
	destroy();
	CEvent event(TRUE,FALSE);
	CREATETRUSTPARAMS params;
	params.hEvent	= event;
	params.nResult	= -1;
	params.lppsh	= lppsh;
	if(::PostThreadMessage(dwTrust,WM_CREATETRUST,WPARAM(this),LPARAM(&params))) {
		event.wait(INFINITE);
	} else {
		CHECKRESULT_API(0);
	}
	return params.nResult;
}
int CPropertySheet::run(DWORD dwTrust, HWND hParent, HMODULE hModule, LPCTSTR pszCaption, UINT nStartPage, UINT nPages, HPROPSHEETPAGE *phpsh)
{
	PROPSHEETHEADER psh = { sizeof(psh) };
	psh.dwFlags		= PSH_DEFAULT /*| PSH_PROPTITLE*/;
	psh.hwndParent	= hParent;
	psh.hInstance	= hModule;
	psh.hIcon		= NULL;
	psh.pszCaption	= pszCaption;
	psh.nPages		= nPages;
	psh.nStartPage	= nStartPage;
	psh.phpage		= phpsh;
	psh.pfnCallback	= NULL;
	return run(dwTrust,&psh);
}
int CPropertySheet::run(DWORD dwTrust, HWND hParent, HMODULE hModule, LPCTSTR pszCaption, UINT nStartPage, HPROPSHEETPAGE hpsh0, HPROPSHEETPAGE hpsh1, HPROPSHEETPAGE hpsh2, HPROPSHEETPAGE hpsh3, HPROPSHEETPAGE hpsh4, HPROPSHEETPAGE hpsh5, HPROPSHEETPAGE hpsh6, HPROPSHEETPAGE hpsh7, HPROPSHEETPAGE hpsh8, HPROPSHEETPAGE hpsh9, HPROPSHEETPAGE hpsh10, HPROPSHEETPAGE hpsh11, HPROPSHEETPAGE hpsh12, HPROPSHEETPAGE hpsh13, HPROPSHEETPAGE hpsh14, HPROPSHEETPAGE hpsh15)
{
	HPROPSHEETPAGE p[16] = {hpsh0,hpsh1,hpsh2,hpsh3,hpsh4,hpsh5,hpsh6,hpsh7,hpsh8,hpsh9,hpsh10,hpsh11,hpsh12,hpsh13,hpsh14,hpsh15};
	HPROPSHEETPAGE a[16] = {0};
	int n = 0;
	for(; n<ARRAYLEN(p)&&p[n]; n++) {
		a[n] = p[n];
	}
	return run(dwTrust,hParent,hModule,pszCaption,nStartPage,n,a);
}

/*****************************************************************
	�f�B�X�p�b�`���A�n���h��
*****************************************************************/
int CALLBACK CPropertySheet::PropSheetProc(HWND hwnd, UINT uMsg, LPARAM lParam)
{
	switch(uMsg) {
	case PSCB_INITIALIZED:
		{
			// ���̎��_��hwnd�͗��p�\
			TRACE("PSPCB_INITIALIZED\n");
		}
		break;
	case PSCB_PRECREATE:
		{
			// ���̎��_��hwnd�͗��p�s��
			LPDLGTEMPLATE p = LPDLGTEMPLATE(lParam);
			TRACE("PSPCB_PRECREATE\n");
		}
		break;
	}
	return 1;
}
// �X���b�h���ׂ��E�B���h�E�쐬����
LRESULT CPropertySheet::wmCreateTrust(WPARAM wParam, LPARAM lParam)
{
	LPTYPE pObj = LPTYPE(wParam);
	LPCREATETRUSTPARAMS pParams = LPCREATETRUSTPARAMS(lParam);
	if(pParams->lppsh->dwFlags & PSH_MODELESS) {
		pParams->nResult = (int)pObj->create(pParams->lppsh);
	} else {
		pParams->nResult = pObj->run(pParams->lppsh);
	}
	if(pParams->hEvent) {
		::SetEvent(pParams->hEvent);
	}
	return pParams->nResult;
}

/*****************************************************************
	�v���p�e�B�y�[�W
*****************************************************************/
void CPropertySheetPage::destroy()
{
	BASE::destroy();
	if(handle()) {
		::DestroyPropertySheetPage(handle());
	}
	init();
}
HPROPSHEETPAGE CPropertySheetPage::create(LPCPROPSHEETPAGE lppsp)
{
	destroy();
	PROPSHEETPAGE psp = *lppsp;
	//psp.dwFlags |= PSP_USECALLBACK;
	psp.pfnDlgProc	= DialogProc;
	psp.lParam		= LPARAM(this);
	psp.pfnCallback	= PropSheetPageProc;
	return m_hpsp=::CreatePropertySheetPage(&psp);
}
HPROPSHEETPAGE CPropertySheetPage::create(HMODULE hModule, LPCSTR pszTemplate, LPCSTR pszTitle)
{
	PROPSHEETPAGE psp = { sizeof(psp) };
	psp.dwFlags		= PSP_DEFAULT | (pszTitle ? PSP_USETITLE : 0);
	psp.hInstance	= hModule;
	psp.pszTemplate	= pszTemplate;
	psp.hIcon		= NULL;
	psp.pszTitle	= pszTitle;
	psp.pfnDlgProc	= DialogProc;
	psp.lParam		= LPARAM(this);
	psp.pfnCallback	= NULL;
	psp.pcRefParent	= NULL;
	return create(&psp);
}
HPROPSHEETPAGE CPropertySheetPage::create(HMODULE hModule, LPCDLGTEMPLATE pszTemplate, LPCSTR pszTitle)
{
	PROPSHEETPAGE psp = { sizeof(psp) };
	psp.dwFlags		= PSP_DEFAULT | PSP_DLGINDIRECT | (pszTitle ? PSP_USETITLE : 0);
	psp.hInstance	= hModule;
	psp.pResource	= pszTemplate;
	psp.hIcon		= NULL;
	psp.pszTitle	= pszTitle;
	psp.pfnDlgProc	= DialogProc;
	psp.lParam		= LPARAM(this);
	psp.pfnCallback	= NULL;
	psp.pcRefParent	= NULL;
	return create(&psp);
}
/*****************************************************************
	�f�B�X�p�b�`���A�n���h��
*****************************************************************/
UINT CALLBACK CPropertySheetPage::PropSheetPageProc(HWND hwnd, UINT uMsg, LPPROPSHEETPAGE ppsp)
{
	UINT nRes = 0;
	switch(uMsg) {
#if (_WIN32_IE >= 0x0500)
	case PSPCB_ADDREF:
		TRACE("PSPCB_ADDREF\n");
		break;
#endif
	case PSPCB_CREATE:
		TRACE("PSPCB_CREATE\n");
		nRes = 1;
		break;
	case PSPCB_RELEASE:
		TRACE("PSPCB_RELEASE\n");
		break;
	}
	return nRes;
}
BOOL CALLBACK CPropertySheetPage::DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg==WM_INITDIALOG) {
		LPPROPSHEETPAGE ppsp = LPPROPSHEETPAGE(lParam);
		LPTYPE pObj = LPTYPE(ppsp->lParam);
		SetObject(hwnd,pObj);
		::SetWindowLong(hwnd,DWL_DLGPROC,LONG(BASE::DialogProc));	// ��������������DialogProc����ꊷ����
		pObj->hwnd(hwnd);		// �I�[�i�[�n���h���X�V
		return pObj->handleProc(uMsg,wParam,lParam);
	}
	return FALSE;
}

/*****************************************************************
	�R�����_�C�A���O
*****************************************************************/
/*****************************************************************
	�R�����_�C�A���O���N���X
*****************************************************************/
BOOL CCommonDialog::CheckResult(BOOL bResult, LPCTSTR szFileName, UINT nLineNumber)
{
	if(!bResult) {
		DWORD dwResult = ::CommDlgExtendedError();
		if(dwResult != ERROR_SUCCESS) {
			PrintResult(dwResult,szFileName,nLineNumber);
		}
	}
	return bResult;
}
void CCommonDialog::PrintResult(DWORD dwResult, LPCTSTR szFileName, UINT nLineNumber)
{
	TCHAR str[128];
	LPCTSTR p = GetErrorMessage(dwResult);
	::OutputDebugString(p);
	::OutputDebugString(": File ");
	::OutputDebugString(szFileName);
	::OutputDebugString(", Line ");
	sprintf(str, "%u\n", nLineNumber);
	::OutputDebugString(str);
}
LPCTSTR CCommonDialog::GetErrorMessage(DWORD dwResult)
{
	static const struct {
		HRESULT code;
		LPCTSTR str;
	} assoc[] = {
#define METHOD(eno) {eno,#eno},
		/* �R�����_�C�A���O���� */
		METHOD(CDERR_DIALOGFAILURE)
		METHOD(CDERR_GENERALCODES)
		METHOD(CDERR_STRUCTSIZE)
		METHOD(CDERR_INITIALIZATION)
		METHOD(CDERR_NOTEMPLATE)
		METHOD(CDERR_NOHINSTANCE)
		METHOD(CDERR_LOADSTRFAILURE)
		METHOD(CDERR_FINDRESFAILURE)
		METHOD(CDERR_LOADRESFAILURE)
		METHOD(CDERR_LOCKRESFAILURE)
		METHOD(CDERR_MEMALLOCFAILURE)
		METHOD(CDERR_MEMLOCKFAILURE)
		METHOD(CDERR_NOHOOK)
		METHOD(CDERR_REGISTERMSGFAIL)
		/* PrintDialog */
		METHOD(PDERR_PRINTERCODES)
		METHOD(PDERR_SETUPFAILURE)
		METHOD(PDERR_PARSEFAILURE)
		METHOD(PDERR_RETDEFFAILURE)
		METHOD(PDERR_LOADDRVFAILURE)
		METHOD(PDERR_GETDEVMODEFAIL)
		METHOD(PDERR_INITFAILURE)
		METHOD(PDERR_NODEVICES)
		METHOD(PDERR_NODEFAULTPRN)
		METHOD(PDERR_DNDMMISMATCH)
		METHOD(PDERR_CREATEICFAILURE)
		METHOD(PDERR_PRINTERNOTFOUND)
		METHOD(PDERR_DEFAULTDIFFERENT)
		/* ChooseFont */
		METHOD(CFERR_CHOOSEFONTCODES)
		METHOD(CFERR_NOFONTS)
		METHOD(CFERR_MAXLESSTHANMIN)
		/* GetOpenFileName,GetSaveFileName */
		METHOD(FNERR_FILENAMECODES)
		METHOD(FNERR_SUBCLASSFAILURE)
		METHOD(FNERR_INVALIDFILENAME)
		METHOD(FNERR_BUFFERTOOSMALL)
		/* FindText,ReplaceText */
		METHOD(FRERR_FINDREPLACECODES)
		METHOD(FRERR_BUFFERLENGTHZERO)
		/* ChooseColor */
		METHOD(CCERR_CHOOSECOLORCODES)
		/* */
		METHOD(COMDLGERR_LASTERROR)
#undef METHOD
	};
	for(int i=0; i<ARRAYLEN(assoc); i++) {
		if(assoc[i].code == dwResult) {
			return assoc[i].str;
		}
	}
	return assoc[ARRAYLEN(assoc)-1].str;
}

/*****************************************************************
	�J���[�I���_�C�A���O
*****************************************************************/
BOOL CChooseColorDialog::run(LPCHOOSECOLOR lpcc)
{
	destroy();
	lpcc->Flags		|= CC_ENABLEHOOK;
	lpcc->lCustData	= LPARAM(this);
	lpcc->lpfnHook	= CCHookProc;
	BOOL bRes = ::ChooseColor(lpcc);
	CHECKRESULT_COMDLG(bRes);
	return bRes;
}
// ��{�I�ɂ�CDialog::DialogProc�Ɠ������������
UINT_PTR CALLBACK CChooseColorDialog::CCHookProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPTYPE pObj = LPTYPE(GetObject(hwnd));
	if(!pObj) {
		if(uMsg==WM_INITDIALOG) {
			LPCHOOSECOLOR lpcc = LPCHOOSECOLOR(lParam);
			pObj = LPTYPE(lpcc->lCustData);
			SetObject(hwnd,pObj);
			pObj->hwnd(hwnd);		// �I�[�i�[�n���h���X�V
			return pObj->handleProc(uMsg,wParam,lParam);
		} else {
			return FALSE;
		}
	} else {
		if(uMsg==WM_CLOSE||uMsg==WM_DESTROY||(uMsg==WM_SYSCOMMAND&&wParam==SC_CLOSE)) {
			RemoveObject(hwnd);		// �z�h�~�̂��ߍŏ��Ɋ֘A�t������������
			pObj->show(SW_HIDE);	// �������J047671
			LRESULT lRes = pObj->handleProc(uMsg,wParam,lParam);
			if(pObj->handleAfterClose()==FALSE) {
				::PostQuitMessage(0);
			}
			pObj->hwnd(NULL);		// �I�[�i�[�n���h���X�V
			return lRes;
		} else {
			return pObj->handleProc(uMsg,wParam,lParam);
		}
	}
}

/*****************************************************************
	�t�H���g�I���_�C�A���O
*****************************************************************/
BOOL CChooseFontDialog::run(LPCHOOSEFONT lpcf)
{
	destroy();
	lpcf->Flags		|= CF_ENABLEHOOK;
	lpcf->lCustData	= LPARAM(this);
	lpcf->lpfnHook	= CFHookProc;
	BOOL bRes = ::ChooseFont(lpcf);
	CHECKRESULT_COMDLG(bRes);
	return bRes;
}
// ��{�I�ɂ�CDialog::DialogProc�Ɠ������������
UINT_PTR CALLBACK CChooseFontDialog::CFHookProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPTYPE pObj = LPTYPE(GetObject(hwnd));
	if(!pObj) {
		if(uMsg==WM_INITDIALOG) {
			LPCHOOSEFONT lpcf = LPCHOOSEFONT(lParam);
			pObj = LPTYPE(lpcf->lCustData);
			SetObject(hwnd,pObj);
			pObj->hwnd(hwnd);		// �I�[�i�[�n���h���X�V
			return pObj->handleProc(uMsg,wParam,lParam);
		} else {
			return FALSE;
		}
	} else {
		if(uMsg==WM_CLOSE||uMsg==WM_DESTROY||(uMsg==WM_SYSCOMMAND&&wParam==SC_CLOSE)) {
			if(pObj->handleBeforeClose()) {
				RemoveObject(hwnd);		// �z�h�~�̂��ߍŏ��Ɋ֘A�t������������
				pObj->show(SW_HIDE);	// �������J047671
				LRESULT lRes = pObj->handleProc(uMsg,wParam,lParam);
				if(pObj->handleAfterClose()==FALSE) {
					::PostQuitMessage(0);
				}
				pObj->hwnd(NULL);		// �I�[�i�[�n���h���X�V
				return TRUE;
			}
			return FALSE;
		} else {
			return pObj->handleProc(uMsg,wParam,lParam);
		}
	}
}

/*****************************************************************
	�t�@�C���I���_�C�A���O
*****************************************************************/
BOOL CGetOpenFileNameDialog::run(LPOPENFILENAME lpofn)
{
	destroy();
	//��ofn�̃T�C�Y�ɂ���ăf�t�H���g�̓��삪�قȂ�
	//(�T�C�Y�Ńo�[�W�����𔻒肵�Ă���炵��)
	lpofn->Flags		= lpofn->Flags|OFN_EXPLORER|OFN_ENABLEHOOK;
	lpofn->lCustData	= LPARAM(this);
	lpofn->lpfnHook		= OFNHookProc;
	BOOL bRes = ::GetOpenFileName(lpofn);
	CHECKRESULT_COMDLG(bRes);
	return bRes;
}
BOOL CGetSaveFileNameDialog::run(LPOPENFILENAME lpofn)
{
	destroy();
	//��ofn�̃T�C�Y�ɂ���ăf�t�H���g�̓��삪�قȂ�
	//(�T�C�Y�Ńo�[�W�����𔻒肵�Ă���炵��)
	lpofn->Flags		= lpofn->Flags|OFN_EXPLORER|OFN_ENABLEHOOK;
	lpofn->lCustData	= LPARAM(this);
	lpofn->lpfnHook		= OFNHookProc;
	BOOL bRes = ::GetSaveFileName(lpofn);
	CHECKRESULT_COMDLG(bRes);
	return bRes;
}
// ��{�I�ɂ�CDialog::DialogProc�Ɠ������������
UINT_PTR CALLBACK CGetOpenFileNameDialog::OFNHookProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPTYPE pObj = LPTYPE(GetObject(hwnd));
	if(!pObj) {
		if(uMsg==WM_INITDIALOG) {
			LPOPENFILENAME lpcf = LPOPENFILENAME(lParam);
			pObj = LPTYPE(lpcf->lCustData);
			SetObject(hwnd,pObj);
			pObj->hwnd(hwnd);		// �I�[�i�[�n���h���X�V
			return pObj->handleProc(uMsg,wParam,lParam);
		} else {
			return FALSE;
		}
	} else {
		if(uMsg==WM_CLOSE||uMsg==WM_DESTROY||(uMsg==WM_SYSCOMMAND&&wParam==SC_CLOSE)) {
			RemoveObject(hwnd);		// �z�h�~�̂��ߍŏ��Ɋ֘A�t������������
			pObj->show(SW_HIDE);	// �������J047671
			LRESULT lRes = pObj->handleProc(uMsg,wParam,lParam);
			if(pObj->handleAfterClose()==FALSE) {
				::PostQuitMessage(0);
			}
			pObj->hwnd(NULL);		// �I�[�i�[�n���h���X�V
			return lRes;
		} else {
			return pObj->handleProc(uMsg,wParam,lParam);
		}
	}
}

/*****************************************************************
	�e��R���g���[��
*****************************************************************/

/*****************************************************************
	�R�����R���g���[��
*****************************************************************/

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
