/*
	���j���[�T�|�[�g�N���X
	Copyright (C)1998-99 H.Mutsuki
*/

#include "util.h"
#include "menu.h"

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
*****************************************************************/
/*****************************************************************
	���j���[���N���X
*****************************************************************/
/*****************************************************************
	�I�u�W�F�N�g�̍쐬/�폜
*****************************************************************/
HMENU CMenu::create()
{
	destroy();
	m_hMenu = ::CreateMenu();
	return handle();
}
HMENU CMenu::create(HINSTANCE hInstance, LPCTSTR lpMenuName)
{
	destroy();
	CHECKRESULT_API(m_hMenu=::LoadMenu(hInstance,lpMenuName));
	return handle();
}
HMENU CMenu::create(CONST MENUTEMPLATE *lpMenuTemplate)
{
	destroy();
	CHECKRESULT_API(m_hMenu=::LoadMenuIndirect(lpMenuTemplate));
	return handle();
}
/*****************************************************************
	�|�b�v�A�b�v���j���[���N���X
*****************************************************************/
/*****************************************************************
	�I�u�W�F�N�g�̍쐬/�폜
*****************************************************************/
HMENU CPopupMenu::create()
{
	destroy();
	handle(::CreatePopupMenu());
	return handle();
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
