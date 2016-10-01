/*
	acm �h���C�o�[�T�|�[�g�N���X
	Copyright (C)2000 H.Mutsuki
*/

#include "util.h"
#include "mmutil.h"
#include "acmdrv.h"
#include <msacmdrv.h>	// from DDK

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
*****************************************************************/
/*****************************************************************
	MSACM����CODEC�h���C�o�[�̂ЂȌ`
*****************************************************************/
LRESULT CacmDriver::DriverProc(DWORD dwId, HDRVR hdrvr, UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
	LRESULT lRes = 0L;
	LPTYPE p = LPTYPE(dwId);
	do {
		if(uMsg==DRV_LOAD) {
			lRes = 1L;			// 0�ȊO�̒l��Ԃ��Ɛ���
			break;
		} else if(uMsg==DRV_OPEN) {
			LPACMDRVOPENDESC paod = (LPACMDRVOPENDESC)lParam2;
			if(paod) {
				if(paod->fccType!=ACMDRIVERDETAILS_FCCTYPE_AUDIOCODEC) {
					lRes = 0L;	// ������CODEC�^�C�v
					break;
				}
			}
			p = new TYPE();		// �V�K�C���X�^���X�쐬
			if(!p) {
				if(paod) {
					paod->dwError = MMSYSERR_NOMEM;
				}
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
			lRes = BASE::DriverProc(dwId,hdrvr,uMsg,lParam1,lParam2);
		}
	} while(0);
	return lRes;
}
LRESULT CacmDriver::driverProc(UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
#define METHOD(msg,handler) case msg: return handler(lParam1,lParam2);
	switch(uMsg) {
	HMC_CACMDRIVERHANDLER_ALIST
	default:
		return BASE::driverProc(uMsg,lParam1,lParam2);
	}
#undef METHOD
}
LRESULT CacmDriver::dmDefault(UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
	LRESULT lRes = 0L;
	switch(uMsg) {
	case ACMDM_DRIVER_NOTIFY:
	case ACMDM_DRIVER_ABOUT:
	case ACMDM_DRIVER_DETAILS:
	case ACMDM_HARDWARE_WAVE_CAPS_INPUT:
	case ACMDM_HARDWARE_WAVE_CAPS_OUTPUT:
	case ACMDM_FORMATTAG_DETAILS:
	case ACMDM_FORMAT_DETAILS:
	case ACMDM_FORMAT_SUGGEST:
	case ACMDM_FILTERTAG_DETAILS:
	case ACMDM_FILTER_DETAILS:
	case ACMDM_STREAM_OPEN:
	case ACMDM_STREAM_CLOSE:
	case ACMDM_STREAM_SIZE:
	case ACMDM_STREAM_CONVERT:
	//case ACMDM_STREAM_RESET:		// ���������Ă��Ȃ��Ă�����ɌĂяo�����\��������
	//case ACMDM_STREAM_PREPARE:	// ���������Ă��Ȃ��Ă�����ɌĂяo�����\��������
	//case ACMDM_STREAM_UNPREPARE:	// ���������Ă��Ȃ��Ă�����ɌĂяo�����\��������
	//case ACMDM_STREAM_UPDATE:		// ���������Ă��Ȃ��Ă�����ɌĂяo�����\��������
		//lRes = MMSYSERR_NODRIVER;
		lRes = MMSYSERR_NOTSUPPORTED;
		break;
	default:
		lRes = BASE::dmDefault(uMsg,lParam1,lParam2);
		break;
	}
	return lRes;
}
#define METHOD(msg,handler) \
LRESULT CacmDriver::handler(LPARAM lParam1, LPARAM lParam2) {	\
	return dmUnhandled(msg,lParam1,lParam2);	\
}
HMC_CACMDRIVERHANDLER_ALIST
#undef METHOD
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
