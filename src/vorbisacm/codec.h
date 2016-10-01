/*
	Ogg Vorbis CODEC for MSACM
	Copyright (C)2001 H.Mutsuki
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __INC_VORBISACM_CODEC__
#define __INC_VORBISACM_CODEC__

#include "hmc/util.h"
#include "hmc/mmutil.h"
#include "hmc/acmdrv.h"
#include <msacm.h>
#include <msacmdrv.h>	// from DDK
#pragma warning(push,1)
#include "vorbis/codec.h"
#pragma warning(pop)
#include "vorbisacm.h"

// ＊注意＊
// このファイルはvorbis.acmの実装用ヘッダです
// あなたのアプリケーションでvorbis.acmを使用するのに必要ではありません
// vorbis.acm関係の定数、構造体を必要とする場合vorbisacm.hをincludeしてください
// このファイルをincludeするとACM関係のAPIはUNICODE用に定義されます

/*****************************************************************
	Ogg Vorbis CODEC for MSACM
*****************************************************************/
/*****************************************************************
	VORBISACMのバージョン
*****************************************************************/
// このソースでは以下のlibVorbisを使用している
// libogg-1.0rc3.zip       31-Dec-2001 18:32   271k
// libvorbis-1.0rc3.zip    31-Dec-2001 18:33   990k
#define VORBISACM_VERSION			0x20020201
#define LIBVORBIS_VERSION			0x20011231

/*****************************************************************
	グローバル
*****************************************************************/
extern "C" LRESULT WINAPI DriverProc(DWORD dwId, HDRVR hdrvr, UINT uMsg, LPARAM lParam1, LPARAM lParam2);

/*****************************************************************
	色々、定数とか……
*****************************************************************/
struct FORMATDETAIL
{
	DWORD nSamplesPerSec;
	WORD  nChannels;
	WORD  wBitsPerSample;
	DWORD nAvgBytesPerSec;
	float flQuality;
};

extern const FORMATDETAIL aPCMFormatIndexToDetail[];
extern const FORMATDETAIL aOggFormatIndexToDetail[];

#ifdef _HAVE_DUMPEDFORMATDETAILS
extern const WAVEFORMATEX **aDumpedFormatDetails[];
#endif

/*****************************************************************
	ユーティリティ
*****************************************************************/
BOOL pcmIsValidFormat(CONST WAVEFORMATEX *pwfx);
BOOL oggIsValidFormat(CONST WAVEFORMATEX *pwfx);
DWORD oggAvgBytesPerSec(CONST WAVEFORMATEX *pwfx, DWORD mode);
DWORD oggFormatToIndex(CONST WAVEFORMATEX *pwfx);
DWORD oggFormatMakeupExtraByte(WAVEFORMATEX *pwfx);
DWORD GetLargestFormat(WAVEFORMATEX *pwfx);
DWORD FormatFormatDetail(WCHAR *szFormat, size_t cbBuf, const FORMATDETAIL *pDetail);

/*****************************************************************
	ストリームインスタンス基底クラス(今のところ何の意味も無い)
*****************************************************************/
#pragma warning(push)
#pragma warning(disable: 4355)
class COggVorbisStreamInstance
{
public:
	DECLARE_SYNONYM(COggVorbisStreamInstance,TYPE);
	DECLARE_SYNONYM(HMC::TLinkTree<TYPE>,LINK);
	operator LPTYPE() { return this; }
	COggVorbisStreamInstance() : m_link(this) {}
	virtual ~COggVorbisStreamInstance() {
		m_link.unlink();
	}
public:
	LINK m_link;
};
#pragma warning(pop)
/*****************************************************************
	vorbis.acmドライバ
*****************************************************************/
class CacmDriverOggVorbis : public HMC::CacmDriver
{
public:
	DECLARE_SYNONYM(CacmDriverOggVorbis,TYPE);
	DECLARE_SYNONYM(HMC::CacmDriver,BASE);
	operator LPTYPE() { return this; }
public:
	CacmDriverOggVorbis() { init(); }
	virtual ~CacmDriverOggVorbis() { destroy(); }
private:
	void init();
public:
	void destroy();
public:
	void *operator new(size_t dwBytes);
	void operator delete(void *pvMem);
public:
	static LRESULT CALLBACK DriverProc(DWORD dwId, HDRVR hdrvr, UINT uMsg, LPARAM lParam1, LPARAM lParam2);
private:
	enum {
		STREAMMODE_VORBIS_ENCODE  = 1,
		STREAMMODE_VORBIS_DECODE  = 2,
	};
private:
	virtual LRESULT dmConfigure(LPARAM lParam1, LPARAM lParam2);
	virtual LRESULT dmQueryConfigure(LPARAM lParam1, LPARAM lParam2);
	virtual LRESULT dmDriverDetails(LPARAM lParam1, LPARAM lParam2);
	virtual LRESULT dmDriverAbout(LPARAM lParam1, LPARAM lParam2);
	virtual LRESULT dmFormatSuggest(LPARAM lParam1, LPARAM lParam2);
	virtual LRESULT dmFormatTagDetails(LPARAM lParam1, LPARAM lParam2);
	virtual LRESULT dmFormatDetails(LPARAM lParam1, LPARAM lParam2);
	virtual LRESULT dmStreamOpen(LPARAM lParam1, LPARAM lParam2);
	virtual LRESULT dmStreamClose(LPARAM lParam1, LPARAM lParam2);
	virtual LRESULT dmStreamSize(LPARAM lParam1, LPARAM lParam2);
	virtual LRESULT dmStreamConvert(LPARAM lParam1, LPARAM lParam2);
};

/*****************************************************************
*****************************************************************/

#endif	/* __INC_VORBISACM_CODEC__ */

/* Local Variables:						*/
/* tab-width:4							*/
/* c-basic-offset:4						*/
/* c-indent-level:4						*/
/* c-label-offset:-4					*/
/* c-continued-statement-offset:4		*/
/* file-coding-system:*sjis*dos			*/
/* buffer-file-coding-system:sjis-dos	*/
/* End:									*/
