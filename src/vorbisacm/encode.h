/*
	Ogg Vorbis CODEC for MSACM
	Copyright (C)2001 H.Mutsuki
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __INC_VORBISACM_ENCODE__
#define __INC_VORBISACM_ENCODE__

#include "hmc/util.h"
#pragma warning(push,1)
#include "vorbis/codec.h"
#pragma warning(pop)
#include "vorbisacm.h"

/*****************************************************************
	Ogg Vorbis CODEC for MSACM
*****************************************************************/
/*****************************************************************
	Ogg Vorbis エンコード部
*****************************************************************/
class COggVorbisEncode : public COggVorbisStreamInstance
{
public:
	DECLARE_SYNONYM(COggVorbisEncode,TYPE);
	DECLARE_SYNONYM(COggVorbisStreamInstance,BASE);
	operator LPTYPE() { return this; }
	friend DWORD oggFormatMakeupExtraByte(WAVEFORMATEX *pwfx);	// 恥！！
public:
	COggVorbisEncode(CacmDriverOggVorbis *pDriver) : pDriver(pDriver) { init(); }
	~COggVorbisEncode() { destroy(); }
private:
	void init();
public:
	void destroy();
public:
	LRESULT Create(CONST WAVEFORMATEX *pwfxSrc, CONST WAVEFORMATEX *pwfxDst);
	LRESULT ConvertCallback(LPACMDRVSTREAMINSTANCE padsi, LPACMDRVSTREAMHEADER padsh);
private:
	DWORD ReadData (LPACMDRVSTREAMINSTANCE padsi, LPACMDRVSTREAMHEADER padsh);
	DWORD WritePage(LPACMDRVSTREAMINSTANCE padsi, LPACMDRVSTREAMHEADER padsh);
private:
	enum {
		READSIZE = 1024,
	};
	enum {
		STREAMSERIALNO_AUDIO    = 0,	// オーディオストリームのシリアルナンバー
		STREAMSERIALNO_PADDING  = -1,	// パディングストリームのシリアルナンバー
	};
	enum {
		ENCODE_FLAGS_PSEUDOCBR		= 0x00000080,
		ENCODE_FLAGS_RATEMANAGED	= 0x00000040,
		ENCODE_FLAGS_MODE1			= 0x00000001,
		ENCODE_FLAGS_MODE2			= 0x00000002,
		ENCODE_FLAGS_MODE3			= 0x00000004,
		ENCODE_FLAGS_HEADERPREPARED	= 0x80000000,
		ENCODE_FLAGS_EOS			= 0x08000000,
		ENCODE_FLAGS_EOF			= 0x04000000,
	};
private:
	// ドライバインスタンス
	CacmDriverOggVorbis *pDriver;
	// コールバック処理用
	DWORD dwFlags;				// 各種フラグ
	DWORD cbSrcLengthUsed;		// PCM入力バッファ処理済バイト数
	DWORD cbDstLengthUsed;		// Bit出力バッファ処理済バイト数
	// ビットレート測定用
	float flCurBitrate;			// 瞬間ビットレート
	float flMaxBitrate;			// 最大ビットレート
	float flEstBitrate;			// 積算ビットレート
	DWORD cbSrcPrepared;		// 総処理済入力バイト数
	DWORD cbDstPrepared;		// 総処理済出力バイト数
	DWORD cbPadPrepared;		// 総処理済Pad出力バイト数
	DWORD cbSrcLastPrepared;	// 総処理済入力バイト数
	DWORD cbDstLastPrepared;	// 総処理済出力バイト数
	DWORD cbPadLastPrepared;	// 総処理済Pad出力バイト数
	// パディング処理用
	ogg_stream_state os_pad;	// パディング用ストリーム状態
	ogg_packet       op_pad;	// パディング用パケット
	// OggVorbisストリーム処理用
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

#endif	/* __INC_VORBISACM_ENCODE__ */

/* Local Variables:						*/
/* tab-width:4							*/
/* c-basic-offset:4						*/
/* c-indent-level:4						*/
/* c-label-offset:-4					*/
/* c-continued-statement-offset:4		*/
/* file-coding-system:*sjis*dos			*/
/* buffer-file-coding-system:sjis-dos	*/
/* End:									*/
