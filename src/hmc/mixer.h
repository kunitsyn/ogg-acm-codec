/*
	Mixerサポートクラス
	Copyright (C)2000 H.Mutsuki
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __INC_HMC_MIXER__
#define __INC_HMC_MIXER__

#include "util.h"
#include "mmutil.h"

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
*****************************************************************/
class CMixerObj;
class CMixer;
/*****************************************************************
	グローバル
*****************************************************************/
/*****************************************************************
	ミキサーボリュームの取得・設定
*****************************************************************/
MMRESULT GetMixerVolume(UINT uMxId, DWORD dwComponentType, DWORD *lpdwValue, DWORD *lpdwMinimum, DWORD *lpdwMaximum, DWORD *lpdwSteps);
MMRESULT SetMixerVolume(UINT uMxId, DWORD dwComponentType, DWORD dwValue);

BOOL GetSpeakersVolume(UINT uMxId, DWORD *lpdwValue, DWORD *lpdwMinimum, DWORD *lpdwMaximum, DWORD *lpdwSteps);
BOOL SetSpeakersVolume(UINT uMxId, DWORD dwValue);
BOOL GetWaveOutVolume(UINT uMxId, DWORD *lpdwValue, DWORD *lpdwMinimum, DWORD *lpdwMaximum, DWORD *lpdwSteps);
BOOL SetWaveOutVolume(UINT uMxId, DWORD dwValue);
BOOL GetSynthesizerVolume(UINT uMxId, DWORD *lpdwValue, DWORD *lpdwMinimum, DWORD *lpdwMaximum, DWORD *lpdwSteps);
BOOL SetSynthesizerVolume(UINT uMxId, DWORD dwValue);
BOOL GetCDAudioVolume(UINT uMxId, DWORD *lpdwValue, DWORD *lpdwMinimum, DWORD *lpdwMaximum, DWORD *lpdwSteps);
BOOL SetCDAudioVolume(UINT uMxId, DWORD dwValue);
BOOL GetDigitalVolume(UINT uMxId, DWORD *lpdwValue, DWORD *lpdwMinimum, DWORD *lpdwMaximum, DWORD *lpdwSteps);
BOOL SetDigitalVolume(UINT uMxId, DWORD dwValue);

BOOL GetSpeakersVolumeByLong(UINT uMxId, DWORD *lpdwValue);
BOOL SetSpeakersVolumeByLong(UINT uMxId, DWORD dwValue);
BOOL GetWaveOutVolumeByLong(UINT uMxId, DWORD *lpdwValue);
BOOL SetWaveOutVolumeByLong(UINT uMxId, DWORD dwValue);
BOOL GetSynthesizerVolumeByLong(UINT uMxId, DWORD *lpdwValue);
BOOL SetSynthesizerVolumeByLong(UINT uMxId, DWORD dwValue);
BOOL GetCDAudioVolumeByLong(UINT uMxId, DWORD *lpdwValue);
BOOL SetCDAudioVolumeByLong(UINT uMxId, DWORD dwValue);
BOOL GetDigitalVolumeByLong(UINT uMxId, DWORD *lpdwValue);
BOOL SetDigitalVolumeByLong(UINT uMxId, DWORD dwValue);

