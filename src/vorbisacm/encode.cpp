/*
	Ogg Vorbis CODEC for MSACM
	Copyright (C)2001 H.Mutsuki
*/

#include "hmc/util.h"
#include "hmc/mmutil.h"
#include "codec.h"
#include "encode.h"
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
	Ogg Vorbis エンコード部
*****************************************************************/
void COggVorbisEncode::init()
{
	// 悪い初期化の見本！(よい子は真似しないでね)
	size_t n = offsetof(TYPE,pDriver) + sizeof(pDriver);
	memset((LPBYTE)this+n,0,sizeof(*this)-n);
}
void COggVorbisEncode::destroy()
{
	ogg_stream_clear(&os_pad);
	ogg_stream_clear(&os);
	vorbis_block_clear(&vb);
	vorbis_dsp_clear(&vd);
	vorbis_comment_clear(&vc);
	vorbis_info_clear(&vi);
	init();
}
LRESULT COggVorbisEncode::Create(CONST WAVEFORMATEX *pwfxSrc, CONST WAVEFORMATEX *pwfxDst)
{
	LRESULT lRes = MMSYSERR_NOERROR;
	destroy();
	do {
		if(pwfxDst->wFormatTag==WAVE_FORMAT_VORBIS1P ||
		   pwfxDst->wFormatTag==WAVE_FORMAT_VORBIS2P ||
		   pwfxDst->wFormatTag==WAVE_FORMAT_VORBIS3P) {
			dwFlags |= ENCODE_FLAGS_PSEUDOCBR;
		}
		if(pwfxSrc->wFormatTag==WAVE_FORMAT_VORBIS1 ||
		   pwfxSrc->wFormatTag==WAVE_FORMAT_VORBIS1P) {
			dwFlags |= ENCODE_FLAGS_MODE1;
		}
		if(pwfxSrc->wFormatTag==WAVE_FORMAT_VORBIS2 ||
		   pwfxSrc->wFormatTag==WAVE_FORMAT_VORBIS2P) {
			dwFlags |= ENCODE_FLAGS_MODE2;
		}
		if(pwfxSrc->wFormatTag==WAVE_FORMAT_VORBIS3 ||
		   pwfxSrc->wFormatTag==WAVE_FORMAT_VORBIS3P) {
			dwFlags |= ENCODE_FLAGS_MODE3;
		}
		// エンコードモード初期化
		vorbis_info_init(&vi);
		WORD  c = pwfxDst->nChannels;
		DWORD s = pwfxDst->nSamplesPerSec;
		DWORD i = oggFormatToIndex(pwfxDst);
		DWORD r = aOggFormatIndexToDetail[i].nAvgBytesPerSec * 8;
		float q = aOggFormatIndexToDetail[i].flQuality;
		if(dwFlags & ENCODE_FLAGS_PSEUDOCBR) {
#if 0	// 問題が多いので当面ABRは使用しないことに……
			if(s>42000 && s<46000) {	// ※libVorbis I 20011231の場合44KHz以外でのビットレート管理はサポートしないのだ
				if(vorbis_encode_init(&vi,c,s,r,r,r)!=0) {
					lRes = ACMERR_NOTPOSSIBLE;
					break;
				}
				dwFlags |= ENCODE_FLAGS_RATEMANAGED;
			} else {
				if(vorbis_encode_init_vbr(&vi,c,s,q)!=0) {
					lRes = ACMERR_NOTPOSSIBLE;
					break;
				}
			}
#else
			if(vorbis_encode_init_vbr(&vi,c,s,q)!=0) {
				lRes = ACMERR_NOTPOSSIBLE;
				break;
			}
#endif
		} else {
			if(vorbis_encode_init_vbr(&vi,c,s,q)!=0) {
				lRes = ACMERR_NOTPOSSIBLE;
				break;
			}
		}
		// コメント初期化
		vorbis_comment_init(&vc);
		vorbis_comment_add_tag(&vc,"ENCODER","vorbis.acm");
		// エンコーダ状態データ、ストレージ初期化
		vorbis_analysis_init(&vd,&vi);
		vorbis_block_init(&vd,&vb);
		// シリアルナンバー初期化
		ogg_stream_init(&os,STREAMSERIALNO_AUDIO);			// オーディオ
		ogg_stream_init(&os_pad,STREAMSERIALNO_PADDING);	// パディング
		// ヘッダ作成(必須の３項目)
		ogg_packet header_main;			// Vorbisヘッダ
		ogg_packet header_comments;		// コメントヘッダ
		ogg_packet header_codebooks;	// コードブックヘッダ
		// ヘッダパケット作成
		vorbis_analysis_headerout(&vd,&vc,&header_main,&header_comments,&header_codebooks);
		// ヘッダパケット出力
		ogg_stream_packetin(&os,&header_main);
		ogg_stream_packetin(&os,&header_comments);
		ogg_stream_packetin(&os,&header_codebooks);
	} while(0);
	if(lRes!=MMSYSERR_NOERROR) {
		destroy();
	}
	return lRes;
}
/*****************************************************************
	変換コールバック
*****************************************************************/
LRESULT COggVorbisEncode::ConvertCallback(LPACMDRVSTREAMINSTANCE padsi, LPACMDRVSTREAMHEADER padsh)
{
	LRESULT lRes = MMSYSERR_NOERROR;

	// ヘッダ書き出し処理
	if(!(dwFlags & ENCODE_FLAGS_HEADERPREPARED)) {
		// mode2,mode3の場合はヘッダ出力を捨てる
		if(dwFlags & (ENCODE_FLAGS_MODE2|ENCODE_FLAGS_MODE3)) {
			while(1) {
				if(ogg_stream_flush(&os,&og)==0) {
					break;
				}
			}
			dwFlags |= ENCODE_FLAGS_HEADERPREPARED;
		} else {
			while(1) {
				if(0<cbDstLengthUsed) {
					TRACE("Continue Writing: %u/%u\n",cbDstLengthUsed,og.header_len+og.body_len);
					goto continue_write_header;
				}
				if(cbDstLengthUsed==0) {
					if(ogg_stream_flush(&os,&og)==0) {
						dwFlags |= ENCODE_FLAGS_HEADERPREPARED;
						break;
					}
				}
			continue_write_header:
				DWORD cbWrote = WritePage(padsi,padsh);
				if(cbWrote<(DWORD)og.header_len+og.body_len) {
					goto carry_header;	// ヘッダ書き出し持ち越し
				}
			}
			cbDstPrepared = 0;	// ※ヘッダサイズはPAD算出に含めない
		}
	}

	// 読み込み、解析、書き出しループ
	while(!(dwFlags&ENCODE_FLAGS_EOS)) {
		DWORD cbSamples = 0;
		if(0<cbDstLengthUsed) {
			TRACE("Continue Writing: %u/%u\n",cbDstLengthUsed,og.header_len+og.body_len);
			goto continue_write;
		}
		if(dwFlags & ENCODE_FLAGS_EOF) {
			dwFlags |= ENCODE_FLAGS_EOS;
		}
		if(0<padsh->cbSrcLength) {
			cbSamples = ReadData(padsi,padsh);
			if(0<cbSrcLengthUsed) {
				goto carry_read;	// データ読み込み持ち越し
			}
			if(cbSamples==0 && padsh->cbSrcLength==padsh->cbSrcLengthUsed) {
				if(padsh->fdwConvert & ACM_STREAMCONVERTF_END) {
					dwFlags |= ENCODE_FLAGS_EOF;
				} else {
					goto carry_read;	// データ読み込み持ち越し(その２)
				}
			}
		}
		vorbis_analysis_wrote(&vd,cbSamples);
		while(1) {
			if(vorbis_analysis_blockout(&vd,&vb)!=1) {
				if(padsh->cbSrcLength==0) {
					dwFlags |= ENCODE_FLAGS_EOS;
				}
				break;
			}
			vorbis_analysis(&vb,NULL);
			vorbis_bitrate_addblock(&vb);
			while(vorbis_bitrate_flushpacket(&vd,&op)) {
				ogg_stream_packetin(&os,&op);
				while(!(dwFlags&ENCODE_FLAGS_EOS)) {
					if(ogg_stream_pageout(&os,&og)==0) {
						break;
					}
				continue_write:
					DWORD cbWrote = WritePage(padsi,padsh);
					if(cbWrote<(DWORD)og.header_len+og.body_len) {
						goto carry_write;	// ページ書き出し持ち越し
					}
					if(ogg_page_eos(&og)) {
						dwFlags |= ENCODE_FLAGS_EOS;
						break;
					}
				}
			}
		}
	}
 carry_read:
	// 出力が０バイトの場合ストリームの終了とみなされる場合がある
	// ストリーム誤終了防止のため強制的にオーディオまたはパディングページを出力
	// ※僅かにBitrateが増加するがデコード結果に影響はない
	if(0<padsh->cbSrcLengthUsed && padsh->cbDstLengthUsed==0) {
	insert_padding:
		if(ogg_stream_flush(&os,&og)!=0) {
			DWORD cbWrote = WritePage(padsi,padsh);
			if(cbWrote<(DWORD)og.header_len+og.body_len) {
				goto carry_write;		// ページ書き出し持ち越し
			}
		} else {
			op_pad.packet = NULL;
			op_pad.bytes  = 0;
			ogg_stream_packetin(&os_pad,&op_pad);
			if(ogg_stream_flush(&os_pad,&og)!=0) {
				if((DWORD)og.header_len+og.body_len<=padsh->cbDstLength-padsh->cbDstLengthUsed) {
					cbPadPrepared += og.header_len+og.body_len;
					DWORD cbWrote = WritePage(padsi,padsh);
					ASSERT((DWORD)og.header_len+og.body_len<=cbWrote);
				} else {
					goto carry_write;	// パディングが書き込み限界に到達した場合は抜ける
				}
			}
		}
	}
	// 擬似CBR用パディング挿入
	if(dwFlags & ENCODE_FLAGS_PSEUDOCBR) {
		DWORD cbSrcPerSec = HMC::GetAvgBytesPerSec(padsi->pwfxSrc);
		DWORD cbDstPerSec = HMC::GetAvgBytesPerSec(padsi->pwfxDst);
		DWORD dwEstimate = (DWORD)((QWORD)cbSrcPrepared * cbDstPerSec / cbSrcPerSec);
		DWORD cbMergin = (dwFlags & ENCODE_FLAGS_EOS) ? 0 : min(cbDstPerSec/16, padsh->cbDstLength/16);
		if(cbMergin<cbDstPrepared-cbPadPrepared && cbDstPrepared+cbMergin<dwEstimate) {
			goto insert_padding;	// 無理やり流用……
		}
	}
 carry_write:
	// 強制的に入力を完全消費
	// ※誤終了防止用…しかし余分にメモリを消費する
	// 場合によっては、積算的にメモリ消費量が増加することがある……
	while(padsh->cbSrcLengthUsed < padsh->cbSrcLength) {
		DWORD cbSamples = 0;
		if(0<padsh->cbSrcLength) {
			cbSamples = ReadData(padsi,padsh);
			if(0<cbSrcLengthUsed) {
				//goto carry_write;	// データ読み込み持ち越し
				goto carry_write_carry;
			}
			if(cbSamples==0 && padsh->cbSrcLength==padsh->cbSrcLengthUsed) {
				//goto carry_write;	// データ読み込み持ち越し(その２)
				goto carry_write_carry;
			}
		}
		vorbis_analysis_wrote(&vd,cbSamples);
	}
 carry_write_carry:
 carry_header:
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
	flEstBitrate = (float)((LONG64)cbDstPrepared*HMC::GetAvgBytesPerSec(padsi->pwfxSrc)*8) / max(1,cbSrcPrepared) / 1000;
	flCurBitrate = (float)((LONG64)(cbDstPrepared-cbDstLastPrepared)*HMC::GetAvgBytesPerSec(padsi->pwfxSrc)*8) / max(1,cbSrcPrepared-cbSrcLastPrepared) / 1000;
	flMaxBitrate = max(flMaxBitrate,flCurBitrate);
	cbSrcLastPrepared = cbSrcPrepared;
	cbDstLastPrepared = cbDstPrepared;
	cbPadLastPrepared = cbPadPrepared;
#endif
	TRACE("Converting: %u/%u/%u, Est:%.2f, Cur:%.2f, Max:%.2f\n",cbSrcPrepared,cbDstPrepared,cbPadPrepared,flEstBitrate,flCurBitrate,flMaxBitrate);
	return lRes;
}
/*****************************************************************
	PCMデータ読み込み
*****************************************************************/
DWORD COggVorbisEncode::ReadData(LPACMDRVSTREAMINSTANCE padsi, LPACMDRVSTREAMHEADER padsh)
{
	DWORD cbSamples = READSIZE;
	DWORD cbUnit = HMC::GetBytesPerSample(padsi->pwfxSrc);
	DWORD cbSrcLengthRemain = padsh->cbSrcLength - padsh->cbSrcLengthUsed;
	cbSamples = min(cbSamples,(cbSrcLengthUsed+cbSrcLengthRemain)/cbUnit);

	// RAWPCM(16Bits)を解析用floatバッファに展開
	float **ppfDst = vorbis_analysis_buffer(&vd,READSIZE);	// 解析用floatバッファ
	DWORD cbOffset = cbSrcLengthUsed / cbUnit;				// 解析用floatバッファのオフセット
	LPBYTE pbSrc = padsh->pbSrc + padsh->cbSrcLengthUsed;	// acm側PCM入力バッファ
	DWORD cbRead = min(cbSamples-cbOffset,cbSrcLengthRemain/cbUnit);	// 今回読み込み分のサンプル数
	if(vi.channels==1) {
		for(DWORD n=0; n<cbRead; n++) {
			ppfDst[0][cbOffset+n] = (SHORT)(((CHAR*)pbSrc)[n*2+1]<<8|pbSrc[n*2+0]) / 32768.0f;
			ASSERT(-1.0f<=ppfDst[0][cbOffset+n] && ppfDst[0][cbOffset+n]<=1.0f);
		}
	} else if(vi.channels==2) {
		for(DWORD n=0; n<cbRead; n++) {
			ppfDst[0][cbOffset+n] = (SHORT)(((CHAR*)pbSrc)[n*4+1]<<8|pbSrc[n*4+0]) / 32768.0f;
			ppfDst[1][cbOffset+n] = (SHORT)(((CHAR*)pbSrc)[n*4+3]<<8|pbSrc[n*4+2]) / 32768.0f;
			ASSERT(-1.0f<=ppfDst[0][cbOffset+n] && ppfDst[0][cbOffset+n]<=1.0f);
			ASSERT(-1.0f<=ppfDst[1][cbOffset+n] && ppfDst[1][cbOffset+n]<=1.0f);
		}
	}

	// 各種カーソル更新
	padsh->cbSrcLengthUsed += cbRead * cbUnit;	// acm側PCM入力オフセット
	cbSrcLengthUsed        += cbRead * cbUnit;	// 解析用floatオフセット
	cbSrcPrepared          += cbRead * cbUnit;	// パディング算出用全長

	if(cbRead==0 && !(padsh->fdwConvert & ACM_STREAMCONVERTF_END)) {
		return 0;
	} else if(cbOffset+cbRead<cbSamples) {
		TRACE("Carried(R): %u/%u\n",cbRead,cbSamples);
		return cbRead;			// 読み込みサンプル数を返す(部分)
	} else {
		ASSERT(cbOffset+cbRead==cbSamples);
		cbSrcLengthUsed = 0;	// 処理済バイト数をクリア
		//vorbis_analysis_wrote(&vd,cbSamples);
		return cbSamples;		// 読み込みサンプル数を返す(全長)
	}
}
/*****************************************************************
	ページ書き出し
*****************************************************************/
DWORD COggVorbisEncode::WritePage(LPACMDRVSTREAMINSTANCE padsi, LPACMDRVSTREAMHEADER padsh)
{
	DWORD len1 = og.header_len;	// ヘッダ長
	DWORD len2 = og.body_len;	// ボディ長

	// ヘッダ部書き出し
	if(cbDstLengthUsed < len1) {
		DWORD len = padsh->cbDstLength - padsh->cbDstLengthUsed;
		len = min(len, len1-cbDstLengthUsed);
		LPBYTE pd = padsh->pbDst + padsh->cbDstLengthUsed;
		LPBYTE ps = (LPBYTE)og.header + cbDstLengthUsed;
		memcpy(pd,ps,len);
		padsh->cbDstLengthUsed += len;
		cbDstLengthUsed += len;
		cbDstPrepared += len;
	}
	// ボディ部書き出し
	if(cbDstLengthUsed < len1+len2) {
		DWORD len = padsh->cbDstLength - padsh->cbDstLengthUsed;
		len = min(len, len2-(cbDstLengthUsed-len1));
		LPBYTE pd = padsh->pbDst + padsh->cbDstLengthUsed;
		LPBYTE ps = (LPBYTE)og.body + (cbDstLengthUsed-len1);
		memcpy(pd,ps,len);
		padsh->cbDstLengthUsed += len;
		cbDstLengthUsed += len;
		cbDstPrepared += len;
	}

	if(cbDstLengthUsed < len1+len2) {
		TRACE("Carried(W): %u/%u\n",len1+len2-cbDstLengthUsed,len1+len2);
		return cbDstLengthUsed;	// 書き込みバイト数を返す(部分)
	} else {
		cbDstLengthUsed = 0;	// 処理済みバイト数をクリア
		return len1+len2;		// 書き込みバイト数を返す(全長)
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
