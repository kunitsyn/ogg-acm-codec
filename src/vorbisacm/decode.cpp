/*
	Ogg Vorbis CODEC for MSACM
	Copyright (C)2001 H.Mutsuki
*/

#include "hmc/util.h"
#include "hmc/mmutil.h"
#include "codec.h"
#include "decode.h"
#include "resource.h"

#pragma warning(push,1)
#include "vorbis/codec.h"
#include "vorbis/vorbisenc.h"
#pragma warning(pop)

#ifndef _DEBUG
#ifdef RELEASEDEBUG
#undef TRACE
#define TRACE HMC::Trace
#endif
#endif

/*****************************************************************
	Ogg Vorbis for MSACM
*****************************************************************/
/*****************************************************************
	Ogg Vorbis デコード部インスタンス
*****************************************************************/
void COggVorbisDecode::init()
{
	// 悪い初期化の見本！(よい子は真似しないでね)
	size_t n = offsetof(TYPE,pDriver) + sizeof(pDriver);
	memset((LPBYTE)this+n,0,sizeof(*this)-n);
}
void COggVorbisDecode::destroy()
{
	ogg_stream_clear(&os);
	vorbis_block_clear(&vb);
	vorbis_dsp_clear(&vd);
	vorbis_comment_clear(&vc);
	vorbis_info_clear(&vi);
	ogg_sync_clear(&oy);
	init();
}
LRESULT COggVorbisDecode::Create(CONST WAVEFORMATEX *pwfxSrc, CONST WAVEFORMATEX *pwfxDst)
{
	LRESULT lRes = MMSYSERR_NOERROR;
	destroy();
	if(pwfxDst->wFormatTag==WAVE_FORMAT_VORBIS1P ||
	   pwfxDst->wFormatTag==WAVE_FORMAT_VORBIS2P ||
	   pwfxDst->wFormatTag==WAVE_FORMAT_VORBIS3P) {
		dwFlags |= DECODE_FLAGS_PSEUDOCBR;
	}
	if(pwfxSrc->wFormatTag==WAVE_FORMAT_VORBIS1 ||
	   pwfxSrc->wFormatTag==WAVE_FORMAT_VORBIS1P) {
		dwFlags |= DECODE_FLAGS_MODE1;
	}
	if(pwfxSrc->wFormatTag==WAVE_FORMAT_VORBIS2 ||
	   pwfxSrc->wFormatTag==WAVE_FORMAT_VORBIS2P) {
		dwFlags |= DECODE_FLAGS_MODE2;
	}
	if(pwfxSrc->wFormatTag==WAVE_FORMAT_VORBIS3 ||
	   pwfxSrc->wFormatTag==WAVE_FORMAT_VORBIS3P) {
		dwFlags |= DECODE_FLAGS_MODE3;
	}
	if(lRes!=MMSYSERR_NOERROR) {
		destroy();
	}
	return lRes;
}
/*****************************************************************
	変換コールバック
*****************************************************************/
LRESULT COggVorbisDecode::ConvertCallback(LPACMDRVSTREAMINSTANCE padsi, LPACMDRVSTREAMHEADER padsh)
{
	LRESULT lRes = MMSYSERR_NOERROR;

	// ヘッダ読み込み処理
	if(!(dwFlags & DECODE_FLAGS_HEADERPREPARED)) {
		lRes = PrepareHeader(padsi,padsh);
		if(!(dwFlags & DECODE_FLAGS_HEADERPREPARED)) {
			goto end;
		}
		cbSrcPrepared = 0;	// ヘッダ分は含めない
#ifdef _DEBUG
		// コメント抽出
		char **ptr=vc.user_comments;
		while(*ptr){
			TRACE("%s\n",*ptr);
			++ptr;
		}
		TRACE("\nBitstream is %d channel, %ldHz\n",vi.channels,vi.rate);
		TRACE("Encoded by: %s\n\n",vc.vendor);
#endif
	}

	// 読み込み、解析、書き出しループ
	while(!(dwFlags&DECODE_FLAGS_EOS)) {
		int result = 0;
		if(0<cbDstLengthUsed) {
			TRACE("Continue Writing: %u/%u\n",cbDstLengthUsed,0);
			goto continue_write;
		}
		if(0<cbSrcLengthUsed) {
			TRACE("Continue Reading: %u/%u\n",cbSrcLengthUsed,0);
			goto continue_read;
		}
		if(dwFlags & DECODE_FLAGS_EOF) {
			dwFlags |= DECODE_FLAGS_EOS;
		}
		result = ogg_sync_pageout(&oy,&og);
		if(result!=1) {
		continue_read:
			DWORD cbRead = ReadData(padsi,padsh);
			if(cbRead==0 && padsh->cbSrcLength==padsh->cbSrcLengthUsed) {
				if(padsh->fdwConvert & ACM_STREAMCONVERTF_END) {
					dwFlags |= DECODE_FLAGS_EOF;
				} else {
					goto carry_read;		// 読み込み持ち越し
				}
			}
			if(result<0) {
				//※mode2,3の先頭で発生する可能性があるがそれは予期したエラーなので無視してよい
				if(!(dwFlags&(DECODE_FLAGS_MODE2|DECODE_FLAGS_MODE3) && (long)(vd.sequence)==-1)) {
					TRACE("Corrupt or missing data in bitstream (frame %ld); continuing...\n", (long)(vd.sequence));
				}
			}
			continue;
		}
		ogg_stream_pagein(&os,&og);	// can safely ignore errors at this point
		while(1) {
			result = ogg_stream_packetout(&os,&op);
			if(result==0) {
				break;		// need more data
			}
			if(result<0) {
				continue;	// missing or corrupt data at this page position
			}
			if(vorbis_synthesis(&vb,&op)==0) {
				vorbis_synthesis_blockin(&vd,&vb);
			}
		continue_write:
			while(1) {		// 単一ページRAWPCM展開
				DWORD cbWrote = WriteData(padsi,padsh);
				if(padsh->cbDstLength==padsh->cbDstLengthUsed) {
					goto carry_write;
				}
				if(cbWrote==0) {
					break;
				}
			}
		}
	}

 carry_read:
 carry_write:
	// 入力消費の最後のチャンス
	// 強制的に入力を完全消費
	// ※誤終了防止用…しかし余分にメモリを消費する
	// 場合によっては、積算的にメモリ消費量が増加することがある……
	while(padsh->cbSrcLengthUsed < padsh->cbSrcLength) {
		ReadData(padsi,padsh);
	}
 end:
	// 各種オーバーラン、アンダーランチェック
	ASSERT(padsh->cbSrcLengthUsed <= padsh->cbSrcLength);
	ASSERT(padsh->cbDstLengthUsed <= padsh->cbDstLength);
	// 入力が完全に消費されないと残データがドロップされる場合がある
	if(padsh->cbSrcLengthUsed < padsh->cbSrcLength) {
		TRACE("Input data may be dropped %u[bytes] (frame %ld)\n",padsh->cbSrcLength-padsh->cbSrcLengthUsed,(long)(vd.sequence));
	}
	// 出力が０バイトの場合ストリームの終了とみなされる場合がある
	if(0<padsh->cbSrcLength && 0==padsh->cbDstLengthUsed) {
		TRACE("Output data may be dropped (frame %ld)\n",(long)(vd.sequence));
	}

	// ビットレート測定用データ更新
#if defined(_DEBUG) || defined(RELEASEDEBUG)
	float flLastBitrate = flCurBitrate;
	flEstBitrate = (float)((LONG64)cbSrcPrepared*HMC::GetAvgBytesPerSec(padsi->pwfxDst)*8) / max(1,cbDstPrepared) / 1000;
	flCurBitrate = (float)((LONG64)(cbSrcPrepared-cbSrcLastPrepared)*HMC::GetAvgBytesPerSec(padsi->pwfxDst)*8) / max(1,cbDstPrepared-cbDstLastPrepared) / 1000;
	flMaxBitrate = max(flMaxBitrate,flCurBitrate);
	cbSrcLastPrepared = cbSrcPrepared;
	cbDstLastPrepared = cbDstPrepared;
	cbPadLastPrepared = cbPadPrepared;
#endif
	TRACE("Converting: %u/%u/%u, Est:%.2f, Cur:%.2f, Max:%.2f\n",cbSrcPrepared,cbDstPrepared,cbPadPrepared,flEstBitrate,flCurBitrate,flMaxBitrate);
	return lRes;
}
/*****************************************************************
	ヘッダ読み込み、検証
*****************************************************************/
LRESULT COggVorbisDecode::PrepareHeader(LPACMDRVSTREAMINSTANCE padsi, LPACMDRVSTREAMHEADER padsh)
{
	LRESULT lRes = MMSYSERR_NOERROR;
	// ヘッダ読み込み処理(phase1)
	if(!(dwFlags & DECODE_FLAGS_HEADERPHASE1)) {
		ogg_sync_init(&oy);
		if(dwFlags & DECODE_FLAGS_MODE1) {
			ReadData(padsi,padsh);
		} else if(dwFlags & DECODE_FLAGS_MODE2) {
			WAVEFORMATEX *pwfx = padsi->pwfxSrc;
			DWORD cbSize = HMC::GetFormatSize(pwfx)-sizeof(OGGWAVEFORMAT);
			LPBYTE pbDst = (LPBYTE)ogg_sync_buffer(&oy,cbSize);
			memcpy(pbDst,(LPBYTE)pwfx+sizeof(OGGWAVEFORMAT),cbSize);
			ogg_sync_wrote(&oy,cbSize);
		} else if(dwFlags & DECODE_FLAGS_MODE3) {
			WAVEFORMATEX *pwfx = (WAVEFORMATEX *)alloca(sizeof(WAVEFORMATEX)+OGGWAVEFORMATMAXEXTRABYTES);
			memset(pwfx,0,sizeof(WAVEFORMATEX)+OGGWAVEFORMATMAXEXTRABYTES);
			memcpy(pwfx,padsi->pwfxSrc,sizeof(OGGWAVEFORMAT));
			if(dwFlags & DECODE_FLAGS_PSEUDOCBR) {
				pwfx->wFormatTag = WAVE_FORMAT_VORBIS2P;
			} else {
				pwfx->wFormatTag = WAVE_FORMAT_VORBIS2;
			}
			pwfx->cbSize = OGGWAVEFORMATMAXEXTRABYTES;
			oggFormatMakeupExtraByte(pwfx);
			DWORD cbSize = HMC::GetFormatSize(pwfx)-sizeof(OGGWAVEFORMAT);
			LPBYTE pbDst = (LPBYTE)ogg_sync_buffer(&oy,cbSize);
			memcpy(pbDst,(LPBYTE)pwfx+sizeof(OGGWAVEFORMAT),cbSize);
			ogg_sync_wrote(&oy,cbSize);
		}
		if(ogg_sync_pageout(&oy,&og)!=1) {
			if(padsh->cbSrcLength==padsh->cbSrcLengthUsed) {
				goto carry_read;	// 読み込み持ち越し(多分巧くリカバリできない……)
			}
			TRACE("Must not be Vorbis data\n");
			lRes = ACMERR_NOTPOSSIBLE;
			goto end;
		}
		ogg_stream_init(&os,ogg_page_serialno(&og));
		vorbis_info_init(&vi);
		vorbis_comment_init(&vc);
		if(ogg_stream_pagein(&os,&og)<0) {
			TRACE("Error reading first page of Ogg bitstream data.\nStream version mismatch perhaps.\n");
			lRes = ACMERR_NOTPOSSIBLE;
			goto end;
		}
		if(ogg_stream_packetout(&os,&op)!=1) {
			TRACE("Error reading initial header packet.\nno page? must not be vorbis\n");
			lRes = ACMERR_NOTPOSSIBLE;
			goto end;
		}
		if(vorbis_synthesis_headerin(&vi,&vc,&op)<0) {
			TRACE("This Ogg bitstream does not contain Vorbis audio data.\nerror case; not a vorbis header\n");
			lRes = ACMERR_NOTPOSSIBLE;
			goto end;
		}
		dwFlags |= DECODE_FLAGS_HEADERPHASE1;
	}
	// ヘッダ読み込み処理(phase2,3)
	while(!(dwFlags & DECODE_FLAGS_HEADERPREPARED)) {
		while(!(dwFlags & DECODE_FLAGS_HEADERPREPARED)) {
			int result = 0;
			if(0<cbSrcLengthUsed) {
				TRACE("Continue Reading: %u/%u\n",cbSrcLengthUsed,0);
				goto continue_read;
			}
			result = ogg_sync_pageout(&oy,&og);
			if(result==0) {
				if(padsh->cbSrcLength==padsh->cbSrcLengthUsed) {
					goto carry_read;
				}
			continue_read:
				ReadData(padsi,padsh);
				continue;
			}
			if(result==1) {
				ogg_stream_pagein(&os,&og);
				while(!(dwFlags & DECODE_FLAGS_HEADERPREPARED)) {
					result = ogg_stream_packetout(&os,&op);
					if(result==0) {
						break;
					}
					if(result<0) {
						TRACE("Corrupt secondary header. Exiting.\n");
						lRes = ACMERR_NOTPOSSIBLE;
						goto end;
					}
					vorbis_synthesis_headerin(&vi,&vc,&op);
					if(dwFlags & DECODE_FLAGS_HEADERPHASE2) {
						dwFlags |= DECODE_FLAGS_HEADERPHASE3;
					} else {
						dwFlags |= DECODE_FLAGS_HEADERPHASE2;
					}
				}
			}
		}
	}
	vorbis_synthesis_init(&vd,&vi);
	vorbis_block_init(&vd,&vb);
 carry_read:
 end:
	return lRes;
}
/*****************************************************************
	データ読み込み
*****************************************************************/
DWORD COggVorbisDecode::ReadData(LPACMDRVSTREAMINSTANCE padsi, LPACMDRVSTREAMHEADER padsh)
{
	DWORD cbBuffSize = READSIZE;
	DWORD cbOffset = cbSrcLengthUsed;
	DWORD cbSrcLengthRemain = padsh->cbSrcLength - padsh->cbSrcLengthUsed;
	cbBuffSize = min(cbBuffSize,(cbSrcLengthUsed+cbSrcLengthRemain));

	// 入力ストリームを合成用バッファにコピー
	LPBYTE pbDst = (LPBYTE)ogg_sync_buffer(&oy,READSIZE) + cbOffset;
	LPBYTE pbSrc = padsh->pbSrc + padsh->cbSrcLengthUsed;
	DWORD cbRead = min(cbBuffSize-cbOffset, cbSrcLengthRemain);
	memcpy(pbDst,pbSrc,cbRead);

	// 各種カーソル更新
	padsh->cbSrcLengthUsed += cbRead;	// acm側入力オフセット
	cbSrcLengthUsed        += cbRead;	// ogg_sync_bufferで取得したバッファのオフセット
	cbSrcPrepared          += cbRead;	// 全長算出用

	if(cbRead==0 && !(padsh->fdwConvert & ACM_STREAMCONVERTF_END)) {
		return 0;
	} else if(cbOffset+cbRead<cbBuffSize) {
		TRACE("Carried(R): %u/%u\n",cbRead,cbBuffSize);
		return cbRead;			// 読み込みバイト数を返す(部分)
	} else {
		cbSrcLengthUsed = 0;	// 処理済バイト数をクリア
		ASSERT(cbOffset+cbRead==cbBuffSize);
		ogg_sync_wrote(&oy,cbBuffSize);
		return cbBuffSize;		// 読み込みバイト数を返す(全長)
	}
}
/*****************************************************************
	PCMデータ書き出し
*****************************************************************/
DWORD COggVorbisDecode::WriteData(LPACMDRVSTREAMINSTANCE padsi, LPACMDRVSTREAMHEADER padsh)
{
	DWORD cbSamples = 0;
	float **ppfSrc = NULL;
	cbSamples = vorbis_synthesis_pcmout(&vd,&ppfSrc);
	if(cbSamples==0) {
		return 0;
	}

	DWORD cbUnit   = HMC::GetBytesPerSample(padsi->pwfxDst);
	DWORD cbOffset = cbDstLengthUsed / cbUnit;
	DWORD cbRead   = min(cbSamples-cbOffset, (padsh->cbDstLength - padsh->cbDstLengthUsed) / cbUnit);
	LPBYTE pbDst   = padsh->pbDst + padsh->cbDstLengthUsed;

	// 合成floatバッファからacmの出力バッファへ展開
	for(WORD c=0; c<vi.channels; c++) {
		SHORT *pd = ((SHORT *)pbDst) + c;
		float *ps = &ppfSrc[c][cbOffset];
		size_t inc = cbUnit / sizeof(SHORT);
		for(DWORD n=0; n<cbRead; n++) {
			LONG lValue = (LONG)(ps[n] * 32767.f);
			*pd = (SHORT)max(SHRT_MIN,min(lValue,SHRT_MAX));
			pd += inc;
		}
	}

	// 各種カーソル更新
	padsh->cbDstLengthUsed += cbRead * cbUnit;	// acm側PCM出力オフセット
	cbDstLengthUsed        += cbRead * cbUnit;	// 合成用floatオフセット
	cbDstPrepared          += cbRead * cbUnit;	// 全長算出用

	if(cbRead==0) {
		return 0;
	} else if(cbOffset+cbRead<cbSamples) {
		TRACE("Carried(W): %u/%u\n",cbRead,cbSamples);
		return cbRead;		// 読み込みサンプル数を返す(部分)
	} else {
		ASSERT(cbOffset+cbRead==cbSamples);
		cbDstLengthUsed = 0;
		vorbis_synthesis_read(&vd,cbSamples);
		return cbSamples;	// 読み込みサンプル数を返す(全長)
	}
}
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
