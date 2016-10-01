/*
	acm ドライバーサポートクラス
	Copyright (C)2000 H.Mutsuki
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __INC_HMC_ACMDRV__
#define __INC_HMC_ACMDRV__

#include "util.h"
#include "driver.h"
#include <msacm.h>

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
*****************************************************************/
class CacmDriver;
/*****************************************************************
	MSACM準拠CODECドライバーのひな形
*****************************************************************/
class CacmDriver : public CDriver
{
public:
	DECLARE_SYNONYM(CacmDriver,TYPE);
	DECLARE_SYNONYM(CDriver,BASE);
	operator LPTYPE() { return this; }
public:
	static LRESULT CALLBACK DriverProc(DWORD dwId, HDRVR hdrvr, UINT uMsg, LPARAM lParam1, LPARAM lParam2);
protected:
	virtual LRESULT driverProc(UINT uMsg, LPARAM lParam1, LPARAM lParam2);
	virtual LRESULT dmDefault(UINT uMsg, LPARAM lParam1, LPARAM lParam2);
#define HMC_CACMDRIVERHANDLER_ALIST	\
	METHOD(ACMDM_DRIVER_NOTIFY,dmDriverNotify)	\
	METHOD(ACMDM_DRIVER_ABOUT,dmDriverAbout)	\
	METHOD(ACMDM_DRIVER_DETAILS,dmDriverDetails)	\
	METHOD(ACMDM_HARDWARE_WAVE_CAPS_INPUT,dmHardwareWaveCapsInput)	\
	METHOD(ACMDM_HARDWARE_WAVE_CAPS_OUTPUT,dmHardwareWaveCapsOutput)	\
	METHOD(ACMDM_FORMATTAG_DETAILS,dmFormatTagDetails)	\
	METHOD(ACMDM_FORMAT_DETAILS,dmFormatDetails)	\
	METHOD(ACMDM_FORMAT_SUGGEST,dmFormatSuggest)	\
	METHOD(ACMDM_FILTERTAG_DETAILS,dmFilterTagDetails)	\
	METHOD(ACMDM_FILTER_DETAILS,dmFilterDetails)	\
	METHOD(ACMDM_STREAM_OPEN,dmStreamOpen)	\
	METHOD(ACMDM_STREAM_CLOSE,dmStreamClose)	\
	METHOD(ACMDM_STREAM_SIZE,dmStreamSize)	\
	METHOD(ACMDM_STREAM_CONVERT,dmStreamConvert)	\
	METHOD(ACMDM_STREAM_RESET,dmStreamReset)	\
	METHOD(ACMDM_STREAM_PREPARE,dmStreamPrepare)	\
	METHOD(ACMDM_STREAM_UNPREPARE,dmStreamUnprepare)	\
	METHOD(ACMDM_STREAM_UPDATE,dmStreamUpdate)
#define METHOD(msg,handler) virtual LRESULT handler(LPARAM,LPARAM);
	HMC_CACMDRIVERHANDLER_ALIST
#undef METHOD
};
/*****************************************************************
*****************************************************************/
_HMC_END
/*****************************************************************
*****************************************************************/

#endif	/* __INC_HMC_ACMDRV__ */

/* Local Variables:						*/
/* tab-width:4							*/
/* c-basic-offset:4						*/
/* c-indent-level:4						*/
/* c-label-offset:-4					*/
/* c-continued-statement-offset:4		*/
/* file-coding-system:*sjis*dos			*/
/* buffer-file-coding-system:sjis-dos	*/
/* End:									*/
