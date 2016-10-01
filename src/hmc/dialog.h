/*
	�_�C�A���O�T�|�[�g�N���X
	Copyright (C)1998-2000 H.Mutsuki
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __INC_HMC_DIALOG__
#define __INC_HMC_DIALOG__

#include "util.h"
#include "window.h"
#include <commctrl.h>
#include <commdlg.h>
#include <prsht.h>
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "comdlg32.lib")

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
*****************************************************************/
#ifdef _DEBUG
#define CHECKRESULT_COMDLG(exp)	(HMC::CCommonDialog::CheckResult((exp),__FILE__,__LINE__))
#else
#define CHECKRESULT_COMDLG(exp)	(exp)
#endif
#define COMDLGERR_LASTERROR	(DWORD(~0))
/*****************************************************************
*****************************************************************/
// �e��n���h���[
class CWindowHandlerPseudoModeless;
// �e��_�C�A���O
class CDialog;
class CMessageBox;
class CPropertySheet;
class CPropertySheetPage;
class CPseudoModelessMessageBox;
// �R�����_�C�A���O
class CCommonDialog;
class CChooseColorDialog;
class CChooseFontDialog;
class CGetOpenFileNameDialog;
class CGetSaveFileNameDialog;
//class CPrintDialog;
//class CPrintPropertySheet;
//class CPageSetupDialog;
//class CFindAndReplaceDialog;
// �e��R���g���[��
class CControl;
class CStatic;
class CButton;
class CEdit;
//class CRitchEdit;
class CListBox;
class CComboBox;
class CScrollBar;
// �R�����R���g���[��
class CCommonControl;
class CTrackBar;
class CUpDown;
class CProgressBar;
//class CHeaderControl;
//class CListView;
//class CTreeView;
class CTabControl;
//class CHotKeyControl;
//class CDateTimeControl;

/*****************************************************************
	�O���[�o��
*****************************************************************/
BOOL IsDialog(HWND hwnd);	// hwnd�Ŏ��ʂ����E�B���h�E���_�C�A���O���ǂ�������
BOOL IsModal(HWND hwnd);	// hwnd�Ŏ��ʂ����_�C�A���O�����[�_�����ǂ�������

/*****************************************************************
	��W���̒�`
*****************************************************************/
#if 0
typedef struct {
	DWORD helpID;
	DWORD exStyle;
	DWORD style;
	short x;
	short y;
	short cx;
	short cy;
	DWORD id;	// MSDN���C�u�����ł�WORD�ɂȂ��Ă��邪��������DWORD(Q141201���Q��)
	sz_Or_Ord windowClass;	// �ϒ��c�c�ǂ��\�����邩�ˁH
	sz_Or_Ord title;		// �ϒ��c�c�ǂ��\�����邩�ˁH
	WORD extraCount;
} DLGITEMTEMPLATEEX;
#endif

