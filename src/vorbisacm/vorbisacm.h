/*
	Ogg Vorbis CODEC for MSACM
	Copyright (C)2001 H.Mutsuki
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __INC_VORBISACM_VORBISACM__
#define __INC_VORBISACM_VORBISACM__

#include <mmreg.h>

/*****************************************************************
	MSACM�֌W�̒萔(VID,PID,WAVEFORMATEX::wFormatTag)
*****************************************************************/
#if 0
#define MM_HMS					MM_UNMAPPED
#define MM_HMS_VORBIS			MM_PID_UNMAPPED
#define WAVE_FORMAT_VORBIS		WAVE_FORMAT_DEVELOPMENT
#else
#define MM_HMS					0	// if 0 not registered
#define MM_HMS_VORBIS			0	// if 0 not registered
#define WAVE_FORMAT_VORBIS1		((WORD)'O'+((WORD)'g'<<8))	// 0x674f("Og") ... Original stream compatible
#define WAVE_FORMAT_VORBIS2		((WORD)'P'+((WORD)'g'<<8))	// 0x6750("Pg") ... Have independent header
#define WAVE_FORMAT_VORBIS3		((WORD)'Q'+((WORD)'g'<<8))	// 0x6751("Qg") ... Have no codebook header
#define WAVE_FORMAT_VORBIS1P	((WORD)'o'+((WORD)'g'<<8))	// 0x676f("og") ... Original stream compatible
#define WAVE_FORMAT_VORBIS2P	((WORD)'p'+((WORD)'g'<<8))	// 0x6770("pg") ... Have independent header
#define WAVE_FORMAT_VORBIS3P	((WORD)'q'+((WORD)'g'<<8))	// 0x6771("qg") ... Have no codebook header
#endif

/*****************************************************************
	WAVEFORMATEX�g��
*****************************************************************/
#pragma pack(push,2)
typedef struct ogg_waveformat_tag {
	WAVEFORMATEX wfx;
	DWORD dwVorbisACMVersion;		// vorbis.acm�̃o�[�W����
	DWORD dwLibVorbisVersion;		// libVorbis�̃o�[�W����
	//BYTE byExtra[];				// �s�蒷�̃G�N�X�g���o�C�g
} OGGWAVEFORMAT, *LPOGGWAVEFORMAT;
#pragma pack(pop)

// �G�N�X�g���o�C�g�̍ő咷
// ���w�b�_���i�[����̂ɏ\���ȃT�C�Y���K�v(�T�^�I�ɂ�2�`4KB�O��c�c�o�[�W�����ɂ���đ傫���قȂ�)
// ��acmFormatTagDetails(had,paftd,ACM_FORMATTAGDETAILSF_LARGESTSIZE)�Ŏ��ۂ̃T�C�Y���擾�ł���
#define OGGWAVEFORMATMAXEXTRABYTES	(64*1024-sizeof(WAVEFORMATEX)-2)

/*****************************************************************
*****************************************************************/

#endif	/* __INC_VORBISACM_VORBISACM__ */

/* Local Variables:						*/
/* tab-width:4							*/
/* c-basic-offset:4						*/
/* c-indent-level:4						*/
/* c-label-offset:-4					*/
/* c-continued-statement-offset:4		*/
/* file-coding-system:*sjis*dos			*/
/* buffer-file-coding-system:sjis-dos	*/
/* End:									*/
