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
	MSACM関係の定数(VID,PID,WAVEFORMATEX::wFormatTag)
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
	WAVEFORMATEX拡張
*****************************************************************/
#pragma pack(push,2)
typedef struct ogg_waveformat_tag {
	WAVEFORMATEX wfx;
	DWORD dwVorbisACMVersion;		// vorbis.acmのバージョン
	DWORD dwLibVorbisVersion;		// libVorbisのバージョン
	//BYTE byExtra[];				// 不定長のエクストラバイト
} OGGWAVEFORMAT, *LPOGGWAVEFORMAT;
#pragma pack(pop)

// エクストラバイトの最大長
// ※ヘッダを格納するのに十分なサイズが必要(典型的には2～4KB前後……バージョンによって大きく異なる)
// ※acmFormatTagDetails(had,paftd,ACM_FORMATTAGDETAILSF_LARGESTSIZE)で実際のサイズを取得できる
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