/*****************************************************************
	�[�����[�h���X�_�C�A���O�̋���
*****************************************************************/
class CWindowHandlerPseudoModeless : CWindowHandler
{
public:
	DECLARE_SYNONYM(CWindowHandlerPseudoModeless,TYPE);
	DECLARE_SYNONYM(CWindowHandler,BASE);
public:
	CWindowHandlerPseudoModeless() { init(); }
	CWindowHandlerPseudoModeless(LPBASE p) : BASE(p) { init(); }
	CWindowHandlerPseudoModeless(LPBASE p, HWND hOwner) : BASE(p) { init(); attach(hOwner); }
	virtual ~CWindowHandlerPseudoModeless() { destroy(); }
private:
	void init() { m_hOwner=NULL; }
public:
	void destroy() { init(); }
public:
	HWND attach(HWND hOwner);
protected:
	virtual LRESULT wmInitDialog(WPARAM wParam, LPARAM lParam);
public:
	// ���ʖ�(SetProp��f�t�H���g�N���X���ɗ��p����)
	static  LPCTSTR PropertyName() { return "HMC::CWindowHandlerPseudoModeless"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
private:
	HWND m_hOwner;
};

/*****************************************************************
	���p�_�C�A���O�{�b�N�X
*****************************************************************/
class CDialog : public CWindow
{
public:
	DECLARE_SYNONYM(CDialog,TYPE);
	DECLARE_SYNONYM(CWindow,BASE);
	operator LPTYPE() { return this; }
public:
	CDialog() { init(); }
	virtual ~CDialog() { destroy(); }
private:
	void init() {}
public:
	void destroy() { BASE::destroy(); init(); }
public:
	// ���[�h���X�_�C�A���O�̐���/�I��
	HWND create(HWND hParent, HMODULE hModule, LPCTSTR lpTemplate);
	HWND create(HWND hParent, HMODULE hModule, LPCDLGTEMPLATE lpTemplate);
	// ���[�h���X�_�C�A���O�̐���(���X���b�h�ˑ�)
	HWND create(HWND hTrust, HWND hParent, HMODULE hModule, LPCTSTR lpTemplate);
	HWND create(HWND hTrust, HWND hParent, HMODULE hModule, LPCDLGTEMPLATE lpTemplate);
	HWND create(DWORD dwTrust, HWND hParent, HMODULE hModule, LPCTSTR lpTemplate);
	HWND create(DWORD dwTrust, HWND hParent, HMODULE hModule, LPCDLGTEMPLATE lpTemplate);
	// ���[�_���_�C�A���O�̐���/�I��
	int run(HWND hParent, HMODULE hModule, LPCTSTR lpTemplate);
	int run(HWND hParent, HMODULE hModule, LPCDLGTEMPLATE lpTemplate);
	BOOL end(int nResult);
protected:
	// �r���g�C���f�B�X�p�b�`��
	static BOOL CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	// �f�B�X�p�b�`���A�n���h��
	virtual BOOL translateProc(LPMSG lpMsg) { return ::IsDialogMessage(hwnd(),lpMsg); }
	virtual LRESULT wmInitDialog(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmSysCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmClose(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmCreateTrust(WPARAM wParam, LPARAM lParam);
	virtual LRESULT wmDefault(UINT uMsg, WPARAM wParam, LPARAM lParam);
	// ��DialogProc���g��Ȃ��^�C�v�̃_�C�A���O�ł́ACDialog::wmDefault()���g���Ă͂����Ȃ�
	// ���̂悤�ȃ^�C�v�̃_�C�A���O��CDialog���p������ꍇBASE::BASE::wmDefault()���ĂԂׂ��I
public:
	// ���ʖ�(SetProp��f�t�H���g�N���X���ɗ��p����)
	static  LPCTSTR PropertyName() { return "HMC::CDialog"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
public:
	// �_�C�A���O�쐬���̎Q�ƃf�[�^
	virtual LPCTSTR dlgTemplateName() const { return NULL; }
	virtual LPDLGTEMPLATE dlgTemplate() const { return NULL; }
	virtual LPCTSTR dlgTitleString() const { return NULL; }
private:
	// ���X���b�h�ւ�CREATE�˗��p�p�����[�^
	struct CREATETRUSTPARAMS {
		HANDLE hEvent;
		HWND hParent;
		HMODULE hModule;
		LPCTSTR lpResource;
		LPCDLGTEMPLATE lpTemplate;
	};
	DECLARE_SYNONYM(CREATETRUSTPARAMS,CREATETRUSTPARAMS);
};

/*****************************************************************
	���b�Z�[�W�{�b�N�X
*****************************************************************/
class CMessageBox : public CDialog
{
public:
	DECLARE_SYNONYM(CMessageBox,TYPE);
	DECLARE_SYNONYM(CDialog,BASE);
	operator LPTYPE() { return this; }
public:
	CMessageBox() { init(); }
	CMessageBox(LPMSGBOXPARAMS lpMsgBoxParams) { init(); run(lpMsgBoxParams); }
	CMessageBox(HWND hParent, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, WORD wLanguageId=LANG_USER_DEFAULT) { init(); run(hParent,lpText,lpCaption,uType,wLanguageId); }
	virtual ~CMessageBox() { destroy(); }
private:
	void init() {}
public:
	void destroy() { BASE::destroy(); init(); }
public:
	// ���[�_���_�C�A���O�̐���
	int run(LPMSGBOXPARAMS lpMsgBoxParams);
	int run(HWND hParent, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, WORD wLanguageId=LANG_USER_DEFAULT);
	// ���[�_���_�C�A���O�̐���(���X���b�h�ˑ�)
	int run(HWND hTrust, LPMSGBOXPARAMS lpMsgBoxParams);
	int run(HWND hTrust, HWND hParent, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, WORD wLanguageId=LANG_USER_DEFAULT);
	int run(DWORD dwTrust, LPMSGBOXPARAMS lpMsgBoxParams);
	int run(DWORD dwTrust, HWND hParent, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, WORD wLanguageId=LANG_USER_DEFAULT);
protected:
	// �f�B�X�p�b�`���A�n���h��
	static VOID CALLBACK MsgBoxCallback(LPHELPINFO lpHelpInfo);
	virtual VOID onHelp(LPHELPINFO lpHelpInfo) {}
	virtual LRESULT beforeCreateHook(HWND hwnd, LPCBT_CREATEWND pCreateWnd) { subclass(hwnd); return BASE::beforeCreateHook(hwnd,pCreateWnd); }
	virtual LRESULT wmDefault(UINT uMsg, WPARAM wParam, LPARAM lParam) { return BASE::BASE::wmDefault(uMsg,wParam,lParam); }
	virtual LRESULT wmCreateTrust(WPARAM wParam, LPARAM lParam);
public:
	// ���ʖ�(SetProp��f�t�H���g�N���X���ɗ��p����)
	static  LPCTSTR PropertyName() { return "HMC::CMessageBox"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
private:
	// ���X���b�h�ւ�CREATE�˗��p�p�����[�^
	struct CREATETRUSTPARAMS {
		HANDLE hEvent;
		int nResult;
		LPMSGBOXPARAMS lpMsgBoxParams;
	};
	DECLARE_SYNONYM(CREATETRUSTPARAMS,CREATETRUSTPARAMS);
};

/*****************************************************************
	�[�����[�h���X���b�Z�[�W�{�b�N�X
*****************************************************************/
class CPseudoModelessMessageBox : public CMessageBox
{
public:
	DECLARE_SYNONYM(CPseudoModelessMessageBox,TYPE);
	DECLARE_SYNONYM(CMessageBox,BASE);
	operator LPTYPE() { return this; }
public:
	CPseudoModelessMessageBox() { init(); }
	CPseudoModelessMessageBox(LPMSGBOXPARAMS lpMsgBoxParams) { init(); run(lpMsgBoxParams); }
	CPseudoModelessMessageBox(HWND hParent, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, WORD wLanguageId=LANG_USER_DEFAULT) { init(); run(hParent,lpText,lpCaption,uType,wLanguageId); }
	virtual ~CPseudoModelessMessageBox() { destroy(); }
private:
	void init() {}
public:
	void destroy() { BASE::destroy(); init(); }
public:
	// ���[�_���_�C�A���O�̐���/�I��
	int run(LPMSGBOXPARAMS lpMsgBoxParams);
	int run(HWND hParent, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, WORD wLanguageId=LANG_USER_DEFAULT);
	// ���[�_���_�C�A���O�̐���(���X���b�h�ˑ�)
	int run(HWND hTrust, LPMSGBOXPARAMS lpMsgBoxParams);
	int run(HWND hTrust, HWND hParent, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, WORD wLanguageId=LANG_USER_DEFAULT);
	int run(DWORD dwTrust, LPMSGBOXPARAMS lpMsgBoxParams);
	int run(DWORD dwTrust, HWND hParent, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, WORD wLanguageId=LANG_USER_DEFAULT);
protected:
	// �f�B�X�p�b�`���A�n���h��
	virtual LRESULT wmCreateTrust(WPARAM wParam, LPARAM lParam);
public:
	// ���ʖ�(SetProp��f�t�H���g�N���X���ɗ��p����)
	static  LPCTSTR PropertyName() { return "HMC::CPseudoModelessMessageBox"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
private:
	// ���X���b�h�ւ�CREATE�˗��p�p�����[�^
	struct CREATETRUSTPARAMS {
		HANDLE hEvent;
		int nResult;
		LPMSGBOXPARAMS lpMsgBoxParams;
	};
	DECLARE_SYNONYM(CREATETRUSTPARAMS,CREATETRUSTPARAMS);
};

/*****************************************************************
	�v���p�e�B�V�[�g
*****************************************************************/
#ifndef PSN_GETOBJECT				// _WIN32_IE >= 0x0400
#define PSN_GETOBJECT				(PSN_FIRST-10)
#endif
#ifndef PSN_TRANSLATEACCELERATOR	// _WIN32_IE >= 0x0500
#define PSN_TRANSLATEACCELERATOR	(PSN_FIRST-12)
#endif
#ifndef PSN_QUERYINITIALFOCUS		// _WIN32_IE >= 0x0500
#define PSN_QUERYINITIALFOCUS		(PSN_FIRST-13)
#endif
class CPropertySheet : public CDialog
{
public:
	DECLARE_SYNONYM(CPropertySheet,TYPE);
	DECLARE_SYNONYM(CDialog,BASE);
	operator LPTYPE() { return this; }
public:
	CPropertySheet() { init(); }
	virtual ~CPropertySheet() { destroy(); }
private:
	void init() {}
public:
	void destroy() { BASE::destroy(); init(); }
public:
	// ���[�h���X�_�C�A���O�̐���
	HWND create(LPCPROPSHEETHEADER lppsh);
	HWND create(HWND hParent, HMODULE hModule, LPCTSTR pszCaption, UINT nStartPage, UINT nPages, HPROPSHEETPAGE *phpsh);
	HWND create(HWND hParent, HMODULE hModule, LPCTSTR pszCaption, UINT nStartPage, HPROPSHEETPAGE hpsh0, HPROPSHEETPAGE hpsh1=NULL, HPROPSHEETPAGE hpsh2=NULL, HPROPSHEETPAGE hpsh3=NULL, HPROPSHEETPAGE hpsh4=NULL, HPROPSHEETPAGE hpsh5=NULL, HPROPSHEETPAGE hpsh6=NULL, HPROPSHEETPAGE hpsh7=NULL, HPROPSHEETPAGE hpsh8=NULL, HPROPSHEETPAGE hpsh9=NULL, HPROPSHEETPAGE hpsh10=NULL, HPROPSHEETPAGE hpsh11=NULL, HPROPSHEETPAGE hpsh12=NULL, HPROPSHEETPAGE hpsh13=NULL, HPROPSHEETPAGE hpsh14=NULL, HPROPSHEETPAGE hpsh15=NULL);
	// ���[�h���X�_�C�A���O�̐���(���X���b�h�ˑ�)
	HWND create(HWND hTrust, LPCPROPSHEETHEADER lppsh);
	HWND create(HWND hTrust, HWND hParent, HMODULE hModule, LPCTSTR pszCaption, UINT nStartPage, UINT nPages, HPROPSHEETPAGE *phpsh);
	HWND create(HWND hTrust, HWND hParent, HMODULE hModule, LPCTSTR pszCaption, UINT nStartPage, HPROPSHEETPAGE hpsh0, HPROPSHEETPAGE hpsh1=NULL, HPROPSHEETPAGE hpsh2=NULL, HPROPSHEETPAGE hpsh3=NULL, HPROPSHEETPAGE hpsh4=NULL, HPROPSHEETPAGE hpsh5=NULL, HPROPSHEETPAGE hpsh6=NULL, HPROPSHEETPAGE hpsh7=NULL, HPROPSHEETPAGE hpsh8=NULL, HPROPSHEETPAGE hpsh9=NULL, HPROPSHEETPAGE hpsh10=NULL, HPROPSHEETPAGE hpsh11=NULL, HPROPSHEETPAGE hpsh12=NULL, HPROPSHEETPAGE hpsh13=NULL, HPROPSHEETPAGE hpsh14=NULL, HPROPSHEETPAGE hpsh15=NULL);
	HWND create(DWORD dwTrust, LPCPROPSHEETHEADER lppsh);
	HWND create(DWORD dwTrust, HWND hParent, HMODULE hModule, LPCTSTR pszCaption, UINT nStartPage, UINT nPages, HPROPSHEETPAGE *phpsh);
	HWND create(DWORD dwTrust, HWND hParent, HMODULE hModule, LPCTSTR pszCaption, UINT nStartPage, HPROPSHEETPAGE hpsh0, HPROPSHEETPAGE hpsh1=NULL, HPROPSHEETPAGE hpsh2=NULL, HPROPSHEETPAGE hpsh3=NULL, HPROPSHEETPAGE hpsh4=NULL, HPROPSHEETPAGE hpsh5=NULL, HPROPSHEETPAGE hpsh6=NULL, HPROPSHEETPAGE hpsh7=NULL, HPROPSHEETPAGE hpsh8=NULL, HPROPSHEETPAGE hpsh9=NULL, HPROPSHEETPAGE hpsh10=NULL, HPROPSHEETPAGE hpsh11=NULL, HPROPSHEETPAGE hpsh12=NULL, HPROPSHEETPAGE hpsh13=NULL, HPROPSHEETPAGE hpsh14=NULL, HPROPSHEETPAGE hpsh15=NULL);
	// ���[�_���_�C�A���O�̐���
	int run(LPCPROPSHEETHEADER lppsh);
	int run(HWND hParent, HMODULE hModule, LPCTSTR pszCaption, UINT nStartPage, UINT nPages, HPROPSHEETPAGE *phpsh);
	int run(HWND hParent, HMODULE hModule, LPCTSTR pszCaption, UINT nStartPage, HPROPSHEETPAGE hpsh0, HPROPSHEETPAGE hpsh1=NULL, HPROPSHEETPAGE hpsh2=NULL, HPROPSHEETPAGE hpsh3=NULL, HPROPSHEETPAGE hpsh4=NULL, HPROPSHEETPAGE hpsh5=NULL, HPROPSHEETPAGE hpsh6=NULL, HPROPSHEETPAGE hpsh7=NULL, HPROPSHEETPAGE hpsh8=NULL, HPROPSHEETPAGE hpsh9=NULL, HPROPSHEETPAGE hpsh10=NULL, HPROPSHEETPAGE hpsh11=NULL, HPROPSHEETPAGE hpsh12=NULL, HPROPSHEETPAGE hpsh13=NULL, HPROPSHEETPAGE hpsh14=NULL, HPROPSHEETPAGE hpsh15=NULL);
	// ���[�_���_�C�A���O�̐���(���X���b�h�ˑ�)
	int run(HWND hTrust, LPCPROPSHEETHEADER lppsh);
	int run(HWND hTrust, HWND hParent, HMODULE hModule, LPCTSTR pszCaption, UINT nStartPage, UINT nPages, HPROPSHEETPAGE *phpsh);
	int run(HWND hTrust, HWND hParent, HMODULE hModule, LPCTSTR pszCaption, UINT nStartPage, HPROPSHEETPAGE hpsh0, HPROPSHEETPAGE hpsh1=NULL, HPROPSHEETPAGE hpsh2=NULL, HPROPSHEETPAGE hpsh3=NULL, HPROPSHEETPAGE hpsh4=NULL, HPROPSHEETPAGE hpsh5=NULL, HPROPSHEETPAGE hpsh6=NULL, HPROPSHEETPAGE hpsh7=NULL, HPROPSHEETPAGE hpsh8=NULL, HPROPSHEETPAGE hpsh9=NULL, HPROPSHEETPAGE hpsh10=NULL, HPROPSHEETPAGE hpsh11=NULL, HPROPSHEETPAGE hpsh12=NULL, HPROPSHEETPAGE hpsh13=NULL, HPROPSHEETPAGE hpsh14=NULL, HPROPSHEETPAGE hpsh15=NULL);
	int run(DWORD dwTrust, LPCPROPSHEETHEADER lppsh);
	int run(DWORD dwTrust, HWND hParent, HMODULE hModule, LPCTSTR pszCaption, UINT nStartPage, UINT nPages, HPROPSHEETPAGE *phpsh);
	int run(DWORD dwTrust, HWND hParent, HMODULE hModule, LPCTSTR pszCaption, UINT nStartPage, HPROPSHEETPAGE hpsh0, HPROPSHEETPAGE hpsh1=NULL, HPROPSHEETPAGE hpsh2=NULL, HPROPSHEETPAGE hpsh3=NULL, HPROPSHEETPAGE hpsh4=NULL, HPROPSHEETPAGE hpsh5=NULL, HPROPSHEETPAGE hpsh6=NULL, HPROPSHEETPAGE hpsh7=NULL, HPROPSHEETPAGE hpsh8=NULL, HPROPSHEETPAGE hpsh9=NULL, HPROPSHEETPAGE hpsh10=NULL, HPROPSHEETPAGE hpsh11=NULL, HPROPSHEETPAGE hpsh12=NULL, HPROPSHEETPAGE hpsh13=NULL, HPROPSHEETPAGE hpsh14=NULL, HPROPSHEETPAGE hpsh15=NULL);
public:
	// �R���g���[���ɑ΂��鑀��(�����ǉ�)
	VOID setTitle(BOOL fAddPrefix, /*LPTSTR*/LPCTSTR lpszText) { PropSheet_SetTitle(hwnd(),fAddPrefix,lpszText); }
#if (_WIN32_IE >= 0x0500)
	int  getResult() const { return PropSheet_GetResult(hwnd()); }
#endif
	HWND getTabControl() const { return PropSheet_GetTabControl(hwnd()); }
	BOOL isDialogMessage(LPMSG pMsg) const { return PropSheet_IsDialogMessage(hwnd(),pMsg); }
#if (_WIN32_IE >= 0x0500)
	BOOL recalcPageSizes() { return PropSheet_RecalcPageSizes(hwnd()); }
#endif
	BOOL pressButton(int iButton) { return PropSheet_PressButton(hwnd(),iButton); }
	// �y�[�W�̒ǉ��E�폜
	BOOL addPage(HPROPSHEETPAGE hpage) { return PropSheet_AddPage(hwnd(),hpage); }
	VOID removePage(int index, HPROPSHEETPAGE hpage) { PropSheet_RemovePage(hwnd(),index,hpage); }
	BOOL insertPage(int index, HPROPSHEETPAGE hpage) { return PropSheet_InsertPage(hwnd(),(WPARAM)index,hpage); }
	BOOL insertPage(HPROPSHEETPAGE hpageInsertAfter, HPROPSHEETPAGE hpage) { return PropSheet_InsertPage(hwnd(),(WPARAM)hpageInsertAfter,hpage); }
	// �y�[�W�I���E�I����Ԏ擾
	HWND getCurrentPageHwnd() const { return PropSheet_GetCurrentPageHwnd(hwnd()); }
	BOOL setCurSel(HPROPSHEETPAGE hpage, int index) { return PropSheet_SetCurSel(hwnd(),hpage,index); }
	BOOL setCurSel(HPROPSHEETPAGE hpage) { return PropSheet_SetCurSel(hwnd(),hpage,0); }
	BOOL setCurSel(int index) { return PropSheet_SetCurSel(hwnd(),NULL,index); }
	BOOL setCurSelByID(int id) { return PropSheet_SetCurSelByID(hwnd(),id); }
	// �y�[�W���̎擾
#if (_WIN32_IE >= 0x0500)
	int  hwndToIndex(HWND hPage) const { return PropSheet_HwndToIndex(hwnd(),hPage); }
	int  idToIndex(int iPageID) const { return PropSheet_IdToIndex(hwnd(),iPageID); }
	HWND indexToHwnd(int iPageIndex) const { return PropSheet_IndexToHwnd(hwnd(),iPageIndex); }
	int  indexToId(int iPageIndex) const { return PropSheet_IndexToId(hwnd(),iPageIndex); }
	HPROPSHEETPAGE indexToPage(int iPageIndex) const { return PropSheet_IndexToPage(hwnd(),iPageIndex); }
	int  pageToIndex(HPROPSHEETPAGE hPage) const { return PropSheet_PageToIndex(hwnd(),hPage); }
#endif
	// �v���p�e�B����
	BOOL apply() { return PropSheet_Apply(hwnd()); }
	void calcelToClose() { PropSheet_CancelToClose(hwnd()); }
	BOOL changed(HWND hPage) { return PropSheet_Changed(hwnd(),hPage); }
	VOID unChanged(HWND hwndPage) { PropSheet_UnChanged(hwnd(),hwndPage); }
	int  querySiblings(WPARAM param1, LPARAM param2) const { return PropSheet_QuerySiblings(hwnd(),param1,param2); }
	VOID rebootSystem() { PropSheet_RebootSystem(hwnd()); }
	VOID restartWindows() { PropSheet_RestartWindows(hwnd()); }
	// �E�B�U�[�h�֌W
	VOID setFinishText(/*LPTSTR*/LPCTSTR lpszText) { PropSheet_SetFinishText(hwnd(),lpszText); }
#if (_WIN32_IE >= 0x0500)
	VOID setHeaderSubTitle(int iPageIndex, LPCSTR pszHeaderSubTitle) { PropSheet_SetHeaderSubTitle(hwnd(),iPageIndex,pszHeaderSubTitle); }
	int  setHeaderTitle(int iPageIndex, LPCSTR pszHeaderTitle) { return PropSheet_SetHeaderTitle(hwnd(),iPageIndex,pszHeaderTitle); }
#endif
	VOID setWizButtons(DWORD dwFlags) { PropSheet_SetWizButtons(hwnd(),dwFlags); }
protected:
	// �E�B���h�E�쐬���̃t�b�N(�����ǉ�)
protected:
	// �f�B�X�p�b�`���A�n���h��(�����ǉ�)
	static int CALLBACK PropSheetProc(HWND hwnd, UINT uMsg, LPARAM lParam);
	virtual BOOL translateProc(LPMSG lpMsg) { return send(PSM_ISDIALOGMESSAGE,0,LPARAM(lpMsg)); }
	virtual LRESULT beforeCreateHook(HWND hwnd, LPCBT_CREATEWND pCreateWnd) { subclass(hwnd); return BASE::beforeCreateHook(hwnd,pCreateWnd); }
	virtual LRESULT wmDefault(UINT uMsg, WPARAM wParam, LPARAM lParam) { return BASE::BASE::wmDefault(uMsg,wParam,lParam); }
	virtual LRESULT wmCreateTrust(WPARAM wParam, LPARAM lParam);
public:
	// ���ʖ�(SetProp��f�t�H���g�N���X���ɗ��p����)
	static  LPCTSTR PropertyName() { return "HMC::CPropertySheet"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
private:
	// ���X���b�h�ւ�CREATE�˗��p�p�����[�^
	struct CREATETRUSTPARAMS {
		HANDLE hEvent;
		int nResult;
		LPCPROPSHEETHEADER lppsh;
	};
	DECLARE_SYNONYM(CREATETRUSTPARAMS,CREATETRUSTPARAMS);
};

/*****************************************************************
	�v���p�e�B�y�[�W
*****************************************************************/
class CPropertySheetPage : public CDialog
{
public:
	DECLARE_SYNONYM(CPropertySheetPage,TYPE);
	DECLARE_SYNONYM(CDialog,BASE);
	operator LPTYPE() { return this; }
public:
	CPropertySheetPage() { init(); }
	virtual ~CPropertySheetPage() { destroy(); }
private:
	void init() { m_bOwner=FALSE; m_hpsp=NULL; }
public:
	void destroy();
public:
	// �I�u�W�F�N�g�̍쐬
	HPROPSHEETPAGE create(LPCPROPSHEETPAGE lppsp);
	HPROPSHEETPAGE create(HMODULE hModule, LPCSTR pszTemplate, LPCSTR pszTitle);
	HPROPSHEETPAGE create(HMODULE hModule, LPCDLGTEMPLATE pszTemplate, LPCSTR pszTitle);
	void map(HPROPSHEETPAGE hpsp) { destroy(); m_bOwner=FALSE; m_hpsp=hpsp; }
public:
	// �R���g���[���ɑ΂��鑀��(�����ǉ�)
protected:
	// �E�B���h�E�쐬���̃t�b�N(�����ǉ�)
protected:
	// �f�B�X�p�b�`���A�n���h��(�����ǉ�)
	static UINT CALLBACK PropSheetPageProc(HWND hwnd, UINT uMsg, LPPROPSHEETPAGE ppsp);
	static BOOL CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);	// �ꎞ�I��DialogProc�cWM_INITDIALOG�ȍ~�ABASE::DialogProc�ɏ���������
	virtual BOOL translateProc(LPMSG lpMsg) { return FALSE; }	// PropertySheet�ɏ���������K�v������
public:
	// ���ʖ�(SetProp��f�t�H���g�N���X���ɗ��p����)
	static  LPCTSTR PropertyName() { return "HMC::CPropertySheetPage"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
public:
	// �N���X���̎擾/�ݒ�
	CPropertySheet *getDialog() const { return dynamic_cast<CPropertySheet *>(FromHandle(parent())); }
	HPROPSHEETPAGE handle() const { return m_hpsp; }
	operator HPROPSHEETPAGE() const { return handle(); }
	BOOL isValid() const { return !!handle(); }
	BOOL isOwner() const { return m_bOwner; }
	BOOL ownership(BOOL f) { return m_bOwner=f; }
private:
	BOOL m_bOwner;
	HPROPSHEETPAGE m_hpsp;
};

/*****************************************************************
	�R�����_�C�A���O
*****************************************************************/
/*****************************************************************
	�R�����_�C�A���O���N���X
*****************************************************************/
class CCommonDialog : public CDialog
{
public:
	DECLARE_SYNONYM(CCommonDialog,TYPE);
	DECLARE_SYNONYM(CDialog,BASE);
	operator LPTYPE() { return this; }
protected:
	CCommonDialog() { init(); }
	virtual ~CCommonDialog() { destroy(); }
private:
	void init() {}
public:
	void destroy() { BASE::destroy(); init(); }
public:
	// �I�u�W�F�N�g�̍쐬/�폜
public:
	// �R���g���[���ɑ΂��鑀��(�����ǉ�)
protected:
	// �E�B���h�E�쐬���̃t�b�N(�����ǉ�)
protected:
	// �f�B�X�p�b�`���A�n���h��(�����ǉ�)
public:
	// ���ʖ�(SetProp��f�t�H���g�N���X���ɗ��p����)
	static  LPCTSTR PropertyName() { return "HMC::CommonDialog"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
public:
	// ���U���g�R�[�h����
	static BOOL CheckResult(BOOL bResult, LPCTSTR szFileName, UINT nLineNumber);
	static void PrintResult(DWORD dwResult, LPCTSTR szFileName, UINT nLineNumber);
	static LPCTSTR GetErrorMessage(DWORD dwResult);
};

/*****************************************************************
	�J���[�I���_�C�A���O
*****************************************************************/
class CChooseColorDialog : public CCommonDialog
{
public:
	DECLARE_SYNONYM(CChooseColorDialog,TYPE);
	DECLARE_SYNONYM(CDialog,BASE);
	operator LPTYPE() { return this; }
public:
	CChooseColorDialog() { init(); }
	virtual ~CChooseColorDialog() { destroy(); }
private:
	void init() {}
public:
	void destroy() { BASE::destroy(); init(); }
public:
	// ���[�_���_�C�A���O�̐���
	BOOL run(LPCHOOSECOLOR lpcc);
public:
	// �R���g���[���ɑ΂��鑀��(�����ǉ�)
protected:
	// �E�B���h�E�쐬���̃t�b�N(�����ǉ�)
protected:
	// �f�B�X�p�b�`���A�n���h��(�����ǉ�)
	static UINT_PTR CALLBACK CCHookProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	// ���ʖ�(SetProp��f�t�H���g�N���X���ɗ��p����)
	static  LPCTSTR PropertyName() { return "HMC::CChooseColorDialog"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
};

/*****************************************************************
	�t�H���g�I���_�C�A���O
*****************************************************************/
class CChooseFontDialog : public CCommonDialog
{
public:
	DECLARE_SYNONYM(CChooseFontDialog,TYPE);
	DECLARE_SYNONYM(CDialog,BASE);
	operator LPTYPE() { return this; }
public:
	CChooseFontDialog() { init(); }
	virtual ~CChooseFontDialog() { destroy(); }
private:
	void init() {}
public:
	void destroy() { BASE::destroy(); init(); }
public:
	// ���[�_���_�C�A���O�̐���
	BOOL run(LPCHOOSEFONT lpcf);
public:
	// �R���g���[���ɑ΂��鑀��(�����ǉ�)
protected:
	// �E�B���h�E�쐬���̃t�b�N(�����ǉ�)
protected:
	// �f�B�X�p�b�`���A�n���h��(�����ǉ�)
	static UINT_PTR CALLBACK CFHookProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	// ���ʖ�(SetProp��f�t�H���g�N���X���ɗ��p����)
	static  LPCTSTR PropertyName() { return "HMC::CChooseFontDialog"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
};

/*****************************************************************
	�t�@�C���I���_�C�A���O
*****************************************************************/
class CGetOpenFileNameDialog : public CCommonDialog
{
public:
	DECLARE_SYNONYM(CGetOpenFileNameDialog,TYPE);
	DECLARE_SYNONYM(CDialog,BASE);
	operator LPTYPE() { return this; }
public:
	CGetOpenFileNameDialog() { init(); }
	virtual ~CGetOpenFileNameDialog() { destroy(); }
private:
	void init() {}
public:
	void destroy() { BASE::destroy(); init(); }
public:
	// ���[�_���_�C�A���O�̐���
	BOOL run(LPOPENFILENAME lpofn);
public:
	// �R���g���[���ɑ΂��鑀��(�����ǉ�)
protected:
	// �E�B���h�E�쐬���̃t�b�N(�����ǉ�)
protected:
	// �f�B�X�p�b�`���A�n���h��(�����ǉ�)
	static UINT_PTR CALLBACK OFNHookProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	// ���ʖ�(SetProp��f�t�H���g�N���X���ɗ��p����)
	static  LPCTSTR PropertyName() { return "HMC::CGetOpenFileNameDialog"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
};
class CGetSaveFileNameDialog : public CGetOpenFileNameDialog
{
public:
	DECLARE_SYNONYM(CGetSaveFileNameDialog,TYPE);
	DECLARE_SYNONYM(CGetOpenFileNameDialog,BASE);
	operator LPTYPE() { return this; }
public:
	CGetSaveFileNameDialog() { init(); }
	virtual ~CGetSaveFileNameDialog() { destroy(); }
private:
	void init() {}
public:
	void destroy() { BASE::destroy(); init(); }
public:
	// ���[�_���_�C�A���O�̐���
	BOOL run(LPOPENFILENAME lpofn);
public:
	// ���ʖ�(SetProp��f�t�H���g�N���X���ɗ��p����)
	static  LPCTSTR PropertyName() { return "HMC::CGetSaveFileNameDialog"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
};

/*****************************************************************
	�e��R���g���[��
*****************************************************************/
/*****************************************************************
	�R���g���[�����N���X(����ł͉��̋@�\������)
*****************************************************************/
class CControl : public CWindow
{
public:
	DECLARE_SYNONYM(CControl,TYPE);
	DECLARE_SYNONYM(CWindow,BASE);
	operator LPTYPE() { return this; }
public:
	CControl() { init(); }
	CControl(HWND hwnd) { init(); map(hwnd); }
	CControl(HWND hwnd, UINT nId) { init(); map(hwnd,nId); }
	virtual ~CControl() { destroy(); }
private:
	void init() {}
public:
	void destroy() { BASE::destroy(); init(); }
public:
	// �I�u�W�F�N�g�̍쐬/�폜
public:
	// �R���g���[���ɑ΂��鑀��(�����ǉ�)
protected:
	// �E�B���h�E�쐬���̃t�b�N(�����ǉ�)
protected:
	// �f�B�X�p�b�`���A�n���h��(�����ǉ�)
public:
	// ���ʖ�(SetProp��f�t�H���g�N���X���ɗ��p����)
	static  LPCTSTR PropertyName() { return "HMC::Control"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
	virtual LPCTSTR wndclassName() const { return NULL; }
};

/*****************************************************************
	�X�^�e�B�b�N�R���g���[��
*****************************************************************/
class CStatic : public CControl
{
public:
	DECLARE_SYNONYM(CStatic,TYPE);
	DECLARE_SYNONYM(CControl,BASE);
	operator LPTYPE() { return this; }
public:
	CStatic() { init(); }
	CStatic(HWND hwnd) : BASE(hwnd) { init(); }
	CStatic(HWND hwnd, UINT nId) : BASE(hwnd,nId) { init(); }
	virtual ~CStatic() { destroy(); }
private:
	void init() {}
public:
	void destroy() { BASE::destroy(); init(); }
public:
	// �I�u�W�F�N�g�̍쐬/�폜
public:
	// �R���g���[���ɑ΂��鑀��(�����ǉ�)
	HICON getIcon() const { SUGGEST(haveStyle(SS_ICON)); return (HICON)send(STM_GETICON); }
	HICON setIcon(HICON h) { SUGGEST(haveStyle(SS_ICON)); return (HICON)send(STM_SETICON,(WPARAM)h); }
	//HICON getIcon() const { SUGGEST(haveStyle(SS_ICON)); return (HICON)send(STM_GETIMAGE,IMAGE_ICON); }
	//HICON setIcon(HICON h) { SUGGEST(haveStyle(SS_ICON)); return (HICON)send(STM_SETIMAGE,IMAGE_ICON,(STM_GETIMAGE,IMAGE_ICON)); }
	HBITMAP getBitmap() const { SUGGEST(haveStyle(SS_BITMAP)); return (HBITMAP)send(STM_GETIMAGE,IMAGE_BITMAP); }
	HBITMAP setBitmap(HBITMAP h) { SUGGEST(haveStyle(SS_BITMAP)); return (HBITMAP)send(STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)h); }
	HCURSOR getCursor() const { SUGGEST(haveStyle(SS_ICON)); return (HCURSOR)send(STM_GETIMAGE,IMAGE_CURSOR); }
	HCURSOR setCursor(HCURSOR h) { SUGGEST(haveStyle(SS_ICON)); return (HCURSOR)send(STM_SETIMAGE,IMAGE_CURSOR,(LPARAM)h); }
	HENHMETAFILE getEnhMetaFile() const { SUGGEST(haveStyle(SS_ENHMETAFILE)); return (HENHMETAFILE)send(STM_GETIMAGE,IMAGE_ENHMETAFILE); }
	HENHMETAFILE setEnhMetaFile(HENHMETAFILE h) { SUGGEST(haveStyle(SS_ENHMETAFILE)); return (HENHMETAFILE)send(STM_SETIMAGE,IMAGE_ENHMETAFILE,(LPARAM)h); }
protected:
	// �E�B���h�E�쐬���̃t�b�N(�����ǉ�)
	virtual DWORD createWindowStyle() const { return SS_SIMPLE; }
protected:
	// �f�B�X�p�b�`���A�n���h��(�����ǉ�)
public:
	// ���ʖ�(SetProp��f�t�H���g�N���X���ɗ��p����)
	static  LPCTSTR PropertyName() { return "HMC::CStatic"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
	virtual LPCTSTR wndclassName() const { return "STATIC"; }
};

/*****************************************************************
	�{�^���R���g���[��
*****************************************************************/
class CButton : public CControl
{
public:
	DECLARE_SYNONYM(CButton,TYPE);
	DECLARE_SYNONYM(CControl,BASE);
	operator LPTYPE() { return this; }
public:
	CButton() { init(); }
	CButton(HWND hwnd) : BASE(hwnd) { init(); }
	CButton(HWND hwnd, UINT nId) : BASE(hwnd,nId) { init(); }
	virtual ~CButton() { destroy(); }
private:
	void init() {}
public:
	void destroy() { BASE::destroy(); init(); }
public:
	// �I�u�W�F�N�g�̍쐬/�폜
public:
	// �R���g���[���ɑ΂��鑀��(�����ǉ�)
	void click() { send(BM_CLICK); }
	void check(BOOL fCheck=TRUE) { setCheck(fCheck); }
	void gray (BOOL fCheck=TRUE) { setGray (fCheck); }
	void setCheck(BOOL fCheck=TRUE) { send(BM_SETCHECK,fCheck?BST_CHECKED:BST_UNCHECKED); }
	void setGray (BOOL fCheck=TRUE) { send(BM_SETCHECK,fCheck?BST_INDETERMINATE:BST_UNCHECKED); }
	UINT getState() const { return send(BM_GETSTATE); }
	UINT getCheck() const { return send(BM_GETCHECK); }
	BOOL setState(BOOL fHighlight) { return send(BM_SETSTATE,fHighlight); }	// ���� �n�C���C�g�̐ݒ�
	BOOL setStyle(DWORD fdwStyle, BOOL fRedraw=TRUE) { return send(BM_SETSTYLE,fdwStyle,MAKELPARAM(fRedraw,0)); }
	// �`�F�b�N��Ԃ̎擾
	BOOL isChecked() const { return getCheck()==BST_CHECKED; }
	BOOL isGrayed() const { return getCheck()==BST_INDETERMINATE; }
	BOOL isPushed() const { return !!(getState()&BST_PUSHED); }
	BOOL isFocus() const { return !!(getState()&BST_FOCUS); }
	// �C���[�W�̐ݒ�
	HBITMAP getBitmap() const { return (HBITMAP)send(BM_GETIMAGE,IMAGE_BITMAP); }
	HBITMAP setBitmap(HBITMAP h) { return (HBITMAP)send(BM_SETIMAGE,IMAGE_BITMAP,(LPARAM)h); }
	HICON getIcon() const { return (HICON)send(BM_GETIMAGE,IMAGE_ICON); }
	HICON setIcon(HICON h) { return (HICON)send(BM_SETIMAGE,IMAGE_ICON,(LPARAM)h); }
protected:
	// �E�B���h�E�쐬���̃t�b�N(�����ǉ�)
	virtual DWORD createWindowStyle() const { return BS_PUSHBUTTON|BS_TEXT; }
protected:
	// �f�B�X�p�b�`���A�n���h��(�����ǉ�)
public:
	// ���ʖ�(SetProp��f�t�H���g�N���X���ɗ��p����)
	static  LPCTSTR PropertyName() { return "HMC::CButton"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
	virtual LPCTSTR wndclassName() const { return "BUTTON"; }
};

/*****************************************************************
	�G�f�B�b�g�R���g���[��
*****************************************************************/
class CEdit : public CControl
{
public:
	DECLARE_SYNONYM(CEdit,TYPE);
	DECLARE_SYNONYM(CControl,BASE);
	operator LPTYPE() { return this; }
public:
	CEdit() { init(); }
	CEdit(HWND hwnd) : BASE(hwnd) { init(); }
	CEdit(HWND hwnd, UINT nId) : BASE(hwnd,nId) { init(); }
	virtual ~CEdit() { destroy(); }
private:
	void init() {}
public:
	void destroy() { BASE::destroy(); init(); }
public:
	// �I�u�W�F�N�g�̍쐬/�폜
public:
	// �R���g���[���ɑ΂��鑀��(�����ǉ�)
	BOOL canUndo() const { return !!send(EM_CANUNDO); }
	BOOL emptyUndoBuffer() { return !!send(EM_EMPTYUNDOBUFFER); }
	BOOL getModify() const { return !!send(EM_GETMODIFY); }
	void setModify(BOOL fModify) { send(EM_SETMODIFY,fModify); }
	BOOL getReadOnly() const { return haveStyle(EM_SETREADONLY); }
	BOOL setReadOnly(BOOL fReadOnly) { return !!send(EM_SETREADONLY,fReadOnly); }
	BOOL setLimitText(UINT cchMax) { return !!send(EM_SETLIMITTEXT,cchMax); }
	UINT getLimitText() const { return !!send(EM_GETLIMITTEXT); }
	HLOCAL getHandle() const { SUGGEST(haveStyle(DS_LOCALEDIT)); return (HLOCAL)send(EM_GETHANDLE); }
	void setHandle(HLOCAL hLocal) { send(EM_SETHANDLE,(WPARAM)hLocal); }
	// ���C�A�E�g
	BOOL setTabStops(UINT cTabs, LPDWORD lpdwTabs) { return send(EM_SETTABSTOPS,cTabs,(LPARAM)lpdwTabs); }
	BOOL setMargins(SHORT nLeft, SHORT nRight) { return send(EM_SETMARGINS,EC_LEFTMARGIN|EC_RIGHTMARGIN,MAKELPARAM(nLeft,nRight)); }
	BOOL setLMargins(int nWidth) { return send(EM_SETMARGINS,EC_LEFTMARGIN ,nWidth); }
	BOOL setRMargins(int nWidth) { return send(EM_SETMARGINS,EC_RIGHTMARGIN,nWidth); }
	int  getMargins() const { return send(EM_GETMARGINS); }
	int  getLMargin() const { return LOWORD(getMargins()); }
	int  getRMargin() const { return HIWORD(getMargins()); }
	void getRect(LPRECT lprc) const { send(EM_GETRECT,0,(LPARAM)lprc); }
	void setRect(const RECT *lprc) { send(EM_SETRECT,0,(LPARAM)lprc); }	// NULL���w�肷��ƃf�t�H���g�ɐݒ�
	void setRectNP(const RECT *lprc) { send(EM_SETRECT,0,(LPARAM)lprc); }	// NULL���w�肷��ƃf�t�H���g�ɐݒ�
	// �����ʒu
	int  posFromChar(int nIndex) const { return send(EM_POSFROMCHAR,nIndex); }
	int  charFromPos(SHORT x, SHORT y) const { return send(EM_CHARFROMPOS,0,MAKELPARAM(x,y)); }
	// �e�L�X�g���C��
	LONG getFirstVisibleLine() const { return send(EM_GETFIRSTVISIBLELINE); }
	LONG getLineCount() const { return send(EM_GETLINECOUNT); }
	int  lineFromChar(int nIndex) const { return send(EM_LINEFROMCHAR,nIndex); }
	int  lineLength(LONG ich) const { return send(EM_LINELENGTH,ich); }
	int  getLine(int nLine, LPTSTR lpBuf, WORD cbBuf) const { *((LPWORD)lpBuf)=cbBuf; int n=send(EM_GETLINE,nLine,(LPARAM)lpBuf); lpBuf[min(cbBuf-1,n)]='\0'; return n; }
	int  lineIndex(int nLine) const { return send(EM_LINEINDEX,nLine); }
	// �X�N���[��
	BOOL lineScroll(int v, int h) { return send(EM_LINESCROLL,v,h); }
	BOOL scroll(int nAction) { return send(EM_SCROLL,nAction); }
	void scrollCaret() { send(EM_SCROLLCARET); }
	int getThumb() const { return send(EM_GETTHUMB); }
	// �p�X���[�h����
	TCHAR getPasswordChar() const { return (TCHAR)(send(EM_GETPASSWORDCHAR)); }
	void setPasswordChar(TCHAR c) { send(EM_SETPASSWORDCHAR,c); }
	// �I�����
	BOOL getSel(LPDWORD lpdwStart, LPDWORD lpdwEnd) { return send(EM_GETSEL,(WPARAM)lpdwStart,(LPARAM)lpdwEnd); }
	void setSel(int nStart, int nEnd) const { send(EM_SETSEL,nStart,nEnd); }
	// �ҏW����
	BOOL fmtLines(BOOL fAddEOL) { return send(EM_FMTLINES,fAddEOL); }
	void replaceSel(BOOL fCanUndo, LPCTSTR lpszText) { send(EM_REPLACESEL,fCanUndo,(LPARAM)lpszText); }
	BOOL undo() { return !!send(EM_UNDO); }
	void clear() { send(WM_CLEAR); }
	void copy() { send(WM_COPY); }
	void cut() { send(WM_CUT); }
	void paste() { send(WM_PASTE); }
	// IME�֌W
#if(WINVER >= 0x0500)
	int getIMEStatus(int nType) const { return send(EM_GETIMESTATUS,nType); }
	int setIMEStatus(int nType, int nData) { return send(EM_SETIMESTATUS,nType,nData); }
#endif
	// ���[�h�u���C�N�֌W
	EDITWORDBREAKPROC getWordBreakProc() const { return (EDITWORDBREAKPROC)send(EM_GETWORDBREAKPROC); }
	void setWordBreakProc(EDITWORDBREAKPROC pWordBreakProc)  { send(EM_SETWORDBREAKPROC,0,(LPARAM)pWordBreakProc); }
protected:
	// �E�B���h�E�쐬���̃t�b�N(�����ǉ�)
	virtual DWORD createWindowStyle() const { return ES_LEFT|ES_MULTILINE|ES_AUTOVSCROLL|ES_AUTOHSCROLL|ES_WANTRETURN; }
protected:
	// �f�B�X�p�b�`���A�n���h��(�����ǉ�)
public:
	// ���ʖ�(SetProp��f�t�H���g�N���X���ɗ��p����)
	static  LPCTSTR PropertyName() { return "HMC::CEdit"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
	virtual LPCTSTR wndclassName() const { return "EDIT"; }
};

/*****************************************************************
	���X�g�{�b�N�X
*****************************************************************/
class CListBox : public CControl
{
public:
	DECLARE_SYNONYM(CListBox,TYPE);
	DECLARE_SYNONYM(CControl,BASE);
	operator LPTYPE() { return this; }
public:
	CListBox() { init(); }
	CListBox(HWND hwnd) : BASE(hwnd) { init(); }
	CListBox(HWND hwnd, UINT nId) : BASE(hwnd,nId) { init(); }
	virtual ~CListBox() { destroy(); }
private:
	void init() {}
public:
	void destroy() { BASE::destroy(); init(); }
public:
	// �I�u�W�F�N�g�̍쐬
public:
	// �R���g���[���ɑ΂��鑀��(�����ǉ�)
	int initStorage(UINT nItems, UINT nBytes) { return send(LB_INITSTORAGE,nItems,nBytes); }
	int resetContent() { return send(LB_RESETCONTENT); }
	int getCount() const { return send(LB_GETCOUNT); }
	int setCount(int nCount) { SUGGEST(haveStyle(LBS_NODATA)&&!haveStyle(LBS_HASSTRINGS)); return send(LB_SETCOUNT,nCount); }
	// ���ڂ̒ǉ��E�폜
	int addString(LPCTSTR lpString) { SUGGEST(haveStyle(LBS_HASSTRINGS)); return send(LB_ADDSTRING,0,(LPARAM)lpString); }
	int insertString(int nIndex, LPCTSTR lpString) { SUGGEST(haveStyle(LBS_HASSTRINGS)); return send(LB_INSERTSTRING,nIndex,(LPARAM)lpString); }
	int deleteString(int nIndex) { SUGGEST(haveStyle(LBS_HASSTRINGS)); return send(LB_DELETESTRING,nIndex,0); }
	int getText(int nIndex, LPTSTR pbuf) const { SUGGEST(haveStyle(LBS_HASSTRINGS)); return send(LB_GETTEXT,nIndex,(LPARAM)pbuf); }
	int getTextLen(int nIndex) const { SUGGEST(haveStyle(LBS_HASSTRINGS)); return send(LB_GETTEXTLEN,nIndex); }
	// �t�@�C������
	int dir(UINT uType, LPCTSTR lpPath) { return send(LB_DIR,uType,(LPARAM)lpPath); }
	int addFile(LPCTSTR lpFileName) { return send(LB_ADDFILE,0,(LPARAM)lpFileName); }
	// ����(������)����
	int findString(int nStartIndex, LPCTSTR lpString) const { return send(LB_FINDSTRING,nStartIndex,(LPARAM)lpString); }
	int findStringExact(int nStartIndex, LPCTSTR lpString) const { return send(LB_FINDSTRINGEXACT,nStartIndex,(LPARAM)lpString); }
	int selectString(int nStartIndex, LPCTSTR lpString) { return send(LB_SELECTSTRING,nStartIndex,(LPARAM)lpString); }
	// ���ڑI��
	int getSel(int nIndex) const { return send(LB_GETSEL,nIndex); }
	int setSel(BOOL fHighlight, int nIndex) { return send(LB_SETSEL,fHighlight,nIndex); }
	int getCurSel() const { return send(LB_GETCURSEL); }
	int setCurSel(int nIndex) { return send(LB_SETCURSEL,nIndex); }
	int getSelCount() const { SUGGEST(haveStyle(LBS_MULTIPLESEL)); return send(LB_GETSELCOUNT); }
	int getSelItems(DWORD ccbuf, LPINT pbuf) const { SUGGEST(haveStyle(LBS_MULTIPLESEL)); return send(LB_GETSELITEMS,ccbuf,(LPARAM)pbuf); }
	int selItemRange(BOOL fHighlight, SHORT nFirstItem, SHORT nLastItem) { SUGGEST(haveStyle(LBS_MULTIPLESEL)); return send(LB_SELITEMRANGE,fHighlight,MAKELPARAM(nFirstItem,nLastItem)); }
	int selItemRangeEx(int nFirstItem, int nLastItem) { SUGGEST(haveStyle(LBS_MULTIPLESEL)); return send(LB_SELITEMRANGEEX,nFirstItem,nLastItem); }
	// �A���J�[�E�L�����b�g
	int getAnchorIndex() const { return send(LB_GETANCHORINDEX); }
	int setAnchorIndex(int nIndex) { return send(LB_SETANCHORINDEX,nIndex); }
	int getCaretIndex() const { return send(LB_GETCARETINDEX); }
	int setCarentIndex(int nIndex, BOOL fScroll) { return send(LB_SETCARETINDEX,nIndex,fScroll); }
	// �\��(�X�N���[��)���
	int setTopIndex(int nIndex) { return send(LB_SETTOPINDEX,nIndex); }
	int getTopIndex() const { return send(LB_GETTOPINDEX); }
	// �A�C�e���\���ʒu
	int itemFromPoint(SHORT x, SHORT y) const { return send(LB_ITEMFROMPOINT,0,MAKELPARAM(x,y)); }
	int getItemRect(int nIndex, LPRECT lprc) const { return send(LB_GETITEMRECT,nIndex,(LPARAM)lprc); }
	// �A�C�e���ŗL�f�[�^
	int getItemData(int nIndex) const { return send(LB_GETITEMDATA,nIndex); }
	int setItemData(int nIndex, int nValue) { return send(LB_SETITEMDATA,nIndex,nValue); }
	// �^�u�E�J����
	int setTabStops(UINT ccbuf, const UINT *lpValue) { SUGGEST(haveStyle(LBS_USETABSTOPS)); return send(LB_SETTABSTOPS,ccbuf,(LPARAM)lpValue); }
	int setColumnWidth(int nWidth) { SUGGEST(haveStyle(LBS_MULTICOLUMN)); return send(LB_SETCOLUMNWIDTH,nWidth); }
	// �����X�N���[���ݒ�
	int getHorizontalExtent() const { SUGGEST(haveStyle(WS_HSCROLL)); return send(LB_GETHORIZONTALEXTENT); }
	int setHorizontalExtent(int nWidth) { SUGGEST(haveStyle(WS_HSCROLL)); return send(LB_SETHORIZONTALEXTENT,nWidth); }
	// �I�[�i�[�h���[�\���ݒ�
	int getItemHeight(int nIndex) const { SUGGEST(haveStyle(LBS_OWNERDRAWVARIABLE)); return send(LB_GETITEMHEIGHT,nIndex); }
	int setItemHeight(int nIndex, int nHeight) { SUGGEST(haveStyle(LBS_OWNERDRAWVARIABLE)); return send(LB_SETITEMHEIGHT,nIndex,nHeight); }
	// ���P�[���̐ݒ�
	int getLocale() const { return send(LB_GETLOCALE); }
	int setLocale(int nLocale) { return send(LB_SETLOCALE,nLocale); }
public:
	// ���[�e�B���e�B
	int DlgDirList(LPTSTR lpPathSpec, int nIDListBox, int nIDStaticPath, UINT uFileType) { int i=::DlgDirList(hwnd(),lpPathSpec,nIDListBox,nIDStaticPath,uFileType); CHECKRESULT_API(i); return i; }
	BOOL DlgDirSelectEx(LPTSTR lpString, int nCount, int nIDListBox) { BOOL b=::DlgDirSelectEx(hwnd(),lpString,nCount,nIDListBox); CHECKRESULT_API(b); return b; }
#if(WINVER >= 0x0500)
	DWORD GetListBoxInfo() const { return ::GetListBoxInfo(hwnd()); }
#endif
protected:
	// �E�B���h�E�쐬���̃t�b�N(�����ǉ�)
protected:
	// �f�B�X�p�b�`���A�n���h��(�����ǉ�)
public:
	// ���ʖ�(SetProp��f�t�H���g�N���X���ɗ��p����)
	static  LPCTSTR PropertyName() { return "HMC::CListBox"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
	virtual LPCTSTR wndclassName() const { return "ListBox"; }		// ��`��������Ȃ��̂Ńn�[�h�R�[�h
};

/*****************************************************************
	�R���{�{�b�N�X
*****************************************************************/
class CComboBox : public CControl
{
public:
	DECLARE_SYNONYM(CComboBox,TYPE);
	DECLARE_SYNONYM(CControl,BASE);
	operator LPTYPE() { return this; }
public:
	CComboBox() { init(); }
	CComboBox(HWND hwnd) : BASE(hwnd) { init(); }
	CComboBox(HWND hwnd, UINT nId) : BASE(hwnd,nId) { init(); }
	virtual ~CComboBox() { destroy(); }
private:
	void init() {}
public:
	void destroy() { BASE::destroy(); init(); }
public:
	// �I�u�W�F�N�g�̍쐬/�폜
public:
	// �R���g���[���ɑ΂��鑀��(�����ǉ�)
	BOOL resetContent() { return send(CB_RESETCONTENT)==CB_OKAY; }
	BOOL limitText(LONG cchLimit=0) { return send(CB_LIMITTEXT,cchLimit,0); }
	LONG getCount() const { return send(CB_GETCOUNT); }
	// �g���X�^�C���֌W
	BOOL getExtendedUI() const { return send(CB_GETEXTENDEDUI); }
	BOOL setExtendedUI(BOOL fExtended=TRUE) { return send(CB_SETEXTENDEDUI,fExtended,0)==CB_OKAY; }
	// ���ڂ̒ǉ��E�폜
	BOOL addString(LPCSTR lpszText) { LRESULT l=send(CB_ADDSTRING,0,(LPARAM)lpszText); return !(l==CB_ERR||l==CB_ERRSPACE); }	// LPCTSTR�ł͂Ȃ�
	BOOL deleleString(LONG nIndex) { return send(CB_DELETESTRING)!=CB_ERR; }
	BOOL insertString(LONG nIndex, LPCSTR lpszText) { LRESULT l=send(CB_INSERTSTRING,0,(LPARAM)lpszText); return !(l==CB_ERR||l==CB_ERRSPACE); }	// LPCTSTR�ł͂Ȃ�
	// �t�@�C������
	BOOL dir(UINT uAttrs=0, LPCTSTR lpszFileSpec="*.*") { LRESULT l=send(CB_DIR,uAttrs,(LPARAM)lpszFileSpec); return !(l==CB_ERR||l==CB_ERRSPACE); }
	// ����(������)����
	LONG findString(LONG indexStart/*=-1*/, LPCTSTR lpszFind) const { return send(CB_FINDSTRING,indexStart,(LPARAM)lpszFind); }
	LONG findStringExact(LONG indexStart/*=-1*/, LPCTSTR lpszFind) const { return send(CB_FINDSTRINGEXACT,indexStart,(LPARAM)lpszFind); }
	LONG selectString(LONG nIndexStart, LPCSTR lpszText) { return send(CB_SELECTSTRING,nIndexStart,(LPARAM)lpszText); }
	// ���ڑI��
	LONG getCurSel() const { return send(CB_GETCURSEL); }
	LONG setCurSel(LONG nIndex) { return send(CB_SETCURSEL,nIndex,0); }
	// �h���b�v�_�E�����
	BOOL getDroppedControlRect(LPRECT lprc) const { return send(CB_GETDROPPEDCONTROLRECT,0,(LPARAM)lprc)==CB_OKAY; }
	BOOL getDroppedState() const { return send(CB_GETDROPPEDSTATE); }
	BOOL showDropDown(BOOL fShow=TRUE) { return send(CB_SHOWDROPDOWN,fShow,0); }
	// �G�f�B�b�g�֌W
	DWORD getEditSel(LPDWORD lpdwStart=NULL, LPDWORD lpdwEnd=NULL) const { return send(CB_GETEDITSEL,(WPARAM)lpdwStart,(LPARAM)lpdwEnd); }
	// ���X�g�{�b�N�X����
	LONG getLBText(LONG nIndex, LPCSTR lpszBuf) const { SUGGEST(CB_GETLBTEXTLEN); return send(CB_GETLBTEXT,nIndex,(LPARAM)lpszBuf); }	// ����LPCSTR�Ȃ́H
	LONG getLBTextLen(LONG nIndex) const { SUGGEST(CB_GETLBTEXTLEN); return send(CB_GETLBTEXTLEN,nIndex,0); }
	// �A�C�e���ŗL�f�[�^
	DWORD getItemData(LONG nIndex) const { return send(CB_GETITEMDATA,nIndex,0); }
	BOOL setItemData(LONG nIndex, DWORD dwData) const { return send(CB_SETITEMDATA,nIndex,dwData)!=CB_ERR; }
	// �I�[�i�[�h���[�\���ݒ�
	LONG getItemHeight(LONG nIndex) const { SUGGEST(haveStyle(CBS_OWNERDRAWVARIABLE)); return send(CB_GETITEMHEIGHT,nIndex,0); }
	BOOL setItemHeight(LONG nIndex, LONG nHeight) const { SUGGEST(haveStyle(CBS_OWNERDRAWVARIABLE)); return send(CB_SETITEMHEIGHT,nIndex,nHeight)!=CB_ERR; }
	// ���P�[���̐ݒ�
	WORD getLocale() const { return (WORD)send(CB_GETLOCALE); }
	BOOL setLocale(WORD wLocale) { return send(CB_SETLOCALE,wLocale,0); }
protected:
	// �E�B���h�E�쐬���̃t�b�N(�����ǉ�)
	virtual DWORD createWindowStyle() const { return CBS_DROPDOWN|CBS_HASSTRINGS|CBS_DISABLENOSCROLL; }
protected:
	// �f�B�X�p�b�`���A�n���h��(�����ǉ�)
public:
	// ���ʖ�(SetProp��f�t�H���g�N���X���ɗ��p����)
	static  LPCTSTR PropertyName() { return "HMC::CComboBox"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
	virtual LPCTSTR wndclassName() const { return "COMBOBOX"; }
};

/*****************************************************************
	�X�N���[���o�[
*****************************************************************/
class CScrollBar : public CControl
{
public:
	DECLARE_SYNONYM(CScrollBar,TYPE);
	DECLARE_SYNONYM(CControl,BASE);
	operator LPTYPE() { return this; }
public:
	CScrollBar() { init(); }
	CScrollBar(HWND hwnd) : BASE(hwnd) { init(); }
	CScrollBar(HWND hwnd, UINT nId) : BASE(hwnd,nId) { init(); }
	virtual ~CScrollBar() { destroy(); }
private:
	void init() {}
public:
	void destroy() { BASE::destroy(); init(); }
public:
	// �I�u�W�F�N�g�̍쐬/�폜
public:
	// �R���g���[���ɑ΂��鑀��(�����ǉ�)
	BOOL enableArrows(UINT nOption) { return send(SBM_ENABLE_ARROWS,nOption); }
	// �X�N���[���ʒu
	int getPos() const { return send(SBM_GETPOS); }
	int setPos(int nPos, BOOL fRedraw=TRUE) { return send(SBM_SETPOS,nPos,fRedraw); }
	// �X�N���[�������W
	void getRange(LPINT lpMin, LPINT lpMax) const { send(SBM_GETRANGE,(WPARAM)lpMin,(LPARAM)lpMax); }
	int setRange(int nMin, int nMax) { return send(SBM_SETRANGE,nMin,nMax); }
	int setRangeRedraw(int nMin, int nMax) { return send(SBM_SETRANGEREDRAW,nMin,nMax); }
	// SCROLLINFO�֌W
	BOOL getScrollInfo(LPSCROLLINFO lpsi) const { return send(SBM_GETSCROLLINFO,0,(LPARAM)lpsi); }	// SCROLLINFO��cbSize,fMask��\�ߐݒ肵�Ă����K�v������
	int setScrollInfo(LPCSCROLLINFO lpsi) { return send(SBM_SETSCROLLINFO,0,(LPARAM)lpsi); }
protected:
	// �E�B���h�E�쐬���̃t�b�N(�����ǉ�)
protected:
	// �f�B�X�p�b�`���A�n���h��(�����ǉ�)
public:
	// ���ʖ�(SetProp��f�t�H���g�N���X���ɗ��p����)
	static  LPCTSTR PropertyName() { return "HMC::CScrollBar"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
	virtual LPCTSTR wndclassName() const { return "SCROLLBAR"; }
};

/*****************************************************************
	�R�����R���g���[��
*****************************************************************/
/*****************************************************************
	�R�����R���g���[�����N���X(����ł͉��̋@�\������)
*****************************************************************/
class CCommonControl : public CControl
{
public:
	DECLARE_SYNONYM(CCommonControl,TYPE);
	DECLARE_SYNONYM(CControl,BASE);
	operator LPTYPE() { return this; }
public:
	CCommonControl() { init(); }
	CCommonControl(HWND hwnd) { init(); map(hwnd); }
	CCommonControl(HWND hwnd, UINT nId) { init(); map(hwnd,nId); }
	virtual ~CCommonControl() { destroy(); }
private:
	void init() {}
public:
	void destroy() { BASE::destroy(); init(); }
public:
	// �I�u�W�F�N�g�̍쐬/�폜
public:
	// �R���g���[���ɑ΂��鑀��(�����ǉ�)
protected:
	// �E�B���h�E�쐬���̃t�b�N(�����ǉ�)
protected:
	// �f�B�X�p�b�`���A�n���h��(�����ǉ�)
public:
	// ���ʖ�(SetProp��f�t�H���g�N���X���ɗ��p����)
	static  LPCTSTR PropertyName() { return "HMC::CommonControl"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
	virtual LPCTSTR wndclassName() const { return NULL; }
};
/*****************************************************************
	�g���b�N�o�[(�X���C�_�[)�R���g���[��
*****************************************************************/
class CTrackBar : public CCommonControl
{
public:
	DECLARE_SYNONYM(CTrackBar,TYPE);
	DECLARE_SYNONYM(CCommonControl,BASE);
	operator LPTYPE() { return this; }
public:
	CTrackBar() { init(); }
	CTrackBar(HWND hwnd) : BASE(hwnd) { init(); }
	CTrackBar(HWND hwnd, UINT nId) : BASE(hwnd,nId) { init(); }
	virtual ~CTrackBar() { destroy(); }
private:
	void init() {}
public:
	void destroy() { BASE::destroy(); init(); }
public:
	// �I�u�W�F�N�g�̍쐬/�폜
public:
	// �R���g���[���ɑ΂��鑀��(�����ǉ�)
	// �X���C�_�ʒu
	LONG getPos() const { return send(TBM_GETPOS); }
	void setPos(LONG nPos, BOOL fRedraw=TRUE) { send(TBM_SETPOS,fRedraw,nPos); }
	// �`�b�N�ݒ�
	LONG getTic(LONG nTic) const { return send(TBM_GETTIC,MAKEWPARAM(nTic,0)); }
	LONG getTicPos(LONG nTic) const { return send(TBM_GETTICPOS,MAKEWPARAM(nTic,0)); }
	ULONG getNumTics() const { return send(TBM_GETNUMTICS); }
	LPDWORD getPTics() const { return (LPDWORD)send(TBM_GETPTICS); }
	BOOL setTic(LONG nTic) { return send(TBM_SETTIC,0,nTic); }
	void setTicFreq(UINT nFreq) { send(TBM_SETTICFREQ,nFreq,0); }
	void crearTics(BOOL fRedraw=TRUE) { send(TBM_CLEARTICS,fRedraw); }
	// �����W�̐ݒ�
	LONG getMaxRange() const { return send(TBM_GETRANGEMAX); }
	void setMaxRange(LONG nMax, BOOL fRedraw=TRUE) { send(TBM_SETRANGEMAX,fRedraw,nMax); }
	LONG getMinRange() const { return send(TBM_GETRANGEMIN); }
	void setMinRange(LONG nMin, BOOL fRedraw=TRUE) { send(TBM_SETRANGEMIN,fRedraw,nMin); }
	void setRange(LONG nMin, LONG nMax, BOOL fRedraw=TRUE) { send(TBM_SETRANGE,fRedraw,MAKELPARAM(nMin,nMax)); }
	// �I�����
	void setSel(LONG nMin, LONG nMax, BOOL fRedraw=TRUE) { send(TBM_SETSEL,fRedraw,MAKELPARAM(nMin,nMax)); }
	LONG getSelStart() const { return send(TBM_GETSELSTART); }
	LONG getSelEnd() const { return send(TBM_GETSELEND); }
	void setSelStart(LONG nMin, BOOL fRedarw=TRUE) const { send(TBM_SETSELSTART,fRedarw,nMin); }
	void setSelEnd(LONG nMax, BOOL fRedarw=TRUE) const { send(TBM_SETSELEND,fRedarw,nMax); }
	void crearSel(BOOL fRedraw=TRUE) { send(TBM_CLEARSEL,fRedraw); }
	// �L�[�{�[�h���͊��x
	LONG getLineSize() const { return send(TBM_GETLINESIZE); }
	LONG setLineSize(LONG nSize) { return send(TBM_SETLINESIZE,0,nSize); }
	LONG getPageSize() const { return send(TBM_GETPAGESIZE); }
	LONG setPageSize(LONG nSize) { return send(TBM_SETPAGESIZE,0,nSize); }
	// �o�f�B�A�`���l���A�T���\���ݒ�
	HWND getBuddy(BOOL fLocation=TRUE) const { return (HWND)send(TBM_GETBUDDY,fLocation); }
	HWND setBuddy(HWND hwnd, BOOL fLocation=TRUE) { return (HWND)send(TBM_SETBUDDY,fLocation,(LPARAM)hwnd); }
	void getChannelRect(LPRECT lprc) const { send(TBM_GETCHANNELRECT,0,(LPARAM)lprc); }
	UINT getThumbLength() const { return send(TBM_GETTHUMBLENGTH); }
	void setThumbLength(UINT nLength) const { send(TBM_SETTHUMBLENGTH,nLength,0); }
	void getThumbRect(LPRECT lprc) const { send(TBM_GETTHUMBRECT,0,(LPARAM)lprc); }
#if (_WIN32_IE >= 0x0300)
	// �c�[���`�b�v�֌W
	HWND getToolTips() const { return (HWND)send(TBM_GETTOOLTIPS); }
	void setToolTips(HWND hwnd) { send(TBM_SETTOOLTIPS,(WPARAM)hwnd,0); }
	LONG setTipSide(LONG fLocation) { return send(TBM_SETTIPSIDE,fLocation,0); }
#endif
#if (_WIN32_IE >= 0x0400)
	// UNICODE�ݒ�
	BOOL getUnicodeFormat() const { return send(TBM_GETUNICODEFORMAT); }
	BOOL setUnicodeFormat(BOOL fUnicode) { return send(TBM_SETUNICODEFORMAT); }
#endif
protected:
	// �E�B���h�E�쐬���̃t�b�N(�����ǉ�)
	virtual DWORD createWindowStyle() const { return TBS_HORZ; }
protected:
	// �f�B�X�p�b�`���A�n���h��(�����ǉ�)
public:
	// ���ʖ�(SetProp��f�t�H���g�N���X���ɗ��p����)
	static  LPCTSTR PropertyName() { return "HMC::CTrackBar"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
	virtual LPCTSTR wndclassName() const { return TRACKBAR_CLASS; }
};

/*****************************************************************
	�A�b�v�_�E��(�X�s���{�^��)�R���g���[��
*****************************************************************/
class CUpDown : public CCommonControl
{
public:
	DECLARE_SYNONYM(CUpDown,TYPE);
	DECLARE_SYNONYM(CCommonControl,BASE);
	operator LPTYPE() { return this; }
public:
	CUpDown() { init(); }
	CUpDown(HWND hwnd) : BASE(hwnd) { init(); }
	CUpDown(HWND hwnd, UINT nId) : BASE(hwnd,nId) { init(); }
	virtual ~CUpDown() { destroy(); }
private:
	void init() {}
public:
	void destroy() { BASE::destroy(); init(); }
public:
	// �I�u�W�F�N�g�̍쐬/�폜
	//HWND create(HWND hParent, UINT nId, DWORD dwStyle, int x, int y, int w, int h, HWND hBuddy, int nUpper, int nLower, int nPos) { subclass(::CreateUpDownControl(dwStyle,x,y,w,h,hParent,nId,::GetModuleHandle(NULL),hBuddy,nUpper,nLower,nPos)); ownership(TRUE); return !!handle(); }
public:
	// �R���g���[���ɑ΂��鑀��(�����ǉ�)
	// �A�N�Z�����[�V�����ݒ�
	LONG getAccel(LONG nAccels, LPUDACCEL paAccels) const { return send(UDM_GETACCEL,nAccels,(LPARAM)paAccels); }
	BOOL setAccel(LONG nAccels, LPUDACCEL paAccels) { return send(UDM_SETACCEL,nAccels,(LPARAM)paAccels); }
	// ��ݒ�
	LONG getBase() const { return send(UDM_GETBASE); }
	LONG setBase(LONG nBase) { return send(UDM_SETBASE,nBase); }
	// �o�f�B�\���ݒ�
	HWND getBuddy() const { return (HWND)send(UDM_GETBUDDY); }
	HWND setBuddy(HWND hwnd) { return (HWND)send(UDM_SETBUDDY,(WPARAM)hwnd,0); }
	// �ʒu
	LONG getPos() const { return send(UDM_GETPOS); }
	LONG setPos(LONG nPos) { return send(UDM_SETPOS,0,MAKELPARAM(nPos,0)); }
#if (_WIN32_IE >= 0x0500)
	LONG getPos32() const { return send(UDM_GETPOS32); }
	LONG setPos32(LONG nPos) { return send(UDM_SETPOS32,0,nPos); }
#endif
	// �����W�ݒ�
	UINT getRange() const { return send(UDM_GETRANGE); }
	void setRange(SHORT nMin, SHORT nMax) { send(UDM_SETRANGE,0,MAKELPARAM(nMin,nMax)); }
#if (_WIN32_IE >= 0x0400)
	void getRange32(LPINT pMin, LPINT pMax) { send(UDM_SETRANGE32,(WPARAM)pMin,(LPARAM)pMax); }
	void setRange32(INT nMin, INT nMax) { send(UDM_SETRANGE32,nMin,nMax); }
#endif
	// UNICODE�ݒ�
#if (_WIN32_IE >= 0x0400)
	BOOL getUnicodeFormat() const { return send(UDM_GETUNICODEFORMAT); }
	BOOL setUnicodeFormat(BOOL fUnicode) { return send(UDM_SETUNICODEFORMAT); }
#endif
protected:
	// �E�B���h�E�쐬���̃t�b�N(�����ǉ�)
	virtual DWORD createWindowStyle() const { return UDS_ARROWKEYS; }
protected:
	// �f�B�X�p�b�`���A�n���h��(�����ǉ�)
public:
	// ���ʖ�(SetProp��f�t�H���g�N���X���ɗ��p����)
	static  LPCTSTR PropertyName() { return "HMC::CUpDown"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
	virtual LPCTSTR wndclassName() const { return UPDOWN_CLASS; }
};

/*****************************************************************
	�v���O���X�R���g���[��
*****************************************************************/
class CProgressBar : public CCommonControl
{
public:
	DECLARE_SYNONYM(CProgressBar,TYPE);
	DECLARE_SYNONYM(CCommonControl,BASE);
	operator LPTYPE() { return this; }
public:
	CProgressBar() { init(); }
	CProgressBar(HWND hwnd) : BASE(hwnd) { init(); }
	CProgressBar(HWND hwnd, UINT nId) : BASE(hwnd,nId) { init(); }
	virtual ~CProgressBar() { destroy(); }
private:
	void init() {}
public:
	void destroy() { BASE::destroy(); init(); }
public:
	// �I�u�W�F�N�g�̍쐬/�폜
public:
	// �R���g���[���ɑ΂��鑀��(�����ǉ�)
	// �ʒu�ݒ�
	LONG getPos() const { return send(PBM_GETPOS); }
	LONG setPos(LONG nPos) const { return send(PBM_SETPOS,nPos,0); }
	// �ʒu�X�V
	LONG deltaPos(LONG nIncrement) { return send(PBM_DELTAPOS,nIncrement,0); }
	// �����W�ݒ�
	LONG setRange(SHORT nMin, SHORT nMax) { return send(PBM_SETRANGE,0,MAKELPARAM(nMin,nMax)); }
#if (_WIN32_IE >= 0x0300)
	DWORD setRange32(LONG nMin, LONG nMax) { return send(PBM_SETRANGE32,nMin,nMax); }
#endif
	LONG getRange(BOOL fWitchLimit, PPBRANGE ppBRange) const { return send(PBM_GETRANGE,fWitchLimit,(LPARAM)ppBRange); }
	// �X�e�b�v�ݒ�
	LONG setStep(LONG nStepInc) { return send(PBM_SETSTEP,nStepInc,0); }
	LONG stepIt() { return send(PBM_STEPIT); }
	// �J���[�ݒ�
//#if (_WIN32_IE >= 0x0300)
#if (_WIN32_IE >= 0x0400)
	COLORREF setBarColor(COLORREF col) { return send(PBM_SETBARCOLOR,0,col); }
//#endif
	COLORREF setBkColor(COLORREF col) { return send(PBM_SETBKCOLOR,0,col); }
#endif
protected:
	// �E�B���h�E�쐬���̃t�b�N(�����ǉ�)
	//virtual DWORD createWindowStyle() const { return PBS_SMOOTH; }
protected:
	// �f�B�X�p�b�`���A�n���h��(�����ǉ�)
public:
	// ���ʖ�(SetProp��f�t�H���g�N���X���ɗ��p����)
	static  LPCTSTR PropertyName() { return "HMC::CProgressBar"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
	virtual LPCTSTR wndclassName() const { return PROGRESS_CLASS; }
};

/*****************************************************************
	�^�u�R���g���[��
*****************************************************************/
class CTabControl : public CCommonControl
{
public:
	DECLARE_SYNONYM(CTabControl,TYPE);
	DECLARE_SYNONYM(CCommonControl,BASE);
	operator LPTYPE() { return this; }
public:
	CTabControl() { init(); }
	CTabControl(HWND hwnd) : BASE(hwnd) { init(); }
	CTabControl(HWND hwnd, UINT nId) : BASE(hwnd,nId) { init(); }
	virtual ~CTabControl() { destroy(); }
private:
	void init() {}
public:
	void destroy() { BASE::destroy(); init(); }
public:
	// �I�u�W�F�N�g�̍쐬/�폜
public:
	// �R���g���[���ɑ΂��鑀��(�����ǉ�)
	// �C���[�W���X�g����
	HIMAGELIST getImageList() const { return TabCtrl_GetImageList(hwnd()); }
	HIMAGELIST setImageList(HIMAGELIST himl) { return TabCtrl_SetImageList(hwnd(),himl); }
	// �A�C�e��
	int getItemCount() const { return TabCtrl_GetItemCount(hwnd()); }
	BOOL getItem(int iItem, LPTCITEM pitem) const { return TabCtrl_GetItem(hwnd(),iItem,pitem); }
	BOOL setItem(int iItem, LPTCITEM pitem) { return TabCtrl_SetItem(hwnd(),iItem,pitem); }
	int  insertItem(int iItem, LPTCITEM pitem) { return TabCtrl_InsertItem(hwnd(),iItem,pitem); }
	BOOL deleteItem(int iItem) { return TabCtrl_DeleteItem(hwnd(),iItem); }
	BOOL deleteAllItems() { return TabCtrl_DeleteAllItems(hwnd()); }
	BOOL getItemRect(int iItem, LPRECT lprc) const { return TabCtrl_GetItemRect(hwnd(),iItem,lprc); }
	// �I�����
	int  getCurSel() const { return TabCtrl_GetCurSel(hwnd()); }
	int  setCurSel(int iItem) { return TabCtrl_SetCurSel(hwnd(),iItem); }
	//
	int hitTest(LPTCHITTESTINFO pinfo) const { return TabCtrl_HitTest(hwnd(),pinfo); }
	BOOL setItemExtra(int cb) { return TabCtrl_SetItemExtra(hwnd(),cb); }
	VOID adjustRect(BOOL bLarger, LPRECT lprc) { TabCtrl_AdjustRect(hwnd(),bLarger,lprc); }
	DWORD setItemSize(int cx, int cy) { return TabCtrl_SetItemSize(hwnd(),cx,cy); }
	void removeImage(int iImage) { TabCtrl_RemoveImage(hwnd(),iImage); }
	void setPadding(int cx, int cy) { TabCtrl_SetPadding(hwnd(),cx,cy); }
	int  getRowCount() const { return TabCtrl_GetRowCount(hwnd()); }
	HWND getToolTips() const { return TabCtrl_GetToolTips(hwnd()); }
	void setToolTips(HWND hwndTT) { TabCtrl_SetToolTips(hwnd(),hwndTT); }
	int  getCurFocus() const { return TabCtrl_GetCurFocus(hwnd()); }
	VOID setCurFocus(int iItem) { TabCtrl_SetCurFocus(hwnd(),iItem); }
#if (_WIN32_IE >= 0x0300)
	int setMinTabWidth(INT cx) { return TabCtrl_SetMinTabWidth(hwnd(),cx); }
	void deselectAll(BOOL fExcludeFocus) { TabCtrl_DeselectAll(hwnd(),fExcludeFocus); }
#endif
#if (_WIN32_IE >= 0x0400)
	BOOL highlightItem(INT idItem, WORD fHighlight) { return TabCtrl_HighlightItem(hwnd(),idItem,fHighlight); }
	DWORD setExtendedStyle(DWORD dwExStyle) { return TabCtrl_SetExtendedStyle(hwnd(),dwExStyle); }
	DWORD getExtendedStyle() const { return TabCtrl_GetExtendedStyle(hwnd()); }
	BOOL setUnicodeFormat(BOOL fUnicode) { return TabCtrl_SetUnicodeFormat(hwnd(),fUnicode); }
	BOOL getUnicodeFormat() const { return TabCtrl_GetUnicodeFormat(hwnd()); }
#endif
protected:
	// �E�B���h�E�쐬���̃t�b�N(�����ǉ�)
protected:
	// �f�B�X�p�b�`���A�n���h��(�����ǉ�)
public:
	// ���ʖ�(SetProp��f�t�H���g�N���X���ɗ��p����)
	static  LPCTSTR PropertyName() { return "HMC::CTabControl"; }
	virtual LPCTSTR identityName() const { return PropertyName(); }
	virtual LPCTSTR wndclassName() const { return WC_TABCONTROL; }
};

/*****************************************************************
*****************************************************************/
_HMC_END
/*****************************************************************
*****************************************************************/

#endif	/* __INC_HMC_DIALOG__ */

/* Local Variables:						*/
/* tab-width:4							*/
/* c-basic-offset:4						*/
/* c-indent-level:4						*/
/* c-label-offset:-4					*/
/* c-continued-statement-offset:4		*/
/* file-coding-system:*sjis*dos			*/
/* buffer-file-coding-system:sjis-dos	*/
/* End:									*/
