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
	Ogg Vorbis �G���R�[�h��
*****************************************************************/
void COggVorbisEncode::init()
{
	// �����������̌��{�I(�悢�q�͐^�����Ȃ��ł�)
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
		// �G���R�[�h���[�h������
		vorbis_info_init(&vi);
		WORD  c = pwfxDst->nChannels;
		DWORD s = pwfxDst->nSamplesPerSec;
		DWORD i = oggFormatToIndex(pwfxDst);
		DWORD r = aOggFormatIndexToDetail[i].nAvgBytesPerSec * 8;
		float q = aOggFormatIndexToDetail[i].flQuality;
		if(dwFlags & ENCODE_FLAGS_PSEUDOCBR) {
#if 0	// ��肪�����̂œ���ABR�͎g�p���Ȃ����ƂɁc�c
			if(s>42000 && s<46000) {	// ��libVorbis I 20011231�̏ꍇ44KHz�ȊO�ł̃r�b�g���[�g�Ǘ��̓T�|�[�g���Ȃ��̂�
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
		// �R�����g������
		vorbis_comment_init(&vc);
		vorbis_comment_add_tag(&vc,"ENCODER","vorbis.acm");
		// �G���R�[�_��ԃf�[�^�A�X�g���[�W������
		vorbis_analysis_init(&vd,&vi);
		vorbis_block_init(&vd,&vb);
		// �V���A���i���o�[������
		ogg_stream_init(&os,STREAMSERIALNO_AUDIO);			// �I�[�f�B�I
		ogg_stream_init(&os_pad,STREAMSERIALNO_PADDING);	// �p�f�B���O
		// �w�b�_�쐬(�K�{�̂R����)
		ogg_packet header_main;			// Vorbis�w�b�_
		ogg_packet header_comments;		// �R�����g�w�b�_
		ogg_packet header_codebooks;	// �R�[�h�u�b�N�w�b�_
		// �w�b�_�p�P�b�g�쐬
		vorbis_analysis_headerout(&vd,&vc,&header_main,&header_comments,&header_codebooks);
		// �w�b�_�p�P�b�g�o��
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
	�ϊ��R�[���o�b�N
*****************************************************************/
LRESULT COggVorbisEncode::ConvertCallback(LPACMDRVSTREAMINSTANCE padsi, LPACMDRVSTREAMHEADER padsh)
{
	LRESULT lRes = MMSYSERR_NOERROR;

	// �w�b�_�����o������
	if(!(dwFlags & ENCODE_FLAGS_HEADERPREPARED)) {
		// mode2,mode3�̏ꍇ�̓w�b�_�o�͂��̂Ă�
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
					goto carry_header;	// �w�b�_�����o�������z��
				}
			}
			cbDstPrepared = 0;	// ���w�b�_�T�C�Y��PAD�Z�o�Ɋ܂߂Ȃ�
		}
	}

	// �ǂݍ��݁A��́A�����o�����[�v
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
				goto carry_read;	// �f�[�^�ǂݍ��ݎ����z��
			}
			if(cbSamples==0 && padsh->cbSrcLength==padsh->cbSrcLengthUsed) {
				if(padsh->fdwConvert & ACM_STREAMCONVERTF_END) {
					dwFlags |= ENCODE_FLAGS_EOF;
				} else {
					goto carry_read;	// �f�[�^�ǂݍ��ݎ����z��(���̂Q)
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
						goto carry_write;	// �y�[�W�����o�������z��
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
	// �o�͂��O�o�C�g�̏ꍇ�X�g���[���̏I���Ƃ݂Ȃ����ꍇ������
	// �X�g���[����I���h�~�̂��ߋ����I�ɃI�[�f�B�I�܂��̓p�f�B���O�y�[�W���o��
	// ���͂���Bitrate���������邪�f�R�[�h���ʂɉe���͂Ȃ�
	if(0<padsh->cbSrcLengthUsed && padsh->cbDstLengthUsed==0) {
	insert_padding:
		if(ogg_stream_flush(&os,&og)!=0) {
			DWORD cbWrote = WritePage(padsi,padsh);
			if(cbWrote<(DWORD)og.header_len+og.body_len) {
				goto carry_write;		// �y�[�W�����o�������z��
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
					goto carry_write;	// �p�f�B���O���������݌��E�ɓ��B�����ꍇ�͔�����
				}
			}
		}
	}
	// �[��CBR�p�p�f�B���O�}��
	if(dwFlags & ENCODE_FLAGS_PSEUDOCBR) {
		DWORD cbSrcPerSec = HMC::GetAvgBytesPerSec(padsi->pwfxSrc);
		DWORD cbDstPerSec = HMC::GetAvgBytesPerSec(padsi->pwfxDst);
		DWORD dwEstimate = (DWORD)((QWORD)cbSrcPrepared * cbDstPerSec / cbSrcPerSec);
		DWORD cbMergin = (dwFlags & ENCODE_FLAGS_EOS) ? 0 : min(cbDstPerSec/16, padsh->cbDstLength/16);
		if(cbMergin<cbDstPrepared-cbPadPrepared && cbDstPrepared+cbMergin<dwEstimate) {
			goto insert_padding;	// ������藬�p�c�c
		}
	}
 carry_write:
	// �����I�ɓ��͂����S����
	// ����I���h�~�p�c�������]���Ƀ������������
	// �ꍇ�ɂ���ẮA�ώZ�I�Ƀ���������ʂ��������邱�Ƃ�����c�c
	while(padsh->cbSrcLengthUsed < padsh->cbSrcLength) {
		DWORD cbSamples = 0;
		if(0<padsh->cbSrcLength) {
			cbSamples = ReadData(padsi,padsh);
			if(0<cbSrcLengthUsed) {
				//goto carry_write;	// �f�[�^�ǂݍ��ݎ����z��
				goto carry_write_carry;
			}
			if(cbSamples==0 && padsh->cbSrcLength==padsh->cbSrcLengthUsed) {
				//goto carry_write;	// �f�[�^�ǂݍ��ݎ����z��(���̂Q)
				goto carry_write_carry;
			}
		}
		vorbis_analysis_wrote(&vd,cbSamples);
	}
 carry_write_carry:
 carry_header:
	// �e��I�[�o�[�����A�A���_�[�����`�F�b�N
	ASSERT(padsh->cbSrcLengthUsed <= padsh->cbSrcLength);
	ASSERT(padsh->cbDstLengthUsed <= padsh->cbDstLength);
	// ���͂����S�ɏ����Ȃ��Ǝc�f�[�^���h���b�v�����ꍇ������
	if(padsh->cbSrcLengthUsed < padsh->cbSrcLength) {
		TRACE("Input data may be dropped %u[bytes] (frame %ld)\n",padsh->cbSrcLength-padsh->cbSrcLengthUsed,(long)(vd.sequence));
	}
	// �o�͂��O�o�C�g�̏ꍇ�X�g���[���̏I���Ƃ݂Ȃ����ꍇ������
	if(0<padsh->cbSrcLength && 0==padsh->cbDstLengthUsed) {
		TRACE("Output data may be dropped (frame %ld)\n",(long)(vd.sequence));
	}

	// �r�b�g���[�g����p�f�[�^�X�V
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
	PCM�f�[�^�ǂݍ���
*****************************************************************/
DWORD COggVorbisEncode::ReadData(LPACMDRVSTREAMINSTANCE padsi, LPACMDRVSTREAMHEADER padsh)
{
	DWORD cbSamples = READSIZE;
	DWORD cbUnit = HMC::GetBytesPerSample(padsi->pwfxSrc);
	DWORD cbSrcLengthRemain = padsh->cbSrcLength - padsh->cbSrcLengthUsed;
	cbSamples = min(cbSamples,(cbSrcLengthUsed+cbSrcLengthRemain)/cbUnit);

	// RAWPCM(16Bits)����͗pfloat�o�b�t�@�ɓW�J
	float **ppfDst = vorbis_analysis_buffer(&vd,READSIZE);	// ��͗pfloat�o�b�t�@
	DWORD cbOffset = cbSrcLengthUsed / cbUnit;				// ��͗pfloat�o�b�t�@�̃I�t�Z�b�g
	LPBYTE pbSrc = padsh->pbSrc + padsh->cbSrcLengthUsed;	// acm��PCM���̓o�b�t�@
	DWORD cbRead = min(cbSamples-cbOffset,cbSrcLengthRemain/cbUnit);	// ����ǂݍ��ݕ��̃T���v����
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

	// �e��J�[�\���X�V
	padsh->cbSrcLengthUsed += cbRead * cbUnit;	// acm��PCM���̓I�t�Z�b�g
	cbSrcLengthUsed        += cbRead * cbUnit;	// ��͗pfloat�I�t�Z�b�g
	cbSrcPrepared          += cbRead * cbUnit;	// �p�f�B���O�Z�o�p�S��

	if(cbRead==0 && !(padsh->fdwConvert & ACM_STREAMCONVERTF_END)) {
		return 0;
	} else if(cbOffset+cbRead<cbSamples) {
		TRACE("Carried(R): %u/%u\n",cbRead,cbSamples);
		return cbRead;			// �ǂݍ��݃T���v������Ԃ�(����)
	} else {
		ASSERT(cbOffset+cbRead==cbSamples);
		cbSrcLengthUsed = 0;	// �����σo�C�g�����N���A
		//vorbis_analysis_wrote(&vd,cbSamples);
		return cbSamples;		// �ǂݍ��݃T���v������Ԃ�(�S��)
	}
}
/*****************************************************************
	�y�[�W�����o��
*****************************************************************/
DWORD COggVorbisEncode::WritePage(LPACMDRVSTREAMINSTANCE padsi, LPACMDRVSTREAMHEADER padsh)
{
	DWORD len1 = og.header_len;	// �w�b�_��
	DWORD len2 = og.body_len;	// �{�f�B��

	// �w�b�_�������o��
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
	// �{�f�B�������o��
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
		return cbDstLengthUsed;	// �������݃o�C�g����Ԃ�(����)
	} else {
		cbDstLengthUsed = 0;	// �����ς݃o�C�g�����N���A
		return len1+len2;		// �������݃o�C�g����Ԃ�(�S��)
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
