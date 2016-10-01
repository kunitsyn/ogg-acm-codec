/*
	mmsystem �T�|�[�g�N���X
	Copyright (C)1996-98 H.Mutsuki
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __INC_HMC_MMUTIL__
#define __INC_HMC_MMUTIL__

#include "util.h"
#include "com.h"
#include <mmsystem.h>
#include <mmreg.h>
#pragma comment(lib, "winmm.lib")

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
*****************************************************************/
#ifdef _DEBUG
#define CHECKRESULT_MM(exp)	(HMC::CmmSys::CheckResult((exp),__FILE__,__LINE__))
#else
#define CHECKRESULT_MM(exp)	(exp)
#endif
/*****************************************************************
*****************************************************************/
class CmmSys;
class CWaveFormatEx;
class CAudioParser;
class CAudioWriter;
/*****************************************************************
	MMSystem���[�e�B���e�B�N���X
*****************************************************************/
class CmmSys
{
public:
	// ���U���g�R�[�h����
	static MMRESULT CheckResult(MMRESULT dwResult, LPCTSTR szFileName, UINT nLineNumber);
	static void PrintResult(MMRESULT dwResult, LPCTSTR szFileName, UINT nLineNumber);
	static LPCTSTR GetErrorMessage(MMRESULT dwResult);
};

/*****************************************************************
	�O���[�o��
*****************************************************************/
// FOURCC�Ŏw�肳���`�����N�̃|�C���^�𓾂�
FOURCC *FindRIFFChunk(LPCVOID pvForm, FOURCC fcc);
FOURCC *GetRIFFNext(const FOURCC *pfcc);
DWORD GetRIFFLength(const FOURCC *pfcc);
DWORD GetRIFFWaveDataLength(LPCVOID pv);
LPCVOID GetRIFFWaveData(LPCVOID pv);

// �����f�[�^����������
LPVOID WriteMute(LPVOID dest, size_t count, LPWAVEFORMATEX pwfx);
LPVOID WriteMute(LPVOID dest, size_t count, WORD nChannels, WORD wBitsPerSample);

// db<->Percent�ϊ�(���l�\���ɒ���)
LONG db2perByLong(LONG lValue);		// 100db=10000,100% =10000
LONG per2dbByLong(LONG lValue);		// 100% =10000,100db=10000
double db2per(double dValue);	// 100db=100.0,100% =1.000
double per2db(double dValue);	// 100% =1.000,100db=100.0

