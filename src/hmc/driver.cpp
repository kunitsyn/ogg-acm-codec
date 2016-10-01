/*
	�h���C�o�[�T�|�[�g�N���X
	Copyright (C)1998 H.Mutsuki
*/

#include "util.h"
#include "driver.h"

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
	�h���C�o�[���N���X
*****************************************************************/
HDRVR CDrvr::create(LPCWSTR lpDriverName, LPCWSTR lpSectionName, LONG lParam)
{
	destroy();
	m_hdrvr = ::OpenDriver(lpDriverName,lpSectionName,lParam);
	return handle();
}
LRESULT CDrvr::close(LPARAM lParam1, LPARAM lParam2)
{
	LRESULT lRes = 0;
	if(handle()) {
		lRes = ::CloseDriver(handle(),lParam1,lParam2);
		m_hdrvr = NULL;
	}
	return lRes;
}

/*****************************************************************
	�h���C�o�[�n���h���N���X
*****************************************************************/
/*****************************************************************
	�e���|�����}�b�v�̍쐬/�폜
*****************************************************************/
void CDriverHandler::map(HDRVR hdrvr)
{
	if(BASE::handle()!=hdrvr) {
		if(isEnable() && hdrvr==NULL) {
			handleDisableProc();
		}
		BASE::map(hdrvr);	// ��I�[�i�[�n���h���X�V
		if(isEnable() && BASE::isValid()) {
			handleEnableProc();
		}
	}
	DWORD nChilds = GetChildCount();
	for(DWORD n=0; n<nChilds; n++) {
		LPTYPE pChild = GetChild(n);
		ASSERT(pChild);
		pChild->map(hdrvr);
	}
}
/*****************************************************************
	�I�[�i�[�n���h���X�V
*****************************************************************/
void CDriverHandler::handle(HDRVR hdrvr)
{
	BASE::handle(hdrvr);		// �I�[�i�[�n���h���X�V
	DWORD nChilds = GetChildCount();
	for(DWORD n=0; n<nChilds; n++) {
		LPTYPE pChild = GetChild(n);
		ASSERT(pChild);
		pChild->map(hdrvr);
	}
}
/*****************************************************************
	�n���h���̗L�������؂�ւ�
*****************************************************************/
BOOL CDriverHandler::enable()
{
	BOOL old = isEnable();
	if(!m_bEnable) {
		m_bEnable = TRUE;
		handleEnableProc();
	}
	return old;
}
BOOL CDriverHandler::disable()
{
	BOOL old = isEnable();
	if(!m_bEnable) {
		handleDisableProc();
		m_bEnable = FALSE;
	}
	return old;
}
void CDriverHandler::handleEnableProc()
{
	DWORD nChilds = GetChildCount();
	for(LONG n=nChilds-1; 0<=n; n--) {
		LPTYPE pChild = GetChild(n);
		ASSERT(pChild);
		pChild->handleEnableProc();
	}
	if(isEnable()) {
		enableProc();
	}
}
void CDriverHandler::handleDisableProc()
{
	DWORD nChilds = GetChildCount();
	for(LONG n=nChilds-1; 0<=n; n--) {
		LPTYPE pChild = GetChild(n);
		ASSERT(pChild);
		pChild->handleDisableProc();
	}
	if(isEnable()) {
		disableProc();
	}
}
/*****************************************************************
	�`�F�C���f�B�X�p�b�`��
*****************************************************************/
// ���D�揇�ʂ�driverProc���Ăяo��(��ԍŏ��̓����)
LRESULT CDriverHandler::handleProc(UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
	for(LPTYPE p=end(); p; p=p->prev()) {
		if(p->isEnable()) {
			LPTYPE pChild = p->GetChild(0);
			if(pChild) {
				return pChild->handleProc(uMsg,lParam1,lParam2);
			}
			return p->driverProc(uMsg,lParam1,lParam2);
		}
	}
	return dmUnhandled(uMsg,lParam1,lParam2);
}
// ��������Ȃ����b�Z�[�W������driverProc�Ƀ��_�C���N�g
LRESULT CDriverHandler::unhandledProc(UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
	LPTYPE p = prev();
	for(; p; p=p->prev()) {
		if(p->isEnable()) {
			LPTYPE pChild = p->GetChild(0);
			if(pChild) {
				return pChild->handleProc(uMsg,lParam1,lParam2);
			}
			return p->driverProc(uMsg,lParam1,lParam2);
		}
	}
	if(p=GetOwner()) {
		if(p->isEnable()) {
			return p->driverProc(uMsg,lParam1,lParam2);
		} else {
			return p->unhandledProc(uMsg,lParam1,lParam2);
		}
	}
	return dmDefault(uMsg,lParam1,lParam2);
}
/*****************************************************************
	�f�B�X�p�b�`���A�n���h��
*****************************************************************/
// ���b�Z�[�W�f�B�X�p�b�`��
LRESULT CDriverHandler::driverProc(UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
#define METHOD(msg,handler) case msg: return handler(lParam1,lParam2);
	switch(uMsg) {
	HMC_CDRIVERHANDLER_ALIST
	default:
		return dmUnhandled(uMsg,lParam1,lParam2);
	}
#undef METHOD
}
// �f�t�H���g�n���h��
LRESULT CDriverHandler::dmDefault(UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
	return ::DefDriverProc(DWORD(this),handle(),uMsg,lParam1,lParam2);
}
// ���n���h�������t�B���^
LRESULT CDriverHandler::dmUnhandled(UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
	return unhandledProc(uMsg,lParam1,lParam2);
}
// �n���h���錾�{��
#if 0
#define METHOD(msg,handler) \
LRESULT CDriverHandler::handler(LPARAM lParam1, LPARAM lParam2) {	\
	return dmUnhandled(msg,lParam1,lParam2);	\
}
HMC_CDRIVERHANDLER_ALIST
#undef METHOD
#else
LRESULT CDriverHandler::dmLoad(LPARAM lParam1, LPARAM lParam2)
{
	return 1L;	// ��0�Ő���
}
LRESULT CDriverHandler::dmEnable(LPARAM lParam1, LPARAM lParam2)
{
	return 0L;	// �Ԃ�l�͖��������
}
LRESULT CDriverHandler::dmOpen(LPARAM lParam1, LPARAM lParam2)
{
	return LRESULT(this);	// ���񂩂�dwId�Ƃ��Ďg�p�����
}
LRESULT CDriverHandler::dmClose(LPARAM lParam1, LPARAM lParam2)
{
	return 1L;	// ��0�Ő���
}
LRESULT CDriverHandler::dmDisable(LPARAM lParam1, LPARAM lParam2)
{
	return 0L;	// �Ԃ�l�͖��������
}
LRESULT CDriverHandler::dmFree(LPARAM lParam1, LPARAM lParam2)
{
	return 0L;	// �Ԃ�l�͖��������
}
LRESULT CDriverHandler::dmConfigure(LPARAM lParam1, LPARAM lParam2)
{
	return DRVCNF_OK;
	//return DRVCNF_CANCEL;
	//return DRVCNF_RESTART;
}
LRESULT CDriverHandler::dmQueryConfigure(LPARAM lParam1, LPARAM lParam2)
{
	return 0L;	// ��0��Configure�Ή�
}
LRESULT CDriverHandler::dmInstall(LPARAM lParam1, LPARAM lParam2)
{
	return DRVCNF_OK;
	//return DRVCNF_CANCEL;
	//return DRVCNF_RESTART;
}
LRESULT CDriverHandler::dmRemove(LPARAM lParam1, LPARAM lParam2)
{
	return 0L;	// �Ԃ�l�͖��������
}
LRESULT CDriverHandler::dmExitSession(LPARAM lParam1, LPARAM lParam2)
{
	return 0L;	// �Ԃ�l�͖��������
}
LRESULT CDriverHandler::dmPower(LPARAM lParam1, LPARAM lParam2)
{
	return 1L;	// ��0�Ő���
}
#endif
/*****************************************************************
	���p�h���C�o�[�N���X
*****************************************************************/
/*****************************************************************
	���b�Z�[�W�f�B�X�p�b�`��
*****************************************************************/
LRESULT CALLBACK CDriver::DriverProc(DWORD dwId, HDRVR hdrvr, UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
	LRESULT lRes = 0L;
	LPTYPE p = LPTYPE(dwId);
	do {
		if(uMsg==DRV_LOAD) {
			lRes = 1L;			// 0�ȊO�̒l��Ԃ��Ɛ���
			break;
		} else if(uMsg==DRV_OPEN) {
			p = new TYPE();		// �V�K�C���X�^���X�쐬
			if(!p) {
				lRes = 0L;		// �V�K�C���X�^���X�̍쐬�Ɏ��s�����ꍇ��0��Ԃ�
				break;
			}
			p->map(hdrvr);		// �n���h���Ɗ֘A�t��(CDrvr��close()����̂�h�����ߔ�I�[�i�[�Ƃ���)
		}
		if(p) {
			if(p->handle()==hdrvr) {
				lRes = p->driverProc(uMsg,lParam1,lParam2);
				if(uMsg==DRV_OPEN) {
					lRes = (LRESULT)p;	// �Ԃ�l�͎��񂩂�dwId�Ƃ��Ďg����
				} else if(uMsg==DRV_CLOSE) {
					delete p;	// �C���X�^���X�폜
				}
			} else {
				lRes = MMSYSERR_INVALHANDLE;
			}
		} else {
			lRes = ::DefDriverProc(dwId,hdrvr,uMsg,lParam1,lParam2);
		}
	} while(0);
	return lRes;
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