/*****************************************************************
	ミキサーオブジェクト
*****************************************************************/
class CMixerObj
{
public:
	DECLARE_SYNONYM(CMixerObj,TYPE);
	operator LPTYPE() { return this; }
protected:
	CMixerObj() { init(); }
	~CMixerObj() { destroy(); }
private:
	void init() { m_bOwner=TRUE; m_hmxobj=NULL; }
protected:
	void destroy() { init(); }
public:
	// テンポラリマップの作成
	void map(HMIXEROBJ h) { destroy(); m_hmxobj=h; }
public:
	// 各種操作
	MMRESULT SetControlDetails(LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails) { MMRESULT h=::mixerSetControlDetails(handle(),pmxcd,fdwDetails); CHECKRESULT_MM(h); return h; }
	MMRESULT GetControlDetails(LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails) { MMRESULT h=::mixerGetControlDetails(handle(),pmxcd,fdwDetails); CHECKRESULT_MM(h); return h; }
	MMRESULT GetID(UINT *puMxId, DWORD fdwId) { MMRESULT h=::mixerGetID(handle(),puMxId,fdwId); CHECKRESULT_MM(h); return h; }
	MMRESULT GetLineControls(LPMIXERLINECONTROLS pmxlc, DWORD fdwControls) { MMRESULT h=::mixerGetLineControls(handle(),pmxlc,fdwControls); CHECKRESULT_MM(h); return h; }
	MMRESULT GetLineInfo(LPMIXERLINE pmxl, DWORD fdwInfo) { MMRESULT h=::mixerGetLineInfo(handle(),pmxl,fdwInfo); CHECKRESULT_MM(h); return h; }
public:
	// 非オブジェクト操作
	static MMRESULT GetDevCaps(UINT uMxId, LPMIXERCAPS pmxcaps, UINT cbmxcaps) { MMRESULT h=::mixerGetDevCaps(uMxId,pmxcaps,cbmxcaps); CHECKRESULT_MM(h); return h; }
	static UINT mixerGetNumDevs() { return ::mixerGetNumDevs(); }
public:
	// クラス情報の取得/設定
	HMIXEROBJ handle() const { return m_hmxobj; }
	operator HMIXEROBJ() const { return handle(); }
	BOOL isValid() const { return !!handle(); }
	BOOL isOwner() const { return m_bOwner; }
	BOOL ownership(BOOL f) { return m_bOwner=f; }
protected:
	HMIXEROBJ handle(HMIXEROBJ h) { return m_hmxobj=h; }
private:
	BOOL m_bOwner;		// 所有権フラグ
	HMIXEROBJ m_hmxobj;	// ミキサーオブジェクトハンドル
};
/*****************************************************************
	ミキサー
*****************************************************************/
class CMixer : public CMixerObj
{
public:
	DECLARE_SYNONYM(CMixer,TYPE);
	DECLARE_SYNONYM(CMixerObj,BASE);
	operator LPTYPE() { return this; }
public:
	CMixer() { init(); }
	virtual ~CMixer() { destroy(); }
private:
	void init() {}
public:
	void destroy() { if(isOwner()&&handle()) close(); BASE::destroy(); init(); }
public:
	// オブジェクトの作成/削除
	MMRESULT open(UINT uMxId, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen);
	MMRESULT open(UINT uMxId, DWORD fdwOpen=0L);
	MMRESULT close();
	// テンポラリマップの作成
	void map(HMIXER hmx) { BASE::map((HMIXEROBJ)hmx); }
public:
	// 各種操作
	DWORD message(UINT uMsg, DWORD dwParam1, DWORD dwParam2);
protected:
	// ミキサーコールバック
	static BOOL DriverCallback(DWORD dwCallback, DWORD dwFlags, HDRVR hdrvr, DWORD msg, DWORD dwUser, DWORD dwParam1, DWORD dwParam2);
	virtual BOOL driverCallback(DWORD dwFlags, HDRVR hdrvr, DWORD msg, DWORD dwUser, DWORD dwParam1, DWORD dwParam2);
public:
	// クラス情報の取得/設定
	HMIXER handle() const { return (HMIXER)BASE::handle(); }
	operator HMIXER() const { return handle(); }
	BOOL isValid() const { return BASE::isValid(); }
	BOOL isOwner() const { return BASE::isOwner(); }
	BOOL ownership(BOOL f) { return BASE::ownership(f); }
};
/*****************************************************************
*****************************************************************/
_HMC_END
/*****************************************************************
*****************************************************************/

#endif	/* __INC_HMC_MIXER__ */

/* Local Variables:						*/
/* tab-width:4							*/
/* c-basic-offset:4						*/
/* c-indent-level:4						*/
/* c-label-offset:-4					*/
/* c-continued-statement-offset:4		*/
/* file-coding-system:*sjis*dos			*/
/* buffer-file-coding-system:sjis-dos	*/
/* End:									*/