// �F��ԕϊ�(ColorFaq���Q�Ƃ̂���)
// RGB<->CMY�ϊ�
void rgb2cmy(double r, double g, double b, double &c, double &m, double &y);
void cmy2rgb(double c, double m, double y, double &r, double &g, double &b);
// CMY<->CMYK�ϊ�
void cmy2cmyk(double c, double m, double y, double &C, double &M, double &Y, double &k);
void cmyk2cmy(double c, double m, double y, double k, double &C, double &M, double &Y);
// RGB<->CMYK�ϊ�
void rgb2cmyk(double r, double g, double b, double &c, double &m, double &y, double &k);
void cmyk2rgb(double c, double m, double y, double k, double &r, double &g, double &b);
// RGB<->HSL�ϊ�
void rgb2hsl(double r, double g, double b, double &h, double &s, double &l);
void hsl2rgb(double h, double s, double l, double &r, double &g, double &b);
// RGB<->CIE XYZ�ϊ�
void rgb2xyz(double r, double g, double b, double &x, double &y, double &z);
void xyz2rgb(double x, double y, double z, double &r, double &g, double &b);
void rgb2xyz709(double r, double g, double b, double &x, double &y, double &z);
void xyz7092rgb(double x, double y, double z, double &r, double &g, double &b);
void rgb2itu(double r, double g, double b, double &x, double &y, double &z);
void itu2rgb(double x, double y, double z, double &r, double &g, double &b);
// CIE XYZ<->CIE Luv�ϊ�
void xyz2luv(double x, double y, double z, double &l, double &u, double &v);
void luv2xyz(double l, double u, double v, double &x, double &y, double &z);
// CIE XYZ<->CIE Lab�ϊ�
void xyz2lab(double x, double y, double z, double &l, double &a, double &b);
void lab2xyz(double l, double a, double b, double &x, double &y, double &z);
// CIE Lab<->LCH�ϊ�
void lab2lch(double l, double a, double b, double &L, double &c, double &h);
void lch2lab(double l, double c, double h, double &L, double &a, double &b);	// �s��
// RGB<->YUV
void rgb2yuv(double r, double g, double b, double &y, double &u, double &v);
void yuv2rgb(double y, double u, double v, double &r, double &g, double &b);
// RGB<->YIQ
void rgb2yiq(double r, double g, double b, double &y, double &i, double &q);
void yiq2rgb(double y, double i, double q, double &r, double &g, double &b);
// YUV<->YIQ
void yuv2yiq(double y, double u, double v, double &Y, double &i, double &q);
void yiq2yuv(double y, double i, double q, double &Y, double &u, double &v);
// RGB<->YCbCr(with Rec 709 specs, ITU)
void rgb2ycbcr(double r, double g, double b, double &y, double &cb, double &cr);
void ycbcr2rgb(double y, double cb, double cr, double &r, double &g, double &b);
// RGB<->YCbCr(with Rec 601-1 specs ... TIFF and JPEG works)
void rgb2ycbcr601(double r, double g, double b, double &y, double &cb, double &cr);
void ycbcr2rgb601(double y, double cb, double cr, double &r, double &g, double &b);
// SMPTE-C RGB<->CIE XYZ(D65)
void rgb2xyzd65(double r, double g, double b, double &x, double &y, double &z);		// Gamma�͂ǂ�����H
void xyzd652rgb(double x, double y, double z, double &r, double &g, double &b);		// Gamma�͂ǂ�����H
// SMPTE-240M YPbPr<->RGB
void rgb2ypbpr(double r, double g, double b, double &y, double &pb, double &pr);	// Gamma�͂ǂ�����H
void ypbpr2rgb(double y, double pb, double pr, double &r, double &g, double &b);	// Gamma�͂ǂ�����H
// Xerox Corporation YES<->CIE XYZ(D50)
void xyzd502yes(double x, double y, double z, double &Y, double &e, double &s);		// Gamma�͂ǂ�����H
void yes2xyzd50(double y, double e, double s, double &x, double &Y, double &z);		// Gamma�͂ǂ�����H
// Xerox Corporation YES<->CIE XYZ(D65)
void xyzd652yes(double x, double y, double z, double &Y, double &e, double &s);		// Gamma�͂ǂ�����H
void yes2xyzd65(double y, double e, double s, double &x, double &Y, double &z);		// Gamma�͂ǂ�����H
// Kodak Photo CD YCC<->RGB
void rgb2ycc(double r, double g, double b, double &y, double &c1, double &c2);		// Gamma�͂ǂ�����H
void ycc2rgb(double y, double c1, double c2, double &r, double &g, double &b);		// Gamma�͂ǂ�����H

/*****************************************************************
	MMREG.H�ɋL�q����Ȃ��`�����N�\�� (��New Wave RIFF Chunks��蔲��)
*****************************************************************/
#pragma pack(push,4)
/*****************************************************************
	�L���[�|�C���g
*****************************************************************/
// �L���[�w�b�_
typedef struct tag_s_RIFFWAVE_cue_head {
	DWORD dwCuePoints;			// �L���[�|�C���g��
} s_RIFFWAVE_cue_head, s_RIFFWAVE_cue_head, tWaveCueHead;

// �L���[�|�C���g
typedef struct tag_s_RIFFWAVE_cue_point {
	DWORD dwName;				// ���j�[�N�ȃL���[��(org:1)
	DWORD dwPosition;			// �J�n�T���v��
	FOURCC fccChunk;			// cue �̎w�������`�����N(�K��'data')
	DWORD dwChunkStart;			// �`�����N�̊J�n�T���v��(�K��0)
	DWORD dwBlockStart;			// �u���b�N�J�n�o�C�g
	DWORD dwSampleOffset;		// �I�t�Z�b�g�o�C�g
} s_RIFFWAVE_cue_point, tWaveCuePoint;

// �L���[�|�C���g�z��
typedef struct tag_s_RIFFWAVE_cue_info {
	tWaveCueHead	head;
	tWaveCuePoint	sect[1];
} s_RIFFWAVE_cue_info, tWaveCueInfo;

/*****************************************************************
	�v���C���X�g
*****************************************************************/
// �Z�O�����g�w�b�_
typedef struct tag_s_RIFFWAVE_plst_head {
	DWORD dwSegments;			// �v���C�Z�O�����g��
} s_RIFFWAVE_plst_head, s_RIFFWAVE_plst_head, tWavePlstHead;

// �Z�O�����g
typedef struct tag_s_RIFFWAVE_plst_segment {
	DWORD dwName;				// cue�`�����N�ԍ�(org:1)
	DWORD dwLength;				// �T���v����
	DWORD dwLoops;				// ���[�v��(0 = skip)
} s_RIFFWAVE_plst_segment, tWavePlstSegment;

// �Z�O�����g�z��
typedef struct tag_s_RIFFWAVE_plst_info {
	tWavePlstHead		head;
	tWavePlstSegment	sect[1];
} s_RIFFWAVE_plst_info, tWavePlstInfo;

