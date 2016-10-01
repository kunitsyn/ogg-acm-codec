/*
	Mixerサポートクラス
	Copyright (C)2000 H.Mutsuki
*/

#include "util.h"
#include "mixer.h"

#define MMNOMIDIDEV			// MIDI support
#define MMNOWAVEDEV			// Waveform support
#define MMNOAUXDEV			// Auxiliary output support
#define MMNOMIXERDEV		// Mixer support
#define MMNOTIMERDEV		// Timer support
#define MMNOJOYDEV			// Joystick support
#define MMNOMCIDEV			// MCI support

#include <mmddk.h>			// from NTDDK

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
*****************************************************************/
/*****************************************************************
	グローバル
*****************************************************************/
/*****************************************************************
	ミキサーボリュームの取得
*****************************************************************/
MMRESULT GetMixerVolume(UINT uMxId, DWORD dwComponentType, DWORD *lpdwValue, DWORD *lpdwMinimum, DWORD *lpdwMaximum, DWORD *lpdwSteps)
{
	MMRESULT h = MMSYSERR_NOERROR;
	do {
		CMixer mixer;
		mixer.open(uMxId);

		// SRC SYNTHESIZERのMIXERLINEを取得
		MIXERLINE mxl = {sizeof(mxl)};
		mxl.dwComponentType = dwComponentType;
		h = mixer.GetLineInfo(&mxl,MIXER_GETLINEINFOF_COMPONENTTYPE);
		if(h!=MMSYSERR_NOERROR) break;

		// SRC SYNTHESIZERのVOLUME MIXERLINECONTROLSを取得
		MIXERLINECONTROLS mxlc = {sizeof(mxlc)};
		MIXERCONTROL mxc = {sizeof(mxc)};
		mxlc.dwLineID = mxl.dwLineID;
		mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
		mxlc.cControls = 1;
		mxlc.cbmxctrl = sizeof(mxc);
		mxlc.pamxctrl = &mxc;
		h = mixer.GetLineControls(&mxlc,MIXER_GETLINECONTROLSF_ONEBYTYPE);
		if(h!=MMSYSERR_NOERROR) break;

		// SRC SYNTHESIZER UNIFORM音量取得
		MIXERCONTROLDETAILS mxcd = {sizeof(mxcd)};
		MIXERCONTROLDETAILS_UNSIGNED vol = {0};
		mxcd.cbDetails = sizeof(vol);
		mxcd.dwControlID = mxc.dwControlID;
		mxcd.paDetails = &vol;
		mxcd.cChannels = MIXERCONTROL_CONTROLF_UNIFORM;
		h = mixer.GetControlDetails(&mxcd,MIXER_GETCONTROLDETAILSF_VALUE);
		if(h!=MMSYSERR_NOERROR) break;

		// 返り値の充填
		if(lpdwValue  ) *lpdwValue   = vol.dwValue;
		if(lpdwMinimum) *lpdwMinimum = mxc.Bounds.dwMinimum;
		if(lpdwMaximum) *lpdwMaximum = mxc.Bounds.dwMaximum;
		if(lpdwSteps  ) *lpdwSteps   = mxc.Metrics.cSteps;
	} while(0);
	return h;
}
/*****************************************************************
	ミキサーボリュームの設定
*****************************************************************/
MMRESULT SetMixerVolume(UINT uMxId, DWORD dwComponentType, DWORD dwValue)
{
	MMRESULT h = MMSYSERR_NOERROR;
	do {
		CMixer mixer;
		mixer.open(uMxId);

		// SRC SYNTHESIZERのMIXERLINEを取得
		MIXERLINE mxl = {sizeof(mxl)};
		mxl.dwComponentType = dwComponentType;
		h = mixer.GetLineInfo(&mxl,MIXER_GETLINEINFOF_COMPONENTTYPE);
		if(h!=MMSYSERR_NOERROR) break;

		// SRC SYNTHESIZERのVOLUME MIXERLINECONTROLSを取得
		MIXERLINECONTROLS mxlc = {sizeof(mxlc)};
		MIXERCONTROL mxc = {sizeof(mxc)};
		mxlc.dwLineID = mxl.dwLineID;
		mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
		mxlc.cControls = 1;
		mxlc.cbmxctrl = sizeof(mxc);
		mxlc.pamxctrl = &mxc;
		h = mixer.GetLineControls(&mxlc,MIXER_GETLINECONTROLSF_ONEBYTYPE);
		if(h!=MMSYSERR_NOERROR) break;

		// SRC SYNTHESIZER UNIFORM音量取得
		MIXERCONTROLDETAILS mxcd = {sizeof(mxcd)};
		MIXERCONTROLDETAILS_UNSIGNED vol = {0};
		mxcd.cbDetails = sizeof(vol);
		mxcd.dwControlID = mxc.dwControlID;
		mxcd.paDetails = &vol;
		mxcd.cChannels = MIXERCONTROL_CONTROLF_UNIFORM;
		h = mixer.GetControlDetails(&mxcd,MIXER_GETCONTROLDETAILSF_VALUE);
		if(h!=MMSYSERR_NOERROR) break;

		// SRC SYNTHESIZER UNIFORM音量設定(※現在値と異なる場合のみ設定)
		dwValue = max(mxc.Bounds.dwMinimum,min(mxc.Bounds.dwMaximum,dwValue));
		//if(ALIGN(dwValue,mxc.Metrics.cSteps)!=ALIGN(vol.dwValue,mxc.Metrics.cSteps)) {
		if(dwValue != vol.dwValue) {
			vol.dwValue = dwValue;
			h = mixer.SetControlDetails(&mxcd,MIXER_SETCONTROLDETAILSF_VALUE);
		}
	} while(0);
	return h;
}
/*****************************************************************
	スピーカーボリュームの取得・設定
*****************************************************************/
BOOL GetSpeakersVolume(UINT uMxId, DWORD *lpdwValue, DWORD *lpdwMinimum, DWORD *lpdwMaximum, DWORD *lpdwSteps)
{
	return MMSYSERR_NOERROR==GetMixerVolume(uMxId,MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,lpdwValue,lpdwMinimum,lpdwMaximum,lpdwSteps);
}
BOOL SetSpeakersVolume(UINT uMxId, DWORD dwValue)
{
	return MMSYSERR_NOERROR==SetMixerVolume(uMxId,MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,dwValue);
}
BOOL GetSpeakersVolumeByLong(UINT uMxId, DWORD *lpdwValue)
{
	DWORD v,n,m,s;
	if(GetSpeakersVolume(0,&v,&n,&m,&s)) {
		*lpdwValue = 65536/(m-n)*v;
		return TRUE;
	}
	return FALSE;
}
BOOL SetSpeakersVolumeByLong(UINT uMxId, DWORD dwValue)
{
	DWORD v,n,m,s;
	if(GetSpeakersVolume(0,&v,&n,&m,&s)) {
		dwValue = 65536/(m-n)*dwValue;
		return SetSpeakersVolume(uMxId,dwValue);
	}
	return FALSE;
}
/*****************************************************************
	WAVEボリュームの取得・設定
*****************************************************************/
BOOL GetWaveOutVolume(UINT uMxId, DWORD *lpdwValue, DWORD *lpdwMinimum, DWORD *lpdwMaximum, DWORD *lpdwSteps)
{
	return MMSYSERR_NOERROR==GetMixerVolume(uMxId,MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT,lpdwValue,lpdwMinimum,lpdwMaximum,lpdwSteps);
}
BOOL SetWaveOutVolume(UINT uMxId, DWORD dwValue)
{
	return MMSYSERR_NOERROR==SetMixerVolume(uMxId,MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT,dwValue);
}
BOOL GetWaveOutVolumeByLong(UINT uMxId, DWORD *lpdwValue)
{
	DWORD v,n,m,s;
	if(GetWaveOutVolume(0,&v,&n,&m,&s)) {
		*lpdwValue = 65536/(m-n)*v;
		return TRUE;
	}
	return FALSE;
}
BOOL SetWaveOutVolumeByLong(UINT uMxId, DWORD dwValue)
{
	DWORD v,n,m,s;
	if(GetWaveOutVolume(0,&v,&n,&m,&s)) {
		dwValue = 65536/(m-n)*dwValue;
		return SetWaveOutVolume(uMxId,dwValue);
	}
	return FALSE;
}
/*****************************************************************
	MIDIボリュームの取得・設定
*****************************************************************/
BOOL GetSynthesizerVolume(UINT uMxId, DWORD *lpdwValue, DWORD *lpdwMinimum, DWORD *lpdwMaximum, DWORD *lpdwSteps)
{
	return MMSYSERR_NOERROR==GetMixerVolume(uMxId,MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER,lpdwValue,lpdwMinimum,lpdwMaximum,lpdwSteps);
}
BOOL SetSynthesizerVolume(UINT uMxId, DWORD dwValue)
{
	return MMSYSERR_NOERROR==SetMixerVolume(uMxId,MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER,dwValue);
}
BOOL GetSynthesizerVolumeByLong(UINT uMxId, DWORD *lpdwValue)
{
	DWORD v,n,m,s;
	if(GetSynthesizerVolume(0,&v,&n,&m,&s)) {
		*lpdwValue = 65536/(m-n)*v;
		return TRUE;
	}
	return FALSE;
}
BOOL SetSynthesizerVolumeByLong(UINT uMxId, DWORD dwValue)
{
	DWORD v,n,m,s;
	if(GetSynthesizerVolume(0,&v,&n,&m,&s)) {
		dwValue = 65536/(m-n)*dwValue;
		return SetSynthesizerVolume(uMxId,dwValue);
	}
	return FALSE;
}
/*****************************************************************
	CD Audioボリュームの取得・設定
*****************************************************************/
BOOL GetCDAudioVolume(UINT uMxId, DWORD *lpdwValue, DWORD *lpdwMinimum, DWORD *lpdwMaximum, DWORD *lpdwSteps)
{
	return MMSYSERR_NOERROR==GetMixerVolume(uMxId,MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC,lpdwValue,lpdwMinimum,lpdwMaximum,lpdwSteps);
}
BOOL SetCDAudioVolume(UINT uMxId, DWORD dwValue)
{
	return MMSYSERR_NOERROR==SetMixerVolume(uMxId,MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC,dwValue);
}
BOOL GetCDAudioVolumeByLong(UINT uMxId, DWORD *lpdwValue)
{
	DWORD v,n,m,s;
	if(GetCDAudioVolume(0,&v,&n,&m,&s)) {
		*lpdwValue = 65536/(m-n)*v;
		return TRUE;
	}
	return FALSE;
}
BOOL SetCDAudioVolumeByLong(UINT uMxId, DWORD dwValue)
{
	DWORD v,n,m,s;
	if(GetCDAudioVolume(0,&v,&n,&m,&s)) {
		dwValue = 65536/(m-n)*dwValue;
		return SetCDAudioVolume(uMxId,dwValue);
	}
	return FALSE;
}
/*****************************************************************
	デジタル入力ボリュームの取得・設定
*****************************************************************/
BOOL GetDigitalVolume(UINT uMxId, DWORD *lpdwValue, DWORD *lpdwMinimum, DWORD *lpdwMaximum, DWORD *lpdwSteps)
{
	return MMSYSERR_NOERROR==GetMixerVolume(uMxId,MIXERLINE_COMPONENTTYPE_SRC_DIGITAL,lpdwValue,lpdwMinimum,lpdwMaximum,lpdwSteps);
}
BOOL SetDigitalVolume(UINT uMxId, DWORD dwValue)
{
	return MMSYSERR_NOERROR==SetMixerVolume(uMxId,MIXERLINE_COMPONENTTYPE_SRC_DIGITAL,dwValue);
}
BOOL GetDigitalVolumeByLong(UINT uMxId, DWORD *lpdwValue)
{
	DWORD v,n,m,s;
	if(GetDigitalVolume(0,&v,&n,&m,&s)) {
		*lpdwValue = 65536/(m-n)*v;
		return TRUE;
	}
	return FALSE;
}
BOOL SetDigitalVolumeByLong(UINT uMxId, DWORD dwValue)
{
	DWORD v,n,m,s;
	if(GetDigitalVolume(0,&v,&n,&m,&s)) {
		dwValue = 65536/(m-n)*dwValue;
		return SetDigitalVolume(uMxId,dwValue);
	}
	return FALSE;
}
/*****************************************************************
	ミキサー基底クラス
*****************************************************************/
MMRESULT CMixer::open(UINT uMxId, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen)
{
	destroy();
	HMIXER hmx = NULL;
	MMRESULT h = ::mixerOpen(&hmx,uMxId,dwCallback,dwInstance,fdwOpen);
	CHECKRESULT_MM(h);
	if(h==MMSYSERR_NOERROR) {
		BASE::handle((HMIXEROBJ)hmx);
	}
	return h;
}
MMRESULT CMixer::open(UINT uMxId, DWORD fdwOpen)
{
	return open(uMxId,(DWORD)DriverCallback,(DWORD)this,fdwOpen);
}
MMRESULT CMixer::close()
{
	MMRESULT h = ::mixerClose(handle());
	CHECKRESULT_MM(h);
	BASE::handle(NULL);
	return h;
}
DWORD CMixer::message(UINT uMsg, DWORD dwParam1, DWORD dwParam2)
{
	DWORD dw = ::mixerMessage(handle(),uMsg,dwParam1,dwParam2);
	switch(dw) {
	case MMSYSERR_INVALHANDLE:
	case MMSYSERR_INVALPARAM:
	case MMSYSERR_NOTSUPPORTED:
		CHECKRESULT_MM(dw);
	}
	return dw;
}
BOOL CMixer::DriverCallback(DWORD dwCallback, DWORD dwFlags, HDRVR hdrvr, DWORD msg, DWORD dwUser, DWORD dwParam1, DWORD dwParam2)
{
	ASSERT(dwFlags==DCB_FUNCTION);
	return (LPTYPE(dwCallback))->driverCallback(dwFlags,hdrvr,msg,dwUser,dwParam1,dwParam2);
}
BOOL CMixer::driverCallback(DWORD dwFlags, HDRVR hdrvr, DWORD msg, DWORD dwUser, DWORD dwParam1, DWORD dwParam2)
{
	return TRUE;
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
