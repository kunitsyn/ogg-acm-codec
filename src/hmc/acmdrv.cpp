/*
	acm ドライバーサポートクラス
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
	MSACM準拠CODECドライバーのひな形
*****************************************************************/
LRESULT CacmDriver::DriverProc(DWORD dwId, HDRVR hdrvr, UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
	LRESULT lRes = 0L;
	LPTYPE p = LPTYPE(dwId);
	do {
		if(uMsg==DRV_LOAD) {
			lRes = 1L;			// 0以外の値を返すと成功
			break;
		} else if(uMsg==DRV_OPEN) {
			LPACMDRVOPENDESC paod = (LPACMDRVOPENDESC)lParam2;
			if(paod) {
				if(paod->fccType!=ACMDRIVERDETAILS_FCCTYPE_AUDIOCODEC) {
					lRes = 0L;	// 無効なCODECタイプ
					break;
				}
			}
			p = new TYPE();		// 新規インスタンス作成
			if(!p) {
				if(paod) {
					paod->dwError = MMSYSERR_NOMEM;
				}
				lRes = 0L;		// 新規インスタンスの作成に失敗した場合は0を返す
				break;
			}
			p->map(hdrvr);		// ハンドルと関連付け(CDrvrがclose()するのを防ぐため非オーナーとする)
		}
		if(p) {
			if(p->handle()==hdrvr) {
				lRes = p->driverProc(uMsg,lParam1,lParam2);
				if(uMsg==DRV_OPEN) {
					lRes = (LRESULT)p;	// 返り値は次回からdwIdとして使われる
				} else if(uMsg==DRV_CLOSE) {
					delete p;	// インスタンス削除
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
	//case ACMDM_STREAM_RESET:		// ※実装していなくても正常に呼び出される可能性がある
	//case ACMDM_STREAM_PREPARE:	// ※実装していなくても正常に呼び出される可能性がある
	//case ACMDM_STREAM_UNPREPARE:	// ※実装していなくても正常に呼び出される可能性がある
	//case ACMDM_STREAM_UPDATE:		// ※実装していなくても正常に呼び出される可能性がある
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