/*****************************************************************
	�T���v���[�C���t�H���[�V����
*****************************************************************/
// �T���v���[���
typedef struct tag_s_RIFFWAVE_smpl_head {
	DWORD dwManufacturer;		// ������ID
	DWORD dwProduct;			// ���iID
	DWORD dwSamplePeriod;		// 1/nSamplePerSec(�Œ菬��)
	DWORD dwMIDIUnityNote;		// MIDI�Đ��s�b�`
	DWORD dwMIDIPitchFraction;	// MIDI�`���[�j���O�f�[�^
	DWORD dwSMPTEFormat;		// SMTPE�t�H�[�}�b�g�^�C�v
	DWORD dwSMPTEOffset;		// SMTPE�����I�t�Z�b�g
	DWORD cSampleLoops;			// �T���v�����[�v��
	DWORD cbSamplerData;		// �T���v���[�ŗL�f�[�^�o�C�g��
} s_RIFFWAVE_smpl_head, s_RIFFWAVE_smpl_head, tWaveSmplHead;

// �T���v���[���[�v
typedef struct tag_s_RIFFWAVE_smpl_loop {
	DWORD dwIdentifier;			// cue�`�����N�ԍ�
	DWORD dwType;				// ���[�v�^�C�v
	DWORD dwStart;				// �J�n�T���v��
	DWORD dwEnd;				// �I���T���v��
	DWORD dwFraction;			// �`���[�j���O�f�[�^
	DWORD dwPlayCount;			// ���[�v��(0 = ��)
} s_RIFFWAVE_smpl_loop, tWaveSmplLoop;

// �T���v���[���ƃ��[�v�z��
typedef struct tag_s_RIFFWAVE_smpl_info {
	tWaveSmplHead	head;
	tWaveSmplLoop	sect[1];
} s_RIFFWAVE_smpl_info, tWaveSmplInfo;

/*****************************************************************
*****************************************************************/
#pragma pack(pop)

/*****************************************************************
	�`�����N�֌W�̍\���̃A�N�Z�X�֐�
*****************************************************************/
// �e���\���̂̃A�h���X�𓾂�
tWaveCuePoint    *GetCuePoint   (tWaveCueInfo  *pi, DWORD dwSection);
tWavePlstSegment *GetPlaySegment(tWavePlstInfo *pi, DWORD dwSection);
tWaveSmplLoop    *GetSamplerLoop(tWaveSmplInfo *pi, DWORD dwSection);

// �e���\���̂̃G���g���[���𓾂�
inline DWORD GetCuePoint(const tWaveCueInfo *pi)
{
	return pi ? pi->head.dwCuePoints : 0;
}
inline DWORD GetPlaySegment(const tWavePlstInfo *pi)
{
	return pi ? pi->head.dwSegments  : 0;
}
inline DWORD GetSamplerLoop(const tWaveSmplInfo *pi)
{
	return pi ? pi->head.cSampleLoops: 0;
}

/*****************************************************************
	WAVEFORMATEX�֌W(�C�����C���֐�)
*****************************************************************/
// WAVEFORMATEX����T���v�����̃o�C�g���𓾂�
inline WORD GetBytesPerSample(CONST WAVEFORMATEX *pwfx)
{
	return pwfx->nChannels * pwfx->wBitsPerSample / 8;
}
// WAVEFORMATEX����b�ԃo�C�g���𓾂�
inline DWORD GetBytesPerSec(CONST WAVEFORMATEX *pwfx)
{
	return pwfx->nSamplesPerSec * GetBytesPerSample(pwfx);
}
inline DWORD GetAvgBytesPerSec(CONST WAVEFORMATEX *pwfx)
{
	DWORD n = pwfx->nAvgBytesPerSec;
	n = 1<n ? n : HMC::GetBytesPerSec(pwfx);
	n = ALIGN(n,max(1,pwfx->nBlockAlign));
	return n;
}
// WAVEFORMATEX����b�ԃr�b�g���𓾂�
inline DWORD GetBitsPerSec(CONST WAVEFORMATEX *pwfx)
{
	return pwfx->nChannels * pwfx->nSamplesPerSec * pwfx->wBitsPerSample;
}
// WAVEFORMATEX�̃T�C�Y�𓾂�
inline size_t GetFormatSize(CONST WAVEFORMATEX *pwfx)
{
	size_t size = sizeof(WAVEFORMATEX);
	if(pwfx->wFormatTag == WAVE_FORMAT_PCM) {
		size -= sizeof(pwfx->cbSize);
	} else {
		size += pwfx->cbSize;
	}
	return size;
}

