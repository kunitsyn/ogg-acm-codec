/*
	�����I�u�W�F�N�g�N���X
	Copyright (C)1998 H.Mutsuki
*/

#include "util.h"
#include "sync.h"

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
*****************************************************************/
/*****************************************************************
	�O���[�o��
*****************************************************************/
/*****************************************************************
	�I�u�W�F�N�g�n���h���𕡐�����
*****************************************************************/
HANDLE DuplicateHandle(HANDLE hSource)
{
	HANDLE hTarget = INVALID_HANDLE_VALUE;
	::DuplicateHandle(GetCurrentProcess(),	// �������̃v���Z�X�̃n���h��
					hSource,				// ��������n���h��
					GetCurrentProcess(),	// ������̃v���Z�X�̃n���h��
					&hTarget,				// �������ꂽ�n���h���̃A�h���X
					0,//PROCESS_ALL_ACCESS,	// �����n���h���ɑ΂���A�N�Z�X
					TRUE,					// �n���h���p���t���O
					DUPLICATE_SAME_ACCESS	// �I�v�V�����̓���
					);
	CHECKRESULT_API(hTarget && hTarget != INVALID_HANDLE_VALUE);
	return hTarget;
}

/*****************************************************************
	�����I�u�W�F�N�g���N���X
*****************************************************************/
/*****************************************************************
	��I�u�W�F�N�g����
*****************************************************************/
DWORD CSyncObject::wait(DWORD nCount, CONST HANDLE *lpHandles, BOOL fWaitAll, DWORD dwTimeout)
{
	DWORD dw = 0;
	dw = ::WaitForMultipleObjects(nCount,lpHandles,fWaitAll,dwTimeout);
	CHECKRESULT_API(dw!=WAIT_FAILED);
	return dw;
}
/*****************************************************************
	�C�x���g
*****************************************************************/
/*****************************************************************
	�I�u�W�F�N�g�̍쐬/�폜
*****************************************************************/
HANDLE CEvent::create(BOOL fManual, BOOL fInitial, LPCTSTR lpName, LPSECURITY_ATTRIBUTES lpsa)
{
	destroy();
	HANDLE h = ::CreateEvent(lpsa,fManual,fInitial,lpName);
	CHECKRESULT_API(h);
	BASE::handle(h);
	return h;
}
HANDLE CEvent::create(LPCTSTR lpName, DWORD dwDesired, BOOL bInherit)
{
	destroy();
	HANDLE h = ::OpenEvent(dwDesired,bInherit,lpName);
	CHECKRESULT_API(h);
	BASE::handle(h);
	return h;
}
/*****************************************************************
	�~���[�e�b�N�X
*****************************************************************/
/*****************************************************************
	�I�u�W�F�N�g�̍쐬/�폜
*****************************************************************/
HANDLE CMutex::create(BOOL fInitial, LPCTSTR lpName, LPSECURITY_ATTRIBUTES lpsa)
{
	destroy();
	HANDLE h = ::CreateMutex(lpsa,fInitial,lpName);
	CHECKRESULT_API(h);
	BASE::handle(h);
	return h;
}
HANDLE CMutex::create(LPCTSTR lpName, DWORD dwDesired, BOOL bInherit)
{
	destroy();
	HANDLE h = ::OpenMutex(dwDesired,bInherit,lpName);
	CHECKRESULT_API(h);
	BASE::handle(h);
	return h;
}
/*****************************************************************
	�Z�}�t�H
*****************************************************************/
/*****************************************************************
	�I�u�W�F�N�g�̍쐬/�폜
*****************************************************************/
HANDLE CSemaphore::create(LONG lInit, LONG lMax, LPCTSTR lpName, LPSECURITY_ATTRIBUTES lpsa)
{
	destroy();
	HANDLE h = ::CreateSemaphore(lpsa,lInit,lMax,lpName);
	CHECKRESULT_API(h);
	BASE::handle(h);
	return h;
}
HANDLE CSemaphore::create(LPCTSTR lpName, DWORD dwDesired, BOOL bInherit)
{
	destroy();
	HANDLE h = ::OpenSemaphore(dwDesired,bInherit,lpName);
	CHECKRESULT_API(h);
	BASE::handle(h);
	return h;
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
