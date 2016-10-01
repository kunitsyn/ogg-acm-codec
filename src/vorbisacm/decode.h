/*
	Ogg Vorbis CODEC for MSACM
	Copyright (C)2001 H.Mutsuki
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __INC_VORBISACM_DECODE__
#define __INC_VORBISACM_DECODE__

#include "hmc/util.h"
#pragma warning(push,1)
#include "vorbis/codec.h"
#pragma warning(pop)
#include "vorbisacm.h"

/*****************************************************************
	Ogg Vorbis CODEC for MSACM
*****************************************************************/
/*****************************************************************
	Ogg Vorbis �f�R�[�h��
*****************************************************************/
class COggVorbisDecode : public COggVorbisStreamInstance
{
public:
	DECLARE_SYNONYM(COggVorbisDecode,TYPE);
	DECLARE_SYNONYM(COggVorbisStreamInstance,BASE);
	operator LPTYPE() { return this; }
public:
	COggVorbisDecode(CacmDriverOggVorbis *pDriver) : pDriver(pDriver) { init(); }
	~COggVorbisDecode() { destroy(); }
private:
	void init();
public:
	void destroy();
public:
	LRESULT Create(CONST WAVEFORMATEX *pwfxSrc, CONST WAVEFORMATEX *pwfxDst);
	LRESULT ConvertCallback(LPACMDRVSTREAMINSTANCE padsi, LPACMDRVSTREAMHEADER padsh);
private:
	LRESULT PrepareHeader(LPACMDRVSTREAMINSTANCE padsi, LPACMDRVSTREAMHEADER padsh);
	DWORD ReadData (LPACMDRVSTREAMINSTANCE padsi, LPACMDRVSTREAMHEADER padsh);
	DWORD WriteData(LPACMDRVSTREAMINSTANCE padsi, LPACMDRVSTREAMHEADER padsh);
private:
	enum {
		READSIZE = 4*1024,
	};
	enum {
		DECODE_FLAGS_PSEUDOCBR		= 0x00000080,
		DECODE_FLAGS_MODE1			= 0x00000001,
		DECODE_FLAGS_MODE2			= 0x00000002,
		DECODE_FLAGS_MODE3			= 0x00000004,
		DECODE_FLAGS_HEADERPHASE1	= 0x10000000,
		DECODE_FLAGS_HEADERPHASE2	= 0x20000000,
		DECODE_FLAGS_HEADERPHASE3	= 0x40000000,
		DECODE_FLAGS_HEADERPREPARED	= DECODE_FLAGS_HEADERPHASE3,
		DECODE_FLAGS_EOS			= 0x08000000,
		DECODE_FLAGS_EOF			= 0x04000000,
	};
private:
	// �h���C�o�C���X�^���X
	CacmDriverOggVorbis *pDriver;
	// �R�[���o�b�N�����p
	DWORD dwFlags;				// �e��t���O
	DWORD cbSrcLengthUsed;		// PCM���̓o�b�t�@�����σo�C�g��
	DWORD cbDstLengthUsed;		// Bit�o�̓o�b�t�@�����σo�C�g��
	// �r�b�g���[�g����p
	float flCurBitrate;			// �u�ԃr�b�g���[�g
	float flMaxBitrate;			// �ő�r�b�g���[�g
	float flEstBitrate;			// �ώZ�r�b�g���[�g
	DWORD cbSrcPrepared;		// �������ϓ��̓o�C�g��
	DWORD cbDstPrepared;		// �������Ϗo�̓o�C�g��
	DWORD cbPadPrepared;		// ��������Pad�o�̓o�C�g��
	DWORD cbSrcLastPrepared;	// �������ϓ��̓o�C�g��
	DWORD cbDstLastPrepared;	// �������Ϗo�̓o�C�g��
	DWORD cbPadLastPrepared;	// ��������Pad�o�̓o�C�g��
	// OggVorbis�X�g���[�������p
	ogg_sync_state   oy;		// sync and verify incoming physical bitstream
	ogg_stream_state os;		// take physical pages, weld into a logical stream of packets
	ogg_page         og;		// one Ogg bitstream page.  Vorbis packets are inside
	ogg_packet       op;		// one raw packet of data for decode
	vorbis_info      vi;		// struct that stores all the static vorbis bitstream settings
	vorbis_comment   vc;		// struct that stores all the user comments
	vorbis_dsp_state vd;		// central working state for the packet->PCM decoder
	vorbis_block     vb;		// local working space for packet->PCM decode
};

/*****************************************************************
*****************************************************************/

#endif	/* __INC_VORBISACM_DECODE__ */

/* Local Variables:						*/
/* tab-width:4							*/
/* c-basic-offset:4						*/
/* c-indent-level:4						*/
/* c-label-offset:-4					*/
/* c-continued-statement-offset:4		*/
/* file-coding-system:*sjis*dos			*/
/* buffer-file-coding-system:sjis-dos	*/
/* End:									*/