/*****************************************************************
	WAVEFORMATEX ���b�p�[(��cbSize�ɂ͑Ή����Ă��Ȃ�)
*****************************************************************/
class CWaveFormatEx : public WAVEFORMATEX
{
public:
	CWaveFormatEx() { memset(this, 0, sizeof(*this)); }
	CWaveFormatEx(const LPWAVEFORMATEX pwfx) { let(pwfx); }
	CWaveFormatEx(const WAVEFORMATEX &wfx) { let((LPWAVEFORMATEX)&wfx); }
	CWaveFormatEx& operator=(const WAVEFORMATEX &t) {
		if(&t!=this) {
			memcpy(this, &t, GetFormatSize());
		}
		return *this;
	}
	CWaveFormatEx(WORD wFormatTag, WORD nChannels, DWORD nSamplesPerSec, DWORD nAvgBytesPerSec, WORD nBlockAlign, WORD wBitsPerSample, WORD cbSize) { let(wFormatTag,nChannels,nSamplesPerSec,nAvgBytesPerSec,nBlockAlign,wBitsPerSample,cbSize); }
	CWaveFormatEx(DWORD nSamplesPerSec, WORD wBitsPerSample, WORD nChannels) { let(nSamplesPerSec,wBitsPerSample,nChannels); }
public:
	void let(const LPWAVEFORMATEX pwfx) {
		memcpy(this, pwfx, sizeof(*this));
	}
	void let(WORD wFormatTag, WORD nChannels, DWORD nSamplesPerSec, DWORD nAvgBytesPerSec, WORD nBlockAlign, WORD wBitsPerSample, WORD cbSize) {
		this->wFormatTag		= wFormatTag;
		this->nChannels			= nChannels;
		this->nSamplesPerSec	= nSamplesPerSec;
		this->nAvgBytesPerSec	= nAvgBytesPerSec;
		this->nBlockAlign		= nBlockAlign;
		this->wBitsPerSample	= wBitsPerSample;
		this->cbSize			= cbSize;
	}
	void let(DWORD nSamplesPerSec, WORD wBitsPerSample, WORD nChannels) {	// �������̏��Ԃɒ���
		this->wFormatTag		= WAVE_FORMAT_PCM;
		this->nChannels			= nChannels;
		this->nSamplesPerSec	= nSamplesPerSec;
		this->wBitsPerSample	= wBitsPerSample;
		this->nAvgBytesPerSec	= GetBytesPerSec();
		this->nBlockAlign		= GetBytesPerSample();
		this->cbSize			= 0;
	}
public:
	WORD  GetBytesPerSample() const { return HMC::GetBytesPerSample(this); }
	DWORD GetBytesPerSec()	  const { return HMC::GetBytesPerSec(this);    }
	DWORD GetBitsPerSec()     const { return HMC::GetBitsPerSec(this);     }
	DWORD GetFormatSize()     const { return HMC::GetFormatSize(this);     }
	bool operator==(WAVEFORMATEX &t) const {
		return !memcmp(this, &t, GetFormatSize());
	}
};

/*****************************************************************
	CAudioParser
*****************************************************************/
class CAudioParser : public CDelegateStream
{
public:
	DECLARE_SYNONYM(CAudioParser,TYPE);
	DECLARE_SYNONYM(CDelegateStream,BASE);
	operator LPTYPE() { return this; }
public:
	// CAudioParser�̒ǉ��C���^�[�t�F�[�X
	virtual HRESULT STDMETHODCALLTYPE GetFormat(LPWAVEFORMATEX lpwfx) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetMediaLength(LPDWORD lpcbSize) = 0;
};

/*****************************************************************
	CAudioWriter
*****************************************************************/
class CAudioWriter : public CDelegateStream
{
public:
	DECLARE_SYNONYM(CAudioWriter,TYPE);
	DECLARE_SYNONYM(CDelegateStream,BASE);
	operator LPTYPE() { return this; }
public:
	// CAudioWriter�̒ǉ��C���^�[�t�F�[�X
	virtual HRESULT STDMETHODCALLTYPE GetFormat(LPWAVEFORMATEX lpwfx) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetFormat(const LPWAVEFORMATEX lpwfx) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetMediaLength(LPDWORD lpcbSize) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetMediaLength(DWORD cbSize) = 0;
};

/*****************************************************************
*****************************************************************/
_HMC_END
/*****************************************************************
*****************************************************************/

#endif	/* __INC_HMC_MMUTIL__ */

/* Local Variables:						*/
/* tab-width:4							*/
/* c-basic-offset:4						*/
/* c-indent-level:4						*/
/* c-label-offset:-4					*/
/* c-continued-statement-offset:4		*/
/* file-coding-system:*sjis*dos			*/
/* buffer-file-coding-system:sjis-dos	*/
/* End:									*/
