/*
	�L�[�{�[�h�A�N�Z�����[�^
	Copyright (C)1998 H.Mutsuki
*/

#include "util.h"
#include "accel.h"

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
*****************************************************************/
/*****************************************************************
	�L�[�{�[�h�A�N�Z�����[�^
*****************************************************************/
void CAccel::destroy()
{
	if(isOwner()&&handle()) {
		CHECKRESULT_API(::DestroyAcceleratorTable(handle()));
	}
	init();
}
HACCEL CAccel::create(LPACCEL lpaccl, int cEntries)
{
	destroy();
	m_hAccel=::CreateAcceleratorTable(lpaccl,cEntries);
	CHECKRESULT_API(handle());
	return handle();
}
HACCEL CAccel::create(HINSTANCE hInstance, LPCTSTR lpTableName)
{
	destroy();
	m_bOwner = FALSE;	// �J���̕K�v�Ȃ�
	m_hAccel=::LoadAccelerators(hInstance,lpTableName);
	CHECKRESULT_API(handle());
	return handle();
}
HACCEL CAccel::create(BYTE fVirt, WORD key, WORD cmd)
{
	ACCEL accl = { fVirt, key, cmd };
	return create(&accl,1);
}
HACCEL CAccel::create(size_t c, BYTE fVirt, WORD key, WORD cmd, ...)
{
	va_list args;
	va_start(args, c);
	return create(c, args);
}
HACCEL CAccel::create(size_t c, va_list args)
{
	LPACCEL p = (LPACCEL)alloca(sizeof(ACCEL) * c);
	for(size_t i=0; i<c; i++) {
		p[i].fVirt = va_arg(args, BYTE);
		p[i].key   = va_arg(args, WORD);
		p[i].cmd   = va_arg(args, WORD);
	}
	return create(p, c);
}
int CAccel::translate(HWND hwnd, LPMSG lpMsg) const
{
	int iRes=::TranslateAccelerator(hwnd,handle(),lpMsg);
	//CHECKRESULT_API(iRes);
	//���E�B���h�E�̏�Ԃɂ���Ă͏�Ɏ��s����\��������
	//  ���̂��߂����ł̃`�F�b�N���s��Ȃ����Ƃɂ���
	//  TranslateAccelerator()�̃h�L�������g��ǂނׂ�
	return iRes;
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
