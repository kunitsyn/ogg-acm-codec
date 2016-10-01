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

// �����Ӂ�
// ���̃t�@�C����vorbis.acm�̎����p�w�b�_�ł�
// ���Ȃ��̃A�v���P�[�V������vorbis.acm���g�p����̂ɕK�v�ł͂���܂���
// vorbis.acm�֌W�̒萔�A�\���̂�K�v�Ƃ���ꍇvorbisacm.h��include���Ă�������
// ���̃t�@�C����include�����ACM�֌W��API��UNICODE�p�ɒ�`����܂�

/*****************************************************************
	Ogg Vorbis CODEC for MSACM
*****************************************************************/
/*****************************************************************
	VORBISACM�̃o�[�W����
*****************************************************************/
// ���̃\�[�X�ł͈ȉ���libVorbis���g�p���Ă���
// libogg-1.0rc3.zip       31-Dec-2001 18:32   271k
// libvorbis-1.0rc3.zip    31-Dec-2001 18:33   990k
#define VORBISACM_VERSION			0x20020201
#define LIBVORBIS_VERSION			0x20011231

/*****************************************************************
	�O���[�o��
*****************************************************************/
extern "C" LRESULT WINAPI DriverProc(DWORD dwId, HDRVR hdrvr, UINT uMsg, LPARAM lParam1, LPARAM lParam2);

/*****************************************************************
	�F�X�A�萔�Ƃ��c�c
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
	���[�e�B���e�B
*****************************************************************/
BOOL pcmIsValidFormat(CONST WAVEFORMATEX *pwfx);
BOOL oggIsValidFormat(CONST WAVEFORMATEX *pwfx);
DWORD oggAvgBytesPerSec(CONST WAVEFORMATEX *pwfx, DWORD mode);
DWORD oggFormatToIndex(CONST WAVEFORMATEX *pwfx);
DWORD oggFormatMakeupExtraByte(WAVEFORMATEX *pwfx);
DWORD GetLargestFormat(WAVEFORMATEX *pwfx);
DWORD FormatFormatDetail(WCHAR *szFormat, size_t cbBuf, const FORMATDETAIL *pDetail);

/*****************************************************************
	�X�g���[���C���X�^���X���N���X(���̂Ƃ��뉽�̈Ӗ�������)
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
	vorbis.acm�h���C�o
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
